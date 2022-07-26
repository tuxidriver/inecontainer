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
* This file implements tests of the FreeSpaceTracker class.
***********************************************************************************************************************/

#include <QDebug>
#include <QtTest/QtTest>

#include <vector>

#include <free_space.h>
#include <container_area.h>
#include <free_space_tracker.h>

#include "test_free_space_tracker.h"

/***********************************************************************************************************************
 * FreeSpaceTrackerWrapper
 */

class FreeSpaceTrackerWrapper:public FreeSpaceTracker {
    public:
        FreeSpaceTrackerWrapper(unsigned long long containerSize);

        long long size() final;

        std::vector<ContainerArea> flushedAreas() const;

        void clearFlushedAreas();

        void setVerboseFlush(bool nowVerbose = true);

        bool verboseFlush() const;

        bool flushFreeSpace(bool flushAll = true);

    protected:
        bool flushArea(const ContainerArea& area) final;

    private:
        unsigned long long         currentContainerSize;
        bool                       currentVerboseFlush;
        std::vector<ContainerArea> currentFlushedAreas;
};


FreeSpaceTrackerWrapper::FreeSpaceTrackerWrapper(unsigned long long containerSize) {
    currentContainerSize = containerSize;
    currentVerboseFlush  = false;
}


long long FreeSpaceTrackerWrapper::size() {
    return currentContainerSize;
}


std::vector<ContainerArea> FreeSpaceTrackerWrapper::flushedAreas() const {
    return currentFlushedAreas;
}


void FreeSpaceTrackerWrapper::clearFlushedAreas() {
    currentFlushedAreas.clear();
}


void FreeSpaceTrackerWrapper::setVerboseFlush(bool nowVerbose) {
    currentVerboseFlush = nowVerbose;
}


bool FreeSpaceTrackerWrapper::verboseFlush() const {
    return currentVerboseFlush;
}


bool FreeSpaceTrackerWrapper::flushFreeSpace(bool flushAll) {
    return FreeSpaceTracker::flushFreeSpace(flushAll);
}


bool FreeSpaceTrackerWrapper::flushArea(const ContainerArea& area) {
    if (currentVerboseFlush) {
        qDebug() << "Area " << area.startingIndex() << " - " << area.endingIndex() - 1;
    }

    currentFlushedAreas.push_back(area);
    return true;
}

/***********************************************************************************************************************
 * TestFreeSpaceTracker
 */

