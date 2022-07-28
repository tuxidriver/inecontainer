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
project(inecontainer LANGUAGES CXX)

SET(CMAKE_CXX_STANDARD 14)

# Allow building either static or shared libraries
set(${PROJECT_NAME}_TYPE CACHE STRING "Static or shared library")
set_property(CACHE ${PROJECT_NAME}_TYPE PROPERTY STRINGS "SHARED;STATIC")
IF(NOT ${${PROJECT_NAME}_TYPE})
    SET(${PROJECT_NAME}_TYPE STATIC)
ENDIF()

add_library(${PROJECT_NAME} ${${PROJECT_NAME}_TYPE}
            source/container_status_base.cpp
            source/container_status.cpp
            source/container_impl.cpp
            source/container_container_private.cpp
            source/container_container.cpp
            source/container_memory_container_private.cpp
            source/container_memory_container.cpp
            source/container_file_container_private.cpp
            source/container_file_container.cpp
            source/virtual_file_impl.cpp
            source/container_virtual_file_private.cpp
            source/container_virtual_file.cpp
            source/container_area.cpp
            source/free_space_data.cpp
            source/free_space.cpp
            source/free_space_tracker.cpp
            source/chunk_map_data.cpp
            source/scatter_gather_list_segment.cpp
            source/chunk_header.cpp
            source/chunk.cpp
            source/file_header_chunk.cpp
            source/fill_chunk.cpp
            source/stream_chunk.cpp
            source/stream_start_chunk.cpp
            source/stream_data_chunk.cpp
)

set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE 1)

target_include_directories(${PROJECT_NAME} PUBLIC "include")
include_directories("include")

install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
install(FILES include/container_status_base.h DESTINATION include)
install(FILES include/container_status.h DESTINATION include)
install(FILES include/container_container.h DESTINATION include)
install(FILES include/container_memory_container.h DESTINATION include)
install(FILES include/container_file_container.h DESTINATION include)
install(FILES include/container_virtual_file.h DESTINATION include)
