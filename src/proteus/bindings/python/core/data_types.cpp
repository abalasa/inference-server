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

/**
 * @file
 * @brief Implements the Python bindings for the data_types.hpp header
 */

#include "proteus/core/data_types.hpp"

#include <pybind11/pybind11.h>

#ifdef PROTEUS_ENABLE_VITIS
#include <xir/util/data_type.hpp>
#endif

#include "proteus/bindings/python/helpers/print.hpp"

namespace py = pybind11;

using proteus::types::DataType;

void wrapDataType(py::module_& m) {
  py::enum_<DataType>(m, "DataType")
    .value("BOOL", DataType::BOOL)
    .value("UINT8", DataType::UINT8)
    .value("UINT16", DataType::UINT16)
    .value("UINT32", DataType::UINT32)
    .value("UINT64", DataType::UINT64)
    .value("INT8", DataType::INT8)
    .value("INT16", DataType::INT16)
    .value("INT32", DataType::INT32)
    .value("INT64", DataType::INT64)
    .value("FP16", DataType::FP16)
    .value("FP32", DataType::FP32)
    .value("FP64", DataType::FP64)
    .value("STRING", DataType::STRING)
    .def("__repr__",
         [](const DataType& self) {
           return "DataType(" + proteus::types::mapTypeToStr(self) + ")\n";
         })
    .def("__str__", &proteus::to_string<DataType>);
}

void wrapTypeMaps(py::module_& m) {
  m.def("mapTypeToStr", proteus::types::mapTypeToStr, py::arg("type"));
  m.def("mapStrToType", proteus::types::mapStrToType, py::arg("type"));
#ifdef PROTEUS_ENABLE_VITIS
  py::module_::import("xir").attr("DataType");

  m.def("mapXirType", proteus::types::mapXirType, py::arg("type"));
  m.def("mapTypeToXir", proteus::types::mapTypeToXir, py::arg("type"));
#endif
}
