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
* This file implements the \ref Container::Container class.
***********************************************************************************************************************/

#include <cstdint>
#include <set>
#include <string>
#include <memory>

#include "container_status.h"
#include "container_virtual_file.h"
#include "container_virtual_file_private.h"
#include "container_container_private.h"
#include "container_container.h"

namespace Container {
    Container::Container(const std::string& fileIdentifier, bool ignoreIdentifier) {
        impl = std::make_shared<Container::Private>(fileIdentifier, ignoreIdentifier, this);
        impl->setWeakThis(impl);
    }


    Container::~Container() {}


    Status Container::lastStatus() const {
        return impl->lastStatus();
    }


    std::uint8_t Container::minorVersion() const {
        return impl->minorVersion();
    }


    Container::DirectoryMap Container::directory() {
        return impl->directory();
    }


    Status Container::open() {
        return impl->open();
    }


    Status Container::close() {
        return impl->close();
    }


    std::shared_ptr<VirtualFile> Container::newVirtualFile(const std::string& newVirtualFileName) {
        std::shared_ptr<VirtualFile> virtualFile = impl->newVirtualFile(newVirtualFileName);
        impl->registerFileImplementation(virtualFile->impl);
        return virtualFile;
    }


    Status Container::streamRead() {
        return impl->streamRead();
    }


    VirtualFile* Container::createFile(const std::string& virtualFileName) {
        return new VirtualFile(virtualFileName, this);
    }
}
