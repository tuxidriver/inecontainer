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
* This file implements tests of the FillChunk class.
***********************************************************************************************************************/

#include <QDebug>
#include <QtTest/QtTest>

#include <cstdint>

#define LIBCONTAINER_TEST // Makes the implementation accessible from the public API.

#include <container_container.h>
#include <container_memory_container.h>
#include <container_container_private.h> // temporary
#include <container_impl.h>
#include <fill_chunk.h>

#include "test_fill_chunk.h"

/***********************************************************************************************************************
 * FillChunkWrapper
 */

class FillChunkWrapper:public FillChunk {
    public:
        FillChunkWrapper(std::weak_ptr<ContainerImpl> container, FileIndex fileIndex, unsigned availableSpace = 0);

        FillChunkWrapper(
            std::weak_ptr<ContainerImpl> container,
            FileIndex                    fileIndex,
            std::uint8_t                 commonHeader[Chunk::minimumChunkHeaderSizeBytes]
        );

        ~FillChunkWrapper();

        std::uint8_t* additionalHeader() const;

        unsigned additionalHeaderSizeBytes() const;
};


FillChunkWrapper::FillChunkWrapper(
       std::weak_ptr<ContainerImpl> container,
       FileIndex                    fileIndex,
       unsigned                     availableSpace
   ):FillChunk(
       container,
       fileIndex,
       availableSpace
   ) {}


FillChunkWrapper::FillChunkWrapper(
        std::weak_ptr<ContainerImpl> container,
        FileIndex                    fileIndex,
        std::uint8_t                 commonHeader[Chunk::minimumChunkHeaderSizeBytes]
    ):FillChunk(
        container,
        fileIndex,
        commonHeader
    ) {}


FillChunkWrapper::~FillChunkWrapper() {}


std::uint8_t* FillChunkWrapper::additionalHeader() const {
    return Chunk::additionalHeader();
}


unsigned FillChunkWrapper::additionalHeaderSizeBytes() const {
    return Chunk::additionalHeaderSizeBytes();
}

/***********************************************************************************************************************
 * TestFillChunk
 */

void TestFillChunk::testConstructors() {
    Container::MemoryContainer container("Inesonic, LLC./nAleph");
    FillChunkWrapper chunk1(dynamic_cast<Container::Container&>(container).impl, 0);
    QVERIFY(chunk1.fillSpaceBytes() == 32);

    for (unsigned i=0 ; i<7 ; ++i) {
        unsigned expectedSize = 1 << (i + 5);

        FillChunkWrapper chunk2(dynamic_cast<Container::Container&>(container).impl, 0, expectedSize - 1);
        if (i == 0) {
            QVERIFY(chunk2.fillSpaceBytes() == 32);
        } else {
            QVERIFY(chunk2.fillSpaceBytes() == expectedSize / 2);
        }

        FillChunkWrapper chunk3(dynamic_cast<Container::Container&>(container).impl, 0, expectedSize);
        QVERIFY(chunk3.fillSpaceBytes() == expectedSize);

        FillChunkWrapper chunk4(dynamic_cast<Container::Container&>(container).impl, 0, expectedSize + 1);
        QVERIFY(chunk4.fillSpaceBytes() == expectedSize);
    }

    FillChunkWrapper chunk6(dynamic_cast<Container::Container&>(container).impl, 0, 8191);
    QVERIFY(chunk6.fillSpaceBytes() == 4096);

    FillChunkWrapper chunk7(dynamic_cast<Container::Container&>(container).impl, 0, 8192);
    QVERIFY(chunk7.fillSpaceBytes() == 4096);

    FillChunkWrapper chunk8(dynamic_cast<Container::Container&>(container).impl, 0, 16384);
    QVERIFY(chunk8.fillSpaceBytes() == 4096);
}


void TestFillChunk::testAccessors() {
    Container::MemoryContainer container("Inesonic, LLC./nAleph");
    FillChunkWrapper chunk(dynamic_cast<Container::Container&>(container).impl, 0);
    QVERIFY(chunk.fillSpaceBytes() == 32);

    for (unsigned i=0 ; i<7 ; ++i) {
        unsigned expectedSize = 1 << (i + 5);

        chunk.setBestFitSize(expectedSize - 1);
        if (i == 0) {
            QVERIFY(chunk.fillSpaceBytes() == 32);
        } else {
            QVERIFY(chunk.fillSpaceBytes() == expectedSize / 2);
        }

        chunk.setBestFitSize(expectedSize);
        QVERIFY(chunk.fillSpaceBytes() == expectedSize);

        chunk.setBestFitSize(expectedSize + 1);
        QVERIFY(chunk.fillSpaceBytes() == expectedSize);
    }

    chunk.setBestFitSize(8191);
    QVERIFY(chunk.fillSpaceBytes() == 4096);

    chunk.setBestFitSize(8192);
    QVERIFY(chunk.fillSpaceBytes() == 4096);

    chunk.setBestFitSize(16384);
    QVERIFY(chunk.fillSpaceBytes() == 4096);
}


void TestFillChunk::testSaveLoadMethods() {
    Container::MemoryContainer container("Inesonic, LLC./nAleph");

    Container::Status status = container.open();
    QVERIFY(!status);

    FillChunkWrapper chunk1(dynamic_cast<Container::Container&>(container).impl, 0);

    status = chunk1.save();
    QVERIFY(status.success());

    FillChunkWrapper chunk2(dynamic_cast<Container::Container&>(container).impl, 0);
    status = chunk2.load(true);
    QVERIFY(status.success());

    QVERIFY(chunk1.type() == chunk2.type());
}
