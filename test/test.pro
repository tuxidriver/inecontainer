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

TEMPLATE = app
QT += core testlib
CONFIG += testcase c++14

HEADERS = test_status.h \
          test_container_area.h \
          test_scatter_gather_list_segment.h \
          test_free_space.h \
          test_free_space_data.h \
          test_free_space_tracker.h \
          test_ring_buffer.h \
          test_chunk_map_data.h \
          test_chunk_header.h \
          test_chunk.h \
          test_fill_chunk.h \
          test_file_header_chunk.h \
          test_stream_start_chunk.h \
          test_stream_data_chunk.h \
          test_container_base.h \
          test_memory_container.h \
          test_file_container.h \
          test_virtual_file.h

SOURCES = test_status.cpp \
          test_inecontainer.cpp \
          test_container_area.cpp \
          test_scatter_gather_list_segment.cpp \
          test_free_space.cpp \
          test_free_space_data.cpp \
          test_free_space_tracker.cpp \
          test_ring_buffer.cpp \
          test_chunk_map_data.cpp \
          test_chunk_header.cpp \
          test_chunk.cpp \
          test_fill_chunk.cpp \
          test_file_header_chunk.cpp \
          test_stream_start_chunk.cpp \
          test_stream_data_chunk.cpp \
          test_container_base.cpp \
          test_memory_container.cpp \
          test_file_container.cpp \
          test_virtual_file.cpp

########################################################################################################################
# Libraries
#

INECONTAINER_BASE = $${OUT_PWD}/../inecontainer

INCLUDEPATH += $${PWD}/../inecontainer/include
INCLUDEPATH += $${PWD}/../inecontainer/source # Needed for white-box tests

INCLUDEPATH += $${BOOST_INCLUDE}

unix {
    CONFIG(debug, debug|release) {
        LIBS += -L$${INECONTAINER_BASE}/build/debug/ -linecontainer
        PRE_TARGETDEPS += $${INECONTAINER_BASE}/build/debug/libinecontainer.a
    } else {
        LIBS += -L$${INECONTAINER_BASE}/build/release/ -linecontainer
        PRE_TARGETDEPS += $${INECONTAINER_BASE}/build/release/libinecontainer.a
   }
}

win32 {
    CONFIG(debug, debug|release) {
        LIBS += $${INECONTAINER_BASE}/build/Debug/inecontainer.lib
        PRE_TARGETDEPS += $${INECONTAINER_BASE}/build/Debug/inecontainer.lib
    } else {
        LIBS += $${INECONTAINER_BASE}/build/Release/inecontainer.lib
        PRE_TARGETDEPS += $${INECONTAINER_BASE}/build/Release/inecontainer.lib
    }
}

########################################################################################################################
# Locate build intermediate and output products
#

TARGET = test_inecontainer

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
