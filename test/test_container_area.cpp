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
* This file implements tests of the ContainerArea class.
***********************************************************************************************************************/

#include <QDebug>
#include <QtTest/QtTest>

#include <container_area.h>

#include "test_container_area.h"

void TestContainerArea::testConstructorsDestructors() {
    ContainerArea area51;
    QVERIFY(area51.startingIndex() == 0);
    QVERIFY(area51.areaSize() == 0);

    ContainerArea area52(1, 2);
    QVERIFY(area52.startingIndex() == 1);
    QVERIFY(area52.areaSize() == 2);

    ContainerArea area53 = area52;
    QVERIFY(area52.startingIndex() == 1);
    QVERIFY(area52.areaSize() == 2);
}


void TestContainerArea::testAccessors() {
    ContainerArea area51;
    QVERIFY(area51.startingIndex() == 0);
    QVERIFY(area51.areaSize() == 0);
    QVERIFY(area51.endingIndex() == 0);

    area51.setStartingIndex(2);
    area51.setAreaSize(3);

    QVERIFY(area51.startingIndex() == 2);
    QVERIFY(area51.areaSize() == 3);
    QVERIFY(area51.endingIndex() == 5);

    area51.setEndingIndex(3);
    QVERIFY(area51.startingIndex() == 2);
    QVERIFY(area51.areaSize() == 1);
    QVERIFY(area51.endingIndex() == 3);
}


void TestContainerArea::testExpansionReduction() {
    ContainerArea area51(10, 20);
    QVERIFY(area51.startingIndex() == 10);
    QVERIFY(area51.areaSize() == 20);

    area51.reduceBy(10, ContainerArea::Side::FROM_FRONT);
    QVERIFY(area51.startingIndex() == 20);
    QVERIFY(area51.areaSize() == 10);

    area51.expandBy(10, ContainerArea::Side::FROM_FRONT);
    QVERIFY(area51.startingIndex() == 10);
    QVERIFY(area51.areaSize() == 20);

    area51.expandBy(10, ContainerArea::Side::FROM_BACK);
    QVERIFY(area51.startingIndex() == 10);
    QVERIFY(area51.areaSize() == 30);

    area51.reduceBy(10, ContainerArea::Side::FROM_BACK);
    QVERIFY(area51.startingIndex() == 10);
    QVERIFY(area51.areaSize() == 20);
}


void TestContainerArea::testAssignmentOperator() {
    ContainerArea area52(1, 2);
    QVERIFY(area52.startingIndex() == 1);
    QVERIFY(area52.areaSize() == 2);

    ContainerArea area51;
    QVERIFY(area51.startingIndex() == 0);
    QVERIFY(area51.areaSize() == 0);

    area51 = area52;
    QVERIFY(area51.startingIndex() == 1);
    QVERIFY(area51.areaSize() == 2);
}
