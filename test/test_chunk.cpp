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
* This file implements tests of the Chunk class.
***********************************************************************************************************************/

#include <QDebug>
#include <QtTest/QtTest>

#include <cstdint>
#include <memory>

#define LIBCONTAINER_TEST // Makes the implementation accessible from the public API.

#include <container_memory_container.h>
#include <container_container_private.h> // temporary
#include <container_impl.h>
#include <chunk.h>

#include "test_chunk.h"

/***********************************************************************************************************************
 * ChunkWrapper
 */

class ChunkWrapper:public Chunk {
    public:
        ChunkWrapper(
            std::weak_ptr<ContainerImpl> container,
            FileIndex                    fileIndex,
            unsigned                     additionalHeaderSizeBytes = 0
        );

        ChunkWrapper(
            std::weak_ptr<ContainerImpl> container,
            FileIndex                    fileIndex,
            std::uint8_t*                rawData,
            unsigned                     rawDataLengthBytes
        );

        ~ChunkWrapper();

        std::uint8_t* additionalHeader() const;

        unsigned additionalHeaderSizeBytes() const;

        void setType(Chunk::Type newType);

        unsigned setNumberValidBytes(unsigned newValidByteCount, bool canChangeChunkSize = false);
};


ChunkWrapper::ChunkWrapper(
        std::weak_ptr<ContainerImpl> container,
        FileIndex                    fileIndex,
        unsigned                     additionalHeaderSizeBytes
    ):Chunk(
        container,
        fileIndex,
        additionalHeaderSizeBytes
    ) {}


ChunkWrapper::ChunkWrapper(
        std::weak_ptr<ContainerImpl> container,
        FileIndex                    fileIndex,
        std::uint8_t*                rawData,
        unsigned                     rawDataLengthBytes
    ):Chunk(
        container,
        fileIndex,
        rawData,
        rawDataLengthBytes
    ) {}


ChunkWrapper::~ChunkWrapper() {}


std::uint8_t* ChunkWrapper::additionalHeader() const {
    return Chunk::additionalHeader();
}


unsigned ChunkWrapper::additionalHeaderSizeBytes() const {
    return Chunk::additionalHeaderSizeBytes();
}


void ChunkWrapper::setType(Chunk::Type newType) {
    Chunk::setType(newType);
}


unsigned ChunkWrapper::setNumberValidBytes(unsigned newValidByteCount, bool canChangeChunkSize) {
    return Chunk::setNumberValidBytes(newValidByteCount, canChangeChunkSize);
}

/***********************************************************************************************************************
 * TestChunk
 */

void TestChunk::testSaveLoadMethods() {
    Container::MemoryContainer container("Inesonic, LLC./nAleph");

    Container::Status status = container.open();
    QVERIFY(!status);

    ChunkWrapper chunk1(dynamic_cast<Container::Container&>(container).impl, 0, 124);

    chunk1.setType(Chunk::Type::STREAM_START_CHUNK);
    chunk1.setNumberValidBytes(124);

    std::uint8_t* payload = chunk1.additionalHeader();
    for (unsigned i=0 ; i<chunk1.additionalHeaderSizeBytes() ; ++i) {
        payload[i] = static_cast<std::uint8_t>(i);
    }

    status = chunk1.save();
    QVERIFY(status.success());

    ChunkWrapper chunk2(dynamic_cast<Container::Container&>(container).impl, 0, 124);
    status = chunk2.load(true); // Include common header during load.
    QVERIFY(status.success());

    QVERIFY(chunk1.type() == chunk2.type());
    QVERIFY(chunk1.numberValidBytes() == chunk2.numberValidBytes());
    QVERIFY(chunk1.additionalHeaderSizeBytes() == chunk2.additionalHeaderSizeBytes());

    for (unsigned i=0 ; i<chunk1.additionalHeaderSizeBytes() ; ++i) {
        QVERIFY(chunk1.additionalHeader()[i] == chunk2.additionalHeader()[i]);
    }
}
