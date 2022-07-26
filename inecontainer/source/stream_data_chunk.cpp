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
* This file implements the \ref StreamDataChunk class.
***********************************************************************************************************************/

#include <cstdint>
#include <memory>
#include <cstring>
#include <cassert>

#include "container_status.h"
#include "container_impl.h"
#include "scatter_gather_list_segment.h"
#include "stream_chunk.h"
#include "stream_data_chunk.h"

StreamDataChunk::StreamDataChunk(
        std::weak_ptr<ContainerImpl> container,
        FileIndex                    fileIndex,
        StreamIdentifier             streamIdentifier,
        unsigned long long           chunkOffset
    ):StreamChunk(
        container,
        fileIndex,
        streamIdentifier,
        numberAdditionalStreamHeaderBytes
    ) {
    setType(ChunkHeader::Type::STREAM_DATA_CHUNK);
    setChunkOffset(chunkOffset);
    clearScatterGatherList();
}


StreamDataChunk::StreamDataChunk(
        std::weak_ptr<ContainerImpl> container,
        FileIndex                    fileIndex,
        std::uint8_t                 commonHeader[Chunk::minimumChunkHeaderSizeBytes]
    ):StreamChunk(
        container,
        fileIndex,
        commonHeader,
        numberAdditionalStreamHeaderBytes
    ) {
    clearScatterGatherList();
}


StreamDataChunk::~StreamDataChunk() {}


unsigned StreamDataChunk::setChunkSize(unsigned newChunkSize) {
    return setBestFitSize(newChunkSize);
}


void StreamDataChunk::setChunkOffset(StreamDataChunk::ChunkOffset newChunkOffset) {
    std::uint8_t* header = StreamChunk::additionalHeader();

    header[0] = static_cast<std::uint8_t>(newChunkOffset      );
    header[1] = static_cast<std::uint8_t>(newChunkOffset >>  8);
    header[2] = static_cast<std::uint8_t>(newChunkOffset >> 16);
    header[3] = static_cast<std::uint8_t>(newChunkOffset >> 24);
    header[4] = static_cast<std::uint8_t>(newChunkOffset >> 32);
    header[5] = static_cast<std::uint8_t>(newChunkOffset >> 40);
}


unsigned long long StreamDataChunk::chunkOffset() const {
    std::uint8_t* header = StreamChunk::additionalHeader();

    unsigned long long offset = (
           static_cast<unsigned long long>(header[0])
        | (static_cast<unsigned long long>(header[1]) <<  8)
        | (static_cast<unsigned long long>(header[2]) << 16)
        | (static_cast<unsigned long long>(header[3]) << 24)
        | (static_cast<unsigned long long>(header[4]) << 32)
        | (static_cast<unsigned long long>(header[5]) << 40)
    );

    return offset;
}


unsigned StreamDataChunk::payloadSize() const {
    return numberValidBytes() - ChunkHeader::additionalHeaderSizeBytes();
}


void StreamDataChunk::clearScatterGatherList() {
    scatterGatherList.clear();
    currentScatterGatherListByteCount = 0;
}


unsigned StreamDataChunk::addScatterGatherListSegment(const ScatterGatherListSegment& newSegment) {
    scatterGatherList.push_back(newSegment);

    unsigned availableSpace         = additionalAvailableSpace();
    unsigned bytesRemainingInChunk  =   availableSpace > currentScatterGatherListByteCount
                                      ? availableSpace - currentScatterGatherListByteCount
                                      : 0;
    unsigned segmentLength          = newSegment.length();
    unsigned expectedBytesProcessed = segmentLength < bytesRemainingInChunk ? segmentLength : bytesRemainingInChunk;

    currentScatterGatherListByteCount += segmentLength;

    return expectedBytesProcessed;
}


unsigned StreamDataChunk::addScatterGatherListSegment(uint8_t* buffer, unsigned bufferLength) {
    return addScatterGatherListSegment(ScatterGatherListSegment(buffer, bufferLength));
}


unsigned StreamDataChunk::scatterGatherListSize() const {
    return static_cast<unsigned>(scatterGatherList.size());
}


ScatterGatherListSegment StreamDataChunk::scatterGatherListSegment(unsigned index) const {
    ScatterGatherListSegment segment;

    if (index < scatterGatherList.size()) {
        segment = scatterGatherList.at(index);
    }

    return segment;
}


Container::Status StreamDataChunk::loadHeader(bool includeCommonHeader) {
    return StreamChunk::load(includeCommonHeader);
}


