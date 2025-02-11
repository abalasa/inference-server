// Copyright 2021 Xilinx Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/**
 * @file
 * @brief Defines the methods for interacting with Proteus in the native C++ API
 */

#ifndef GUARD_PROTEUS_CLIENTS_NATIVE
#define GUARD_PROTEUS_CLIENTS_NATIVE

#include <cstddef>  // for size_t
#include <string>   // for string
#include <vector>   // for vector

#include "proteus/clients/client.hpp"        // for Client
#include "proteus/core/predict_api.hpp"      // for InferenceRequest (ptr only)
#include "proteus/helpers/declarations.hpp"  // for InferenceResponseFuture

// IWYU pragma: no_forward_declare proteus::RequestParameters

namespace proteus {

/// Initialize proteus - calls initializeLogging internally
void initialize();
/// Initialize logging
void initializeLogging();

/// Shut down proteus
void terminate();

/// get log directory
std::string getLogDirectory();

/// Get a string that lists the available kernels ("<name>:i,<name>:j...")
std::string getHardware();

/**
 * @brief Check if a particular kernel exists on the server. The string
 * splitting code is inspired from: https://stackoverflow.com/a/14266139
 *
 * @param kernel kernel name to check if it exists. An empty string is a special
 * name that matches any kernel name.
 * @param num minimum number of the kernels that should be present
 * @return bool
 */
bool hasHardware(const std::string& kernel, size_t num);

class NativeClient : public Client {
 public:
  ServerMetadata serverMetadata() override;
  bool serverLive() override;
  bool serverReady() override;

  /**
   * @brief Check if a worker is ready
   *
   * @param worker name of the worker to check if ready
   */
  bool modelReady(const std::string& model) override;

  /**
   * @brief Load a worker
   *
   * @param model name of the worker to load
   * @param parameters any load-time parameters to pass to the worker
   * @return std::string the qualified name of the worker to make inference
   * requests
   */
  void modelLoad(const std::string& model,
                 RequestParameters* parameters) override;
  /**
   * @brief Unload a worker
   *
   * @param worker name of the worker to unload
   */
  void modelUnload(const std::string& model) override;
  InferenceResponse modelInfer(const std::string& model,
                               const InferenceRequest& request) override;
  std::vector<std::string> modelList() override;

  std::string workerLoad(const std::string& worker,
                         RequestParameters* parameters) override;
  void workerUnload(const std::string& worker) override;

  /**
   * @brief Enqueue an inference request to Proteus
   *
   * @param workerName name of the worker to make the request to
   * @param request the request to make
   * @return InferenceResponseFuture a future to get the results of the request
   */
  static InferenceResponseFuture enqueue(const std::string& workerName,
                                         InferenceRequest request);
};

}  // namespace proteus

#endif  // GUARD_PROTEUS_CLIENTS_NATIVE
