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
* This file implements the \ref VirtualFileImpl class.
***********************************************************************************************************************/

#include <cstdint>
#include <vector>
#include <map>
#include <utility>
#include <string>
#include <memory>
#include <cassert>

#include "container_status.h"
#include "chunk_header.h"
#include "stream_chunk.h"
#include "stream_start_chunk.h"
#include "stream_data_chunk.h"
#include "free_space.h"
#include "free_space_tracker.h"
#include "ring_buffer.h"
#include "chunk_map_data.h"
#include "container_impl.h"
#include "virtual_file_impl.h"

VirtualFileImpl::VirtualFileImpl(
        const std::string&             newName,
        StreamChunk::StreamIdentifier  streamIdentifier,
        std::shared_ptr<ContainerImpl> container) {
    currentName             = newName;
    currentStreamIdentifier = streamIdentifier;
    currentContainer        = container;

    startChunkIndex         = ChunkHeader::invalidFileIndex;
    chunkBuffer             = nullptr;
    chunkBufferFlushNeeded  = false;
    currentChunk            = chunkMap.end();
    currentPosition         = 0;
}


VirtualFileImpl::VirtualFileImpl(std::shared_ptr<StreamStartChunk> streamStartChunk) {
    currentContainer        = streamStartChunk->container();
    currentName             = streamStartChunk->virtualFilename();
    currentStreamIdentifier = streamStartChunk->streamIdentifier();
    startChunkIndex         = streamStartChunk->fileIndex();

    chunkBuffer             = nullptr;
    chunkBufferFlushNeeded  = false;
    currentChunk            = chunkMap.end();
    currentPosition         = 0;
}


VirtualFileImpl::~VirtualFileImpl() {
    if (chunkBuffer != nullptr) {
        delete[] chunkBuffer;
    }
}


std::string VirtualFileImpl::name() const {
    return currentName;
}


void VirtualFileImpl::setStreamIdentifier(StreamChunk::StreamIdentifier newIdentifier) {
    currentStreamIdentifier = newIdentifier;
}


void VirtualFileImpl::setStreamStartIndex(ChunkHeader::FileIndex streamStartFileIndex) {
    startChunkIndex = streamStartFileIndex;
}


StreamChunk::StreamIdentifier VirtualFileImpl::streamIdentifier() const {
    return currentStreamIdentifier;
}


long long VirtualFileImpl::size() {
    long long                      currentSize;
    std::shared_ptr<ContainerImpl> container = currentContainer.lock();

    if (!container) {
        currentSize = -1;
    } else {
        if (container->containerScanNeeded()) {
            Container::Status status = container->scanContainer();
            if (!status.success()) {
                currentSize = -1;
            } else {
                assert(tailBuffer.empty());
                currentSize = currentStoredSize();
            }
        } else {
            currentSize = currentStoredSize() + tailBuffer.count();
        }
    }

    return currentSize;
}


Container::Status VirtualFileImpl::setPosition(unsigned long long newOffset) {
    Container::Status status;

    std::shared_ptr<ContainerImpl> container = currentContainer.lock();
    if (!container) {
        status = Container::ContainerUnavailable();
    }

    long long currentSize = size();

    if (currentSize >= 0 && newOffset <= static_cast<unsigned long long>(currentSize)) {
        currentPosition = newOffset;
    } else {
        status = Container::SeekError(newOffset, static_cast<unsigned long long>(currentSize));
    }

    if (container) {
        container->setLastStatus(status);
    }

    return status;
}


Container::Status VirtualFileImpl::setPositionLast() {
    Container::Status status;

    std::shared_ptr<ContainerImpl> container = currentContainer.lock();
    if (!container) {
        status = Container::ContainerUnavailable();
    }

    if (!status) {
        currentPosition = size();
        container->setLastStatus(status);
    }

    return status;
}


long long VirtualFileImpl::position() const {
    return currentPosition;
}


unsigned long long VirtualFileImpl::bytesInWriteCache() const {
    unsigned cachedBytes = 0;

    if (chunkBufferFlushNeeded && currentChunk != chunkMap.end()) {
        cachedBytes = currentChunk->second.payloadSize();
    }

    cachedBytes += tailBuffer.count();

    return cachedBytes;
}


