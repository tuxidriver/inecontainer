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
* This file implements the \ref Chunk class.
***********************************************************************************************************************/

#include <cstdint>
#include <memory>
#include <cstring>
#include <cassert>

#include "container_status.h"
#include "container_impl.h"
#include "chunk_header.h"
#include "chunk.h"

std::uint32_t Chunk::randomSeed = 0;

Chunk::Chunk(
        std::weak_ptr<ContainerImpl> container,
        ChunkHeader::FileIndex       fileIndex,
        unsigned                     additionalChunkHeaderSizeBytes
    ):ChunkHeader(
        additionalChunkHeaderSizeBytes
    ) {
    currentContainer  = container;
    currentFileIndex  = fileIndex;
    numberLoadedBytes = 0;
}


Chunk::Chunk(
        std::weak_ptr<ContainerImpl> container,
        ChunkHeader::FileIndex       fileIndex,
        std::uint8_t                 commonHeader[Chunk::minimumChunkHeaderSizeBytes],
        unsigned                     additionalHeaderBytes
    ):ChunkHeader(
        commonHeader,
        additionalHeaderBytes
    ) {
    currentContainer  = container;
    currentFileIndex  = fileIndex;
    numberLoadedBytes = 0;
}


Chunk::Chunk(
        std::weak_ptr<ContainerImpl> container,
        ChunkHeader::FileIndex       fileIndex,
        std::uint8_t                 commonHeader[Chunk::minimumChunkHeaderSizeBytes]
    ):ChunkHeader(
        commonHeader
    ) {
    currentContainer  = container;
    currentFileIndex  = fileIndex;
    numberLoadedBytes = 0;
}


Chunk::~Chunk() {}


void Chunk::setFileIndex(ChunkHeader::FileIndex newFileIndex) {
    currentFileIndex = newFileIndex;
}


ChunkHeader::FileIndex Chunk::fileIndex() const {
    return currentFileIndex;
}


std::weak_ptr<ContainerImpl> Chunk::container() const {
    return currentContainer;
}


Container::Status Chunk::load(bool includeCommonHeader) {
    Container::Status status;

    unsigned long long offset;
    unsigned           bytesToLoad;
    std::uint8_t*      basePointer;

    std::shared_ptr<ContainerImpl> container = currentContainer.lock();
    assert(container);

    if (includeCommonHeader) {
        offset      = toPosition(currentFileIndex);
        bytesToLoad = fullHeaderSizeBytes();
        basePointer = fullHeader();
    } else {
        offset      = toPosition(currentFileIndex) + minimumChunkHeaderSizeBytes;
        bytesToLoad = additionalHeaderSizeBytes();
        basePointer = additionalHeader();
    }

    status = container->setPosition(offset);

    if (!status) {
        status = container->read(basePointer, bytesToLoad);
        if (status.success() && Container::ReadSuccessful(status).bytesRead() == bytesToLoad) {
            status = Container::NoStatus();
        }
    }

    return status;
}


Container::Status Chunk::save(bool padToChunkSize) {
    Container::Status status;

    std::shared_ptr<ContainerImpl> container = currentContainer.lock();
    assert(container);

    status = container->setPosition(toPosition(currentFileIndex));

    if (!status) {
        updateCrc();

        status = container->write(fullHeader(), fullHeaderSizeBytes());
        if (status.success() && Container::WriteSuccessful(status).bytesWritten() == fullHeaderSizeBytes()) {
            status = Container::NoStatus();
        }
    }

    if (!status && padToChunkSize) {
        status = writeTail();
    }

    return status;
}


bool Chunk::checkCrc() const {
    return crc() == initializeCrc();
}


void Chunk::updateCrc() {
    setCrc(initializeCrc());
}


Container::Status Chunk::writeTail(unsigned additionalBytes) {
    Container::Status status;

    std::shared_ptr<ContainerImpl> container = currentContainer.lock();
    assert(container);

    if (additionalBytes == 0) {
        unsigned long long currentPosition = container->position();
        unsigned long long chunkEnd        = ChunkHeader::toPosition(currentFileIndex) + chunkSize();

        assert(currentPosition <= chunkEnd);
        assert(chunkEnd - currentPosition <= static_cast<unsigned>(-1));
        additionalBytes = static_cast<unsigned>(chunkEnd - currentPosition);
    }

    if (additionalBytes > 0) {
        unsigned       writeBuffer32Size = (additionalBytes + 3) / 4;
        std::uint32_t* writeBuffer32     = new std::uint32_t[writeBuffer32Size];

        for (unsigned i=0 ; i<writeBuffer32Size ; ++i) {
            randomSeed = (kla * randomSeed) + klc; // Knuth-Lewis PRNG -- Extremely fast and good enough.
            writeBuffer32[i] = randomSeed;
        }

        std::shared_ptr<ContainerImpl> container = currentContainer.lock();
        assert(container);

        std::uint8_t* writeBuffer = reinterpret_cast<std::uint8_t*>(writeBuffer32);

        status = container->write(writeBuffer, additionalBytes);
        if (status.success() && Container::WriteSuccessful(status).bytesWritten() == additionalBytes) {
            status = Container::NoStatus();
        }

        delete[] writeBuffer32;
    }

    return status;
}
