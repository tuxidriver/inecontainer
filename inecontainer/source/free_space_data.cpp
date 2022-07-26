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
* This file implements the \ref FreeSpaceData class.
***********************************************************************************************************************/

#include <map>

#include "chunk_header.h"
#include "free_space_data.h"

FreeSpaceData::FreeSpaceData(ChunkHeader::FileIndex endingIndex, bool reserved, bool fileNeedsUpdate) {
    currentEndingIndex     = endingIndex;
    currentlyReserved      = reserved;
    currentFileNeedsUpdate = fileNeedsUpdate;
}


FreeSpaceData::FreeSpaceData(const FreeSpaceData& other) {
    currentEndingIndex     = other.currentEndingIndex;
    currentlyReserved      = other.currentlyReserved;
    currentFileNeedsUpdate = other.currentFileNeedsUpdate;
}


FreeSpaceData::~FreeSpaceData() {}


void FreeSpaceData::setEndingIndex(ChunkHeader::FileIndex newEndingIndex) {
    currentEndingIndex = newEndingIndex;
}


ChunkHeader::FileIndex FreeSpaceData::endingIndex() const {
    return currentEndingIndex;
}


void FreeSpaceData::setReserved(bool nowReserved) {
    currentlyReserved = nowReserved;
}


void FreeSpaceData::setAvailable(bool nowAvailable) {
    setReserved((!nowAvailable));
}


bool FreeSpaceData::isReserved() const {
    return currentlyReserved;
}


bool FreeSpaceData::isAvailable() const {
    return !isReserved();
}


void FreeSpaceData::setFileUpdateNeeded(bool nowFileUpdateNeeded) {
    currentFileNeedsUpdate = nowFileUpdateNeeded;
}


bool FreeSpaceData::fileUpdateNeeded() const {
    return currentFileNeedsUpdate;
}


FreeSpaceData& FreeSpaceData::operator=(const FreeSpaceData& other) {
    currentEndingIndex     = other.currentEndingIndex;
    currentlyReserved      = other.currentlyReserved;
    currentFileNeedsUpdate = other.currentFileNeedsUpdate;

    return *this;
}
