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
* This header defines the \ref VirtualFileImpl class.
***********************************************************************************************************************/

/* .. sphinx-project inecontainer */

#ifndef VIRTUAL_FILE_IMPL_H
#define VIRTUAL_FILE_IMPL_H

#include <cstdint>
#include <memory>
#include <string>
#include <map>
#include <vector>

#include "container_status.h"
#include "stream_start_chunk.h"
#include "stream_data_chunk.h"
#include "container_impl.h"
#include "stream_start_chunk.h"
#include "free_space.h"
#include "ring_buffer.h"
#include "chunk_map_data.h"

/**
 * Pure virtual virtual file implementation class.  You should derive from this class to create a pimpl for the public
 * container class.
 *
 * Any PIMPL defined as part of the API must be private to the API which then makes the virtual file implementation
 * inaccessible to other classes.  We work around this by making the PIMPL class derived from another class with virtual
 * functions.  This approach also has the side-effect of more fully segregating the public API from the implementation.
 */
class VirtualFileImpl {
    public:
        /**
         * Constructor.
         *
         * \param[in] newName          The name of the virtual file.
         *
         * \param[in] streamIdentifier The identifier that should be used for this stream.
         *
         * \param[in] container        The container that this virtual file must operate on.
         */
        VirtualFileImpl(
            const std::string&             newName,
            StreamChunk::StreamIdentifier  streamIdentifier,
            std::shared_ptr<ContainerImpl> container
        );

        virtual ~VirtualFileImpl();

        /**
         * Constructor
         *
         * \param[in] streamStartChunk The chunk that is used to start the stream.
         */
        VirtualFileImpl(std::shared_ptr<StreamStartChunk> streamStartChunk);

        /**
         * Method that returns the name of this virtual file.
         *
         * \return Returns the virtual file name.
         */
        std::string name() const;

        /**
         * Method that sets the stream identifier.
         *
         * \param[in] newIdentifier The new stream identifier for this file.
         */
        void setStreamIdentifier(StreamChunk::StreamIdentifier newIdentifier);

        /**
         * Method that sets the stream start index.
         *
         * \param[in] streamStartFileIndex The file index of the stream start chunk.
         */
        void setStreamStartIndex(ChunkHeader::FileIndex streamStartFileIndex);

        /**
         * Method that returns the stream identifier tied to this file.
         *
         * \return Returns the stream identifier.
         */
        StreamChunk::StreamIdentifier streamIdentifier() const;

        /**
         * Method that determines the current size of the virtual file.  Note that this operation must scan the
         * container and is, therefore, rather slow.
         *
         * \return Returns the size of the virtual file.  A negative value is returned on error.
         */
        long long size();

        /**
         * Method that seeks to a relative offset in the virtual file.  Note that this operation must scan the
         * container and is, therefore, rather slow.
         *
         * \param[in] newOffset The new offset into the virtual file.
         *
         * \return Returns the status from the seek operation.
         */
        Container::Status setPosition(unsigned long long newOffset);

        /**
         * Method that seeks to the end of the virtual file.  Note that this operation must scan the container to
         * locate the last entry and is therefore rather slow.
         *
         * \return Returns the status from the seek operation.
         */
        Container::Status setPositionLast();

        /**
         * Method that determines the current virtual file pointer into this file.
         *
         * \return Returns the current location.  A negative value is returned on error.
         */
        long long position() const;

        /**
         * Method that returns the number of bytes of data currently cached for this virtual file.
         *
         * \return Returns the number of cached bytes.
         */
        unsigned long long bytesInWriteCache() const;

        /**
         * Method that reads a specified number of bytes of data from the container, if available.
         *
         * \param[in] buffer       The buffer to receive the data.  The buffer must be large enough to hold all the
         *                         requested information.
         *
         * \param[in] desiredCount The number of bytes that should be read, if possible.
         *
         * \return Returns the status from the read operation.  On success an instance of \ref Container::ReadSuccessful
         *         is returned.
         */
        Container::Status read(std::uint8_t* buffer, unsigned desiredCount);

        /**
         * Method that writes a specified number of bytes of data.  You can use this method either during
         * random access or when streaming data to a virtual file in a container.
         *
         * \param[in] buffer       The buffer holding the data to write.
         *
         * \param[in] desiredCount The number of bytes to be written.
         *
         * \return Returns the status from the write operation.  On success an instance of
         *         \ref Container::WriteSuccessful is returned.
         */
        Container::Status write(const std::uint8_t* buffer, unsigned desiredCount);

        /**
         * Method that appends a specified number of bytes of data to the end of the virtual file.
         *
         * \param[in] buffer The buffer holding the data to write.
         *
         * \param[in] count  The number of bytes to be written.
         *
         * \return Returns the status from the write operation.  On success an instance of
         *         \ref Container::WriteSuccessful is returned.
         */
        Container::Status append(const std::uint8_t* buffer, unsigned count);

