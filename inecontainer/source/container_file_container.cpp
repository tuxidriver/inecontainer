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
* This file implements the \ref Container::FileContainer class.
***********************************************************************************************************************/

#include <cstdint>
#include <string>
#include <memory>

#include "container_status.h"
#include "container_virtual_file.h"
#include "container_container.h"
#include "container_file_container_private.h"
#include "container_file_container.h"

namespace Container {
    FileContainer::FileContainer(
            const std::string& fileIdentifier,
            bool               ignoreIdentifier
        ):Container(
            fileIdentifier,
            ignoreIdentifier
        ) {
        impl.reset(new FileContainer::Private(this)); // Note std::make_unique is C++14
    }


    FileContainer::~FileContainer() {}


    Status FileContainer::open(const std::string& filename, OpenMode openMode) {
        Status status = impl->open(filename, openMode);

        if (!status) {
            status = Container::open();
        }

        return status;
    }


    Status FileContainer::close() {
        Status status = Container::close();

        if (!status) {
            status = impl->close();
        }

        return status;
    }


    std::string FileContainer::filename() const {
        return impl->filename();
    }


    FileContainer::OpenMode FileContainer::openMode() const {
        return impl->openMode();
    }


    long long FileContainer::size() {
        return impl->size();
    }


    Status FileContainer::setPosition(unsigned long long newOffset) {
        return impl->setPosition(newOffset);
    }


    Status FileContainer::setPositionLast() {
        return impl->setPositionLast();
    }


    unsigned long long FileContainer::position() const {
        return impl->position();
    }


    Status FileContainer::read(std::uint8_t* buffer, unsigned desiredCount) {
        return impl->read(buffer, desiredCount);
    }


    Status FileContainer::write(const std::uint8_t* buffer, unsigned count) {
        return impl->write(buffer, count);
    }


    bool FileContainer::supportsTruncation() const {
        return impl->supportsTruncation();
    }


    Status FileContainer::truncate() {
        return impl->truncate();
    }


    Status FileContainer::flush() {
        return impl->flush();
    }
}
