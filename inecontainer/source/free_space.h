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
* This header defines the \ref FreeSpace class.
***********************************************************************************************************************/

/* .. sphinx-project inecontainer */

#ifndef FREE_SPACE_H
#define FREE_SPACE_H

#include <map>

#include "chunk_header.h"
#include "container_area.h"

class FreeSpaceData;
class FreeSpaceTracker;

/**
 * Class that extends the \ref ContainerArea class to include additional information useful for managing free space.
 */
class FreeSpace:public ContainerArea {
    friend class FreeSpaceTracker;

    private:
        /**
         * Constructor.  Using this constructor will cause the free space area to be marked as valid.
         *
         * \param[in] freeSpacePosition The position in the free space map where this free space region resides.
         *
         * \param[in] startingIndex     The file index where the area is expected to start.
         *
         * \param[in] areaSize          The size of the area in file index counts.
         */
        FreeSpace(
            std::map<ChunkHeader::FileIndex, FreeSpaceData>::iterator& freeSpacePosition,
            ChunkHeader::FileIndex                                     startingIndex = 0,
            ChunkHeader::FileIndex                                     areaSize = 0
        );

        /**
         * Method that returns an iterator to the free set position.
         *
         * \return Returns an iterator into the free set.
         */
        std::map<ChunkHeader::FileIndex, FreeSpaceData>::iterator iterator() const;

    public:
        /**
         * Default constructor.  Marks the free space area as invalid.
         */
        FreeSpace();

        /**
         * Constructor
         *
         * \param[in] other The instance to be copied.
         */
        FreeSpace(const FreeSpace& other);

        ~FreeSpace();

        /**
         * Assignment operator.
         *
         * \param[in] other The instance to be copied.
         *
         * \return Returns an instance to this.
         */
        FreeSpace& operator=(const FreeSpace& other);

        /**
         * Method you can use to determine if this free space area is valid or invalid.
         *
         * \return Returns true if the free space area is valid.  Returns false if the free space area is invalid.
         */
        bool isValid() const;

        /**
         * Method you can use to determine if this free space area is invalid or valid.
         *
         * \return Returns true if the free space area is invalid.  Returns false if the free space area is valid.
         */
        bool isInvalid() const;

    private:
        /**
         * Iterator to the free space instance.
         */
        std::map<ChunkHeader::FileIndex, FreeSpaceData>::iterator position;

        /**
         * Flag used to indicate if this free space area is valid.
         */
        bool currentlyValid;
};

#endif
