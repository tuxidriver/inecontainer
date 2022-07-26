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
* This file implements base class functions that test the Container::Container class.
***********************************************************************************************************************/

#include <QDebug>
#include <QtTest/QtTest>

#include <cstdint>
#include <cstring>
#include <vector>
#include <memory>
#include <sstream>
#include <random>

#include <container_container.h>
#include <container_status.h>
#include <container_virtual_file.h>

#include "test_container_base.h"

void TestContainerBase::testContainerApi() {
    // Open an empty container and insert a virtual file into it.

    std::shared_ptr<Container::Container> writeContainer = allocateContainer("Inesonic, LLC.\nAleph Test");

    Container::Status status = openContainer(writeContainer, true);
    QVERIFY(status.success());

    std::uint8_t buffer[bufferSizeInBytes];
    for (unsigned i=0 ; i<bufferSizeInBytes ; ++i) {
        buffer[i] = static_cast<std::uint8_t>(i % 254);
    }

    std::shared_ptr<Container::VirtualFile> vf = writeContainer->newVirtualFile("test.dat");
    status = vf->write(buffer, bufferSizeInBytes);
    QVERIFY(status.success());
    QVERIFY(Container::WriteSuccessful(status).bytesWritten() == bufferSizeInBytes);

    status = closeContainer(writeContainer);

    std::memset(buffer, 0, bufferSizeInBytes);

    // Now re-open the container and read out the virtual file.

    std::shared_ptr<Container::Container> readContainer1 = allocateContainer("Inesonic, LLC.\nAleph Test");

    status = openContainer(readContainer1, false);
    QVERIFY(status.success());

    Container::Container::DirectoryMap directory = readContainer1->directory();
    QVERIFY(directory.size() == 1);

    Container::Container::DirectoryMap::iterator pos = directory.find("test.dat");
    QVERIFY(pos != directory.end());

    vf = pos->second;
    QVERIFY(vf->size() == bufferSizeInBytes);

    status = vf->read(buffer, bufferSizeInBytes);
    QVERIFY(status.success());
    QVERIFY(Container::ReadSuccessful(status).bytesRead() == bufferSizeInBytes);

    for (unsigned i=0 ; i<bufferSizeInBytes ; ++i) {
        QVERIFY(buffer[i] == static_cast<std::uint8_t>(i % 254));
    }

    status = closeContainer(readContainer1);
    QVERIFY(!status);

    QVERIFY(containerSize() > bufferSizeInBytes);

    // Lastly, re-open the container API and delete the file.  This will trigger file truncatin to occur.

    std::shared_ptr<Container::Container> readContainer2 = allocateContainer("Inesonic, LLC.\nAleph Test");

    status = openContainer(readContainer2, false);
    QVERIFY(status.success());

    directory = readContainer2->directory();
    QVERIFY(directory.size() == 1);

    pos = directory.find("test.dat");
    QVERIFY(pos != directory.end());

    vf = pos->second;
    QVERIFY(vf->size() == bufferSizeInBytes);

    status = vf->erase();
    QVERIFY(status.success());

    status = closeContainer(readContainer2);
    QVERIFY(!status);

    QVERIFY(containerSize() == 64);
}
