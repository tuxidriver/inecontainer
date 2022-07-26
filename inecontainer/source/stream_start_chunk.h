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
* This header defines the \ref StreamStartChunk class.
***********************************************************************************************************************/

/* .. sphinx-project inecontainer */

#ifndef STREAM_START_CHUNK_H
#define STREAM_START_CHUNK_H

#include <cstdint>
#include <memory>
#include <string>

#include "stream_chunk.h"

/**
 * Class that manages a chunk that represents the start of a stream or virtual file.
 */
class StreamStartChunk:public StreamChunk {
    public:
        /**
         * Constant you can use to determine the maximum virtual filename length.
         */
        static constexpr unsigned maximumVirtualFilenameLength = 119;

        /**
         * Constructor.
         *
         * \param[in] container        The container used to access the requested data.
         *
         * \param[in] fileIndex        The file index where the chunk starts.
         *
         * \param[in] virtualFilename  The virtual filename to refer to this stream as.  The name will be truncated if
         *                             it is excessively long.
         *
         * \param[in] streamIdentifier The identifier associated with this stream.
         */
        StreamStartChunk(
            std::weak_ptr<ContainerImpl> container,
            FileIndex                    fileIndex,
            const std::string&           virtualFilename,
            StreamIdentifier             streamIdentifier
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
        StreamStartChunk(
            std::weak_ptr<ContainerImpl> container,
            FileIndex                    fileIndex,
            std::uint8_t                 commonHeader[Chunk::minimumChunkHeaderSizeBytes]
        );

        ~StreamStartChunk() override;

        /**
         * Method you can use to update the virtual filename tied to this stream.
         *
         * \param[in] newVirtualFilename The new virtual filename tied to this stream.
         */
        void setVirtualFilename(const std::string& newVirtualFilename);

        /**
         * Method you can use to obtain the virtual filename tied to this stream.
         *
         * \return Returns the virtual filename tied to this stream.
         */
        std::string virtualFilename() const;

    protected:
        /**
         * The number of addtional bytes used to track the stream data in this chunk.
         */
        static constexpr unsigned numberAdditionalStreamHeaderBytes = 120;
};

#endif
