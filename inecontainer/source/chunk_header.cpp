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
* This file implements the \ref ChunkHeader class.
***********************************************************************************************************************/

#include <cstdint>
#include <cstring>
#include <cassert>

#include "chunk_header.h"

const unsigned char ChunkHeader::mulDeBruijnBitTable[32] = {
//  0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15
    0,  9,  1, 10, 13, 21,  2, 29, 11, 14, 16, 18, 22, 25,  3, 30, //  +0
    8, 12, 20, 28, 15, 17, 24,  7, 19, 27, 23,  6, 26,  5,  4, 31  // +16
};

const std::uint16_t ChunkHeader::crcTable[256] = {
//    +0      +1      +2      +3      +4      +5      +6      +7
//  ------  ------  ------  ------  ------  ------  ------  ------
    0x0000, 0x8005, 0x800F, 0x000A, 0x801B, 0x001E, 0x0014, 0x8011, //   +0
    0x8033, 0x0036, 0x003C, 0x8039, 0x0028, 0x802D, 0x8027, 0x0022, //   +8
    0x8063, 0x0066, 0x006C, 0x8069, 0x0078, 0x807D, 0x8077, 0x0072, //  +16
    0x0050, 0x8055, 0x805F, 0x005A, 0x804B, 0x004E, 0x0044, 0x8041, //  +24
    0x80C3, 0x00C6, 0x00CC, 0x80C9, 0x00D8, 0x80DD, 0x80D7, 0x00D2, //  +32
    0x00F0, 0x80F5, 0x80FF, 0x00FA, 0x80EB, 0x00EE, 0x00E4, 0x80E1, //  +40
    0x00A0, 0x80A5, 0x80AF, 0x00AA, 0x80BB, 0x00BE, 0x00B4, 0x80B1, //  +48
    0x8093, 0x0096, 0x009C, 0x8099, 0x0088, 0x808D, 0x8087, 0x0082, //  +56
    0x8183, 0x0186, 0x018C, 0x8189, 0x0198, 0x819D, 0x8197, 0x0192, //  +64
    0x01B0, 0x81B5, 0x81BF, 0x01BA, 0x81AB, 0x01AE, 0x01A4, 0x81A1, //  +72
    0x01E0, 0x81E5, 0x81EF, 0x01EA, 0x81FB, 0x01FE, 0x01F4, 0x81F1, //  +80
    0x81D3, 0x01D6, 0x01DC, 0x81D9, 0x01C8, 0x81CD, 0x81C7, 0x01C2, //  +88
    0x0140, 0x8145, 0x814F, 0x014A, 0x815B, 0x015E, 0x0154, 0x8151, //  +96
    0x8173, 0x0176, 0x017C, 0x8179, 0x0168, 0x816D, 0x8167, 0x0162, // +104
    0x8123, 0x0126, 0x012C, 0x8129, 0x0138, 0x813D, 0x8137, 0x0132, // +112
    0x0110, 0x8115, 0x811F, 0x011A, 0x810B, 0x010E, 0x0104, 0x8101, // +120
    0x8303, 0x0306, 0x030C, 0x8309, 0x0318, 0x831D, 0x8317, 0x0312, // +128
    0x0330, 0x8335, 0x833F, 0x033A, 0x832B, 0x032E, 0x0324, 0x8321, // +136
    0x0360, 0x8365, 0x836F, 0x036A, 0x837B, 0x037E, 0x0374, 0x8371, // +144
    0x8353, 0x0356, 0x035C, 0x8359, 0x0348, 0x834D, 0x8347, 0x0342, // +152
    0x03C0, 0x83C5, 0x83CF, 0x03CA, 0x83DB, 0x03DE, 0x03D4, 0x83D1, // +160
    0x83F3, 0x03F6, 0x03FC, 0x83F9, 0x03E8, 0x83ED, 0x83E7, 0x03E2, // +168
    0x83A3, 0x03A6, 0x03AC, 0x83A9, 0x03B8, 0x83BD, 0x83B7, 0x03B2, // +176
    0x0390, 0x8395, 0x839F, 0x039A, 0x838B, 0x038E, 0x0384, 0x8381, // +184
    0x0280, 0x8285, 0x828F, 0x028A, 0x829B, 0x029E, 0x0294, 0x8291, // +192
    0x82B3, 0x02B6, 0x02BC, 0x82B9, 0x02A8, 0x82AD, 0x82A7, 0x02A2, // +200
    0x82E3, 0x02E6, 0x02EC, 0x82E9, 0x02F8, 0x82FD, 0x82F7, 0x02F2, // +208
    0x02D0, 0x82D5, 0x82DF, 0x02DA, 0x82CB, 0x02CE, 0x02C4, 0x82C1, // +216
    0x8243, 0x0246, 0x024C, 0x8249, 0x0258, 0x825D, 0x8257, 0x0252, // +224
    0x0270, 0x8275, 0x827F, 0x027A, 0x826B, 0x026E, 0x0264, 0x8261, // +232
    0x0220, 0x8225, 0x822F, 0x022A, 0x823B, 0x023E, 0x0234, 0x8231, // +240
    0x8213, 0x0216, 0x021C, 0x8219, 0x0208, 0x820D, 0x8207, 0x0202  // +248
};


