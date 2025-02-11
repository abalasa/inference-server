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
 * @brief Defines the HTTP REST server in Proteus
 */

#ifndef GUARD_PROTEUS_SERVERS_HTTP_SERVER
#define GUARD_PROTEUS_SERVERS_HTTP_SERVER

#include <functional>  // for function
#include <string>      // for allocator, string

#include "proteus/build_options.hpp"        // for PROTEUS_ENABLE_HTTP, PROT...
#include "proteus/observation/logging.hpp"  // for LoggerPtr

#ifdef PROTEUS_ENABLE_HTTP
#include <drogon/HttpController.h>  // for ADD_METHOD_TO, HttpContro...
#include <drogon/HttpRequest.h>     // for HttpRequestPtr
#include <drogon/HttpResponse.h>    // for HttpResponsePtr
#include <drogon/HttpTypes.h>       // for Options, Get, Post
#endif

namespace proteus::http {

#ifdef PROTEUS_ENABLE_HTTP

namespace v2 {

/**
 * @brief Proteus's HTTP server for handling REST requests extends the base
 * HttpController in Drogon and adds the endpoints of interest.
 *
 */
class ProteusHttpServer
  : public drogon::HttpController<ProteusHttpServer, false> {
 public:
  /// Constructor
  explicit ProteusHttpServer();

  METHOD_LIST_BEGIN
#ifdef PROTEUS_ENABLE_REST
  /// Register the getServerLive endpoint
  ADD_METHOD_TO(ProteusHttpServer::getServerLive, "v2/health/live", drogon::Get,
                drogon::Options);
  /// Register the getServerReady endpoint
  ADD_METHOD_TO(ProteusHttpServer::getServerReady, "v2/health/ready",
                drogon::Get, drogon::Options);
  /// Register the getModelReady endpoint
  ADD_METHOD_TO(ProteusHttpServer::getModelReady, "v2/models/{model}/ready",
                drogon::Get, drogon::Options);
  /// Register the getServerMetadata endpoint
  ADD_METHOD_TO(ProteusHttpServer::getServerMetadata, "v2", drogon::Get,
                drogon::Options);
  /// Register the getModelMetadata endpoint
  ADD_METHOD_TO(ProteusHttpServer::getModelMetadata, "v2/models/{model}",
                drogon::Get, drogon::Options);
  /// Register the getHardware endpoint
  ADD_METHOD_TO(ProteusHttpServer::getHardware, "v2/hardware", drogon::Get,
                drogon::Options);
  /// Register the modelList endpoint
  ADD_METHOD_TO(ProteusHttpServer::modelList, "v2/models", drogon::Get,
                drogon::Options);
  /// Register the inferModel endpoint
  ADD_METHOD_TO(ProteusHttpServer::inferModel, "v2/models/{model}/infer",
                drogon::Post, drogon::Options);
  /// Register the load endpoint
  ADD_METHOD_TO(ProteusHttpServer::load, "v2/repository/models/{model}/load",
                drogon::Post, drogon::Options);
  /// Register the unload endpoint
  ADD_METHOD_TO(ProteusHttpServer::unload,
                "v2/repository/models/{model}/unload", drogon::Post,
                drogon::Options);
  /// Register the workerLoad endpoint
  ADD_METHOD_TO(ProteusHttpServer::workerLoad, "v2/workers/{worker}/load",
                drogon::Post, drogon::Options);
  /// Register the workerUnload endpoint
  ADD_METHOD_TO(ProteusHttpServer::workerUnload, "v2/workers/{worker}/unload",
                drogon::Post, drogon::Options);
#endif
#ifdef PROTEUS_ENABLE_METRICS
  /// Register the metrics endpoint
  ADD_METHOD_TO(ProteusHttpServer::metrics, "metrics", drogon::Get);
#endif
  METHOD_LIST_END

#ifdef PROTEUS_ENABLE_REST
  /**
   * @brief Returns 200 if server is live and ready to receive metadata and
   * inference requests
   *
   * @param req the REST request object
   * @param callback the callback function to respond to the client
   */
  void getServerLive(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback);

  /**
   * @brief Returns 200 if all models are ready for inferencing
   *
   * @param req the REST request object
   * @param callback the callback function to respond to the client
   */
  void getServerReady(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback);

  /**
   * @brief Returns 200 if a specific model is ready for inferencing
   *
   * @param req the REST request object
   * @param callback the callback function to respond to the client
   * @param model name of the model to serve the request
   */
  void getModelReady(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback,
    std::string const &model);

  /**
   * @brief Returns metadata associated with Proteus
   *
   * @param req the REST request object
   * @param callback the callback function to respond to the client
   */
  void getServerMetadata(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback);

  /**
   * @brief Returns metadata associated with a model
   *
   * @param req the REST request object
   * @param callback the callback function to respond to the client
   * @param model name of the model to serve the request
   */
  void getModelMetadata(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback,
    std::string const &model);

  /**
   * @brief Returns the available hardware on the server
   *
   * @param req the REST request object
   * @param callback the callback function to respond to the client
   */
  void getHardware(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback);

  /**
   * @brief Returns the active models on the server
   *
   * @param req the REST request object
   * @param callback the callback function to respond to the client
   */
  void modelList(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback);

  /**
   * @brief Handles inference requests for named models
   *
   * @param req the REST request object
   * @param callback the callback function to respond to the client
   * @param model name of the model to serve the request
   */
  void inferModel(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback,
    std::string const &model);

  /**
   * @brief Loads and starts a model
   *
   * @param req the REST request object
   * @param callback the callback function to respond to the client
   * @param model name of the model to load
   */
  void load(const drogon::HttpRequestPtr &req,
            std::function<void(const drogon::HttpResponsePtr &)> &&callback,
            std::string const &model);

  /**
   * @brief Unloads a model
   *
   * @param req the REST request object
   * @param callback the callback function to respond to the client
   * @param model name of the model to unload
   */
  void unload(const drogon::HttpRequestPtr &req,
              std::function<void(const drogon::HttpResponsePtr &)> &&callback,
              std::string const &model);

  /**
   * @brief Loads and starts a worker
   *
   * @param req the REST request object
   * @param callback the callback function to respond to the client
   * @param worker name of the worker to load
   */
  void workerLoad(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback,
    std::string const &worker);

  /**
   * @brief Unloads a worker
   *
   * @param req the REST request object
   * @param callback the callback function to respond to the client
   * @param worker name of the worker to unload
   */
  void workerUnload(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback,
    std::string const &worker);
#endif

#ifdef PROTEUS_ENABLE_METRICS
  /**
   * @brief Returns the raw metric data collected in Proteus
   *
   * @param req the REST request object
   * @param callback the callback function to respond to the client
   */
  void metrics(const drogon::HttpRequestPtr &req,
               std::function<void(const drogon::HttpResponsePtr &)> &&callback);
#endif
#ifdef PROTEUS_ENABLE_LOGGING
 private:
  Logger logger_{Loggers::kServer};
#endif
};

}  // namespace v2

#endif

/**
 * @brief Start the Proteus HTTP REST server
 *
 * @param port the port to use for the server
 */
void start(int port);

/// Stop the REST server
void stop();

}  // namespace proteus::http
#endif  // GUARD_PROTEUS_SERVERS_HTTP_SERVER
