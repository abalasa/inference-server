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

#include "proteus/observation/tracing.hpp"

#include <opentelemetry/exporters/ostream/span_exporter.h>
#include <opentelemetry/sdk/resource/resource.h>
#include <opentelemetry/sdk/trace/simple_processor.h>
#include <opentelemetry/sdk/trace/tracer_provider.h>
#include <opentelemetry/trace/provider.h>

#include <cstdint>  // for int32_t
#include <map>      // for _Rb_tree_iterator, operator!=
#include <string>   // for string, basic_string
#include <utility>  // for pair
#include <variant>  // for get, holds_alternative, visi

#include "proteus/core/predict_api.hpp"  // for Parameter, RequestParameters

#ifdef PROTEUS_ENABLE_TRACING

namespace trace_api = opentelemetry::trace;
namespace trace_sdk = opentelemetry::sdk::trace;
namespace nostd = opentelemetry::nostd;

namespace proteus {

void startTracer() {
  auto exporter =
    std::make_unique<opentelemetry::exporter::trace::OStreamSpanExporter>();
  auto processor =
    std::make_unique<trace_sdk::SimpleSpanProcessor>(std::move(exporter));
  auto provider =
    nostd::shared_ptr<trace_api::TracerProvider>(new trace_sdk::TracerProvider(
      std::move(processor),
      opentelemetry::sdk::resource::Resource::Create({})));

  // Set the global trace provider
  trace_api::Provider::SetTracerProvider(provider);
}

nostd::shared_ptr<trace_api::Tracer> get_tracer() {
  auto provider = trace_api::Provider::GetTracerProvider();
  return provider->GetTracer("proteus");
}

void stopTracer() {
  auto tracer = get_tracer();
  tracer->Close(std::chrono::milliseconds(1));
}

Trace::Trace(const char* name) {
  auto tracer = get_tracer();
  this->spans_.emplace(tracer->StartSpan(name));
}

Trace::~Trace() { this->endTrace(); }

void Trace::startSpan(const char* name) {
  auto scope = trace_api::Scope(this->spans_.top());
  auto tracer = get_tracer();
  this->spans_.emplace(tracer->StartSpan(name));
}

void Trace::setAttribute(nostd::string_view key,
                         const opentelemetry::common::AttributeValue& value) {
  this->spans_.top()->SetAttribute(key, value);
}

void Trace::setAttributes(RequestParameters* parameters) {
  auto data = parameters->data();
  // a range-based for loop doesn't work here because we can't pass the key when
  // it's a structured binding.
  for (auto it = data.begin(); it != data.end(); it++) {
    auto key = it->first;
    auto value = it->second;
    std::visit(
      [key, this](Parameter&& arg) {
        if (std::holds_alternative<bool>(arg))
          this->spans_.top()->SetAttribute(key, std::get<bool>(arg));
        else if (std::holds_alternative<double>(arg))
          this->spans_.top()->SetAttribute(key, std::get<double>(arg));
        else if (std::holds_alternative<int32_t>(arg))
          this->spans_.top()->SetAttribute(key, std::get<int32_t>(arg));
        else if (std::holds_alternative<std::string>(arg))
          this->spans_.top()->SetAttribute(key, std::get<std::string>(arg));
      },
      value);
  }
}

void Trace::endSpan() {
  this->spans_.top()->End();
  this->spans_.pop();
}

void Trace::endTrace() {
  while (!this->spans_.empty()) {
    this->endSpan();
  }
}

TracePtr startTrace(const char* name) { return std::make_unique<Trace>(name); }

}  // namespace proteus

#endif
