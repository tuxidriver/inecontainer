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
* This file implements tests of the ChunkHeader class.
***********************************************************************************************************************/

#include <QDebug>
#include <QtTest/QtTest>

#include <cstdint>
#include <random>

#include <chunk_header.h>

#include "test_chunk_header.h"

/***********************************************************************************************************************
 * ChunkHeaderWrapper
 */

class ChunkHeaderWrapper:public ChunkHeader {
    public:
        ChunkHeaderWrapper(unsigned additionalHeaderSizeBytes = 0);

        ChunkHeaderWrapper(const ChunkHeaderWrapper& other);

        ChunkHeaderWrapper(std::uint8_t* rawData, unsigned rawDataLengthBytes);

        ~ChunkHeaderWrapper();

        const std::uint8_t* fullHeader() const;

        void setType(ChunkHeader::Type newType);

        unsigned setNumberValidBytes(unsigned newValidByteCount, bool canChangeChunkSize = false);

        ChunkHeader::RunningCrc initializeCrc() const;

        static ChunkHeader::RunningCrc calculateCrc(
            ChunkHeader::RunningCrc currentCrc,
            const std::uint8_t*     data,
            unsigned                dataLength
        );

        static unsigned log2(std::uint32_t x);
};


ChunkHeaderWrapper::ChunkHeaderWrapper(unsigned additionalHeaderSizeBytes):ChunkHeader(additionalHeaderSizeBytes) {}


ChunkHeaderWrapper::ChunkHeaderWrapper(const ChunkHeaderWrapper& other):ChunkHeader(other) {}


ChunkHeaderWrapper::ChunkHeaderWrapper(
        std::uint8_t* rawData,
        unsigned      rawDataLengthBytes
    ):ChunkHeader(
        rawData,
        rawDataLengthBytes
    ) {}


ChunkHeaderWrapper::~ChunkHeaderWrapper() {}


const std::uint8_t* ChunkHeaderWrapper::fullHeader() const {
    return ChunkHeader::fullHeader();
}


void ChunkHeaderWrapper::setType(ChunkHeader::Type newType) {
    ChunkHeader::setType(newType);
}


unsigned ChunkHeaderWrapper::setNumberValidBytes(unsigned newValidByteCount, bool canChangeChunkSize) {
    return ChunkHeader::setNumberValidBytes(newValidByteCount, canChangeChunkSize);
}


ChunkHeader::RunningCrc ChunkHeaderWrapper::initializeCrc() const {
    return ChunkHeader::initializeCrc();
}


ChunkHeader::RunningCrc ChunkHeaderWrapper::calculateCrc(
        ChunkHeader::RunningCrc currentCrc,
        const std::uint8_t*     data,
        unsigned                dataLength
    ) {
    return ChunkHeader::calculateCrc(currentCrc, data, dataLength);
}


unsigned ChunkHeaderWrapper::log2(std::uint32_t x) {
    return ChunkHeader::log2(x);
}

/***********************************************************************************************************************
 * TestChunkHeader
 */

void TestChunkHeader::testConstructorsDestructors() {
    ChunkHeaderWrapper chunkHeader1;

    QVERIFY(chunkHeader1.type() == ChunkHeader::Type::FILE_HEADER_CHUNK);
    QVERIFY(chunkHeader1.numberValidBytes() == 0);
    QVERIFY(chunkHeader1.chunkSize() == 32);
    QVERIFY(chunkHeader1.crc() == 0);

    std::uint8_t header[4] = { 0x1F, 0x01, 0x21, 0x43 };
    ChunkHeaderWrapper chunkHeader2(header, 4);

    QVERIFY(chunkHeader2.type() == ChunkHeader::Type::FILL_CHUNK);
    QVERIFY(chunkHeader2.numberValidBytes() == 4084);
    QVERIFY(chunkHeader2.chunkSize() == 4096);
    QVERIFY(chunkHeader2.crc() == 0x4321);

    ChunkHeaderWrapper chunkHeader3(chunkHeader2);

    QVERIFY(chunkHeader3.type() == ChunkHeader::Type::FILL_CHUNK);
    QVERIFY(chunkHeader3.numberValidBytes() == 4084);
    QVERIFY(chunkHeader3.chunkSize() == 4096);
    QVERIFY(chunkHeader3.crc() == 0x4321);
}