        /**
         * Method that truncates the file at the current position.  All data after the current position will be
         * discarded.
         *
         * \return Returns the status from the truncation operation.
         */
        Container::Status truncate();

        /**
         * Method that flushes any pending write data to the media.
         *
         * \return Returns the status from the flush operation.
         */
        Container::Status flush();

        /**
         * Method that deletes this file.  This virtual file object will no longer be valid after calling this
         * method.
         *
         * \return Returns the status from the erase operation.
         */
        Container::Status erase();

        /**
         * Method that renames this file.
         *
         * \param[in] newName The new name to assign to this virtual file.
         *
         * \return Returns the status from the rename operation.
         */
        Container::Status rename(const std::string& newName);

        /**
         * Method you can overload to receive data from the streaming API.  Note that, to avoid multiple instances
         * incorrectly operating on the same data, only the instance that was instantiated by
         * \ref Container::Container::createFile will receive calls to this method.
         *
         * \param[in] buffer        A pointer to the buffer holding the read data.
         *
         * \param[in] bytesReceived A count of the number of bytes received in the buffer.
         *
         * \return Returns the status from the operation.  You should return \ref Container::NoStatus on success or
         *         an instance of a class derived from \ref Container::Status on error.  Note that you can extend
         *         the \ref Container::Status class to create your own error types, if desired.
         */
        virtual Container::Status receivedData(const std::uint8_t* buffer, unsigned bytesReceived) = 0;

        /**
         * Method you can overload to receive notification when the end of file is reached.
         *
         * \return Returns the status from the operation.
         */
        virtual Container::Status endOfFile() = 0;

        /**
         * Method that is called to inform the virtual file of the location of a chunk that belongs to it.
         *
         * \param[in] startingIndex The zero based file index where the chunk can be found.
         *
         * \param[in] baseOffset    The zero based byte offset into the virtual file tied to the chunk.
         *
         * \param[in] payloadSize   The size of the chunk's payload, in bytes.
         */
        void addChunkLocation(
            ChunkHeader::FileIndex startingIndex,
            unsigned long long     baseOffset,
            unsigned               payloadSize
        );

    private:
        /**
         * Value used to indicate the size of the tail storage buffer.
         */
        static constexpr unsigned tailBufferSize = 4096;

        /**
         * Value used to indicate the size of the chunk storage buffer.
         */
        static constexpr unsigned chunkBufferSize = 4096;

        /**
         * Typedef used to track chunks of data associated with this virtual file.
         */
        typedef std::map<unsigned long long, ChunkMapData> ChunkMap;

        /**
         * Typedef used to track chunks of data associated with this virtual file.
         */
        typedef std::pair<unsigned long long, ChunkMapData> ChunkMapPair;

        /**
         * Method that writes the stream start chunk, if needed.  Called by other methods that modify the container to
         * make certain that the stream start chunk exists.
         *
         * \return Returns the status from the operation.
         */
        Container::Status writeStreamStartIfNeeded();

        /**
         * Method that flushes the chunk buffer to the media.
         *
         * \return Returns the status from the operation.
         */
        Container::Status flushChunkBuffer();

        /**
         * Method that loads a chunk into the chunk buffer.
         *
         * \return Returns the status from the operation.
         */
        Container::Status loadChunkIntoBuffer();

        /**
         * Method that determines the current stored size based on the chunk map.
         */
        unsigned long long currentStoredSize();

        /**
         * Method that determines the last known file index for this file.
         */
        ChunkHeader::FileIndex lastKnownFileIndex();

        /**
         * Value used to indicate an invalid virtual file size.
         */
        static constexpr unsigned long long invalidFileOffset = static_cast<unsigned long long>(-1);

        /**
         * Pointer to the container used to read/write virtual file contents.
         */
        std::weak_ptr<ContainerImpl> currentContainer;

        /**
         * The name of this virtual file.
         */
        std::string currentName;

        /**
         * The stream identifier.
         */
        StreamChunk::StreamIdentifier currentStreamIdentifier;

        /**
         * The file index to the file start chunk.
         */
        ChunkHeader::FileIndex startChunkIndex;

        /**
         * Map of chunks actively stored in the container, by byte offset.
         */
        ChunkMap chunkMap;

        /**
         * Iterator into the chunk map for the currently loaded chunk.
         */
        ChunkMap::iterator currentChunk;

        /**
         * Buffer used to perform reads and random writes in the virtual file.
         */
        std::uint8_t* chunkBuffer;

        /**
         * Flag that indicates if the local buffer needs to be flushed.
         */
        bool chunkBufferFlushNeeded;

        /**
         * Ring buffer used to hold partial chunks of data at the end of the file.
         */
        RingBuffer<std::uint8_t, tailBufferSize> tailBuffer;

        /**
         * The the current offset into the file.  Value represents the offset just past the end of the local buffer.
         */
        unsigned long long currentPosition;
};

#endif
