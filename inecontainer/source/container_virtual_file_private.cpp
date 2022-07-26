/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 - 2022 Inesonic, LLC.
*
* MIT License:
*   Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
*   documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
*   rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
*   permit persons to whom the Software is furnished to do so, subject to the following conditions:
*   
*   The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
*   Software.
*   
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
*   WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
*   OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
*   OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref Container::VirtualFile::Private class.
***********************************************************************************************************************/

#include <cstdint>
#include <vector>
#include <map>
#include <string>
#include <memory>

#include "container_container.h"
#include "container_container_private.h"
#include "container_impl.h"
#include "container_virtual_file.h"
#include "virtual_file_impl.h"
#include "container_virtual_file_private.h"

namespace Container {
    VirtualFile::Private::Private(
            const std::string& newName,
            StreamChunk::StreamIdentifier streamIdentifier,
            Container*                    container,
            VirtualFile*                  interface
        ):VirtualFileImpl(
            newName,
            streamIdentifier,
            container->impl
        ) {
        iface = interface;
    }


    VirtualFile::Private::~Private() {}


    Status VirtualFile::Private::receivedData(const std::uint8_t* buffer, unsigned bytesReceived) {
        return iface->receivedData(buffer, bytesReceived);
    }


    Status VirtualFile::Private::endOfFile() {
        return iface->endOfFile();
    }
}
