..
    Copyright 2022 Advanced Micro Devices, Inc.

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

ZenDNN
======

AMD Inference Server is integrated with
`ZenDNN <https://developer.amd.com/zendnn/>`__ optimized libraries.
Currently, the AMD Inference Server supports the ZenDNN optimized for
TensorFlow and PyTorch.
Check the :ref:`examples <zendnn_examples>` to see how to use ZenDNN with the Inference Server.

For ZenDNN performance, please refer to TensorFlow + ZenDNN and
PyTorch + ZenDNN User Guide available at the `ZenDNN AMD Developer <https://developer.amd.com/zendnn/>`_ site.

Setup and Build
---------------

To use Inference Server enabled with TensorFlow/PyTorch+ZenDNN, you'll need Git
and Docker installed on your machine.

1. Download C++ package for TensorFlow/PyTorch+ZenDNN

   1. Go to https://developer.amd.com/zendnn/
   2. Download the file

      1. For TensorFlow: TF_v2.9_ZenDNN_v3.3_C++_API.zip
      2. For PyTorch: PT_v1.11.0_ZenDNN_v3.3_C++_API.zip

   3. For download, you will be required to sign a EULA. Please read
      through carefully and click on accept to download the package.
   4. Copy the downloaded package within the repository. This package
      will be used for further setup.

      .. note:: The package will be needed by the Docker build process so it must be in a location that is not excluded in the ``.dockerignore`` file such as the root of the repository.
      

2. Build the docker with TensorFlow/PyTorch+ZenDNN

   To build docker image with TensorFlow/PyTorch+ZenDNN, use the command below:

   1. For TensorFlow

      .. code-block:: console

         $ ./proteus dockerize --no-vitis --tfzendnn=./local/path/to/TF_v2.9_ZenDNN_v3.3_C++_API.zip

   2. For PyTorch

      .. code-block:: console

         $ ./proteus dockerize --no-vitis --ptzendnn=./local/path/to/PT_v1.11.0_ZenDNN_v3.3_C++_API.zip

   You can add the :code:`--no-vitis` flag to build the docker without Vitis AI components if needed.

   This will build a docker image with all the dependencies required for
   the AMD Inference Server and setup TensorFlow/PyTorch+ZenDNN within the
   image for further usage.

Get objects (models/images)
---------------------------

Run the following command to get some ``git lfs`` assets for examples/tests.

.. code-block:: console

   $ git lfs fetch --all
   $ git lfs pull

To run the examples and test cases, we need to download some models.

* TensorFlow + ZenDNN

   Run the command below to download a ResNet50 tensorflow model from the
   `Vitis AI model zoo <https://github.com/Xilinx/Vitis-AI/blob/master/model_zoo/model-list/tf_resnetv1_50_imagenet_224_224_6.97G_2.5/model.yaml>`__

   .. code-block:: console

      $ ./proteus get --tfzendnn

   The model downloaded will be available at :code:`./external/tensorflow_models.`


* PyTorch + ZenDNN

   Run the command below to download a ResNet50 tensorflow model from
   `Vitis AI model zoo <https://github.com/Xilinx/Vitis-AI/blob/master/model_zoo/model-list/pt_resnet50_imagenet_224_224_8.2G_2.5/model.yaml>`__

   .. code-block:: console

      $ ./proteus get --ptzendnn

   The model downloaded will be available at :code:`./external/pytorch_models`.


Set Up Docker Container
-----------------------

1. Run the container

   By default, the stable dev docker image is built and to run the
   container, use the command:

   .. code-block:: console

      $ ./proteus run --dev

2. Build AMD Inference Server

   Now that the environment is setup within the docker container, we
   need to build the Inference Server. The below command will build the
   stable debug build of the AMD Inference Server.

   .. code-block:: console

      $ ./proteus build --debug

   NOTE: If you are switching containers, and build folder already exits
   in the inference-server folder, please use :code:`--regen --clean` flags to
   regenerate CMakeFiles and do a clean build to avoid any issues.

3. For PyTorch+ZenDNN only
   We need to convert the downloaded PyTorch eager model to TorchScript
   Model (`Exporting to TorchScript docs <https://pytorch.org/tutorials/advanced/cpp_export.html#converting-to-torch-script-via-tracing>`_).

   To convert the model to TorchScript model, follow the steps.

   1. We will need to use the PyTorch python API. Install requirements with:

      .. code-block:: console

         $ pip3 install -r tools/zendnn/requirements.txt

   2. To convert the model to TorchScript Model do:

      .. code-block:: console

         $ python tools/zendnn/convert_to_torchscript.py --graph external/pytorch_models/resnet50_pretrained.pth

      The script will do the following:

      1. Load ResNet50 architecture from tools/zendnn/resnet50.py file.
      2. Load the downloaded weights to the model.
      3. Do a jit trace of model.
      4. Save the traced TorchScript model to the same location with .pt extension.

   The converted TorchScript model will be used by the examples and tests.
   For more info on TorchScript models, please visit `PyTorch docs <https://pytorch.org/tutorials/advanced/cpp_export.html>`_.

Run Tests
---------

To verify the working of TensorFlow+ZenDNN in the AMD Inference Server, run a sample test case. This test will load a model and run with a sample image and assert the output.

1. For TensorFlow + ZenDNN

   .. code-block:: console

      $ ./proteus test -k tfzendnn

2. For PyTorch + ZenDNN

   .. code-block:: console

      $ ./proteus test -k ptzendnn