Container::Status VirtualFileImpl::read(std::uint8_t* buffer, unsigned desiredCount) {
    Container::Status status;

    unsigned long long distanceToEof     = size() - currentPosition;
    unsigned           numberBytesToRead = static_cast<unsigned>(  desiredCount < distanceToEof
                                                                 ? desiredCount
                                                                 : distanceToEof
                                                                );

    std::uint8_t*      bufferSegment   = buffer;
    unsigned           remainingToRead = numberBytesToRead;

    std::shared_ptr<ContainerImpl> container = currentContainer.lock();
    if (!container) {
        status = Container::ContainerUnavailable();
    }

    if (!status && container->containerScanNeeded()) {
        status = container->scanContainer();
    }

    unsigned long long tailBufferBase = currentStoredSize();                    // Inclusive
    unsigned long long readEnd        = currentPosition + remainingToRead;      // Exclusive

    while (!status && remainingToRead > 0 && currentPosition < tailBufferBase) {
        bool chunkLoaded = false;

        if (currentChunk == chunkMap.end()                                              ||
            currentChunk->first > currentPosition                                       ||
            currentChunk->first + currentChunk->second.payloadSize() <= currentPosition    ) {
            // No chunk is loaded or the loaded chunk is not the one we need.

            if (chunkBufferFlushNeeded) {
                assert(currentChunk != chunkMap.end());
                assert(chunkBuffer != nullptr);

                status = flushChunkBuffer();
            }

            if (!status) {
                currentChunk = chunkMap.upper_bound(currentPosition);
                --currentChunk;

                assert(currentChunk != chunkMap.end());

                chunkLoaded = false;
            }
        } else {
            chunkLoaded = true;
        }

        unsigned bytesOfReadData = static_cast<unsigned>(-1);

        if (!status) {
            unsigned           chunkSize           = currentChunk->second.payloadSize();
            unsigned long long chunkStartingOffset = currentChunk->first; // Inclusive
            unsigned long long chunkEndingOffset   = chunkStartingOffset + chunkSize; // Exclusive

            if (chunkLoaded) {
                // We have the data in local memory, copy out what we need.

                unsigned chunkBytesRemaining = static_cast<unsigned>(chunkEndingOffset - currentPosition);
                bytesOfReadData = remainingToRead < chunkBytesRemaining ? remainingToRead : chunkBytesRemaining;

                std::memcpy(bufferSegment, chunkBuffer + (currentPosition - chunkStartingOffset), bytesOfReadData);
            } else {
                // We don't have the chunk in local memory.  We have to read it into either the chunk buffer (and copy
                // portions) or into the read buffer.

                if (readEnd > chunkEndingOffset) {
                    // We're going to read another chunk after this one, read directly into the read buffer.

                    StreamDataChunk chunk(
                        currentContainer,
                        currentChunk->second.startingIndex(),
                        currentStreamIdentifier,
                        chunkStartingOffset
                    );

                    chunk.setChunkSize(ChunkHeader::maximumChunkSize); // Chunk size adjusted during the read.

                    if (currentPosition == chunkStartingOffset) {
                        // We're going to read the entire chunk.

                        bytesOfReadData = chunkSize;
                        chunk.addScatterGatherListSegment(const_cast<std::uint8_t*>(bufferSegment), chunkSize);
                    } else {
                        // A portion of the front of the chunk are not read.  Stream those into the chunk buffer and
                        // throw it away.

                        bytesOfReadData = static_cast<unsigned>(chunkEndingOffset - currentPosition);

                        if (chunkBuffer == nullptr) {
                            chunkBuffer = new std::uint8_t[chunkBufferSize];
                        }

                        chunk.addScatterGatherListSegment(
                            chunkBuffer,
                            static_cast<unsigned>(currentPosition - chunkStartingOffset)
                        );

                        chunk.addScatterGatherListSegment(bufferSegment, bytesOfReadData);
                    }

                    status = chunk.load(true);

                    if (!status && chunk.streamIdentifier() != currentStreamIdentifier) {
                        status = Container::StreamIdentifierMismatch(
                            chunk.streamIdentifier(),
                            currentStreamIdentifier,
                            ChunkHeader::toPosition(chunk.fileIndex())
                        );
                    }

                    if (!status && chunk.chunkOffset() != chunkStartingOffset) {
                        status = Container::OffsetMismatch(
                            chunk.chunkOffset(),
                            currentChunk->first,
                            ChunkHeader::toPosition(chunk.fileIndex())
                        );
                    }

                    unsigned chunkPayloadSize = 0;
                    for (unsigned i=0 ; i<chunk.scatterGatherListSize() ; ++i) {
                        chunkPayloadSize += chunk.scatterGatherListSegment(i).processedCount();
                    }

                    if (!status && chunkPayloadSize != chunkSize) {
                        status = Container::PayloadSizeMismatch(
                            chunkPayloadSize,
                            currentChunk->second.payloadSize(),
                            ChunkHeader::toPosition(chunk.fileIndex())
                        );
                    }

                    currentChunk = chunkMap.end();
                } else {
                    // We end on this chunk so we expect this chunk to reside in the chunk buffer.  Read into the chunk
                    // buffer and copy.

                    status = loadChunkIntoBuffer();

                    if (!status) {
                        unsigned chunkBytesRemaining = static_cast<unsigned>(chunkEndingOffset - currentPosition);
                        bytesOfReadData = remainingToRead < chunkBytesRemaining ? remainingToRead : chunkBytesRemaining;

                        std::memcpy(
                            bufferSegment,
                            chunkBuffer + (currentPosition - chunkStartingOffset),
                            bytesOfReadData
                        );
                    }
                }
            }
        }

        if (!status) {
            assert(bytesOfReadData != static_cast<unsigned>(-1));

            bufferSegment   += bytesOfReadData;
            remainingToRead -= bytesOfReadData;
            currentPosition += bytesOfReadData;
        }
    }

    // If we have any additional data to read, it will be in the tail buffer.  Snoop the tail buffer to read that data.
    if (!status && remainingToRead > 0) {
        assert(currentPosition >= tailBufferBase);

        unsigned offset = static_cast<unsigned>(currentPosition - tailBufferBase);
        assert(remainingToRead <= tailBuffer.count() - offset);

        for (unsigned i=0 ; i<remainingToRead ; ++i) {
            bufferSegment[i] = tailBuffer.snoop(offset + i);
        }

        currentPosition += remainingToRead;
    }

    if (!status) {
        status = Container::ReadSuccessful(numberBytesToRead);
    }

    if (container) {
        container->setLastStatus(status);
    }

    return status;
}


