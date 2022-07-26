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
* This header defines the \ref FreeSpaceTracker class.
***********************************************************************************************************************/

/* .. sphinx-project inecontainer */

#ifndef FREE_SPACE_TRACKER_H
#define FREE_SPACE_TRACKER_H

#include <map>
#include <utility>

#include "chunk_header.h"
#include "container_area.h"
#include "free_space.h"

class FreeSpaceData;

/**
 * Pure virtual class that maintains a database of free space regions within a container.  The class includes methods
 * you can use to register discovered or newly created free space regions, request and reserve free space regions, and
 * remove free space regions from the tracker.
 */
class FreeSpaceTracker {
    public:
        FreeSpaceTracker();

        virtual ~FreeSpaceTracker();

        /**
         * Method that can be used to report a newly discovered free-space region.  The method will automatically merge
         * adjacent free space regions so you can safely call this method with adjacent chunks of data.
         *
         * \param[in] containerArea    The container area where the free space resides.
         *
         * \param[in] fileUpdateNeeded If true, a file update is needed for this newly added area.
         */
        void newFreeSpaceArea(const ContainerArea& containerArea, bool fileUpdateNeeded = false);

        /**
         * Convenience method that can be used to report a newly discovered free-space region.  The method will
         * automatically merge adjacent free space regions so you can safely call this method with adjacent chunks of
         * data.
         *
         * \param[in] startingIndex    The file index where the area is expected to start.
         *
         * \param[in] areaSize         The size of the area in file index counts.
         *
         * \param[in] fileUpdateNeeded If true, a file update is needed for this newly added area.
         */
        void newFreeSpaceArea(
            ChunkHeader::FileIndex startingIndex,
            ChunkHeader::FileIndex areaSize,
            bool                   fileUpdateNeeded = false
        );

        /**
         * Method that identifies the first available free location in the file at or after a specfied file index.
         *
         * \param[in] startingIndex    The lowest allowed file index.  This method will never return an index less than
         *                             this value.
         *
         * \param[in] minimumChunkSize The minimum allowed chunk size in file index values.
         *
         * \param[in] desiredChunkSize The ideal chunk size in file index values.  A value of 0 indicates that the
         *                             minimum chunk size is the desired chunk size.
         *
         * \return Returns an instance of \ref FreeSpace that can be used.
         */
        FreeSpace reserveFreeSpaceArea(
            ChunkHeader::FileIndex startingIndex,
            ChunkHeader::FileIndex minimumChunkSize,
            ChunkHeader::FileIndex desiredChunkSize = 0
        );

        /**
         * Method that releases a reserved free space area.  You must call this method on every free space area you
         * reserve even if that area has been fully consumed.
         *
         * \param[in] freeSpaceRegion A reference to the reserved free space region.
         */
        void releaseReservation(const FreeSpace& freeSpaceRegion);

        /**
         * Method that indicates the number of independently tracked free space regions.  The method exists primarily
         * for testing purposes.
         *
         * An independent region will be created when:
         *
         *     * An area is reserved that can not be described fully by an existing free space map entry.  In this
         *       scenario, the entry in the map is split into two or three sections with the reserved area marked.
         *
         *     * There is a discrete free space region.  Free space regions that butt against each other and are not
         *       reserved are combined.
         *
         * \return Returns The number of distinct free space regions.
         */
        unsigned long numberFreeSpaceRegions() const;

        /**
         * Method that indicates the number of reserved free space regions.  The method exists primarily for testing
         * purposes.
         *
         * \return Returns the number of reserved free space regions.
         */
        unsigned long numberReservations() const;

        /**
         * Pure virtual method that can be used to determine the current container size, in bytes.  The method is used
         * to determine the first location where free space can be freely allocated.
         *
         * \return Returns the size of the container, in bytes.  A value of -1 is returned on error.
         */
        virtual long long size() = 0;

        /**
         * Method that can be called to write all the free space regions to the media.
         *
         * \param[in] flushAll If true, all regions will be flushed whether or not they are marked as requiring an
         *                     update.  If false, only regions marked as requiring an update will be flushed to the
         *                     media.
         */
        bool flushFreeSpace(bool flushAll = false);

    protected:
        /**
         * Pure virtual method that is called to trigger each region to be flushed.
         *
         * \param[in] area The area or region to be flushed.
         *
         * \return Returns true on success, returns false on error.
         */
        virtual bool flushArea(const ContainerArea& area) = 0;

        /**
         * Method that can be called to clear all the available free space data.
         */
        void clearFreeSpace();

    private:
        /**
         * Type used to track free space.
         */
        typedef std::map<ChunkHeader::FileIndex, FreeSpaceData> FreeMap;

        /**
         * Type used to create entries in the free map.
         */
        typedef std::pair<ChunkHeader::FileIndex, FreeSpaceData> FreeMapPair;

        /**
         * Set used to track free space reservations.
         */
        FreeMap freeMap;
};

#endif
