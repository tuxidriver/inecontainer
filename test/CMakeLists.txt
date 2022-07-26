##-*-cmake-*-###########################################################################################################
# Copyright 2016 - 2022 Inesonic, LLC
#
# MIT License:
#   Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
#   documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
#   rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
#   permit persons to whom the Software is furnished to do so, subject to the following conditions:
#   
#   The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
#   Software.
#   
#   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
#   WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
#   OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
#   OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
########################################################################################################################

cmake_minimum_required(VERSION 3.16.3)
project(test LANGUAGES CXX)

enable_testing()

find_package(Qt5 COMPONENTS Core)
find_package(Qt5 COMPONENTS Test)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(test
               test_status.cpp
               test_inecontainer.cpp
               test_container_area.cpp
               test_scatter_gather_list_segment.cpp
               test_free_space.cpp
               test_free_space_data.cpp
               test_free_space_tracker.cpp
               test_ring_buffer.cpp
               test_chunk_map_data.cpp
               test_chunk_header.cpp
               test_chunk.cpp
               test_fill_chunk.cpp
               test_file_header_chunk.cpp
               test_stream_start_chunk.cpp
               test_stream_data_chunk.cpp
               test_container_base.cpp
               test_memory_container.cpp
               test_file_container.cpp
               test_virtual_file.cpp
)
add_test(${PROJECT_NAME} ${PROJECT_NAME})

add_dependencies(${PROJECT_NAME} inecontainer)

target_include_directories(${PROJECT_NAME} PUBLIC "../incontainer/include")
target_include_directories(${PROJECT_NAME} PRIVATE "../incontainer/source")

include_directories("../inecontainer/include")
include_directories("../inecontainer/source")

target_link_libraries(${PROJECT_NAME} inecontainer)
target_link_libraries(${PROJECT_NAME} Qt5::Test)
