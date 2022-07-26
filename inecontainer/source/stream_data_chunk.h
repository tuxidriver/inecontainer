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
* This header defines the \ref StreamDataChunk class.
***********************************************************************************************************************/

/* .. sphinx-project inecontainer */

#ifndef STREAM_DATA_CHUNK_H
#define STREAM_DATA_CHUNK_H

#include <cstdint>
#include <vector>

#include "scatter_gather_list_segment.h"
#include "stream_chunk.h"

/**
 * Class that manages a chunk of data representing the continuation of a stream or virtual file.
 */
class StreamDataChunk:public StreamChunk {
    public:
        /**
         * Type used to represent a byte offset into a chunk.
         */
        typedef unsigned long long ChunkOffset;

        /**
         * Constructor.
         *
         * \param[in] container        The container used to access the requested data.
         *
         * \param[in] fileIndex        The file index where the chunk starts.
         *
         * \param[in] streamIdentifier The identifier associated with this stream.
         *
         * \param[in] chunkOffset      The offset from the start of the stream representing the first byte of payload in
         *                             this chunk.
         */
        StreamDataChunk(
            std::weak_ptr<ContainerImpl> container,
            FileIndex                    fileIndex,
            StreamIdentifier             streamIdentifier,
            unsigned long long           chunkOffset
        );

        /**
         * Constructor.
         *
         * \param[in] container    The container used to access the requested data.
         *
         * \param[in] fileIndex    The file index where the chunk starts.
         *
         * \param[in] commonHeader Array holding header data common to all chunk types.
         */
        StreamDataChunk(
            std::weak_ptr<ContainerImpl> container,
            FileIndex                    fileIndex,
            std::uint8_t                 commonHeader[Chunk::minimumChunkHeaderSizeBytes]
        );

        ~StreamDataChunk() override;

        /**
         * Method that can be used to set the chunk size.  Note that the chunk size must never be more than 2 times the
         * size of the payload.
         *
         * \param[in] newChunkSize The new chunk size, in bytes.
         *
         * \return Returns the actual chunk size which will be equal to or less then the provided chunk size.
         */
        unsigned setChunkSize(unsigned newChunkSize);

        /**
         * Method that can be used to set the bytes offset of the first byte of this chunk in the stream.
         *
         * \param[in] newChunkOffset The new chunk offset.
         */
        void setChunkOffset(ChunkOffset newChunkOffset);

        /**
         * Method that returns the byte offset from the start of the stream represented by the first byte of payload in
         * this chunk.
         *
         * \return Returns the offset from the start of the stream represented by the first byte of the payload in this
         *         chunk.
         */
        ChunkOffset chunkOffset() const;

        /**
         * Method that indicates the payload size for this chunk.
         *
         * \return Returns the payload size for this chunk.
         */
        unsigned payloadSize() const;

        /**
         * Method that clears the scatter-gather list used to track where payload data should be loaded/stored from/to.
         */
        void clearScatterGatherList();

        /**
         * Method that appends an entry to the scatter-gather list.
         *
         * \param[in] newSegment       The segment to be added.
         *
         * \return Returns the number of bytes of this segment that are expected to be written based on the current
         *         chunk size.  The value also represents the expected amount read if the chunk is fully populated.
         */
        unsigned addScatterGatherListSegment(const ScatterGatherListSegment& newSegment);

        /**
         * Convenience method that appends an entry to the scatter-gather list.
         *
         * \param[in] buffer       Base pointer to the buffer to be added.
         *
         * \param[in] bufferLength The length of the buffer to be added.
         *
         * \return Returns the number of bytes of this segment that are expected to be written based on the current
         *         chunk size.  The value also represents the expected amount read if the chunk is fully populated.
         */
        unsigned addScatterGatherListSegment(std::uint8_t* buffer, unsigned bufferLength);

        /**
         * Method that returns the current scatter-gather list size.
         *
         * \return Returns the current scatter-gather list size.
         */
        unsigned scatterGatherListSize() const;

        /**
         * Method that returns a scatter-gather list entry.
         *
         * \param[in] index The zero-based index into the scatter-gather list.
         *
         * \return Returns the requested scatter-gather list entry.  The method will return a default scatter-gather
         *         list segment(with a null base-pointer) if the index is out of range.
         */
        ScatterGatherListSegment scatterGatherListSegment(unsigned index) const;

        /**
         * Method that loads just the header of a chunk into memory from the container.
         *
         * \param[in] includeCommonHeader If true, the portion of the header common to all chunk types will be loaded.
         *                                If false, only the additional header and payload will be loaded.
         *
         * \return Returns the status from the load operation.
         */
        Container::Status loadHeader(bool includeCommonHeader = false);

        /**
         * Method that loads a chunk into memory from the container.
         *
         * To improve speed, the CRC is not checked on the loaded chunk after being loaded.  You can do this as an
         * additional operation, if desired.
         *
         * \param[in] includeCommonHeader If true, the portion of the header common to all chunk types will be loaded.
         *                                If false, only the additional header and payload will be loaded.
         *
         * \return Returns the status from the load operation.
         */
        Container::Status load(bool includeCommonHeader = false) final;

        /**
         * Method that writes the chunk to the container.  The default implementation assumes all valid data is
         * contained in the header.  Note that a save operation might adjust the chunk size downward so be sure to
         * verify the chunk size after each save operation.
         *
         * The method will automatically update the CRC prior to writing the chunk to the container.
         *
         * \param[in] padToChunkSize If true, additional bytes will be written at the end of the chunk, if needed to
         *                           pad the chunk to the correct total byte size.
         *
         * \return Returns the status from the save operation.
         */
        Container::Status save(bool padToChunkSize = true) final;

        /**
         * Method that checks if the CRC is valid.  This version assumes that the entire chunk contents are contained
         * within the additional header data.
         */
        bool checkCrc() const final;

    protected:
        /**
         * Method that is called by the \ref Chunk::save method to calculate the CRC.
         */
        void updateCrc() final;

    private:
        /**
         * The scatter-gather list used during load/save operations.
         */
        std::vector<ScatterGatherListSegment> scatterGatherList;

        /**
         * The current byte count of the scatter-gather list.
         */
        unsigned currentScatterGatherListByteCount;

        /**
         * The number of addtional bytes used to track the stream data in this chunk.
         */
        static constexpr unsigned numberAdditionalStreamHeaderBytes = 6;
};

#endif
