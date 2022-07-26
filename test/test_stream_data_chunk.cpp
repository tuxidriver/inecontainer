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
* This file implements tests of the StreamDataChunk class.
***********************************************************************************************************************/

#include <QDebug>
#include <QtTest/QtTest>

#include <random>

#include <cstdint>
#include <cstdlib>
#include <cstring>

#define LIBCONTAINER_TEST // Makes the implementation accessible from the public API.

#include <container_status.h>
#include <container_container.h>
#include <container_memory_container.h>
#include <container_container_private.h> // temporary
#include <container_impl.h>
#include <stream_data_chunk.h>

#include "test_stream_data_chunk.h"

/***********************************************************************************************************************
 * TestStreamDataChunk
 */

void TestStreamDataChunk::testAccessors() {
    Container::MemoryContainer container("Inesonic, LLC.\nAleph");
    StreamDataChunk chunk(dynamic_cast<Container::Container&>(container).impl, 0, 0, 1024);

    QVERIFY(chunk.streamIdentifier() == 0);
    QVERIFY(chunk.chunkOffset() == 1024);
    QVERIFY(chunk.isLast() == false);

    chunk.setStreamIdentifier(1);
    chunk.setChunkOffset(2048);
    chunk.setLast(true);

    QVERIFY(chunk.streamIdentifier() == 1);
    QVERIFY(chunk.chunkOffset() == 2048);
    QVERIFY(chunk.isLast() == true);
}


void TestStreamDataChunk::testChunkSizeMethod() {
    Container::MemoryContainer container("Inesonic, LLC.\nAleph");

    std::uint8_t* writeBuffer = new std::uint8_t[Chunk::maximumChunkSize];
    for (unsigned i=0 ; i<Chunk::maximumChunkSize ; ++i) {
        writeBuffer[i] = static_cast<std::uint8_t>(255 - (i%255));
    }

    for (unsigned chunkP2=0 ; chunkP2<=7 ; ++chunkP2) {
        StreamDataChunk chunk(dynamic_cast<Container::Container&>(container).impl, 0, 0, 1024);

        unsigned requestedSize       = ChunkHeader::toChunkSize(chunkP2);
        unsigned expectedPayloadSize = requestedSize - 14;

        chunk.setChunkSize(requestedSize);
        chunk.clearScatterGatherList();

        ScatterGatherListSegment segment(writeBuffer, Chunk::maximumChunkSize);
        unsigned actualPayloadSize = chunk.addScatterGatherListSegment(segment);

        QVERIFY(actualPayloadSize == expectedPayloadSize);
    }

    delete [] writeBuffer;
}


void TestStreamDataChunk::testScatterGatherListMethods() {
    std::uint8_t* buffer = new std::uint8_t[Chunk::maximumChunkSize];

    for (unsigned i=0 ; i<Chunk::maximumChunkSize ; ++i) {
        buffer[i] = static_cast<std::uint8_t>(255 - (i%255));
    }

    Container::MemoryContainer container("Inesonic, LLC.\nAleph");
    StreamDataChunk chunk(dynamic_cast<Container::Container&>(container).impl, 0, 0, 1024);

    unsigned actualChunkSize = chunk.setChunkSize(2049);
    QVERIFY(actualChunkSize == 2048);

    ScatterGatherListSegment segment1(buffer +    0,   32);
    ScatterGatherListSegment segment2(buffer +   32,   32);
    ScatterGatherListSegment segment3(buffer +   64,   64);
    ScatterGatherListSegment segment4(buffer +  128,  128);
    ScatterGatherListSegment segment5(buffer +  256,  256);
    ScatterGatherListSegment segment6(buffer +  512,  512);
    ScatterGatherListSegment segment7(buffer + 1024, 2048); // We should only get a bit less than 1/2 this buffer.
    ScatterGatherListSegment segment8(buffer + 1024, 3072); // We should get nothing from this buffer.

    unsigned bytesSegment1 = chunk.addScatterGatherListSegment(segment1);
    unsigned bytesSegment2 = chunk.addScatterGatherListSegment(segment2);
    unsigned bytesSegment3 = chunk.addScatterGatherListSegment(segment3);
    unsigned bytesSegment4 = chunk.addScatterGatherListSegment(segment4);
    unsigned bytesSegment5 = chunk.addScatterGatherListSegment(segment5);
    unsigned bytesSegment6 = chunk.addScatterGatherListSegment(segment6);
    unsigned bytesSegment7 = chunk.addScatterGatherListSegment(segment7);
    unsigned bytesSegment8 = chunk.addScatterGatherListSegment(segment8);

    QVERIFY(bytesSegment1 ==   32); // 2002 bytes remaining
    QVERIFY(bytesSegment2 ==   32); // 1970 bytes remaining
    QVERIFY(bytesSegment3 ==   64); // 1906 bytes remaining
    QVERIFY(bytesSegment4 ==  128); // 1778 bytes remaining
    QVERIFY(bytesSegment5 ==  256); // 1522 bytes remaining
    QVERIFY(bytesSegment6 ==  512); // 1010 bytes remaining
    QVERIFY(bytesSegment7 == 1010); //    0 bytes remaining
    QVERIFY(bytesSegment8 ==    0);

    QVERIFY(chunk.scatterGatherListSize() == 8);

    QVERIFY(chunk.scatterGatherListSegment(0) == segment1);
    QVERIFY(chunk.scatterGatherListSegment(1) == segment2);
    QVERIFY(chunk.scatterGatherListSegment(2) == segment3);
    QVERIFY(chunk.scatterGatherListSegment(3) == segment4);
    QVERIFY(chunk.scatterGatherListSegment(4) == segment5);
    QVERIFY(chunk.scatterGatherListSegment(5) == segment6);
    QVERIFY(chunk.scatterGatherListSegment(6) == segment7);
    QVERIFY(chunk.scatterGatherListSegment(7) == segment8);

    chunk.clearScatterGatherList();
    QVERIFY(chunk.scatterGatherListSize() == 0);

    bytesSegment8 = chunk.addScatterGatherListSegment(segment8);
    QVERIFY(bytesSegment8 == 2048 - 14);

    delete[] buffer;
}