Container::Status VirtualFileImpl::write(const std::uint8_t* buffer, unsigned desiredCount) {
    Container::Status status;

    const std::uint8_t* bufferSegment     = buffer;
    unsigned            remainingInBuffer = desiredCount;

    std::shared_ptr<ContainerImpl> container = currentContainer.lock();
    if (!container) {
        status = Container::ContainerUnavailable();
    }

    if (!status && container->containerScanNeeded()) {
        status = container->scanContainer();
    }

    unsigned long long tailBufferBase = currentStoredSize();                 // Inclusive
    unsigned long long tailBufferEnd  = tailBufferBase + tailBuffer.count(); // Exclusive
    unsigned long long writeEnd       = currentPosition + remainingInBuffer; // Exclusive

    // First do RMW of chunks.
    while (!status && remainingInBuffer > 0 && currentPosition < tailBufferBase) {
        // .................xxxxxxxxxxxxxxxxxxxxxxxx..................
        //                  |                       |
        //                  |                       +---- currentChunk->first + currentChunk->second.payloadSize()
        //                  |
        //                  +---------------------------- currentChunk->first
        //
        //                  ************************ - Chunk is useful if our current position in this range

        bool chunkLoaded = false;

        if (currentChunk == chunkMap.end()                                              ||
            currentChunk->first > currentPosition                                       ||
            currentChunk->first + currentChunk->second.payloadSize() <= currentPosition    ) {
            // No chunk loaded or this is not the chunk we're looking for.

            if (chunkBufferFlushNeeded) {
                assert(currentChunk != chunkMap.end());
                assert(chunkBuffer != nullptr);

                status = flushChunkBuffer();
            }

            if (!status) {
                currentChunk = chunkMap.upper_bound(currentPosition);
                --currentChunk;

                assert(currentChunk != chunkMap.end());

                chunkLoaded = false;
            }
        } else {
            // The loaded chunk is the chunk we're looking for.
            chunkLoaded = true;
        }

        if (!status) {
            unsigned           chunkSize           = currentChunk->second.payloadSize();
            unsigned long long chunkStartingOffset = currentChunk->first; // Inclusive
            unsigned long long chunkEndingOffset   = chunkStartingOffset + chunkSize; // Exclusive

            unsigned bytesOfNewData = 0;

            if (writeEnd > chunkEndingOffset) {
                // We're going to evict this chunk, no need to keep the chunk buffer coherent.

                StreamDataChunk chunk(
                    currentContainer,
                    currentChunk->second.startingIndex(),
                    currentStreamIdentifier,
                    chunkStartingOffset
                );

                chunk.setChunkSize(ChunkHeader::maximumChunkSize);

                if (currentPosition == chunkStartingOffset) {
                    // We're going to overwrite the entire chunk.  Don't need to read-modify-write.

                    chunk.addScatterGatherListSegment(const_cast<std::uint8_t*>(bufferSegment), chunkSize);
                    bytesOfNewData = chunkSize;
                } else {
                    // We're going to only overwrite part of the chunk, we need to read-modify-write.  Load the
                    // chunk, if needed.  No need to keep the chunk cached and no need to make the in-memory copy
                    // coherent.

                    if (!chunkLoaded) {
                        status = loadChunkIntoBuffer();
                    }

                    if (!status) {
                        bytesOfNewData = static_cast<unsigned>(chunkEndingOffset - currentPosition);

                        chunk.addScatterGatherListSegment(
                            chunkBuffer,
                            static_cast<unsigned>(currentPosition - chunkStartingOffset)
                        );

                        chunk.addScatterGatherListSegment(const_cast<std::uint8_t*>(bufferSegment), bytesOfNewData);
                    }
                }

                status = chunk.save(); // Chunk is right-sized here.

                currentChunk           = chunkMap.end();
                chunkBufferFlushNeeded = false;
            } else {
                // The write will end at or before the end of this chunk.  This chunk will stay in the buffer.
                // Need to load the chunk into the buffer and then update the chunk buffer with the new data.

                bytesOfNewData = remainingInBuffer;

                if (!chunkLoaded) {
                    status = loadChunkIntoBuffer();
                }

                if (!status) {
                    std::uint8_t* writeBuffer = chunkBuffer + (currentPosition - chunkStartingOffset);
                    std::memcpy(writeBuffer, bufferSegment, remainingInBuffer);

                    chunkBufferFlushNeeded = true;
                }
            }

            if (!status) {
                bufferSegment     += bytesOfNewData;
                remainingInBuffer -= bytesOfNewData;
                currentPosition   += bytesOfNewData;
            }
        }
    }

    // Snoop the tail buffer to fill in the missing bits, unless we're just going to replace the tail buffer, then
    // clear it out so we replace it in full.
    if (!status && remainingInBuffer > 0) {
        assert(currentPosition >= tailBufferBase);

        if (currentPosition == tailBufferBase && remainingInBuffer >= tailBuffer.length()) {
            // We'll replace the entire tail buffer.  Let's simply clear it out and append.
            tailBuffer.clear();
            tailBufferEnd = tailBufferBase;
        } else {
            unsigned offset                = static_cast<unsigned>(currentPosition - tailBufferBase);
            unsigned remainingInTailBuffer = tailBuffer.count() - offset;
            unsigned entriesToSnoop        =   remainingInBuffer < remainingInTailBuffer
                                             ? remainingInBuffer
                                             : remainingInTailBuffer;

            for (unsigned i=0 ; i<entriesToSnoop ; ++i) {
                tailBuffer.snoop(offset + i) = bufferSegment[i];
            }

            bufferSegment     += entriesToSnoop;
            remainingInBuffer -= entriesToSnoop;
            currentPosition   += entriesToSnoop;
        }
    }

    // Append anything beyond the current EOF.
    if (!status && remainingInBuffer > 0) {
        assert(currentPosition == tailBufferEnd);
        status = append(bufferSegment, remainingInBuffer);
        if (status.success() && Container::WriteSuccessful(status).bytesWritten() == remainingInBuffer) {
            status = Container::NoStatus();
        }
    }

    if (!status) {
        status = Container::WriteSuccessful(desiredCount);
    }

    if (container) {
        container->setLastStatus(status);
    }

    return status;
}


