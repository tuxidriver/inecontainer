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
* This file implements the \ref Container::VirtualFile class.
***********************************************************************************************************************/

#include <cstdint>
#include <vector>
#include <map>
#include <string>
#include <memory>

#include "container_status.h"
#include "stream_chunk.h"
#include "container_container.h"
#include "container_container_private.h"
#include "container_impl.h"
#include "container_virtual_file_private.h"
#include "container_virtual_file.h"

namespace Container {
    VirtualFile::VirtualFile(const std::string& newName, Container* container) {
        StreamChunk::StreamIdentifier streamIdentifier = container->impl->newStreamIdentifier();
        impl = std::make_shared<VirtualFile::Private>(newName, streamIdentifier, container, this);
    }


    VirtualFile::VirtualFile(const VirtualFile& other) {
        impl = other.impl;
    }


    VirtualFile::~VirtualFile() {}


    std::string VirtualFile::name() const {
        return impl->name();
    }


    long long VirtualFile::size() {
        return impl->size();
    }


    Status VirtualFile::setPosition(unsigned long long newOffset) {
        return impl->setPosition(newOffset);
    }


    Status VirtualFile::setPositionLast() {
        return impl->setPositionLast();
    }


    long long VirtualFile::position() const {
        return impl->position();
    }


    unsigned long long VirtualFile::bytesInWriteCache() const {
        return impl->bytesInWriteCache();
    }


    Status VirtualFile::read(std::uint8_t* buffer, unsigned desiredCount) {
        return impl->read(buffer, desiredCount);
    }


    Status VirtualFile::write(const std::uint8_t* buffer, unsigned desiredCount) {
        return impl->write(buffer, desiredCount);
    }


    Status VirtualFile::append(const std::uint8_t* buffer, unsigned desiredCount) {
        return impl->append(buffer, desiredCount);
    }


    Status VirtualFile::truncate() {
        return impl->truncate();
    }


    Status VirtualFile::flush() {
        return impl->flush();
    }


    Status VirtualFile::erase() {
        return impl->erase();
    }


    Status VirtualFile::rename(const std::string& newName) {
        return impl->rename(newName);
    }


    VirtualFile& VirtualFile::operator=(const VirtualFile& other) {
        impl = other.impl;
        return *this;
    }


    Status VirtualFile::receivedData(const std::uint8_t*, unsigned) {
        return StreamingReadError();
    }


    Status VirtualFile::endOfFile() {
        return NoStatus();
    }
}
