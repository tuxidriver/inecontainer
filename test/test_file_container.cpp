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
* This file implements base class functions that test the Container::FileContainer class.
***********************************************************************************************************************/

#include <QDebug>
#include <QtTest/QtTest>
#include <QFileInfo>

#include <memory>
#include <string>

#include <container_container.h>
#include <container_file_container.h>

#include "test_container_base.h"
#include "test_file_container.h"

const char TestFileContainer::containerFilename[] = "test_container.dat";

std::shared_ptr<Container::Container> TestFileContainer::allocateContainer(const std::string& fileIdentifier) {
    return std::make_shared<Container::FileContainer>(fileIdentifier);
}


Container::Status TestFileContainer::openContainer(
        std::shared_ptr<Container::Container> container,
        bool                                  resetContents
    ) {
    std::shared_ptr<Container::FileContainer> mc = std::dynamic_pointer_cast<Container::FileContainer>(container);

    Container::FileContainer::OpenMode openMode =   resetContents
                                                  ? Container::FileContainer::OpenMode::OVERWRITE
                                                  : Container::FileContainer::OpenMode::READ_WRITE;

    return mc->open(containerFilename, openMode);
}


Container::Status TestFileContainer::closeContainer(std::shared_ptr<Container::Container> container) {
    std::shared_ptr<Container::FileContainer> mc = std::dynamic_pointer_cast<Container::FileContainer>(container);
    return mc->close();
}


unsigned long long TestFileContainer::containerSize() const {
    QFileInfo fileInformation(containerFilename);
    return static_cast<unsigned long long>(fileInformation.size());
}