Container::Status VirtualFileImpl::append(const std::uint8_t* buffer, unsigned desiredCount) {
    Container::Status status = writeStreamStartIfNeeded();

    unsigned            remainingInBuffer = desiredCount;
    const std::uint8_t* bufferSegment     = buffer;

    std::shared_ptr<ContainerImpl> container = currentContainer.lock();
    if (!status && !container) {
        status = Container::ContainerUnavailable();
    }

    if (!status && container->containerScanNeeded()) {
        status = container->scanContainer();
    }

    // Write out chunks until we have less than a full chunk left.
    while (!status && tailBuffer.available() <= remainingInBuffer) {
        std::unique_ptr<StreamDataChunk> chunk;
        FreeSpace                        reservedFreeSpace;

        // Position at EOF, allocate new free space.

        reservedFreeSpace = container->reserveFreeSpaceArea(
            lastKnownFileIndex(),
            ChunkHeader::toFileIndex(ChunkHeader::minimumChunkSize),
            ChunkHeader::toFileIndex(ChunkHeader::maximumChunkSize)
        );

        chunk.reset(new StreamDataChunk(
            currentContainer,
            reservedFreeSpace.startingIndex(),
            currentStreamIdentifier,
            currentStoredSize()
        ));

        chunk->setChunkSize(static_cast<unsigned>(ChunkHeader::toPosition(reservedFreeSpace.areaSize())));

        unsigned numberLocalSegments;
        if (tailBuffer.empty()) {
            numberLocalSegments = 0;
        } else {
            std::uint8_t* p1;
            unsigned      l1;
            std::uint8_t* p2;
            unsigned      l2;

            unsigned      tailBufferCount = tailBuffer.bulkExtractionStart(&p1, &l1, &p2, &l2);
            (void) tailBufferCount;
            assert(tailBufferCount == l1 + l2);

            chunk->addScatterGatherListSegment(p1, l1);
            if (l2 == 0) {
                numberLocalSegments = 1;
            } else {
                numberLocalSegments = 2;
                chunk->addScatterGatherListSegment(p2, l2);
            }
        }

        chunk->addScatterGatherListSegment(const_cast<std::uint8_t*>(bufferSegment), remainingInBuffer);
        status = chunk->save();

        if (!status) {
            unsigned freeSpaceAdjustment = ChunkHeader::toFileIndex(chunk->chunkSize());
            reservedFreeSpace.reduceBy(freeSpaceAdjustment, FreeSpace::Side::FROM_FRONT);

            container->releaseReservation(reservedFreeSpace);

            unsigned writtenTailBuffer = 0;
            for (unsigned i=0 ; i<numberLocalSegments ; ++i) {
                writtenTailBuffer += chunk->scatterGatherListSegment(i).processedCount();
            }

            bool success = tailBuffer.bulkExtractionFinish(writtenTailBuffer);
            (void) success;
            assert(success);

            unsigned writtenFromCall = chunk->scatterGatherListSegment(numberLocalSegments).processedCount();
            assert(writtenFromCall <= remainingInBuffer);

            remainingInBuffer -= writtenFromCall;
            bufferSegment     += writtenFromCall;

            unsigned totalWrittenThisChunk = writtenTailBuffer + writtenFromCall;

            addChunkLocation(chunk->fileIndex(), chunk->chunkOffset(), totalWrittenThisChunk);
        }
    }

    // If we have any data left, store it into the local buffer.
    if (!status && remainingInBuffer > 0) {
        unsigned      storedBytes = remainingInBuffer;
        std::uint8_t* p1;
        unsigned      l1;
        std::uint8_t* p2;
        unsigned      l2;

        unsigned availableSpace = tailBuffer.bulkInsertionStart(&p1, &l1, &p2, &l2);
        (void) availableSpace;
        assert(availableSpace > remainingInBuffer);

        unsigned countP1 = (l1 < remainingInBuffer) ? l1 : remainingInBuffer;
        std::memcpy(p1, bufferSegment, countP1);

        remainingInBuffer -= countP1;
        bufferSegment     += countP1;

        if (remainingInBuffer > 0) {
            assert(p2 != nullptr && l2 >= remainingInBuffer);
            std::memcpy(p2, bufferSegment, remainingInBuffer);
        }

        bool success = tailBuffer.bulkInsertionFinish(storedBytes);
        (void) success;
        assert(success);
    }

    if (!status) {
        currentPosition = size();
        status = Container::WriteSuccessful(desiredCount);
    }

    if (container) {
        container->setLastStatus(status);
    }

    return status;
}


