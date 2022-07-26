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
* This file implements the \ref FreeSpaceTracker class.
***********************************************************************************************************************/

#include <map>
#include <cassert>

#include "chunk_header.h"
#include "container_area.h"
#include "free_space.h"
#include "free_space_data.h"
#include "free_space_tracker.h"

FreeSpaceTracker::FreeSpaceTracker() {}


FreeSpaceTracker::~FreeSpaceTracker() {}


void FreeSpaceTracker::newFreeSpaceArea(const ContainerArea& containerArea, bool fileUpdateNeeded) {
    newFreeSpaceArea(containerArea.startingIndex(), containerArea.areaSize(), fileUpdateNeeded);
}


void FreeSpaceTracker::newFreeSpaceArea(
        ChunkHeader::FileIndex startingIndex,
        ChunkHeader::FileIndex areaSize,
        bool                   fileUpdateNeeded
    ) {
    // See if we can merge with the previous region.

    ChunkHeader::FileIndex endingIndex            = startingIndex + areaSize;
    bool                   regionFileUpdateNeeded = fileUpdateNeeded;
    bool                   done                   = false;

    FreeMap::iterator next = freeMap.lower_bound(startingIndex);
    if (next != freeMap.begin()) {
        // There is a previous entry (the "next" entry was not the first in the map).

        FreeMap::iterator previous = next;
        --previous;

        ChunkHeader::FileIndex previousStartingIndex = previous->first;
        ChunkHeader::FileIndex previousEndingIndex   = previous->second.endingIndex();

        assert(previousStartingIndex <= startingIndex); // Indicates lower_bound gave us bad results.

        if (previousEndingIndex >= endingIndex) {
            // We've contained fully within the previous entry, we're done.  Nothing to do.
            done = true;
        } else if (previousEndingIndex >= startingIndex) {
            // We overlap with the previous entry.

            if (previous->second.isReserved()) {
                // Previous has an outstanding reservation, we can't delete it so we adjust where we start this entry.
                startingIndex = previousEndingIndex;
            } else {
                // Previous does not have an outstanding reservation, we take its file update status and starting
                // location and then delete the previosu entry as we're going to create a new entry.

                startingIndex          = previousStartingIndex;
                regionFileUpdateNeeded = previous->second.fileUpdateNeeded() || fileUpdateNeeded;
                freeMap.erase(previous);
            }
        } else {
            // There is no overlap, entries are independent.
        }
    } else {
        // No previous entry.
    }

    if (!done) {
        // Try to replace or fill around regions after the insertion point.

        FreeMap::iterator it = next;
        while (it != freeMap.end() && it->first <= endingIndex) {
            assert(it->first > startingIndex);

            ++next;

            if (it->second.isReserved()) {
                // Can're move this region.  Create a new region and add it.
                FreeSpaceData region(it->first, false, regionFileUpdateNeeded);
                freeMap.insert(FreeMapPair(startingIndex, region));

                startingIndex          = it->second.endingIndex();
                regionFileUpdateNeeded = fileUpdateNeeded;
            } else {
                // We can remove this region.

                if (it->second.endingIndex() > endingIndex) {
                    endingIndex = it->second.endingIndex();
                }

                if (it->second.fileUpdateNeeded()) {
                    regionFileUpdateNeeded = true;
                }

                freeMap.erase(it);
            }

            it = next;
        }

        if (startingIndex < endingIndex) {
            FreeSpaceData region(endingIndex, false, regionFileUpdateNeeded);
            freeMap.insert(FreeMapPair(startingIndex, region));
        }
    }
}


