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
* This file implements the \ref FillChunk class.
***********************************************************************************************************************/

#include <cstdint>
#include <memory>
#include <cstring>
#include <cassert>

#include "fill_chunk.h"

FillChunk::FillChunk(
        std::weak_ptr<ContainerImpl> container,
        FileIndex                    fileIndex,
        unsigned                     availableSpace
    ):Chunk(
        container,
        fileIndex
    ) {
    Chunk::setType(Chunk::Type::FILL_CHUNK);
    Chunk::setBestFitSize(availableSpace);
    Chunk::setAllBytesValid();
}


FillChunk::FillChunk(
        std::weak_ptr<ContainerImpl> container,
        FileIndex                    fileIndex,
        std::uint8_t                 commonHeader[Chunk::minimumChunkHeaderSizeBytes]
    ):Chunk(
        container,
        fileIndex,
        commonHeader,
        0 // Fill chunks require no additional header data.
    ) {}


FillChunk::~FillChunk() {}


unsigned FillChunk::setBestFitSize(unsigned availableSpace) {
    unsigned chunkSize = Chunk::setBestFitSize(availableSpace);
    Chunk::setAllBytesValid();

    return chunkSize;
}


unsigned FillChunk::fillSpaceBytes() const {
    return chunkSize();
}