Container::Status VirtualFileImpl::truncate() {
    Container::Status status;

    std::shared_ptr<ContainerImpl> container = currentContainer.lock();
    if (!container) {
        status = Container::ContainerUnavailable();
    }

    if (!status) {
        // Push everything to the container so we don't have to worry about the caches.
        status = flush();
    }

    if (!status) {
        ChunkMap::iterator pos = chunkMap.upper_bound(currentPosition);
        --pos;

        assert(pos != chunkMap.end());

        if (pos->first < currentPosition) {
            // We must preserve a portion of the first chunk.

            ChunkHeader::FileIndex startingIndex  = pos->second.startingIndex();
            unsigned long long     startingOffset = pos->first;

            std::uint8_t buffer[ChunkHeader::maximumChunkSize];
            StreamDataChunk oldChunk(currentContainer, startingIndex, currentStreamIdentifier, startingOffset);

            if (!status && oldChunk.streamIdentifier() != currentStreamIdentifier) {
                status = Container::StreamIdentifierMismatch(
                    oldChunk.streamIdentifier(),
                    currentStreamIdentifier,
                    ChunkHeader::toPosition(oldChunk.fileIndex())
                );
            }

            if (!status && oldChunk.chunkOffset() != startingOffset) {
                status = Container::OffsetMismatch(
                    oldChunk.chunkOffset(),
                    startingOffset,
                    ChunkHeader::toPosition(oldChunk.fileIndex())
                );
            }

            if (!status) {
                oldChunk.addScatterGatherListSegment(buffer, ChunkHeader::maximumChunkSize);
                status = oldChunk.load(true);
            }

            if (!status) {
                StreamDataChunk newChunk(currentContainer, startingIndex, currentStreamIdentifier, startingOffset);
                newChunk.setChunkSize(oldChunk.chunkSize());

                unsigned long long bytesThisChunk = currentPosition - startingOffset;
                assert(bytesThisChunk <= pos->second.payloadSize());

                newChunk.addScatterGatherListSegment(buffer, static_cast<unsigned>(bytesThisChunk));
                status = newChunk.save();

                if (!status) {
                    assert(newChunk.scatterGatherListSegment(0).processedCount() == bytesThisChunk);

                    pos->second.setPayloadSize(static_cast<unsigned>(bytesThisChunk));

                    if (newChunk.chunkSize() != oldChunk.chunkSize()) {
                        assert(newChunk.chunkSize() < oldChunk.chunkSize());

                        ChunkHeader::FileIndex oldChunkSize = ChunkHeader::toFileIndex(oldChunk.chunkSize());
                        ChunkHeader::FileIndex newChunkSize = ChunkHeader::toFileIndex(newChunk.chunkSize());

                        container->newFreeSpaceArea(startingIndex + newChunkSize, oldChunkSize - newChunkSize, true);
                    }

                    ++pos;
                }
            }
        }

        // Now wipe out any and all remaining chunks.

        while (!status && pos != chunkMap.end()) {
            ChunkHeader::FileIndex startingIndex  = pos->second.startingIndex();
            unsigned long long     startingOffset = pos->first;

            StreamDataChunk chunk(currentContainer, startingIndex, currentStreamIdentifier, startingOffset);

            status = chunk.load(true);

            if (!status && chunk.streamIdentifier() != currentStreamIdentifier) {
                status = Container::StreamIdentifierMismatch(
                    chunk.streamIdentifier(),
                    currentStreamIdentifier,
                    ChunkHeader::toPosition(chunk.fileIndex())
                );
            }

            if (!status && chunk.chunkOffset() != startingOffset) {
                status = Container::OffsetMismatch(
                    chunk.chunkOffset(),
                    startingOffset,
                    ChunkHeader::toPosition(chunk.fileIndex())
                );
            }

            if (!status) {
                container->newFreeSpaceArea(chunk.fileIndex(), ChunkHeader::toFileIndex(chunk.chunkSize()), true);
                pos = chunkMap.erase(pos);
            }
        }

        if (!status) {
            currentChunk = chunkMap.end();
        }
    }

    return status;
}


