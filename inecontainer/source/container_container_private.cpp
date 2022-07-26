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
* This file implements the \ref Container::Container::Private class.
***********************************************************************************************************************/

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <utility>

#include "container_status.h"
#include "chunk_header.h"
#include "container_virtual_file.h"
#include "container_container.h"
#include "container_impl.h"
#include "container_container_private.h"

namespace Container {
    Container::Private::Private(
            const std::string& fileIdentifier,
            bool               ignoreIdentifier,
            Container*         interface
        ):ContainerImpl(
            fileIdentifier,
            ignoreIdentifier
        ) {
        iface = interface;
    }

    Container::Private::~Private() {}


    std::shared_ptr<VirtualFile> Container::Private::callNewVirtualFile(const std::string &newVirtualFileName) {
        return iface->newVirtualFile(newVirtualFileName);
    }


    VirtualFile* Container::Private::createFile(const std::string& virtualFileName) {
        return iface->createFile(virtualFileName);
    }


    long long Container::Private::size() {
        return iface->size();
    }


    Status Container::Private::setPosition(unsigned long long newOffset) {
        return iface->setPosition(newOffset);
    }


    Status Container::Private::setPositionLast() {
        return iface->setPositionLast();
    }


    unsigned long long Container::Private::position() const {
        return iface->position();
    }


    Status Container::Private::read(std::uint8_t* buffer, unsigned desiredCount) {
        return iface->read(buffer, desiredCount);
    }


    Status Container::Private::write(const std::uint8_t* buffer, unsigned count) {
        return iface->write(buffer, count);
    }


    bool Container::Private::supportsTruncation() const {
        return iface->supportsTruncation();
    }


    Status Container::Private::truncate() {
        return iface->truncate();
    }


    Status Container::Private::flush() {
        return iface->flush();
    }
}