ChunkHeader::ChunkHeader(unsigned additionalChunkHeaderSizeBytes) {
    headerSize = minimumChunkHeaderSizeBytes + additionalChunkHeaderSizeBytes;
    header = new std::uint8_t[headerSize];

    header[0] = 0x80;
    header[1] = 0x03;
    header[2] = 0x00;
    header[3] = 0x00;

    setNumberValidBytes(additionalChunkHeaderSizeBytes, true);
}


ChunkHeader::ChunkHeader(
        std::uint8_t commonHeader[ChunkHeader::minimumChunkHeaderSizeBytes],
        unsigned     additionalHeaderBytes
    ) {
    headerSize = additionalHeaderBytes + minimumChunkHeaderSizeBytes;
    header = new std::uint8_t[headerSize];
    memcpy(header, commonHeader, minimumChunkHeaderSizeBytes);
}


ChunkHeader::ChunkHeader(std::uint8_t commonHeader[ChunkHeader::minimumChunkHeaderSizeBytes]) {
    unsigned sp2                = (commonHeader[0] >> 2) & 0x07;
    unsigned numberInvalidBytes = (static_cast<unsigned>(commonHeader[1]) << 3) | ((commonHeader[0] >> 5) & 0x07);
    unsigned chunkSize          = 1 << (sp2 + 5);

    headerSize = chunkSize - numberInvalidBytes;
    header = new std::uint8_t[headerSize];

    memcpy(header, commonHeader, minimumChunkHeaderSizeBytes);
}


ChunkHeader::ChunkHeader(const ChunkHeader& other) {
    headerSize = other.headerSize;
    header = new std::uint8_t[headerSize];
    memcpy(header, other.header, headerSize);
}


ChunkHeader::~ChunkHeader() {
    delete[] header;
}


ChunkHeader::Type ChunkHeader::type() const {
    return static_cast<ChunkHeader::Type>(header[0] & 0x03);
}


unsigned ChunkHeader::numberValidBytes() const {
    unsigned numberInvalidBytes = (static_cast<unsigned>(header[1]) << 3) | ((header[0] >> 5) & 0x07);
    return chunkSize() - numberInvalidBytes - minimumChunkHeaderSizeBytes;
}


unsigned ChunkHeader::chunkSize() const {
    unsigned sp2 = (header[0] >> 2) & 0x07;
    return 1 << (sp2 + 5);
}


void ChunkHeader::setCrc(RunningCrc newCrcValue) {
    header[2] = static_cast<std::uint8_t>(newCrcValue     );
    header[3] = static_cast<std::uint8_t>(newCrcValue >> 8);
}


ChunkHeader::RunningCrc ChunkHeader::crc() const {
    return (static_cast<std::uint16_t>(header[3]) << 8) | header[2];
}


unsigned long long ChunkHeader::toPosition(ChunkHeader::FileIndex index) {
    return 32*index;
}


ChunkHeader::FileIndex ChunkHeader::toFileIndex(unsigned long long position) {
    return static_cast<ChunkHeader::FileIndex>(position / 32);
}


unsigned ChunkHeader::toChunkSize(ChunkHeader::ChunkP2 chunkP2) {
    assert(chunkP2 <= 7);
    return 1 << (chunkP2 + 5);
}


ChunkHeader::ChunkP2 ChunkHeader::toClosestSmallerChunkP2(unsigned long spaceMaximum) {
    ChunkP2 chunkP2 = log2(spaceMaximum);

    if (chunkP2 <= 5) {
        chunkP2 = 0;
    } else {
        chunkP2 -= 5;
    }

    return chunkP2;
}