Container::Status StreamDataChunk::load(bool includeCommonHeader) {
    Container::Status status = loadHeader(includeCommonHeader);

    unsigned payloadBytesRemaining = payloadSize();
    std::vector<ScatterGatherListSegment>::iterator it  = scatterGatherList.begin();
    std::vector<ScatterGatherListSegment>::iterator end = scatterGatherList.end();

    std::shared_ptr<ContainerImpl> cont = container().lock();

    while (!status && payloadBytesRemaining > 0 && it != end) {
        unsigned      segmentLength = it->length();
        std::uint8_t* segmentBase   = it->base();
        unsigned      bytesToRead   = segmentLength < payloadBytesRemaining ? segmentLength : payloadBytesRemaining;

        status = cont->read(segmentBase, bytesToRead);
        if (status.success() && Container::ReadSuccessful(status).bytesRead() == bytesToRead) {
            status = Container::NoStatus();
            it->setProcessedCount(bytesToRead);
        }

        payloadBytesRemaining -= bytesToRead;
        ++it;
    }

    return status;
}


Container::Status StreamDataChunk::save(bool padToChunkSize) {
    unsigned payloadBytesRemaining = additionalAvailableSpace();

    if (currentScatterGatherListByteCount < payloadBytesRemaining) {
        payloadBytesRemaining = currentScatterGatherListByteCount;
    }

    // TODO: Clean-up the statement below.  We're reaching down into some of the gory details of the base class.  We
    //       can also probably simplify the setNumberValidBytes method.  Note that the chunk size may shrink
    //       Assert was included to verify that the things are actually working as expected.

    unsigned actualPayload = ChunkHeader::setNumberValidBytes(
        payloadBytesRemaining + ChunkHeader::additionalHeaderSizeBytes()
    );
    (void) actualPayload;
    assert(actualPayload == payloadBytesRemaining + ChunkHeader::additionalHeaderSizeBytes());

    // Use the base class function to set the container pointer, calculate the CRC, and write the header data.
    Container::Status status = Chunk::save(false);

    std::shared_ptr<ContainerImpl> cont = container().lock();

    std::vector<ScatterGatherListSegment>::iterator it  = scatterGatherList.begin();
    std::vector<ScatterGatherListSegment>::iterator end = scatterGatherList.end();

    while (!status && payloadBytesRemaining > 0 && it != end) {
        unsigned      segmentLength = it->length();
        std::uint8_t* segmentBase   = it->base();
        unsigned      bytesToWrite  = segmentLength < payloadBytesRemaining ? segmentLength : payloadBytesRemaining;

        status = cont->write(segmentBase, bytesToWrite);
        if (status.success() && Container::WriteSuccessful(status).bytesWritten() == bytesToWrite) {
            status = Container::NoStatus();
            it->setProcessedCount(bytesToWrite);
        }

        payloadBytesRemaining -= bytesToWrite;
        ++it;
    }

    if (!status && padToChunkSize) {
        status = writeTail();
    }

    return status;
}


bool StreamDataChunk::checkCrc() const {
    ChunkHeader::RunningCrc currentCrc = initializeCrc();

    unsigned                                              payloadBytesRemaining = additionalAvailableSpace();
    std::vector<ScatterGatherListSegment>::const_iterator it                    = scatterGatherList.cbegin();
    std::vector<ScatterGatherListSegment>::const_iterator end                   = scatterGatherList.cend();

    while (payloadBytesRemaining > 0 && it != end) {
        unsigned      segmentLength  = it->length();
        std::uint8_t* segmentBase    = it->base();
        unsigned      bytesToProcess = segmentLength < payloadBytesRemaining ? segmentLength : payloadBytesRemaining;

        currentCrc = calculateCrc(currentCrc, segmentBase, bytesToProcess);

        payloadBytesRemaining -= bytesToProcess;
        ++it;
    }

    return currentCrc == crc();
}


void StreamDataChunk::updateCrc() {
    ChunkHeader::RunningCrc currentCrc = initializeCrc();

    unsigned                                              payloadBytesRemaining = additionalAvailableSpace();
    std::vector<ScatterGatherListSegment>::const_iterator it                    = scatterGatherList.cbegin();
    std::vector<ScatterGatherListSegment>::const_iterator end                   = scatterGatherList.cend();

    while (payloadBytesRemaining > 0 && it != end) {
        unsigned      segmentLength  = it->length();
        std::uint8_t* segmentBase    = it->base();
        unsigned      bytesToProcess = segmentLength < payloadBytesRemaining ? segmentLength : payloadBytesRemaining;

        currentCrc = calculateCrc(currentCrc, segmentBase, bytesToProcess);

        payloadBytesRemaining -= bytesToProcess;
        ++it;
    }

    setCrc(currentCrc);
}
