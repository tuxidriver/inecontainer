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
* This file implements the \ref StreamChunk class.
***********************************************************************************************************************/

#include <cstdint>
#include <cstring>
#include <cassert>

#include "container_impl.h"
#include "chunk.h"
#include "stream_chunk.h"

StreamChunk::StreamChunk(
        std::weak_ptr<ContainerImpl> container,
        FileIndex                    fileIndex,
        StreamIdentifier             streamIdentifier,
        unsigned                     additionalChunkHeaderSizeBytes
    ):Chunk(
        container,
        fileIndex,
        numberAdditionalStreamHeaderBytes + additionalChunkHeaderSizeBytes
    ) {
    setLast(false);
    setStreamIdentifier(streamIdentifier);
}


StreamChunk::StreamChunk(
        std::weak_ptr<ContainerImpl> container,
        FileIndex                    fileIndex,
        std::uint8_t                 commonHeader[Chunk::minimumChunkHeaderSizeBytes],
        unsigned                     additionalChunkHeaderSizeBytes
    ):Chunk(
        container,
        fileIndex,
        commonHeader,
        numberAdditionalStreamHeaderBytes + additionalChunkHeaderSizeBytes
    ) {
    std::uint8_t* header = Chunk::additionalHeader();
    std::memset(header, 0, numberAdditionalStreamHeaderBytes);
}


StreamChunk::~StreamChunk() {}


void StreamChunk::setStreamIdentifier(StreamIdentifier newStreamIdentifier) {
    std::uint8_t* header = Chunk::additionalHeader();

    header[0] = static_cast<std::uint8_t>(newStreamIdentifier      );
    header[1] = static_cast<std::uint8_t>(newStreamIdentifier >>  8);
    header[2] = static_cast<std::uint8_t>(newStreamIdentifier >> 16);
    header[3] = (static_cast<std::uint8_t>(newStreamIdentifier >> 24) & 0x7F) | (header[3] & 0x80);
}


StreamChunk::StreamIdentifier StreamChunk::streamIdentifier() const {
    std::uint8_t* header = Chunk::additionalHeader();

    StreamIdentifier identifier = (
          header[0]
        | (static_cast<StreamIdentifier>(header[1]) <<  8)
        | (static_cast<StreamIdentifier>(header[2]) << 16)
        | (static_cast<StreamIdentifier>(header[3]) << 24)
    ) & 0x7FFFFFFFUL;

    return identifier;
}


void StreamChunk::setLast(bool nowLast) {
    std::uint8_t* header = Chunk::additionalHeader();

    if (nowLast) {
        header[3] |= 0x80;
    } else {
        header[3] &= 0x7F;
    }
}


bool StreamChunk::isLast() const {
    std::uint8_t* header = Chunk::additionalHeader();
    return (header[3] & 0x80) != 0;
}


std::uint8_t* StreamChunk::additionalHeader() const {
    return Chunk::additionalHeader() + numberAdditionalStreamHeaderBytes;
}


unsigned StreamChunk::additionalHeaderSizeBytes() const {
    return Chunk::additionalHeaderSizeBytes() - numberAdditionalStreamHeaderBytes;
}
