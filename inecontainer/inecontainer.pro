##-*-makefile-*-########################################################################################################
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

########################################################################################################################
# Basic build characteristics
#

TEMPLATE = lib
CONFIG -= qt
CONFIG += static c++14

########################################################################################################################
# Public includes
#

INCLUDEPATH += include
API_HEADERS = include/container_status_base.h \
              include/container_status.h \
              include/container_container.h \
              include/container_memory_container.h \
              include/container_file_container.h \
              include/container_virtual_file.h

########################################################################################################################
# Source files
#

SOURCES = source/container_status_base.cpp \
          source/container_status.cpp \
          source/container_impl.cpp \
          source/container_container_private.cpp \
          source/container_container.cpp \
          source/container_memory_container_private.cpp \
          source/container_memory_container.cpp \
          source/container_file_container_private.cpp \
          source/container_file_container.cpp \
          source/virtual_file_impl.cpp \
          source/container_virtual_file_private.cpp \
          source/container_virtual_file.cpp \
          source/container_area.cpp \
          source/free_space_data.cpp \
          source/free_space.cpp \
          source/free_space_tracker.cpp \
          source/chunk_map_data.cpp \
          source/scatter_gather_list_segment.cpp \
          source/chunk_header.cpp \
          source/chunk.cpp \
          source/file_header_chunk.cpp \
          source/fill_chunk.cpp \
          source/stream_chunk.cpp \
          source/stream_start_chunk.cpp \
          source/stream_data_chunk.cpp

########################################################################################################################
# Private includes
#

INCLUDEPATH += source
PRIVATE_HEADERS = source/container_impl.h \
                  source/container_container_private.h \
                  source/virtual_file_impl.h \
                  source/container_virtual_file_private.h \
                  source/container_memory_container_private.h \
                  source/container_file_container_private.h \
                  source/container_area.h \
                  source/free_space_data.h \
                  source/free_space.h \
                  source/free_space_tracker.h \
                  source/ring_buffer.h \
                  source/chunk_map_data.h \
                  source/scatter_gather_list_segment.h \
                  source/chunk_header.h \
                  source/chunk.h \
                  source/file_header_chunk.h \
                  source/fill_chunk.h \
                  source/stream_chunk.h \
                  source/stream_start_chunk.h \
                  source/stream_data_chunk.h

########################################################################################################################
# Setup headers and installation
#

HEADERS = $$API_HEADERS $$PRIVATE_HEADERS

########################################################################################################################
# Locate build intermediate and output products
#

TARGET = inecontainer

CONFIG(debug, debug|release) {
    unix:DESTDIR = build/debug
    win32:DESTDIR = build/Debug
} else {
    unix:DESTDIR = build/release
    win32:DESTDIR = build/Release
}

OBJECTS_DIR = $${DESTDIR}/objects
MOC_DIR = $${DESTDIR}/moc
RCC_DIR = $${DESTDIR}/rcc
UI_DIR = $${DESTDIR}/ui

