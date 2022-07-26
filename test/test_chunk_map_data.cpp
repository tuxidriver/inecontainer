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
* This file implements tests of the ChunkMapData class.
***********************************************************************************************************************/

#include <QDebug>
#include <QtTest/QtTest>

#include <chunk_map_data.h>

#include "test_chunk_map_data.h"

void TestChunkMapData::testConstructorsDestructors() {
    ChunkMapData data1(1, 2);
    QVERIFY(data1.startingIndex() == 1);
    QVERIFY(data1.payloadSize() == 2);

    ChunkMapData data2(data1);
    QVERIFY(data2.startingIndex() == 1);
    QVERIFY(data2.payloadSize() == 2);
}


void TestChunkMapData::testAccessors() {
    ChunkMapData data(1, 2);

    data.setStartingIndex(3);
    QVERIFY(data.startingIndex() == 3);
    QVERIFY(data.payloadSize() == 2);

    data.setPayloadSize(4);
    QVERIFY(data.startingIndex() == 3);
    QVERIFY(data.payloadSize() == 4);
}


void TestChunkMapData::testAssignmentOperator() {
    ChunkMapData data1(1, 2);
    QVERIFY(data1.startingIndex() == 1);
    QVERIFY(data1.payloadSize() == 2);

    ChunkMapData data2(3, 4);
    QVERIFY(data2.startingIndex() == 3);
    QVERIFY(data2.payloadSize() == 4);

    data2 = data1;
    QVERIFY(data2.startingIndex() == 1);
    QVERIFY(data2.payloadSize() == 2);
}
