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
* This header defines the \ref ContainerArea class.
***********************************************************************************************************************/

/* .. sphinx-project inecontainer */

#ifndef CONTAINER_AREA_H
#define CONTAINER_AREA_H

#include <cstdint>

#include "chunk_header.h"

/**
 * Trivial class that can be used to pass around information about regions in the container.
 */
class ContainerArea {
    public:
        /**
         * Enum indicating which side of the container area to be reduced.
         */
        enum class Side:std::uint8_t {
            /**
             * Indicates the front of the container should be reduced or expanded.  Specifying this side of the
             * container area will adjust the starting index and area.
             */
            FROM_FRONT,

            /**
             * Indicates the back of the container should be reduced or expanded.  Specifying this side of the container
             * area will only adjust the area.
             */
            FROM_BACK
        };

        /**
         * Constructor.
         *
         * \param[in] startingIndex The file index where the area is expected to start.
         *
         * \param[in] areaSize      The size of the area in file index counts.
         */
        ContainerArea(ChunkHeader::FileIndex startingIndex = 0, ChunkHeader::FileIndex areaSize = 0);

        /**
         * Copy constructor.
         *
         * \param[in] other The instance to be copied.
         */
        ContainerArea(const ContainerArea& other);

        ~ContainerArea();

        /**
         * Method you can use to change the starting file index.  The method will cause the ending index to change to
         * keep the area size constant.
         *
         * \param[in] newStartingIndex The new starting index for the area.
         */
        void setStartingIndex(ChunkHeader::FileIndex newStartingIndex);

        /**
         * Method you can use to obtain the starting index of this area.
         *
         * \return Returns the starting index for the area.
         */
        ChunkHeader::FileIndex startingIndex() const;

        /**
         * Method you can use to change the ending file index.  The method changes the area size rather than the
         * starting index.
         *
         * \param[in] newEndingIndex The new ending index for the area.  The value must be equal to or greater than the
         *                           current starting index.
         *
         * \return Returns true if the ending index is valid.  Returns false if the ending index is invalid.
         */
        bool setEndingIndex(ChunkHeader::FileIndex newEndingIndex);

        /**
         * Method you can use to obtain the ending index of this area.
         *
         * \return Returns the ending index for the area.  The value represents the index of the start of the next
         *         area in the container.
         */
        ChunkHeader::FileIndex endingIndex() const;

        /**
         * Method you can use to change the amount of space reported.
         *
         * \param[in] newAreaSize the new area size in \ref ChunkHeader::FileIndex units.
         */
        void setAreaSize(ChunkHeader::FileIndex newAreaSize);

        /**
         * Method you can use to obtain the size of this area in \ref ChunkHeader::FileIndex units.
         *
         * \return Returns the area size in \ref ChunkHeader::FileIndex units.
         */
        ChunkHeader::FileIndex areaSize() const;

        /**
         * Method that reduces the size of the container area by a specified amount.
         *
         * \param[in] amount The amount to reduce the container area by, in \ref ChunkHeader::FileIndex units.
         *
         * \param[in] side   The side of the container area to be reduced.
         *
         * \return Returns true on success, returns false if the amount of reduction is more than the total avaialble
         *         area.
         */
        bool reduceBy(ChunkHeader::FileIndex amount, Side side);

        /**
         * Method that expands the size of the container area by a specified amount.
         *
         * \param[in] amount The amount to expand the container area by, in \ref ChunkHeader::FileIndex units.
         *
         * \param[in] side   The side of the container area to be expanded.
         *
         * \return Returns true on success, returns false if the amount of expansion would push the starting index below
         *         zero.
         */
        bool expandBy(ChunkHeader::FileIndex amount, Side side);

        /**
         * Assignment operator.
         *
         * \param[in] other The instance to copy.
         *
         * \return Returns a reference to this.
         */
        ContainerArea& operator=(const ContainerArea& other);

        /**
         * Comparison operator.
         *
         * \param[in] other The instance to be compared against.
         *
         * \return Returns true if the instances are identical, returns false if they are different.
         */
        bool operator==(const ContainerArea& other) const;

        /**
         * Comparison operator.
         *
         * \param[in] other The instance to be compared against.
         *
         * \return Returns true if the instances are different, returns false if they are identical.
         */
        bool operator!=(const ContainerArea& other) const;

    private:
        /**
         * The starting index.
         */
        ChunkHeader::FileIndex currentStartingIndex;

        /**
         * The area size.
         */
        ChunkHeader::FileIndex currentAreaSize;
};

#endif
