// Copyright 2022 Xilinx Inc.
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

#ifndef GUARD_SRC_PROTEUS_TESTING_GTEST_FIXTURES
#define GUARD_SRC_PROTEUS_TESTING_GTEST_FIXTURES

#include <memory>  // for allocator, unique_ptr
#include <proteus/proteus.hpp>

#include "gtest/gtest.h"  // IWYU pragma: export

class BaseFixture : public testing::Test {
 public:
  static void SetUpTestSuite() { proteus::initialize(); };

  static void TearDownTestSuite() { proteus::terminate(); }
};

class GrpcFixture : public BaseFixture {
 protected:
  void SetUp() override {
    client_ = std::make_unique<proteus::GrpcClient>("localhost:50051");
    if (!client_->serverLive()) {
      proteus::startGrpcServer(50051);
      started_ = true;
      while (!client_->serverLive()) {
        std::this_thread::yield();
      }
    }
  }

  void TearDown() override {
    if (started_) {
      proteus::stopGrpcServer();
    }
  }

  std::unique_ptr<proteus::GrpcClient> client_;
  bool started_ = false;
};

class HttpFixture : public BaseFixture {
 protected:
  void SetUp() override {
    client_ = std::make_unique<proteus::HttpClient>("http://127.0.0.1:8998");
    if (!client_->serverLive()) {
      proteus::startHttpServer(8998);
      started_ = true;
      while (!client_->serverLive()) {
        std::this_thread::yield();
      }
    }
  }

  void TearDown() override {
    if (started_) {
      proteus::stopHttpServer();
    }
  }

  std::unique_ptr<proteus::HttpClient> client_;
  bool started_ = false;
};

#define EXPECT_THROW_CHECK(statement, check, exception) \
  EXPECT_THROW(                                         \
    {                                                   \
      try {                                             \
        statement                                       \
      } catch (const exception& e) {                    \
        check throw;                                    \
      }                                                 \
    },                                                  \
    exception)

#endif  // GUARD_SRC_PROTEUS_TESTING_GTEST_FIXTURES
