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
* This file implements the \ref Container::MemoryContainer::Private class.
***********************************************************************************************************************/

#include <cstdint>
#include <vector>
#include <map>
#include <string>
#include <memory>
#include <cstring>

#include "container_status.h"
#include "container_memory_container.h"
#include "container_memory_container_private.h"

namespace Container {
    MemoryContainer::Private::Private(MemoryContainer* interface) {
        iface           = interface;
        currentPosition = 0;
    }


    MemoryContainer::Private::~Private() {}


    void MemoryContainer::Private::setBuffer(std::shared_ptr<MemoryContainer::MemoryBuffer> newBuffer) {
        memoryBuffer    = newBuffer;
        currentPosition = 0;
    }


    std::shared_ptr<MemoryContainer::MemoryBuffer> MemoryContainer::Private::buffer() {
        return memoryBuffer;
    }


    long long MemoryContainer::Private::size() {
        return memoryBuffer->size();
    }


    Status MemoryContainer::Private::setPosition(unsigned long long newOffset) {
        Status status;

        if (newOffset <= memoryBuffer->size()) {
            currentPosition = newOffset;
        } else {
            status = SeekError(newOffset, memoryBuffer->size());
        }

        return status;
    }


    Status MemoryContainer::Private::setPositionLast() {
        currentPosition = memoryBuffer->size();
        return NoStatus();
    }


    unsigned long long MemoryContainer::Private::position() const {
        return currentPosition;
    }


    Status MemoryContainer::Private::read(std::uint8_t* buffer, unsigned desiredCount) {
        unsigned long long maximumLength = memoryBuffer->size() - currentPosition;

        unsigned           bytesToCopy;
        if (desiredCount < maximumLength) {
            bytesToCopy = desiredCount;
        } else {
            bytesToCopy = static_cast<unsigned>(maximumLength);
        }

        std::uint8_t* data = memoryBuffer->data() + currentPosition;
        std::memcpy(buffer, data, bytesToCopy);

        currentPosition += bytesToCopy;
        return ReadSuccessful(bytesToCopy);
    }


    Status MemoryContainer::Private::write(const std::uint8_t* buffer, unsigned count) {
        unsigned long long remainingInBuffer = memoryBuffer->size() - currentPosition;
        unsigned           bytesWritten      = count;

        if (remainingInBuffer > 0) {
            unsigned numberToOverwrite;
            if (count < remainingInBuffer) {
                numberToOverwrite = count;
            } else {
                numberToOverwrite = static_cast<unsigned>(remainingInBuffer);
            }

            std::uint8_t* data = memoryBuffer->data() + currentPosition;
            std::memcpy(data, buffer, numberToOverwrite);

            buffer += numberToOverwrite;
            count  -= numberToOverwrite;
        }

        while (count > 0) {
            memoryBuffer->push_back(*buffer);
            ++buffer;
            --count;
        }

        currentPosition += bytesWritten;

        return WriteSuccessful(bytesWritten);
    }


    bool MemoryContainer::Private::supportsTruncation() const {
        return true;
    }


    Status MemoryContainer::Private::truncate() {
        memoryBuffer->resize(static_cast<std::vector<std::uint8_t>::size_type>(currentPosition));
        return NoStatus();
    }


    Status MemoryContainer::Private::flush() {
        return NoStatus();
    }
}
