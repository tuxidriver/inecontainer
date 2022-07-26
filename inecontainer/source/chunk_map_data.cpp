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
* This file implements the \ref ChunkMapData class.
***********************************************************************************************************************/

#include "chunk_header.h"
#include "chunk_map_data.h"

ChunkMapData::ChunkMapData(ChunkHeader::FileIndex startingIndex, unsigned payloadSize) {
    currentStartingIndex = startingIndex;
    currentPayloadSize   = payloadSize;
}


ChunkMapData::ChunkMapData(const ChunkMapData& other) {
    currentStartingIndex = other.currentStartingIndex;
    currentPayloadSize   = other.currentPayloadSize;
}


ChunkMapData::~ChunkMapData() {}


void ChunkMapData::setStartingIndex(ChunkHeader::FileIndex newStartingIndex) {
    currentStartingIndex = newStartingIndex;
}


ChunkHeader::FileIndex ChunkMapData::startingIndex() const {
    return currentStartingIndex;
}


void ChunkMapData::setPayloadSize(unsigned newPayloadSize) {
    currentPayloadSize = newPayloadSize;
}


unsigned ChunkMapData::payloadSize() const {
    return currentPayloadSize;
}


ChunkMapData& ChunkMapData::operator=(const ChunkMapData& other) {
    currentStartingIndex = other.currentStartingIndex;
    currentPayloadSize   = other.currentPayloadSize;

    return *this;
}