Container::Status VirtualFileImpl::flush() {
    Container::Status status = writeStreamStartIfNeeded();

    std::shared_ptr<ContainerImpl> container = currentContainer.lock();
    if (!status && !container) {
        status = Container::ContainerUnavailable();
    }

    if (!status && chunkBufferFlushNeeded) {
        status = flushChunkBuffer();
    }

    if (!status) {
        while (!status && tailBuffer.notEmpty()) {
            std::uint8_t* p1;
            unsigned      l1;
            std::uint8_t* p2;
            unsigned      l2;

            FreeSpace reservedFreeSpace = container->reserveFreeSpaceArea(
                lastKnownFileIndex(),
                ChunkHeader::toFileIndex(ChunkHeader::minimumChunkSize),
                ChunkHeader::toFileIndex(ChunkHeader::maximumChunkSize)
            );

            StreamDataChunk chunk(
                currentContainer,
                reservedFreeSpace.startingIndex(),
                currentStreamIdentifier,
                currentStoredSize()
            );

            chunk.setChunkSize(static_cast<unsigned>(ChunkHeader::toPosition(reservedFreeSpace.areaSize())));

            tailBuffer.bulkExtractionStart(&p1, &l1, &p2, &l2);
            chunk.addScatterGatherListSegment(p1, l1);
            if (p2 != nullptr) {
                chunk.addScatterGatherListSegment(p2, l2);
            }

            status = chunk.save();

            if (!status) {
                reservedFreeSpace.reduceBy(ChunkHeader::toFileIndex(chunk.chunkSize()), FreeSpace::Side::FROM_FRONT);
                container->releaseReservation(reservedFreeSpace);

                unsigned numberBytesWritten = 0;
                for (unsigned i=0 ; i<chunk.scatterGatherListSize() ; ++i) {
                    numberBytesWritten += chunk.scatterGatherListSegment(i).processedCount();
                }

                assert(numberBytesWritten <= tailBuffer.count()); // Verify that we're sane.

                addChunkLocation(chunk.fileIndex(), chunk.chunkOffset(), numberBytesWritten);

                tailBuffer.bulkExtractionFinish(numberBytesWritten);
            }
        }
    }

    if (container) {
        container->setLastStatus(status);
    }

    return status;
}


