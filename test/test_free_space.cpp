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
* This file implements tests of the FreeSpace class.
***********************************************************************************************************************/

#include <QDebug>
#include <QtTest/QtTest>

#include <map>

#include <free_space_data.h>
#include <free_space.h>

#include "test_free_space.h"

/***********************************************************************************************************************
 * FreeSpaceTracker
 */

class FreeSpaceTracker:public FreeSpace {
    public:
        static FreeSpace createInstance(
            std::map<ChunkHeader::FileIndex, FreeSpaceData>::iterator& freeSpacePosition,
            ChunkHeader::FileIndex                                     startingIndex = 0,
            ChunkHeader::FileIndex                                     areaSize = 0
        );

        static std::map<ChunkHeader::FileIndex, FreeSpaceData>::iterator iterator(FreeSpace& freeSpace);
};


FreeSpace FreeSpaceTracker::createInstance(
        std::map<ChunkHeader::FileIndex, FreeSpaceData>::iterator& freeSpacePosition,
        ChunkHeader::FileIndex                                     startingIndex,
        ChunkHeader::FileIndex                                     areaSize
    ) {
    return FreeSpace(freeSpacePosition, startingIndex, areaSize);
}


std::map<ChunkHeader::FileIndex, FreeSpaceData>::iterator FreeSpaceTracker::iterator(FreeSpace& freeSpace) {
    return freeSpace.iterator();
}

/***********************************************************************************************************************
 * TestFreeSpace
 */

void TestFreeSpace::testConstructorsDestructors() {
    std::map<ChunkHeader::FileIndex, FreeSpaceData> freeMap;
    std::map<ChunkHeader::FileIndex, FreeSpaceData>::iterator begin = freeMap.begin();
    FreeSpace freeSpace = FreeSpaceTracker::createInstance(begin, 1, 2);

    QVERIFY(freeSpace.startingIndex() == 1);
    QVERIFY(freeSpace.areaSize() == 2);
    QVERIFY(FreeSpaceTracker::iterator(freeSpace) == begin);
}


void TestFreeSpace::testAccessors() {
    std::map<ChunkHeader::FileIndex, FreeSpaceData> freeMap;
    std::map<ChunkHeader::FileIndex, FreeSpaceData>::iterator begin = freeMap.begin();
    FreeSpace freeSpace = FreeSpaceTracker::createInstance(begin, 0, 0);

    QVERIFY(FreeSpaceTracker::iterator(freeSpace) == begin);

    QVERIFY(freeSpace.startingIndex() == 0);
    QVERIFY(freeSpace.areaSize() == 0);
    QVERIFY(freeSpace.endingIndex() == 0);

    freeSpace.setStartingIndex(2);
    freeSpace.setAreaSize(3);

    QVERIFY(freeSpace.startingIndex() == 2);
    QVERIFY(freeSpace.areaSize() == 3);
    QVERIFY(freeSpace.endingIndex() == 5);

    freeSpace.setEndingIndex(3);
    QVERIFY(freeSpace.startingIndex() == 2);
    QVERIFY(freeSpace.areaSize() == 1);
    QVERIFY(freeSpace.endingIndex() == 3);
}


void TestFreeSpace::testExpansionReduction() {
    std::map<ChunkHeader::FileIndex, FreeSpaceData> freeMap;
    std::map<ChunkHeader::FileIndex, FreeSpaceData>::iterator begin = freeMap.begin();
    FreeSpace freeSpace = FreeSpaceTracker::createInstance(begin, 10, 20);

    QVERIFY(FreeSpaceTracker::iterator(freeSpace) == begin);

    QVERIFY(freeSpace.startingIndex() == 10);
    QVERIFY(freeSpace.areaSize() == 20);

    freeSpace.reduceBy(10, ContainerArea::Side::FROM_FRONT);
    QVERIFY(freeSpace.startingIndex() == 20);
    QVERIFY(freeSpace.areaSize() == 10);

    freeSpace.expandBy(10, ContainerArea::Side::FROM_FRONT);
    QVERIFY(freeSpace.startingIndex() == 10);
    QVERIFY(freeSpace.areaSize() == 20);

    freeSpace.expandBy(10, ContainerArea::Side::FROM_BACK);
    QVERIFY(freeSpace.startingIndex() == 10);
    QVERIFY(freeSpace.areaSize() == 30);

    freeSpace.reduceBy(10, ContainerArea::Side::FROM_BACK);
    QVERIFY(freeSpace.startingIndex() == 10);
    QVERIFY(freeSpace.areaSize() == 20);
}


void TestFreeSpace::testAssignmentOperator() {
    std::map<ChunkHeader::FileIndex, FreeSpaceData> freeMap;
    freeMap.insert(std::pair<ChunkHeader::FileIndex, FreeSpaceData>(0, FreeSpaceData(1, false, false)));

    std::map<ChunkHeader::FileIndex, FreeSpaceData>::iterator begin = freeMap.begin();
    std::map<ChunkHeader::FileIndex, FreeSpaceData>::iterator end = freeMap.end();
    FreeSpace freeSpace1 = FreeSpaceTracker::createInstance(begin, 1, 2);
    FreeSpace freeSpace2 = FreeSpaceTracker::createInstance(end, 3, 4);

    QVERIFY(FreeSpaceTracker::iterator(freeSpace1) == begin);
    QVERIFY(freeSpace1.startingIndex() == 1);
    QVERIFY(freeSpace1.areaSize() == 2);

    QVERIFY(FreeSpaceTracker::iterator(freeSpace2) == end);
    QVERIFY(freeSpace2.startingIndex() == 3);
    QVERIFY(freeSpace2.areaSize() == 4);

    freeSpace1 = freeSpace2;

    QVERIFY(FreeSpaceTracker::iterator(freeSpace1) == end);
    QVERIFY(freeSpace1.startingIndex() == 3);
    QVERIFY(freeSpace1.areaSize() == 4);
}
