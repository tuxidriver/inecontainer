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
* This header defines the \ref StreamChunk class.
***********************************************************************************************************************/

/* .. sphinx-project inecontainer */

#ifndef STREAM_CHUNK_H
#define STREAM_CHUNK_H

#include <cstdint>
#include <memory>

#include "chunk.h"

class ContainerImpl;

/**
 * Base class for both the StreamStartChunk and StreamContinuationChunk classes.
 */
class StreamChunk:public Chunk {
    public:
        /**
         * Type used to specify a stream ID.
         */
        typedef std::uint32_t StreamIdentifier;

        /**
         * Value used to indicate an invalid stream identifier
         */
        static constexpr StreamIdentifier invalidStreamIdentifier = static_cast<StreamIdentifier>(-1);

    protected:
        /**
         * Constructor.
         *
         * \param[in] container                      The container used to access the requested data.
         *
         * \param[in] fileIndex                      The file index where the chunk starts.
         *
         * \param[in] streamIdentifier               The identifier associated with this stream.
         *
         * \param[in] additionalChunkHeaderSizeBytes The number of additional bytes in the chunk header.  This value takes
         *                                           into account space used for the stream identifier and EOF flag.
         */
        StreamChunk(
            std::weak_ptr<ContainerImpl> container,
            FileIndex                    fileIndex,
            StreamIdentifier             streamIdentifier,
            unsigned                     additionalChunkHeaderSizeBytes
        );

        /**
         * Constructor.
         *
         * \param[in] container                      The container used to access the requested data.
         *
         * \param[in] fileIndex                      The file index where the chunk starts.
         *
         * \param[in] commonHeader                   Array holding header data common to all chunk types.
         *
         * \param[in] additionalChunkHeaderSizeBytes The number of additional bytes in the chunk header.  This value takes
         *                                           into account space used for the stream identifier and EOF flag.
         */
        StreamChunk(
            std::weak_ptr<ContainerImpl> container,
            FileIndex                    fileIndex,
            std::uint8_t                 commonHeader[Chunk::minimumChunkHeaderSizeBytes],
            unsigned                     additionalChunkHeaderSizeBytes
        );

    public:
        ~StreamChunk() override;

        /**
         * Method you can use to set the stream identifier.
         *
         * \param[in] newStreamIdentifier The new stream identifier.
         */
        void setStreamIdentifier(StreamIdentifier newStreamIdentifier);

        /**
         * Method that returns the stream ID.
         *
         * \return Returns the stream ID for this stream.
         */
        StreamIdentifier streamIdentifier() const;

        /**
         * Method that can be used to indicate if this chunk is the last chunk in the stream.
         *
         * \param[in] nowLast If true, the chunk will be marked as the last chunk in the stream.  If false, the chunk
         *                    will not be marked as the last chunk in the stream.
         */
        void setLast(bool nowLast);

        /**
         * Method that determines if this chunk is the last chunk in the stream.
         *
         * \return Returns true if this chunk is the last chunk for this stream.  Returns false if this chunk is not the
         *         last chunk in the stream.
         */
        bool isLast() const;

    protected:
        /**
         * Method that returns a pointer to the additional header data.  The value takes into account and hides the
         * header already used by this class.
         *
         * \return Returns an array of bytes to the additional allocated chunk header space.
         */
        std::uint8_t* additionalHeader() const;

        /**
         * Method that returns the total size of the additional header, in bytes.  The value takes into account and
         * hides the header already used by this class.
         *
         * \return Returns the size of the header, in bytes.
         */
        unsigned additionalHeaderSizeBytes() const;

        /**
         * The number of header bytes imposed by this class.
         */
        static constexpr unsigned numberAdditionalStreamHeaderBytes = 4;
};

#endif
