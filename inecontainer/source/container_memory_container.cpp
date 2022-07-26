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
* This file implements the \ref Container::MemoryContainer class.
***********************************************************************************************************************/

#include <cstdint>
#include <vector>
#include <map>
#include <string>
#include <memory>

#include "container_status.h"
#include "container_virtual_file.h"
#include "container_container.h"
#include "container_memory_container_private.h"
#include "container_memory_container.h"

namespace Container {
    MemoryContainer::MemoryContainer(
            const std::string& fileIdentifier,
            bool               ignoreIdentifier
        ):Container(
            fileIdentifier,
            ignoreIdentifier
        ) {
        impl.reset(new MemoryContainer::Private(this)); // Note std::make_unique is C++14
    }


    MemoryContainer::~MemoryContainer() {}


    Status MemoryContainer::open(std::shared_ptr<MemoryBuffer> buffer) {
        impl->setBuffer(buffer);
        return Container::open();
    }


    Status MemoryContainer::close() {
        return Container::close();
    }


    std::shared_ptr<std::vector<std::uint8_t>> MemoryContainer::buffer() {
        return impl->buffer();
    }


    long long MemoryContainer::size() {
        return impl->size();
    }


    Status MemoryContainer::setPosition(unsigned long long newOffset) {
        return impl->setPosition(newOffset);
    }


    Status MemoryContainer::setPositionLast() {
        return impl->setPositionLast();
    }


    unsigned long long MemoryContainer::position() const {
        return impl->position();
    }


    Status MemoryContainer::read(std::uint8_t* buffer, unsigned desiredCount) {
        return impl->read(buffer, desiredCount);
    }


    Status MemoryContainer::write(const std::uint8_t* buffer, unsigned count) {
        return impl->write(buffer, count);
    }


    bool MemoryContainer::supportsTruncation() const {
        return impl->supportsTruncation();
    }


    Status MemoryContainer::truncate() {
        return impl->truncate();
    }


    Status MemoryContainer::flush() {
        return impl->flush();
    }
}
