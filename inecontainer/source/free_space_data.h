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
* This header defines the \ref FreeSpaceData class.
***********************************************************************************************************************/

/* .. sphinx-project inecontainer */

#ifndef FREE_SPACE_DATA_H
#define FREE_SPACE_DATA_H

#include "chunk_header.h"

/**
 * Class that is used by the \ref FreeSpaceTracker class to track free space within the container.
 */
class FreeSpaceData {
    public:
        /**
         * Constructor.
         *
         * \param[in] endingIndex     The size of the area in file index counts.
         *
         * \param[in] reserved        If true, this free space region is reserved and in use.  If false, the free space
         *                            region can be freely used.
         *
         * \param[in] fileNeedsUpdate If true, the container needs to be written to mark this free space area as
         *                            available.  If false, the file already has this area marked as free.
         */
        FreeSpaceData(ChunkHeader::FileIndex endingIndex = 0, bool reserved = false, bool fileNeedsUpdate = false);

        /**
         * Copy constructor.
         *
         * \param[in] other The instance to be copied.
         */
        FreeSpaceData(const FreeSpaceData& other);

        ~FreeSpaceData();

        /**
         * Method that can be used to change the ending index of this free space region.
         *
         * \param[in] newEndingIndex The new ending index for this free space region.
         */
        void setEndingIndex(ChunkHeader::FileIndex newEndingIndex);

        /**
         * Method that can be used to obtain the ending index for this free space region.
         *
         * \return Returns the ending index tied to this free space region.
         */
        ChunkHeader::FileIndex endingIndex() const;

        /**
         * Method that can be used to indicate that this free space is reserved or available.
         *
         * \param[in] nowReserved If true, the free space is reserved.  If false, the free space is available to be
         *                        allocated.
         */
        void setReserved(bool nowReserved = true);

        /**
         * Method that can be used to indicate that this free space is available or reserved.
         *
         * \param[in] nowAvailable If true, the free space is available.  If false, the free space is reserved and
         *                         should not be allocated to other uses.
         */
        void setAvailable(bool nowAvailable = true);

        /**
         * Method that can be used to detemrine if this free space region is reserved.
         *
         * \return Returns true if the free space region is reserved.  Returns false if the free space region is
         *         available.
         */
        bool isReserved() const;

        /**
         * Method that can be used to detemrine if this free space region is available.
         *
         * \return Returns true if the free space region is available.  Returns false if the free space region is
         *         reserved.
         */
        bool isAvailable() const;

        /**
         * Method that sets or clears the file update flag.
         *
         * \param[in] nowFileUpdateNeeded If true, the file needs to be updated.  if false, the file does not need to be
         *                                updated.
         */
        void setFileUpdateNeeded(bool nowFileUpdateNeeded = true);

        /**
         * Method that indicates if a file update is needed.
         *
         * \return Returns true if a file update is needed.  Returns false if a file update is not needed.
         */
        bool fileUpdateNeeded() const;

        /**
         * Assignment operator.
         *
         * \param[in] other The instance to be copied.
         *
         * \return A reference to this class instance.
         */
        FreeSpaceData& operator=(const FreeSpaceData& other);

    private:
        /**
         * The ending index of this free space region.
         */
        ChunkHeader::FileIndex currentEndingIndex;

        /**
         * Flag used to indicate if this region is reserved.
         */
        bool currentlyReserved;

        /**
         * Flag used to indicate if the region needs to be written to the media.
         */
        bool currentFileNeedsUpdate;
};

#endif
