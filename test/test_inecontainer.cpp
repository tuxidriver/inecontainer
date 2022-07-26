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
* This file is the main entry point for the inecontainer unit tests.
***********************************************************************************************************************/

#include <QtTest/QtTest>

#include "test_status.h"
#include "test_container_area.h"
#include "test_scatter_gather_list_segment.h"
#include "test_free_space.h"
#include "test_free_space_data.h"
#include "test_free_space_tracker.h"
#include "test_ring_buffer.h"
#include "test_chunk_map_data.h"
#include "test_chunk_header.h"
#include "test_chunk.h"
#include "test_fill_chunk.h"
#include "test_file_header_chunk.h"
#include "test_stream_start_chunk.h"
#include "test_stream_data_chunk.h"
#include "test_memory_container.h"
#include "test_file_container.h"
#include "test_virtual_file.h"

#define TEST(_X) do {                                                  \
    _X _x;                                                          \
    testStatus |= QTest::qExec(&_x, argumentCount, argumentValues); \
} while(false)

int main(int argumentCount, char** argumentValues) {
    int testStatus = 0;

    TEST(TestStatus);
    TEST(TestContainerArea);
    TEST(TestScatterGatherListSegment);
    TEST(TestFreeSpace);
    TEST(TestFreeSpaceData);
    TEST(TestFreeSpaceTracker);
    TEST(TestRingBuffer);
    TEST(TestChunkMapData);
    TEST(TestChunkHeader);
    TEST(TestChunk);
    TEST(TestFillChunk);
    TEST(TestFileHeaderChunk);
    TEST(TestStreamStartChunk);
    TEST(TestStreamDataChunk);
    TEST(TestMemoryContainer);
    TEST(TestFileContainer);
    TEST(TestVirtualFile);

    return testStatus;
}