Container::Status VirtualFileImpl::erase() {
    Container::Status status;

    std::shared_ptr<ContainerImpl> container = currentContainer.lock();
    if (!container) {
        status = Container::ContainerUnavailable();
    }

    if (!status && container->containerScanNeeded()) {
        status = container->scanContainer();
    }

    std::vector<ContainerArea> areasToRelease;

    if (!status && startChunkIndex != ChunkHeader::invalidFileIndex) {
        StreamStartChunk chunk(container, startChunkIndex, currentName, currentStreamIdentifier);
        status = chunk.load(true);

        if (!status && chunk.streamIdentifier() != currentStreamIdentifier) {
            status = Container::StreamIdentifierMismatch(
                chunk.streamIdentifier(),
                currentStreamIdentifier,
                ChunkHeader::toPosition(chunk.fileIndex())
            );
        }

        if (!status && chunk.virtualFilename() != currentName) {
            status = Container::FilenameMismatch(
                chunk.virtualFilename(),
                currentName,
                ChunkHeader::toPosition(startChunkIndex)
            );
        }

        if (!status) {
            areasToRelease.push_back(ContainerArea(startChunkIndex, ChunkHeader::toFileIndex(chunk.chunkSize())));
        }
    }

    ChunkMap::iterator pos = chunkMap.begin();
    ChunkMap::iterator end = chunkMap.end();

    while (!status && pos != end) {
        ChunkHeader::FileIndex startingIndex  = pos->second.startingIndex();
        unsigned long long     startingOffset = pos->first;

        StreamDataChunk chunk(currentContainer, startingIndex, currentStreamIdentifier, startingOffset);

        status = chunk.load(true);

        if (!status && chunk.streamIdentifier() != currentStreamIdentifier) {
            status = Container::StreamIdentifierMismatch(
                chunk.streamIdentifier(),
                currentStreamIdentifier,
                ChunkHeader::toPosition(chunk.fileIndex())
            );
        }

        if (!status && chunk.chunkOffset() != startingOffset) {
            status = Container::OffsetMismatch(
                chunk.chunkOffset(),
                startingOffset,
                ChunkHeader::toPosition(chunk.fileIndex())
            );
        }

        if (!status) {
            areasToRelease.push_back(ContainerArea(startingIndex, ChunkHeader::toFileIndex(chunk.chunkSize())));
            ++pos;
        }
    }

    if (!status) {
        for (std::vector<ContainerArea>::iterator it=areasToRelease.begin(),end=areasToRelease.end() ; it!=end ; ++it) {
            container->newFreeSpaceArea(it->startingIndex(), it->areaSize(), true);
        }

        bool success = container->flushFreeSpace();
        if (!success) {
            status = container->lastStatus();
        }
    }

    if (!status) {
        bool success = container->fileErased(currentName);
        (void) success;
        assert(success);

        container->setLastStatus(status);

        container.reset();
        currentContainer.reset();
    } else {
        if (container) {
            container->setLastStatus(status);
        }
    }

    return status;
}


Container::Status VirtualFileImpl::rename(const std::string& newName) {
    Container::Status status;

    std::shared_ptr<ContainerImpl> container = currentContainer.lock();
    if (!container) {
        status = Container::ContainerUnavailable();
    }

    std::string oldName = currentName;

    if (!status && oldName != newName) {
        if (startChunkIndex != ChunkHeader::invalidFileIndex) {
            StreamStartChunk chunk(currentContainer, startChunkIndex, newName, currentStreamIdentifier);
            status = chunk.save();
        }

        if (!status) {
            currentName = newName;

            bool success = currentContainer.lock()->fileRenamed(oldName, newName);
            (void) success;
            assert(success);
        }
    }

    if (container) {
        container->setLastStatus(status);
    }

    return status;
}


