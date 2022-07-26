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
* This file implements tests of the ScatterGatherListSegment class.
***********************************************************************************************************************/

#include <QDebug>
#include <QtTest/QtTest>

#include <cstdint>

#include <scatter_gather_list_segment.h>

#include "test_scatter_gather_list_segment.h"

void TestScatterGatherListSegment::testConstructorsDestructors() {
    std::uint8_t buffer[2];

    ScatterGatherListSegment segment1;
    QVERIFY(segment1.base() == nullptr);
    QVERIFY(segment1.length() == 0);

    ScatterGatherListSegment segment2(buffer, 2);
    QVERIFY(segment2.base() == buffer);
    QVERIFY(segment2.length() == 2);

    ScatterGatherListSegment segment3(buffer, buffer + 2);
    QVERIFY(segment3.base() == buffer);
    QVERIFY(segment3.length() == 2);

    ScatterGatherListSegment segment4 = segment3;
    QVERIFY(segment4.base() == buffer);
    QVERIFY(segment4.length() == 2);
}


void TestScatterGatherListSegment::testAccessors() {
    std::uint8_t buffer[8];

    ScatterGatherListSegment segment;
    QVERIFY(segment.base() == nullptr);
    QVERIFY(segment.length() == 0);

    segment.update(buffer, 2);
    QVERIFY(segment.base() == buffer);
    QVERIFY(segment.length() == 2);
    QVERIFY(segment.end() == buffer + 2);

    segment.update(buffer + 1, buffer + 4);
    QVERIFY(segment.base() == buffer + 1);
    QVERIFY(segment.length() == 3);
    QVERIFY(segment.end() == buffer + 4);

    segment.setBase(buffer);
    QVERIFY(segment.base() == buffer);
    QVERIFY(segment.length() == 3);

    segment.setLength(2);
    QVERIFY(segment.base() == buffer);
    QVERIFY(segment.length() == 2);
    QVERIFY(segment.end() == buffer + 2);
}


void TestScatterGatherListSegment::testAssignmentOperator() {
    std::uint8_t buffer[2];

    ScatterGatherListSegment segment1(buffer, 2);
    QVERIFY(segment1.base() == buffer);
    QVERIFY(segment1.length() == 2);

    ScatterGatherListSegment segment2;
    QVERIFY(segment2.base() == nullptr);
    QVERIFY(segment2.length() == 0);

    segment2 = segment1;
    QVERIFY(segment2.base() == buffer);
    QVERIFY(segment2.length() == 2);
}
