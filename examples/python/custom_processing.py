# Copyright 2021 Xilinx Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

"""
This example shows how to use the Python API to start the server, make a request
to it over REST and parse the response. In contrast to the simpler Hello World
example, we make an inference to a custom XModel and show how to pre- and post-
process the results. Look at the documentation for more detailed commentary on
this example.
"""

import math
import os
from time import sleep

import cv2
import numpy as np

import proteus
import proteus.clients


def preprocess(images):
    images_preprocessed = []

    data_type = "uint8"
    height = 224
    width = 224
    channels = 3
    layout = "NHWC"
    fix_scale = 1
    mean = [123, 107, 104]

    for image in images:
        image = image.astype(getattr(np, data_type))
        image = cv2.resize(image, (height, width))
        new_image = np.zeros(image.shape, dtype=np.int8)
        if layout == "NCHW":  # image defaults to HWC
            image = np.rollaxis(image, axis=2, start=0)  # reshape to CHW
            for c in range(channels):
                for h in range(height):
                    for w in range(width):
                        new_image[c][h][w] = int((image[c][h][w] - mean[c]) * fix_scale)

        else:
            for h in range(height):
                for w in range(width):
                    for c in range(channels):
                        new_image[h][w][c] = int((image[h][w][c] - mean[c]) * fix_scale)
        images_preprocessed.append(new_image)

    return images_preprocessed


def postprocess(data, k):
    max_val = max(data)
    softmax = [0] * len(data)

    data_sum = 0
    for i, val in enumerate(data):
        softmax[i] = math.exp(val - max_val)
        data_sum += softmax[i]

    for i, val in enumerate(data):
        softmax[i] /= data_sum

    # get the top classifications
    tmp = [i[0] for i in sorted(enumerate(softmax), reverse=True, key=lambda x: x[1])]

    # only return the top k
    return tmp[:k]


def main():
    root = os.getenv("PROTEUS_ROOT")
    assert root is not None

    # +user variables: update as needed!
    batch_size = 4
    path_to_xmodel = "${PROTEUS_ROOT}/external/artifacts/u200_u250/resnet_v1_50_tf/resnet_v1_50_tf.xmodel"
    path_to_image = root + "/tests/assets/dog-3619020_640.jpg"
    # for this image, we know what we expect to receive with this XModel
    gold_response_output = [259, 261, 260, 154, 230]
    # -user variables

    client = proteus.clients.HttpClient("http://127.0.0.1:8998")

    start_server = not client.serverLive()
    if start_server:
        proteus.initialize()
        proteus.clients.startHttpServer(8998)

    # +load worker:
    parameters = proteus.RequestParameters()
    parameters.put("model", path_to_xmodel)
    worker_name = client.workerLoad("Xmodel", parameters)

    ready = False
    while not ready:
        try:
            ready = client.modelReady(worker_name)
        except ValueError:
            pass
    # -load worker:

    # +get images:
    images = []
    for _ in range(batch_size):
        image = cv2.imread(path_to_image)
        images.append(image)
    # -get images:

    # +inference:
    images = preprocess(images)
    # Construct the request and send it
    request = proteus.ImageInferenceRequest(images, True)
    response = client.modelInfer(worker_name, request)
    assert not response.isError(), response.getError()
    outputs = response.getOutputs()
    for output in outputs:
        assert output.datatype == proteus.DataType.INT8
        recv_data = output.getInt8Data()
        # Can optionally post-process the result
        k = postprocess(recv_data, 5)
        assert k == gold_response_output
    # -inference:

    if start_server:
        proteus.clients.stopHttpServer()
        proteus.terminate()
        while client.serverLive():
            sleep(1)

    print("custom_processing.py: Passed")


if __name__ == "__main__":
    main()