ChunkHeader::ChunkP2 ChunkHeader::toClosestLargerChunkP2(unsigned long spaceMinimum) {
    ChunkP2 chunkP2 = log2(spaceMinimum-1) + 1;

    if (chunkP2 <= 5) {
        chunkP2 = 0;
    } else {
        chunkP2 -= 5;
    }

    return chunkP2;
}


uint8_t* ChunkHeader::fullHeader() const {
    return header;
}


unsigned ChunkHeader::fullHeaderSizeBytes() const {
    return headerSize;
}


std::uint8_t* ChunkHeader::additionalHeader() const {
    return header + minimumChunkHeaderSizeBytes;
}


unsigned ChunkHeader::additionalHeaderSizeBytes() const {
    return headerSize - minimumChunkHeaderSizeBytes;
}


unsigned ChunkHeader::additionalAvailableSpace() const {
    return chunkSize() - headerSize;
}


void ChunkHeader::setType(Type newType) {
    header[0] = (header[0] & 0xFC) | (static_cast<std::uint8_t>(newType) & 0x03);
}


unsigned ChunkHeader::setNumberValidBytes(
        unsigned newValidByteCount,
        bool     canGrowChunkSize,
        bool*    chunkSizeChanged
    ) {
    unsigned maximumPayloadSize;
    unsigned currentChunkSize = chunkSize();

    if (canGrowChunkSize) {
        maximumPayloadSize = maximumChunkSize - minimumChunkHeaderSizeBytes;
    } else {
        maximumPayloadSize = currentChunkSize - minimumChunkHeaderSizeBytes;
    }

    if (newValidByteCount > maximumPayloadSize) {
        newValidByteCount = maximumPayloadSize;
    }

    unsigned requiredBits = log2(newValidByteCount + minimumChunkHeaderSizeBytes - 1) + 1;

    if (requiredBits < 5) {
        requiredBits = 5;
    }

    assert(requiredBits <= 5 + 7);

    unsigned newChunkSize = 1 << requiredBits;

    assert(newChunkSize >= newValidByteCount + minimumChunkHeaderSizeBytes);

    unsigned numberInvalidBytes = newChunkSize - newValidByteCount - minimumChunkHeaderSizeBytes;

    unsigned      typeCode = header[0] & 0x03;
    std::uint16_t hdr      = typeCode | ((requiredBits - 5) << 2)  | (numberInvalidBytes << 5);

    header[0] = static_cast<std::uint8_t>(hdr);
    header[1] = static_cast<std::uint8_t>(hdr >> 8);

    if (chunkSizeChanged != nullptr) {
        *chunkSizeChanged = (newChunkSize != currentChunkSize);
    }

    return newValidByteCount;
}


unsigned ChunkHeader::setBestFitSize(unsigned availableSpace) {
    unsigned bestFitSize;
    unsigned chunkP2;
    if (availableSpace < minimumChunkSize) {
        bestFitSize = 0;
        chunkP2     = 0;
    } else {
        unsigned requiredBits = log2(availableSpace);
        if (requiredBits > 7 + 5) {
            requiredBits = 7 + 5;
        }

        bestFitSize = 1 << requiredBits;
        chunkP2     = requiredBits - 5;
    }

    header[0] = (header[0] & 0xE3) | (chunkP2 << 2);

    return bestFitSize;
}


void ChunkHeader::setAllBytesValid() {
    header[0] = header[0] & 0x1F;
    header[1] = 0;
}


ChunkHeader::RunningCrc ChunkHeader::initializeCrc() const {
    RunningCrc currentCrc = (static_cast<RunningCrc>(header[1]) << 8) | header[0];
    return calculateCrc(currentCrc, additionalHeader(), additionalHeaderSizeBytes());
}


ChunkHeader::RunningCrc ChunkHeader::calculateCrc(
        ChunkHeader::RunningCrc currentCrc,
        const std::uint8_t*     data,
        unsigned                dataLength
    ) {
    const std::uint8_t* endingByte = data + dataLength;
    while (data != endingByte) {
        RunningCrc xorValue = crcTable[currentCrc >> 8];
        currentCrc = ((currentCrc << 8) | *data) ^ xorValue;

        ++data;
    }

    return currentCrc;
}


unsigned ChunkHeader::log2(std::uint32_t x) {
    // Find MSB via De Bruijn sequences -- Approach used because it's fast and machine/platform independent.

    x |= x >>  1;
    x |= x >>  2;
    x |= x >>  4;
    x |= x >>  8;
    x |= x >> 16;

    std::uint32_t mul = x * 0x07C4ACDDUL;
    return mulDeBruijnBitTable[mul >> 27];
}