void TestChunkHeader::testLog2Computation() {
    for (std::uint32_t x=1 ; x<=0x1FFFF ; ++x) {
        unsigned l2 = ChunkHeaderWrapper::log2(x);

        std::uint32_t mask = 0xFFFFFFFE;
        unsigned      i    = 0;

        while (x & mask) {
            mask <<= 1;
            ++i;
        }

        QVERIFY(i == l2);
    }
}


void TestChunkHeader::testAccessors() {
    ChunkHeaderWrapper chunkHeader;

    chunkHeader.setType(ChunkHeader::Type::FILL_CHUNK);
    QVERIFY(chunkHeader.type() == ChunkHeader::Type::FILL_CHUNK);

    chunkHeader.setType(ChunkHeader::Type::STREAM_START_CHUNK);
    QVERIFY(chunkHeader.type() == ChunkHeader::Type::STREAM_START_CHUNK);

    chunkHeader.setType(ChunkHeader::Type::STREAM_DATA_CHUNK);
    QVERIFY(chunkHeader.type() == ChunkHeader::Type::STREAM_DATA_CHUNK);

    chunkHeader.setType(ChunkHeader::Type::FILE_HEADER_CHUNK);
    QVERIFY(chunkHeader.type() == ChunkHeader::Type::FILE_HEADER_CHUNK);

    for (unsigned i=0 ; i<4092 ; ++i) {
        unsigned availableSpace = chunkHeader.setNumberValidBytes(i, true);

        QVERIFY(availableSpace == i);

        unsigned expectedChunkP2 = ChunkHeaderWrapper::log2(i + 4 - 1) + 1;
        unsigned expectedChunkSize = expectedChunkP2 < 5 ? 32 : 1 << expectedChunkP2;

        QVERIFY(chunkHeader.chunkSize() == expectedChunkSize);
        QVERIFY(chunkHeader.numberValidBytes() == i);
    }

    chunkHeader.setNumberValidBytes(60, true); // Chunk size should now be limited to 64.
    QVERIFY(chunkHeader.chunkSize() == 64);
    QVERIFY(chunkHeader.numberValidBytes() == 60);

    for (unsigned i=128 ; i>0 ; --i) {
        unsigned availableSpace = chunkHeader.setNumberValidBytes(i, false);

        QVERIFY((i > 60 && availableSpace == 60) || (i <= 60 && availableSpace == i));

        QVERIFY(chunkHeader.chunkSize() == ((availableSpace <= 28) ? 32 : 64));
        QVERIFY(chunkHeader.numberValidBytes() == availableSpace);
    }
}