FreeSpace FreeSpaceTracker::reserveFreeSpaceArea(
        ChunkHeader::FileIndex startingIndex,
        ChunkHeader::FileIndex minimumChunkSize,
        ChunkHeader::FileIndex desiredChunkSize
    ) {
    if (desiredChunkSize == 0) {
        desiredChunkSize = minimumChunkSize;
    }

    FreeMap::iterator it    = freeMap.lower_bound(startingIndex);
    FreeMap::iterator begin = freeMap.begin();
    FreeMap::iterator end   = freeMap.end();

    if (it != begin && (it == end || it->first > startingIndex)) {
        --it;
    }

    // Find usable split.
    ChunkHeader::FileIndex lowestEndingIndex = startingIndex + minimumChunkSize;
    while (it != end                                                     &&
           (it->second.isReserved()                                 ||
            it->second.endingIndex() - it->first < minimumChunkSize ||
            it->second.endingIndex() < lowestEndingIndex               )    ) {
        ++it;
    }

    FreeMap::iterator      freeSpaceIterator;
    ChunkHeader::FileIndex allocationStartingIndex;
    ChunkHeader::FileIndex allocationEndingIndex;
    ChunkHeader::FileIndex allocationAreaSize;

    if (it != end) {
        // We found a usable split.
        ChunkHeader::FileIndex regionStartingIndex = it->first;
        ChunkHeader::FileIndex regionEndingIndex   = it->second.endingIndex();

        bool splitLeft = regionStartingIndex < startingIndex;
        allocationStartingIndex = splitLeft ? startingIndex : regionStartingIndex;

        bool splitRight = allocationStartingIndex + desiredChunkSize < regionEndingIndex;
        allocationEndingIndex = splitRight ? allocationStartingIndex + desiredChunkSize : regionEndingIndex;

        allocationAreaSize = allocationEndingIndex - allocationStartingIndex;

        if (splitLeft) {
            it->second.setEndingIndex(allocationStartingIndex);
            it->second.setFileUpdateNeeded();

            FreeSpaceData newRegion(allocationEndingIndex, true, true);
            freeSpaceIterator = freeMap.insert(FreeMapPair(allocationStartingIndex, newRegion)).first;
        } else {
            it->second.setEndingIndex(allocationEndingIndex);
            it->second.setReserved();
            it->second.setFileUpdateNeeded();

            freeSpaceIterator = it;
        }

        if (splitRight) {
            freeMap.insert(FreeMapPair(allocationEndingIndex, FreeSpaceData(regionEndingIndex, false, true)));
        }
    } else {
        // No usable split.  Add new free space at the end of the file.

        allocationStartingIndex = ChunkHeader::toFileIndex(size());
        allocationAreaSize      = desiredChunkSize;
        allocationEndingIndex   = allocationStartingIndex + allocationAreaSize;

        FreeSpaceData allocatedRegion(allocationEndingIndex, true, true);
        freeSpaceIterator = freeMap.insert(FreeMapPair(allocationStartingIndex, allocatedRegion)).first;
    }

    return FreeSpace(freeSpaceIterator, allocationStartingIndex, allocationAreaSize);
}


void FreeSpaceTracker::releaseReservation(const FreeSpace& freeSpaceRegion) {
    assert(freeSpaceRegion.isValid());

    if (freeSpaceRegion.areaSize() == 0 || freeSpaceRegion.startingIndex() >= ChunkHeader::toFileIndex(size())) {
        // Normal case, we've used the region.  Remove from the map.
        // Alternate case, remaining free space at or past the EOF.  Remove from the map.

        freeMap.erase(freeSpaceRegion.iterator());
    } else {
        // Case where we ended up not using the entire free space region.  Generally will only happen if we reach the
        // end of a virtual file.

        FreeMap::iterator released = freeSpaceRegion.iterator();

        if (released->first != freeSpaceRegion.startingIndex()) {
            // We've adjusted the base pointer so we need to remove and re-insert the entry.

            std::pair<FreeMap::iterator, bool> insertResult = freeMap.insert(
                FreeMapPair(freeSpaceRegion.startingIndex(), FreeSpaceData(released->second.endingIndex(), false, true))
            );

            assert(insertResult.second); // False indicates failure.
            freeMap.erase(released);

            released = insertResult.first;
        } else {
            released->second.setAvailable();
        }

        // See if we can merge with the previous entry.
        if (released != freeMap.begin()) {
            FreeMap::iterator previous = released;
            --previous;

            if (previous->second.isAvailable() && previous->second.endingIndex() >= released->first) {
                // Yes, merge them together.

                assert(previous->second.endingIndex() == released->first); // Map is corrupted.

                previous->second.setEndingIndex(released->second.endingIndex());

                freeMap.erase(released);
                released = previous;
            }
        }

        // See if we can merge with the next entry.
        FreeMap::iterator next = released;
        ++next;

        if (next != freeMap.end()) {
            if (next->second.isAvailable() && released->second.endingIndex() >= next->first) {
                // Yes, merge them together.

                assert(released->second.endingIndex() == next->first); // Map is corrupted.

                released->second.setEndingIndex(next->second.endingIndex());
                freeMap.erase(next);
            }
        }

        released->second.setFileUpdateNeeded();
    }
}


unsigned long FreeSpaceTracker::numberFreeSpaceRegions() const {
    return static_cast<unsigned long>(freeMap.size());
}


unsigned long FreeSpaceTracker::numberReservations() const {
    unsigned long count = 0;

    for (FreeMap::const_iterator pos=freeMap.begin(),end=freeMap.end() ; pos!=end ; ++pos) {
        if (pos->second.isReserved()) {
            ++count;
        }
    }

    return count;
}


bool FreeSpaceTracker::flushFreeSpace(bool flushAll) {
    bool success = true;
    FreeMap::iterator pos = freeMap.begin();
    FreeMap::iterator end = freeMap.end();

    while (success && pos != end) {
        if (flushAll || pos->second.fileUpdateNeeded()) {
            pos->second.setFileUpdateNeeded(false);
            success = flushArea(ContainerArea(pos->first, pos->second.endingIndex() - pos->first));
        }

        ++pos;
    }

    return success;
}


void FreeSpaceTracker::clearFreeSpace() {
    freeMap.clear();
}