void TestStreamDataChunk::testCrcCalculationMethods() {
    // Rather than a full, exhaustive comparison test, we simply create a chunk, randomly inject errors and verify that
    // the errors are detected.

    // TODO: Should probably use a PRNG and display the seed value in case we ever have to debug some low probability
    //       of occurrance error.

    unsigned bufferSize = 2048;
    std::uint8_t* buffer = new std::uint8_t[bufferSize];

    for (unsigned i=0 ; i<bufferSize ; ++i) {
        buffer[i] = static_cast<std::uint8_t>(255 - (i%255));
    }

    Container::MemoryContainer container("Inesonic, LLC.\nAleph");
    Container::Status status = container.open();
    QVERIFY(!status);

    StreamDataChunk chunk(dynamic_cast<Container::Container&>(container).impl, 0, 0, 0);

    unsigned actualChunkSize = chunk.setChunkSize(2048);
    QVERIFY(actualChunkSize == 2048);

    ScatterGatherListSegment segment1(buffer, bufferSize);
    chunk.addScatterGatherListSegment(segment1);

    status = chunk.save();
    QVERIFY(status.success());

    segment1 = chunk.scatterGatherListSegment(0);
    unsigned processedCount = segment1.processedCount();
    QVERIFY(processedCount == 2048 - 14);

    // At this point we have a chunk with a CRC calculated on it.  Repeatedly load the chunk and munge a small number of
    // bits to see if we catch the CRC error.

    std::shared_ptr<std::vector<std::uint8_t>> containerBuffer = container.buffer();

    std::mt19937                    rng;
    std::uniform_int_distribution<> byteGenerator(0, 255);
    std::uniform_int_distribution<> indexGenerator(
        0,
        static_cast<unsigned>(containerBuffer->size() - 3 - 14)
    );

    bool firstPass = true; // We verify that the CRC matches on the first pass.
    for (unsigned i=0 ; i<numberCrcMungeIterations ; ++i) {
        std::uint32_t mask = ((1 << polynomialOrder) - 1) >> 1;
        unsigned offset = byteGenerator(rng) % 9;
        mask <<= offset;

        unsigned index = indexGenerator(rng) + 14;

        std::uint8_t b0 = static_cast<std::uint8_t>(mask      );
        std::uint8_t b1 = static_cast<std::uint8_t>(mask >>  8);
        std::uint8_t b2 = static_cast<std::uint8_t>(mask >> 16);

        if (!firstPass) {
            containerBuffer->at(index + 0) ^= b0;
            containerBuffer->at(index + 1) ^= b1;
            containerBuffer->at(index + 2) ^= b2;
        }

        std::memset(buffer, 0, bufferSize);

        StreamDataChunk chunk(dynamic_cast<Container::Container&>(container).impl, 0, 0, 0);

        ScatterGatherListSegment segment(buffer, bufferSize);
        chunk.addScatterGatherListSegment(segment);

        status = chunk.load(true);
        QVERIFY(chunk.type() == ChunkHeader::Type::STREAM_DATA_CHUNK);
        QVERIFY(chunk.chunkSize() == 2048);

        segment = chunk.scatterGatherListSegment(0);
        QVERIFY(segment.processedCount() == 2048 - 14);

        QVERIFY(chunk.checkCrc() == firstPass);

        if (!firstPass) {
            containerBuffer->at(index + 0) ^= b0;
            containerBuffer->at(index + 1) ^= b1;
            containerBuffer->at(index + 2) ^= b2;
        }

        firstPass = false;
    }
}


void TestStreamDataChunk::testSaveLoadMethods() {
    // Tested by the TestStreamDataChunk::testCrcCalculationMethods method.
}
