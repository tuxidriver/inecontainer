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
* This file implements base class functions that test the Container::MemoryContainer class.
***********************************************************************************************************************/

#include <QDebug>
#include <QtTest/QtTest>

#include <memory>
#include <string>

#include <container_container.h>
#include <container_memory_container.h>

#include "test_container_base.h"
#include "test_memory_container.h"

std::shared_ptr<Container::Container> TestMemoryContainer::allocateContainer(const std::string& fileIdentifier) {
    return std::make_shared<Container::MemoryContainer>(fileIdentifier);
}


Container::Status TestMemoryContainer::openContainer(
        std::shared_ptr<Container::Container> container,
        bool                                  resetContents
    ) {
    std::shared_ptr<Container::MemoryContainer> mc = std::dynamic_pointer_cast<Container::MemoryContainer>(container);

    if (!currentBuffer) {
        currentBuffer = std::make_shared<Container::MemoryContainer::MemoryBuffer>();
    }

    if (resetContents) {
        currentBuffer->clear();
    }

    return mc->open(currentBuffer);
}


Container::Status TestMemoryContainer::closeContainer(std::shared_ptr<Container::Container> container) {
    std::shared_ptr<Container::MemoryContainer> mc = std::dynamic_pointer_cast<Container::MemoryContainer>(container);
    return mc->close();
}


unsigned long long TestMemoryContainer::containerSize() const {
    unsigned long long size;

    if (!currentBuffer) {
        size = 0;
    } else {
        size = currentBuffer->size();
    }

    return size;
}
