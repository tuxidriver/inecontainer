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
CONFIG -= qt
CONFIG += CONSOLE c++14

SOURCES = ic.cpp

########################################################################################################################
# Libraries
#

INECONTAINER_BASE = $${OUT_PWD}/../inecontainer
INCLUDEPATH += $${PWD}/../inecontainer/include

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

TARGET = ic

CONFIG(debug, debug|release) {
    unix:DESTDIR = build/debug
    win32:DESTDIR = build/Debug
} else {
    unix:DESTDIR = build/release
    win32:DESTDIR = build/Release
}

OBJECTS_DIR = $${DESTDIR}/objects
