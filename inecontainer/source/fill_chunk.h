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
* This header defines the \ref FillChunk class.
***********************************************************************************************************************/

/* .. sphinx-project inecontainer */

#ifndef FILL_CHUNK_H
#define FILL_CHUNK_H

#include <cstdint>
#include <memory>
#include <string>

#include "chunk.h"

/**
 * Class that provides support for fill chunks used to mark space in a container as unused.
 */
class FillChunk:public Chunk {
    public:
        /**
         * Constructor.
         *
         * \param[in] container      The container used to access the requested data.
         *
         * \param[in] fileIndex      The file index where the chunk starts.
         *
         * \param[in] availableSpace The space the fill chunk should ideally take up.  Note that the fill chunk will
         *                           always take up this amount of space, or less with the constraint that a full chunk
         *                           can never take up less than 32-bytes of space.
         */
        FillChunk(std::weak_ptr<ContainerImpl> container, FileIndex fileIndex, unsigned availableSpace = 0);

        /**
         * Constructor.
         *
         * \param[in] container    The container used to access the requested data.
         *
         * \param[in] fileIndex    The file index where the chunk starts.
         *
         * \param[in] commonHeader Array holding header data common to all chunk types.
         */
        FillChunk(
            std::weak_ptr<ContainerImpl> container,
            FileIndex                    fileIndex,
            std::uint8_t                 commonHeader[Chunk::minimumChunkHeaderSizeBytes]
        );

        ~FillChunk() override;

        /**
         * Method that finds an optimal chunk size that will fit within a specified amount of space.  The chunk will be
         * configured such that all data is considered valid.
         *
         * You can use this method whe either you have a significant amount of data and want the largest chunk possible
         * that can fit within a specific amount of space.  You can also use this method to create fill chunks to fit
         * within a specified amount of space.  Note that the created chunk may be less than the space desired.
         *
         * \param[in] availableSpace The available space for the chunk, in bytes.
         *
         * \return Returns the largest chunk that can fit within the specified amount of space.  The value will always
         *         be greater or equal to the minimum chunk size (currently 32 bytes) and will be equal to or less than
         *         the available space in bytes.  If the available space is less than the minimum chunk size, a value of
         *         zero is returned.
         */
        unsigned setBestFitSize(unsigned availableSpace);

        /**
         * Method that returns the fill space used by this chunk, in bytes.
         *
         * \return Returns the total space consumed by this chunk, in bytes.
         */
        unsigned fillSpaceBytes() const;
};

#endif
