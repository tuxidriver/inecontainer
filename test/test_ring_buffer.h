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
* This header provides tests for the RingBuffer template class.
***********************************************************************************************************************/

#ifndef TEST_RING_BUFFER_H
#define TEST_RING_BUFFER_H

#include <QObject>
#include <QtTest/QtTest>

class TestRingBuffer:public QObject {
    Q_OBJECT

    private slots:
        void testSingleEntryInsertionExtraction1();

        void testSingleEntryInsertionExtractionSnoop2();

        void testBulkInsertionExtraction();

    private:
        static constexpr unsigned numberInsertions         = 65536*128;
        static constexpr unsigned extractionStartThreshold = 97;
        static constexpr unsigned extractionStopThreshold  = 17;
};

#endif
