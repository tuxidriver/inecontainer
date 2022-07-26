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
* This file implements the \ref StreamStartChunk class.
***********************************************************************************************************************/

#include <cstdint>
#include <memory>
#include <cstring>
#include <cassert>

#include "stream_chunk.h"
#include "stream_start_chunk.h"

StreamStartChunk::StreamStartChunk(
        std::weak_ptr<ContainerImpl> container,
        FileIndex                    fileIndex,
        const std::string&           virtualFilename,
        StreamIdentifier             streamIdentifier
    ):StreamChunk(
        container,
        fileIndex,
        streamIdentifier,
        numberAdditionalStreamHeaderBytes
    ) {
    setType(ChunkHeader::Type::STREAM_START_CHUNK);
    setVirtualFilename(virtualFilename);
}


StreamStartChunk::StreamStartChunk(
        std::weak_ptr<ContainerImpl> container,
        FileIndex                    fileIndex,
        std::uint8_t                 commonHeader[Chunk::minimumChunkHeaderSizeBytes]
    ):StreamChunk(
        container,
        fileIndex,
        commonHeader,
        numberAdditionalStreamHeaderBytes
    ) {
    std::uint8_t* header = additionalHeader();
    std::memset(header, 0, numberAdditionalStreamHeaderBytes);
}


StreamStartChunk::~StreamStartChunk() {}


void StreamStartChunk::setVirtualFilename(const std::string& newVirtualFilename) {
    char*         rawFilename = reinterpret_cast<char*>(additionalHeader());
    unsigned      bytesWritten = static_cast<unsigned>(newVirtualFilename.length() + 1);

    std::strncpy(rawFilename, newVirtualFilename.c_str(), maximumVirtualFilenameLength);
    std::memset(rawFilename + bytesWritten, 0, maximumVirtualFilenameLength - bytesWritten);
}


std::string StreamStartChunk::virtualFilename() const {
    const char* rawFilename = reinterpret_cast<const char*>(additionalHeader());

    // Buffer and use strncpy to keep us from every walking off into unallocated memory if the contents of the chunk are
    // somehow invalid (no termination).

    char filenameBuffer[maximumVirtualFilenameLength + 1];
    std::strncpy(filenameBuffer, rawFilename, maximumVirtualFilenameLength);

    return std::string(filenameBuffer);
}
