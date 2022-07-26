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
* This header defines the \ref ChunkMapData class.
***********************************************************************************************************************/

/* .. sphinx-project inecontainer */

#ifndef FREE_SPACE_DATA_H
#define FREE_SPACE_DATA_H

#include "chunk_header.h"

/**
 * Class that is used to track the location and size of any given chunk.
 */
class ChunkMapData {
    public:
        /**
         * Constructor.
         *
         * \param[in] startingIndex The zero based index to the chunk.
         *
         * \param[in] payloadSize   The size of the chunk payload, in bytes.
         */
        ChunkMapData(ChunkHeader::FileIndex startingIndex, unsigned payloadSize);

        /**
         * Copy constructor.
         *
         * \param[in] other The instance to be copied.
         */
        ChunkMapData(const ChunkMapData& other);

        ~ChunkMapData();

        /**
         * Method that can be used to set the starting index of the chunk.
         *
         * \param[in] newStartingIndex The new starting index of ths chunk.
         */
        void setStartingIndex(ChunkHeader::FileIndex newStartingIndex);

        /**
         * Method that can be used to obtain the starting index for the chunk.
         *
         * \return Returns the starting index tied to this free space region.
         */
        ChunkHeader::FileIndex startingIndex() const;

        /**
         * Method that can be used to set the size of the payload.
         *
         * \param[in] newPayloadSize The new payload size, in bytes.
         */
        void setPayloadSize(unsigned newPayloadSize);

        /**
         * Method that can be used to obtain the payload size, in bytes.
         *
         * \return Returns the payload size, in bytes.
         */
        unsigned payloadSize() const;

        /**
         * Assignment operator.
         *
         * \param[in] other The instance to be copied.
         *
         * \return A reference to this class instance.
         */
        ChunkMapData& operator=(const ChunkMapData& other);

    private:
        /**
         * The starting index of this chunk.
         */
        ChunkHeader::FileIndex currentStartingIndex;

        /**
         * The payload size, in bytes.
         */
        unsigned currentPayloadSize;
};

#endif