void TestFreeSpaceTracker::testNewFreeSpaceArea() {
    FreeSpaceTrackerWrapper tracker(110);

    //                                                                                                     1         1
    // 0         1         2         3         4         5         6         7         8         9         0         1
    // 012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890
    //            *********
    //            aaaaaaaaa

    tracker.newFreeSpaceArea(11,  9);
    QVERIFY(tracker.numberFreeSpaceRegions() == 1);
    QVERIFY(tracker.numberReservations() == 0);

    tracker.clearFlushedAreas();
    tracker.flushFreeSpace(false);

    QVERIFY(tracker.flushedAreas().size() == 0);

    tracker.clearFlushedAreas();
    tracker.flushFreeSpace(true);

    QVERIFY(tracker.flushedAreas().size() == 1);
    QVERIFY(tracker.flushedAreas()[0] == ContainerArea(11, 9));

    //                                                                                                     1         1
    // 0         1         2         3         4         5         6         7         8         9         0         1
    // 012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890
    //           #aaaaaaaaa
    //           aaaaaaaaaa

    tracker.newFreeSpaceArea(10, 1, true);
    QVERIFY(tracker.numberFreeSpaceRegions() == 1);
    QVERIFY(tracker.numberReservations() == 0);

    tracker.clearFlushedAreas();
    tracker.flushFreeSpace(false);

    QVERIFY(tracker.flushedAreas().size() == 1);
    QVERIFY(tracker.flushedAreas()[0] == ContainerArea(10, 10));

    tracker.clearFlushedAreas();
    tracker.flushFreeSpace(true);

    QVERIFY(tracker.flushedAreas().size() == 1);
    QVERIFY(tracker.flushedAreas()[0] == ContainerArea(10, 10));

    //                                                                                                     1         1
    // 0         1         2         3         4         5         6         7         8         9         0         1
    // 012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890
    //           aaaaaaaaaa**********
    //           aaaaaaaaaaaaaaaaaaaa
    //

    tracker.newFreeSpaceArea(20, 10); // Should merge with previous.
    QVERIFY(tracker.numberFreeSpaceRegions() == 1);
    QVERIFY(tracker.numberReservations() == 0);

    tracker.clearFlushedAreas();
    tracker.flushFreeSpace(false);

    QVERIFY(tracker.flushedAreas().size() == 0); // 0 because added region not marked as update needed.

    tracker.clearFlushedAreas();
    tracker.flushFreeSpace(true);

    QVERIFY(tracker.flushedAreas().size() == 1);
    QVERIFY(tracker.flushedAreas()[0] == ContainerArea(10, 20));

    //                                                                                                     1         1
    // 0         1         2         3         4         5         6         7         8         9         0         1
    // 012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890
    //           aaaaa**********aaaaa
    //           aaaaaaaaaaaaaaaaaaaa

    tracker.newFreeSpaceArea(15, 10); // Should just be obsorbed by the entry starting at 10.
    QVERIFY(tracker.numberFreeSpaceRegions() == 1);
    QVERIFY(tracker.numberReservations() == 0);

    tracker.clearFlushedAreas();
    tracker.flushFreeSpace(false);

    QVERIFY(tracker.flushedAreas().size() == 0);

    tracker.clearFlushedAreas();
    tracker.flushFreeSpace(true);

    QVERIFY(tracker.flushedAreas().size() == 1);
    QVERIFY(tracker.flushedAreas()[0] == ContainerArea(10, 20));

    //                                                                                                     1         1
    // 0         1         2         3         4         5         6         7         8         9         0         1
    // 012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890
    //           aaaaaaaaaaaaaaaaaaaa                    **********
    //           aaaaaaaaaaaaaaaaaaaa                    bbbbbbbbbb

    tracker.newFreeSpaceArea(50, 10);
    QVERIFY(tracker.numberFreeSpaceRegions() == 2);
    QVERIFY(tracker.numberReservations() == 0);

    tracker.clearFlushedAreas();
    tracker.flushFreeSpace(true);

    QVERIFY(tracker.flushedAreas().size() == 2);
    QVERIFY(tracker.flushedAreas()[0] == ContainerArea(10, 20));
    QVERIFY(tracker.flushedAreas()[1] == ContainerArea(50, 10));

    //                                                                                                     1         1
    // 0         1         2         3         4         5         6         7         8         9         0         1
    // 012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890
    //           aaaaaaaaaaaaaaaaaaaa                    bbbbbbbbbb          *
    //           aaaaaaaaaaaaaaaaaaaa                    bbbbbbbbbb          c

    tracker.newFreeSpaceArea(70,  1);
    QVERIFY(tracker.numberFreeSpaceRegions() == 3);
    QVERIFY(tracker.numberReservations() == 0);

    tracker.clearFlushedAreas();
    tracker.flushFreeSpace(true);

    QVERIFY(tracker.flushedAreas().size() == 3);
    QVERIFY(tracker.flushedAreas()[0] == ContainerArea(10, 20));
    QVERIFY(tracker.flushedAreas()[1] == ContainerArea(50, 10));
    QVERIFY(tracker.flushedAreas()[2] == ContainerArea(70,  1));

    //                                                                                                     1         1
    // 0         1         2         3         4         5         6         7         8         9         0         1
    // 012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890
    //           aaaaaaaaaaaaaaaaaaaa                    bbbbbbbbbb          c  *
    //           aaaaaaaaaaaaaaaaaaaa                    bbbbbbbbbb          c  d

    tracker.newFreeSpaceArea(73,  1);
    QVERIFY(tracker.numberFreeSpaceRegions() == 4);
    QVERIFY(tracker.numberReservations() == 0);

    tracker.clearFlushedAreas();
    tracker.flushFreeSpace(true);

    QVERIFY(tracker.flushedAreas().size() == 4);
    QVERIFY(tracker.flushedAreas()[0] == ContainerArea(10, 20));
    QVERIFY(tracker.flushedAreas()[1] == ContainerArea(50, 10));
    QVERIFY(tracker.flushedAreas()[2] == ContainerArea(70,  1));
    QVERIFY(tracker.flushedAreas()[3] == ContainerArea(73,  1));

    //                                                                                                     1         1
    // 0         1         2         3         4         5         6         7         8         9         0         1
    // 012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890
    //           aaaaaaaaaaaaaaaaaaaa                    bbbbbbbbbb          c  D

    FreeSpace reservedArea = tracker.reserveFreeSpaceArea(73, 1);
    QVERIFY(tracker.numberFreeSpaceRegions() == 4);
    QVERIFY(tracker.numberReservations() == 1);
    QVERIFY(reservedArea.startingIndex() == 73);
    QVERIFY(reservedArea.areaSize() == 1);

    tracker.clearFlushedAreas();
    tracker.flushFreeSpace(true);

    QVERIFY(tracker.flushedAreas().size() == 4);
    QVERIFY(tracker.flushedAreas()[0] == ContainerArea(10, 20));
    QVERIFY(tracker.flushedAreas()[1] == ContainerArea(50, 10));
    QVERIFY(tracker.flushedAreas()[2] == ContainerArea(70,  1));
    QVERIFY(tracker.flushedAreas()[3] == ContainerArea(73,  1));

    //                                                                                                     1         1
    // 0         1         2         3         4         5         6         7         8         9         0         1
    // 012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890
    //           aaaaaaaaaaaaaaaaaaaa                    bbbbbbbbbb          c  D *
    //           aaaaaaaaaaaaaaaaaaaa                    bbbbbbbbbb          c  D e

    tracker.newFreeSpaceArea(75,  1);
    QVERIFY(tracker.numberFreeSpaceRegions() == 5);
    QVERIFY(tracker.numberReservations() == 1);

    tracker.clearFlushedAreas();
    tracker.flushFreeSpace(true);

    QVERIFY(tracker.flushedAreas().size() == 5);
    QVERIFY(tracker.flushedAreas()[0] == ContainerArea(10, 20));
    QVERIFY(tracker.flushedAreas()[1] == ContainerArea(50, 10));
    QVERIFY(tracker.flushedAreas()[2] == ContainerArea(70,  1));
    QVERIFY(tracker.flushedAreas()[3] == ContainerArea(73,  1));
    QVERIFY(tracker.flushedAreas()[4] == ContainerArea(75,  1));

    //                                                                                                     1         1
    // 0         1         2         3         4         5         6         7         8         9         0         1
    // 012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890
    //           aaaaaaaaaaaaaaaaaaaa                    bbbbbbbbbb**********c  D *
    //           aaaaaaaaaaaaaaaaaaaa                    bbbbbbbbbbbbbbbbbbbbb  C d

    tracker.newFreeSpaceArea(60, 10);
    QVERIFY(tracker.numberFreeSpaceRegions() == 4);
    QVERIFY(tracker.numberReservations() == 1);

    tracker.clearFlushedAreas();
    tracker.flushFreeSpace(true);

    QVERIFY(tracker.flushedAreas().size() == 4);
    QVERIFY(tracker.flushedAreas()[0] == ContainerArea(10, 20));
    QVERIFY(tracker.flushedAreas()[1] == ContainerArea(50, 21));
    QVERIFY(tracker.flushedAreas()[2] == ContainerArea(73,  1));
    QVERIFY(tracker.flushedAreas()[3] == ContainerArea(75,  1));

    //                                                                                                     1         1
    // 0         1         2         3         4         5         6         7         8         9         0         1
    // 012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890
    //           aaaaaaaaaaaaaaaaaaaa                    bbbbbbbbbbbbbbbbbbbbb #C#d
    //           aaaaaaaaaaaaaaaaaaaa                    bbbbbbbbbbbbbbbbbbbbb cDee
    //                                                                         ^ ^^

    tracker.newFreeSpaceArea(72, 3, true);
    QVERIFY(tracker.numberFreeSpaceRegions() == 5);
    QVERIFY(tracker.numberReservations() == 1);

    tracker.clearFlushedAreas();
    tracker.flushFreeSpace(false);

    QVERIFY(tracker.flushedAreas().size() == 2);
    QVERIFY(tracker.flushedAreas()[0] == ContainerArea(72,  1));
    QVERIFY(tracker.flushedAreas()[1] == ContainerArea(74,  2));

    tracker.clearFlushedAreas();
    tracker.flushFreeSpace(true);

    QVERIFY(tracker.flushedAreas().size() == 5);
    QVERIFY(tracker.flushedAreas()[0] == ContainerArea(10, 20));
    QVERIFY(tracker.flushedAreas()[1] == ContainerArea(50, 21));
    QVERIFY(tracker.flushedAreas()[2] == ContainerArea(72,  1));
    QVERIFY(tracker.flushedAreas()[3] == ContainerArea(73,  1));
    QVERIFY(tracker.flushedAreas()[4] == ContainerArea(74,  2));
}