void TestChunkHeader::testStaticMethods() {
    std::mt19937                    rng;
    std::uniform_int_distribution<> positionGenerator;

    unsigned long          pos   = positionGenerator(rng);
    ChunkHeader::FileIndex index = pos / 32;

    QVERIFY(ChunkHeader::toFileIndex(pos) == index);
    QVERIFY(ChunkHeader::toPosition(index) == (pos - pos % 32));

    QVERIFY(ChunkHeader::toChunkSize(0) ==   32);
    QVERIFY(ChunkHeader::toChunkSize(1) ==   64);
    QVERIFY(ChunkHeader::toChunkSize(2) ==  128);
    QVERIFY(ChunkHeader::toChunkSize(3) ==  256);
    QVERIFY(ChunkHeader::toChunkSize(4) ==  512);
    QVERIFY(ChunkHeader::toChunkSize(5) == 1024);
    QVERIFY(ChunkHeader::toChunkSize(6) == 2048);
    QVERIFY(ChunkHeader::toChunkSize(7) == 4096);

    QVERIFY(ChunkHeader::toClosestSmallerChunkP2(4096) == 7);
    QVERIFY(ChunkHeader::toClosestSmallerChunkP2(4095) == 6);
    QVERIFY(ChunkHeader::toClosestSmallerChunkP2(2049) == 6);
    QVERIFY(ChunkHeader::toClosestSmallerChunkP2(2048) == 6);
    QVERIFY(ChunkHeader::toClosestSmallerChunkP2(2047) == 5);
    QVERIFY(ChunkHeader::toClosestSmallerChunkP2(1025) == 5);
    QVERIFY(ChunkHeader::toClosestSmallerChunkP2(1024) == 5);
    QVERIFY(ChunkHeader::toClosestSmallerChunkP2(1023) == 4);
    QVERIFY(ChunkHeader::toClosestSmallerChunkP2( 514) == 4);
    QVERIFY(ChunkHeader::toClosestSmallerChunkP2( 512) == 4);
    QVERIFY(ChunkHeader::toClosestSmallerChunkP2( 511) == 3);
    QVERIFY(ChunkHeader::toClosestSmallerChunkP2( 257) == 3);
    QVERIFY(ChunkHeader::toClosestSmallerChunkP2( 256) == 3);
    QVERIFY(ChunkHeader::toClosestSmallerChunkP2( 255) == 2);
    QVERIFY(ChunkHeader::toClosestSmallerChunkP2( 129) == 2);
    QVERIFY(ChunkHeader::toClosestSmallerChunkP2( 128) == 2);
    QVERIFY(ChunkHeader::toClosestSmallerChunkP2( 127) == 1);
    QVERIFY(ChunkHeader::toClosestSmallerChunkP2(  65) == 1);
    QVERIFY(ChunkHeader::toClosestSmallerChunkP2(  64) == 1);
    QVERIFY(ChunkHeader::toClosestSmallerChunkP2(  63) == 0);
    QVERIFY(ChunkHeader::toClosestSmallerChunkP2(  33) == 0);
    QVERIFY(ChunkHeader::toClosestSmallerChunkP2(  32) == 0);
    QVERIFY(ChunkHeader::toClosestSmallerChunkP2(  31) == 0); // Technically illegal.

    QVERIFY(ChunkHeader::toClosestLargerChunkP2(4096) == 7);
    QVERIFY(ChunkHeader::toClosestLargerChunkP2(4095) == 7);
    QVERIFY(ChunkHeader::toClosestLargerChunkP2(2049) == 7);
    QVERIFY(ChunkHeader::toClosestLargerChunkP2(2048) == 6);
    QVERIFY(ChunkHeader::toClosestLargerChunkP2(2047) == 6);
    QVERIFY(ChunkHeader::toClosestLargerChunkP2(1025) == 6);
    QVERIFY(ChunkHeader::toClosestLargerChunkP2(1024) == 5);
    QVERIFY(ChunkHeader::toClosestLargerChunkP2(1023) == 5);
    QVERIFY(ChunkHeader::toClosestLargerChunkP2( 514) == 5);
    QVERIFY(ChunkHeader::toClosestLargerChunkP2( 512) == 4);
    QVERIFY(ChunkHeader::toClosestLargerChunkP2( 511) == 4);
    QVERIFY(ChunkHeader::toClosestLargerChunkP2( 257) == 4);
    QVERIFY(ChunkHeader::toClosestLargerChunkP2( 256) == 3);
    QVERIFY(ChunkHeader::toClosestLargerChunkP2( 255) == 3);
    QVERIFY(ChunkHeader::toClosestLargerChunkP2( 129) == 3);
    QVERIFY(ChunkHeader::toClosestLargerChunkP2( 128) == 2);
    QVERIFY(ChunkHeader::toClosestLargerChunkP2( 127) == 2);
    QVERIFY(ChunkHeader::toClosestLargerChunkP2(  65) == 2);
    QVERIFY(ChunkHeader::toClosestLargerChunkP2(  64) == 1);
    QVERIFY(ChunkHeader::toClosestLargerChunkP2(  63) == 1);
    QVERIFY(ChunkHeader::toClosestLargerChunkP2(  33) == 1);
    QVERIFY(ChunkHeader::toClosestLargerChunkP2(  32) == 0);
    QVERIFY(ChunkHeader::toClosestLargerChunkP2(  31) == 0); // Technically illegal.
}