void VirtualFileImpl::addChunkLocation(
        ChunkHeader::FileIndex startingIndex,
        unsigned long long     baseOffset,
        unsigned               payloadSize
    ) {
    ChunkMap::iterator pos = chunkMap.find(baseOffset);

    if (pos != chunkMap.end()) {
        pos->second = ChunkMapData(startingIndex, payloadSize);
    } else {
        chunkMap.insert(ChunkMapPair(baseOffset, ChunkMapData(startingIndex, payloadSize)));
    }
}


Container::Status VirtualFileImpl::writeStreamStartIfNeeded() {
    Container::Status status;

    if (startChunkIndex == ChunkHeader::invalidFileIndex) {
        StreamStartChunk chunk(currentContainer, 0, currentName, currentStreamIdentifier);

        std::shared_ptr<ContainerImpl> container = currentContainer.lock();
        FreeSpace reservedFreeSpace = container->reserveFreeSpaceArea(0, ChunkHeader::toFileIndex(chunk.chunkSize()));
        chunk.setFileIndex(reservedFreeSpace.startingIndex());

        status = chunk.save();

        if (!status) {
            reservedFreeSpace.reduceBy(ChunkHeader::toFileIndex(chunk.chunkSize()), FreeSpace::Side::FROM_FRONT);
            container->releaseReservation(reservedFreeSpace);

            startChunkIndex = chunk.fileIndex();
        }
    }

    return status;
}


Container::Status VirtualFileImpl::flushChunkBuffer() {
    Container::Status status;

    StreamDataChunk chunk(
        currentContainer,
        currentChunk->second.startingIndex(),
        currentStreamIdentifier,
        currentChunk->first
    );

    chunk.setChunkSize(ChunkHeader::maximumChunkSize); // The save method will automatically right-size the chunk.
    chunk.addScatterGatherListSegment(chunkBuffer, currentChunk->second.payloadSize());

    status = chunk.save();

    if (!status) {
        chunkBufferFlushNeeded = false;
    }

    return status;
}


Container::Status VirtualFileImpl::loadChunkIntoBuffer() {
    Container::Status status;

    StreamDataChunk chunk(
        currentContainer,
        currentChunk->second.startingIndex(),
        currentStreamIdentifier,
        currentChunk->first
    );

    chunk.setChunkSize(ChunkHeader::maximumChunkSize); // The save method will automatically right-size the chunk.

    if (chunkBuffer == nullptr) {
        chunkBuffer = new std::uint8_t[chunkBufferSize];
    }

    chunk.addScatterGatherListSegment(chunkBuffer, currentChunk->second.payloadSize());

    status = chunk.load(true);

    if (!status && chunk.streamIdentifier() != currentStreamIdentifier) {
        status = Container::StreamIdentifierMismatch(
            chunk.streamIdentifier(),
            currentStreamIdentifier,
            ChunkHeader::toPosition(chunk.fileIndex())
        );
    }

    if (!status && chunk.chunkOffset() != currentChunk->first) {
        status = Container::OffsetMismatch(
            chunk.chunkOffset(),
            currentChunk->first,
            ChunkHeader::toPosition(chunk.fileIndex())
        );
    }

    if (!status && chunk.scatterGatherListSegment(0).processedCount() != currentChunk->second.payloadSize()) {
        status = Container::PayloadSizeMismatch(
            chunk.scatterGatherListSegment(0).processedCount(),
            currentChunk->second.payloadSize(),
            ChunkHeader::toPosition(chunk.fileIndex())
        );
    }

    return status;
}


unsigned long long VirtualFileImpl::currentStoredSize() {
    unsigned long long storedSize;

    if (chunkMap.empty()) {
        storedSize = 0;
    } else {
        ChunkMap::iterator pos = chunkMap.end();
        --pos;

        storedSize = pos->first + pos->second.payloadSize();
    }

    return storedSize;
}


ChunkHeader::FileIndex VirtualFileImpl::lastKnownFileIndex() {
    ChunkHeader::FileIndex lastFileIndex;

    if (chunkMap.empty()) {
        if (startChunkIndex == ChunkHeader::invalidFileIndex) {
            lastFileIndex = 0;
        } else {
            lastFileIndex = startChunkIndex;
        }
    } else {
        ChunkMap::iterator pos = chunkMap.end();
        --pos;

        lastFileIndex = pos->second.startingIndex();
    }

    return lastFileIndex;
}
