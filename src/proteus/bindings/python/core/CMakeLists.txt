# Copyright 2022 Xilinx Inc.
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

list(APPEND TARGETS
    predict_api
    data_types
)

list(APPEND LINK_TARGETS)

foreach(target ${TARGETS})
    add_library(${target}_py OBJECT ${target}.cpp)
    target_include_directories(${target}_py PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/.. ${PROTEUS_INCLUDE_DIRS} ${Python_INCLUDE_DIRS})
    target_link_libraries(${target}_py PRIVATE ${Python_LIBRARIES} ${target} $<TARGET_OBJECTS:${target}>)
    add_dependencies(${target}_py pybind11Mkdoc)

    set(LINK_TARGETS ${LINK_TARGETS} ${target}_py)
endforeach()

# target_link_libraries(data_types_py PRIVATE /usr/lib/python3/dist-packages/xir.cpython-36m-x86_64-linux-gnu.so)

set(CORE_OBJS ${LINK_TARGETS} PARENT_SCOPE)