void TestChunkHeader::testCrcCalculation() {
    // TODO: Should probably use a PRNG and display the seed value in case we ever have to debug some low probability
    //       of occurrance error.

    ChunkHeaderWrapper chunkHeader;

    chunkHeader.setType(ChunkHeader::Type::STREAM_START_CHUNK);
    unsigned availableBytes = chunkHeader.setNumberValidBytes(crcPayloadSize, true);

    QVERIFY(availableBytes == crcPayloadSize);

    std::uint8_t checkData[crcPayloadSize];
    unsigned     i;
    std::mt19937                    rng;
    std::uniform_int_distribution<> byteGenerator(0, 255);
    for (i=0 ; i<crcPayloadSize ; ++i) {
        checkData[i] = static_cast<std::uint8_t>(byteGenerator(rng));
    }

    // Calculate expected CRC value one bit at a time.
    const std::uint8_t* header   = chunkHeader.fullHeader();
    std::uint32_t       lfsr     = (static_cast<std::uint32_t>(header[1]) << 8) | header[0];
    std::uint32_t       lfsrMask = 1 << polynomialOrder;

    for (i=0 ; i<crcPayloadSize ; ++i) {
        std::uint8_t m          = checkData[i];
        std::uint8_t insertMask = 0x80;

        while (insertMask != 0) {
            if (m & insertMask) {
                lfsr = (lfsr << 1) | 1;
            } else {
                lfsr = lfsr << 1;
            }

            if (lfsr & lfsrMask) {
                lfsr ^= crcPolynomial;
            }

            insertMask >>= 1;
        }
    }

    // Now let the chunk header do the computation.
    ChunkHeader::RunningCrc currentCrc = chunkHeader.initializeCrc();
    currentCrc = ChunkHeaderWrapper::calculateCrc(currentCrc, checkData, crcPayloadSize);

    QVERIFY(currentCrc == static_cast<std::uint16_t>(lfsr));

    // Now repeatedly and randomly munge 15 bits and confirm that we detect the error in all cases.
    std::uniform_int_distribution<> indexGenerator(0, crcPayloadSize - 3);
    for (i=0 ; i<numberCrcMungeIterations ; ++i) {
        std::uint32_t mask = ((1 << polynomialOrder) - 1) >> 1;
        unsigned offset = byteGenerator(rng) % 9;
        mask <<= offset;

        unsigned index = indexGenerator(rng);

        std::uint8_t b0 = static_cast<std::uint8_t>(mask      );
        std::uint8_t b1 = static_cast<std::uint8_t>(mask >>  8);
        std::uint8_t b2 = static_cast<std::uint8_t>(mask >> 16);

        checkData[index+0] ^= b0;
        checkData[index+1] ^= b1;
        checkData[index+2] ^= b2;

        ChunkHeader::RunningCrc currentCrc = chunkHeader.initializeCrc();
        currentCrc = ChunkHeaderWrapper::calculateCrc(currentCrc, checkData, crcPayloadSize);
        QVERIFY(currentCrc != chunkHeader.crc());

        checkData[index+0] ^= b0;
        checkData[index+1] ^= b1;
        checkData[index+2] ^= b2;
    }
}
