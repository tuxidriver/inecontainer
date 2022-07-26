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
* This file implements tests of the RingBuffer template class.
***********************************************************************************************************************/

#include <QDebug>
#include <QtTest/QtTest>

#include <random>

#include <ring_buffer.h>

#include "test_ring_buffer.h"

void TestRingBuffer::testSingleEntryInsertionExtraction1() {
    RingBuffer<unsigned, 4096> buffer;

    QVERIFY(buffer.empty() == true);
    QVERIFY(buffer.notEmpty() == false);
    QVERIFY(buffer.full() == false);
    QVERIFY(buffer.notFull() == true);
    QVERIFY(buffer.count() == 0);
    QVERIFY(buffer.length() == 0);
    QVERIFY(buffer.available() == 4096);
    QVERIFY(buffer.size() == 4096);
    QVERIFY(buffer.bufferLength == 4096);

    unsigned i;
    for (i=1 ; i<=buffer.bufferLength ; ++i) {
        bool success = buffer.insert(i);
        QVERIFY(success = true);

        QVERIFY(buffer.empty() == false);
        QVERIFY(buffer.notEmpty() == true);
        QVERIFY(buffer.full() == (i == buffer.bufferLength));
        QVERIFY(buffer.notFull() == (i != buffer.bufferLength));
        QVERIFY(buffer.count() == i);
        QVERIFY(buffer.length() == i);
        QVERIFY(buffer.available() == buffer.bufferLength - i);
        QVERIFY(buffer.size() == buffer.bufferLength);
    }

    bool success = buffer.insert(0);
    QVERIFY(!success);

    i = 0;

    while (buffer.notEmpty()) {
        ++i;
        unsigned v = buffer.extract(0);
        QVERIFY(v == i);

        QVERIFY(buffer.empty() == (i == buffer.bufferLength));
        QVERIFY(buffer.notEmpty() == (i != buffer.bufferLength));
        QVERIFY(buffer.full() == false);
        QVERIFY(buffer.notFull() == true);
        QVERIFY(buffer.count() == buffer.bufferLength - i);
        QVERIFY(buffer.length() == buffer.bufferLength - i);
        QVERIFY(buffer.available() == i);
        QVERIFY(buffer.size() == 4096);
        QVERIFY(buffer.bufferLength == 4096);
    }

    QVERIFY(i == buffer.bufferLength);
}


void TestRingBuffer::testSingleEntryInsertionExtractionSnoop2() {
    RingBuffer<unsigned, 128> buffer;

    unsigned extractionCount = 0;
    for (unsigned insertionCount=1 ; insertionCount<=numberInsertions ; ++insertionCount) {
        buffer.insert(insertionCount);

        if (buffer.length() >= extractionStartThreshold) {
            unsigned numberToSnoop = extractionStartThreshold - extractionStopThreshold;

            for (unsigned snoopOffset=0 ; snoopOffset<numberToSnoop ; ++snoopOffset) {
                unsigned v = buffer.snoop(snoopOffset);
                QVERIFY(v == (extractionCount + snoopOffset + 1));
            }

            do {
                unsigned v = buffer.extract(0);

                ++extractionCount;
                QVERIFY(v == extractionCount);
            } while (buffer.length() > extractionStopThreshold);
        }
    }

    while (buffer.notEmpty()) {
        unsigned v= buffer.extract(0);

        ++extractionCount;
        QVERIFY(v == extractionCount);
    }
}


void TestRingBuffer::testBulkInsertionExtraction() {
    RingBuffer<unsigned, 4096> buffer;

    std::mt19937 rng;

    unsigned insertionCount    = 1;
    unsigned extractionCount   = 1;
    unsigned expectedAvailable = buffer.bufferLength;

    while (insertionCount < numberInsertions) {
        if (buffer.notFull()) {
            unsigned* p1;
            unsigned* p2;
            unsigned  l1;
            unsigned  l2;

            std::uniform_int_distribution<> insertLengthGenerator(1, buffer.available());

            unsigned maximumInsertionCount = buffer.bulkInsertionStart(&p1, &l1, &p2, &l2);

            QVERIFY(maximumInsertionCount == expectedAvailable);

            unsigned numberToInsert = insertLengthGenerator(rng);
            unsigned remaining      = numberToInsert;

            unsigned remainingThisPointer = (l1 < remaining ? l1 : remaining);
            remaining -= remainingThisPointer;

            while (remainingThisPointer > 0) {
                *p1 = insertionCount;

                ++p1;
                ++insertionCount;
                --remainingThisPointer;
            }

            remainingThisPointer  = (l2 < remaining ? l2 : remaining);
            remaining            -= remainingThisPointer;

            QVERIFY(remaining == 0);

            while (remainingThisPointer > 0) {
                *p2 = insertionCount;

                ++p2;
                ++insertionCount;
                --remainingThisPointer;
            }

            bool success = buffer.bulkInsertionFinish(numberToInsert);
            QVERIFY(success);

            expectedAvailable -= numberToInsert;
        }

        if (buffer.notEmpty()) {
            const unsigned* p1;
            const unsigned* p2;
            unsigned        l1;
            unsigned        l2;

            std::uniform_int_distribution<> extractLengthGenerator(1, buffer.length());

            unsigned maximumExtractionCount = buffer.bulkExtractionStart(&p1, &l1, &p2, &l2);

            QVERIFY(maximumExtractionCount == buffer.bufferLength - expectedAvailable);

            unsigned numberToExtract = extractLengthGenerator(rng);
            unsigned remaining       = numberToExtract;

            unsigned remainingThisPointer = (l1 < remaining ? l1 : remaining);
            remaining -= remainingThisPointer;

            while (remainingThisPointer > 0) {
                QVERIFY(*p1 == extractionCount);

                ++p1;
                ++extractionCount;
                --remainingThisPointer;
            }

            remainingThisPointer  = (l2 < remaining ? l2 : remaining);
            remaining            -= remainingThisPointer;
            QVERIFY(remaining == 0);

            while (remainingThisPointer > 0) {
                QVERIFY(*p2 == extractionCount);

                ++p2;
                ++extractionCount;
                --remainingThisPointer;
            }

            bool success = buffer.bulkExtractionFinish(numberToExtract);
            QVERIFY(success);

            expectedAvailable += numberToExtract;
        }
    }

}
