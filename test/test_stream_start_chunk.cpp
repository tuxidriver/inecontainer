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
* This file implements tests of the StreamStartChunk class.
***********************************************************************************************************************/

#include <QDebug>
#include <QtTest/QtTest>

#include <cstdint>

#define LIBCONTAINER_TEST // Makes the implementation accessible from the public API.

#include <container_container.h>
#include <container_memory_container.h>
#include <container_container_private.h> // temporary
#include <container_impl.h>
#include <stream_start_chunk.h>

#include "test_stream_start_chunk.h"

/***********************************************************************************************************************
 * TestStreamStartChunk
 */

void TestStreamStartChunk::testAccessors() {
    Container::MemoryContainer container("Inesonic, LLC./nAleph");

    StreamStartChunk chunk(
        dynamic_cast<Container::Container&>(container).impl,
        0,
        "test_file.dat",
        1
    );

    QVERIFY(chunk.streamIdentifier() == 1);
    QVERIFY(chunk.virtualFilename() == "test_file.dat");
    QVERIFY(chunk.isLast() == false);

    chunk.setStreamIdentifier(2);
    QVERIFY(chunk.streamIdentifier() == 2);

    chunk.setVirtualFilename("test_file_2.dat");
    QVERIFY(chunk.virtualFilename() == "test_file_2.dat");

    chunk.setLast(true);
    QVERIFY(chunk.isLast());
}


void TestStreamStartChunk::testSaveLoadMethods() {
    Container::MemoryContainer container("Inesonic, LLC./nAleph");
    Container::Status status = container.open();
    QVERIFY(!status);

    StreamStartChunk chunk1(
        dynamic_cast<Container::Container&>(container).impl,
        0,
        "test_file.dat",
        1
    );

    status = chunk1.save();
    QVERIFY(status.success());

    StreamStartChunk chunk2(
        dynamic_cast<Container::Container&>(container).impl,
        0,
        "bad.dat",
        2
    );

    status = chunk2.load(true);
    QVERIFY(status.success());

    QVERIFY(chunk2.streamIdentifier() == 1);
    QVERIFY(chunk2.isLast() == false);
    QVERIFY(chunk2.virtualFilename() == "test_file.dat");
}
