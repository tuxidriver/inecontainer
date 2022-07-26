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
* This file implements tests of the FreeSpaceData class.
***********************************************************************************************************************/

#include <QDebug>
#include <QtTest/QtTest>

#include <free_space_data.h>

#include "test_free_space_data.h"

void TestFreeSpaceData::testConstructorsDestructors() {
    FreeSpaceData freeSpaceData1(1, false, false);
    QVERIFY(freeSpaceData1.endingIndex() == 1);
    QVERIFY(freeSpaceData1.isReserved() == false);
    QVERIFY(freeSpaceData1.fileUpdateNeeded() == false);

    FreeSpaceData freeSpaceData2(2, false, true);
    QVERIFY(freeSpaceData2.endingIndex() == 2);
    QVERIFY(freeSpaceData2.isReserved() == false);
    QVERIFY(freeSpaceData2.fileUpdateNeeded() == true);

    FreeSpaceData freeSpaceData3(3, true, false);
    QVERIFY(freeSpaceData3.endingIndex() == 3);
    QVERIFY(freeSpaceData3.isReserved() == true);
    QVERIFY(freeSpaceData3.fileUpdateNeeded() == false);

    FreeSpaceData freeSpaceData4 = freeSpaceData3;
    QVERIFY(freeSpaceData4.endingIndex() == 3);
    QVERIFY(freeSpaceData4.isReserved() == true);
    QVERIFY(freeSpaceData4.fileUpdateNeeded() == false);
}


void TestFreeSpaceData::testAccessors() {
    FreeSpaceData freeSpaceData(1, false, false);
    QVERIFY(freeSpaceData.endingIndex() == 1);
    QVERIFY(freeSpaceData.isReserved() == false);
    QVERIFY(freeSpaceData.fileUpdateNeeded() == false);

    freeSpaceData.setEndingIndex(2);
    QVERIFY(freeSpaceData.endingIndex() == 2);

    freeSpaceData.setReserved();
    QVERIFY(freeSpaceData.isReserved() == true);
    QVERIFY(freeSpaceData.isAvailable() == false);

    freeSpaceData.setReserved(false);
    QVERIFY(freeSpaceData.isReserved() == false);
    QVERIFY(freeSpaceData.isAvailable() == true);

    freeSpaceData.setReserved(true);
    QVERIFY(freeSpaceData.isReserved() == true);
    QVERIFY(freeSpaceData.isAvailable() == false);

    freeSpaceData.setAvailable();
    QVERIFY(freeSpaceData.isReserved() == false);
    QVERIFY(freeSpaceData.isAvailable() == true);

    freeSpaceData.setAvailable(false);
    QVERIFY(freeSpaceData.isReserved() == true);
    QVERIFY(freeSpaceData.isAvailable() == false);

    freeSpaceData.setAvailable(true);
    QVERIFY(freeSpaceData.isReserved() == false);
    QVERIFY(freeSpaceData.isAvailable() == true);

    freeSpaceData.setFileUpdateNeeded();
    QVERIFY(freeSpaceData.fileUpdateNeeded() == true);

    freeSpaceData.setFileUpdateNeeded(false);
    QVERIFY(freeSpaceData.fileUpdateNeeded() == false);

    freeSpaceData.setFileUpdateNeeded(true);
    QVERIFY(freeSpaceData.fileUpdateNeeded() == true);
}

void TestFreeSpaceData::testAssignmentOperator() {
    FreeSpaceData freeSpaceData1(1, false, false);
    QVERIFY(freeSpaceData1.endingIndex() == 1);
    QVERIFY(freeSpaceData1.isReserved() == false);
    QVERIFY(freeSpaceData1.fileUpdateNeeded() == false);

    FreeSpaceData freeSpaceData2(2, true, true);
    QVERIFY(freeSpaceData2.endingIndex() == 2);
    QVERIFY(freeSpaceData2.isReserved() == true);
    QVERIFY(freeSpaceData2.fileUpdateNeeded() == true);

    freeSpaceData1 = freeSpaceData2;
    QVERIFY(freeSpaceData1.endingIndex() == 2);
    QVERIFY(freeSpaceData1.isReserved() == true);
    QVERIFY(freeSpaceData1.fileUpdateNeeded() == true);
}