void TestFreeSpaceTracker::testReserveAndReleaseFreeSpaceArea() {
    FreeSpaceTrackerWrapper tracker(ChunkHeader::toPosition(110));

    //                                                                                                     1         1
    // 0         1         2         3         4         5         6         7         8         9         0         1
    // 012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890
    //           aaaaaaaaaaaaaaaaaaaa                    bbbbbbbbbb          c  d e  f

    tracker.newFreeSpaceArea(10, 20);
    tracker.newFreeSpaceArea(50, 10);
    tracker.newFreeSpaceArea(70,  1);
    tracker.newFreeSpaceArea(73,  1);
    tracker.newFreeSpaceArea(75,  1);
    tracker.newFreeSpaceArea(78,  1);

    // **************************************************************
    // Reserve a single distinct region in full.

    FreeSpace reserved(tracker.reserveFreeSpaceArea(70, 1));
    QVERIFY(reserved.startingIndex() == 70);
    QVERIFY(reserved.areaSize() == 1);
    QVERIFY(tracker.numberReservations() == 1);
    QVERIFY(tracker.numberFreeSpaceRegions() == 6);

    // And release

    tracker.releaseReservation(reserved);

    QVERIFY(tracker.numberReservations() == 0);
    QVERIFY(tracker.numberFreeSpaceRegions() == 6);

    // **************************************************************
    // Reserve a single distinct region after a search.

    reserved = tracker.reserveFreeSpaceArea(60, 1);
    QVERIFY(reserved.startingIndex() == 70);
    QVERIFY(reserved.areaSize() == 1);
    QVERIFY(tracker.numberReservations() == 1);
    QVERIFY(tracker.numberFreeSpaceRegions() == 6);

    // And release

    tracker.releaseReservation(reserved);

    QVERIFY(tracker.numberReservations() == 0);
    QVERIFY(tracker.numberFreeSpaceRegions() == 6);

    // **************************************************************
    // Reserve the first portion of a region.

    reserved = tracker.reserveFreeSpaceArea(10, 10);
    QVERIFY(reserved.startingIndex() == 10);
    QVERIFY(reserved.areaSize() == 10);
    QVERIFY(tracker.numberReservations() == 1);
    QVERIFY(tracker.numberFreeSpaceRegions() == 7);

    // And release

    tracker.releaseReservation(reserved);

    QVERIFY(tracker.numberReservations() == 0);
    QVERIFY(tracker.numberFreeSpaceRegions() == 6);

    // **************************************************************
    // Reserve the first portion of a region after a search.

    reserved = tracker.reserveFreeSpaceArea(40, 8);
    QVERIFY(reserved.startingIndex() == 50);
    QVERIFY(reserved.areaSize() == 8);
    QVERIFY(tracker.numberReservations() == 1);
    QVERIFY(tracker.numberFreeSpaceRegions() == 7);

    // And release

    tracker.releaseReservation(reserved);

    QVERIFY(tracker.numberReservations() == 0);
    QVERIFY(tracker.numberFreeSpaceRegions() == 6);

    // **************************************************************
    // Reserve the last portion of a region.

    reserved = tracker.reserveFreeSpaceArea(52, 2, 8);
    QVERIFY(reserved.startingIndex() == 52);
    QVERIFY(reserved.areaSize() == 8);
    QVERIFY(tracker.numberReservations() == 1);
    QVERIFY(tracker.numberFreeSpaceRegions() == 7);

    // And release

    tracker.releaseReservation(reserved);

    QVERIFY(tracker.numberReservations() == 0);
    QVERIFY(tracker.numberFreeSpaceRegions() == 6);

    // **************************************************************
    // Reserve the middle of a region.

    reserved = tracker.reserveFreeSpaceArea(51, 2, 8);
    QVERIFY(reserved.startingIndex() == 51);
    QVERIFY(reserved.areaSize() == 8);
    QVERIFY(tracker.numberReservations() == 1);
    QVERIFY(tracker.numberFreeSpaceRegions() == 8);

    // And release

    tracker.releaseReservation(reserved);

    QVERIFY(tracker.numberReservations() == 0);
    QVERIFY(tracker.numberFreeSpaceRegions() == 6);

    // **************************************************************
    // Reserve from end of file.

    reserved = tracker.reserveFreeSpaceArea(110, 8);
    QVERIFY(reserved.startingIndex() == 110);
    QVERIFY(reserved.areaSize() == 8);
    QVERIFY(tracker.numberReservations() == 1);
    QVERIFY(tracker.numberFreeSpaceRegions() == 7);

    // And release

    reserved.setAreaSize(0);
    tracker.releaseReservation(reserved);

    QVERIFY(tracker.numberReservations() == 0);
    QVERIFY(tracker.numberFreeSpaceRegions() == 6);

    // **************************************************************
    // Reserve from end of file due to no available regions.

    reserved = tracker.reserveFreeSpaceArea(53, 8);
    QVERIFY(reserved.startingIndex() == 110);
    QVERIFY(reserved.areaSize() == 8);
    QVERIFY(tracker.numberReservations() == 1);
    QVERIFY(tracker.numberFreeSpaceRegions() == 7);

    // And release

    tracker.releaseReservation(reserved);

    QVERIFY(tracker.numberReservations() == 0);
    QVERIFY(tracker.numberFreeSpaceRegions() == 6);
}
