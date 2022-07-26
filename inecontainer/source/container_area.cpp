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
* This file implements the \ref ContainerArea class.
***********************************************************************************************************************/

#include "chunk_header.h"
#include "container_area.h"

ContainerArea::ContainerArea(ChunkHeader::FileIndex startingIndex, ChunkHeader::FileIndex areaSize) {
    currentStartingIndex = startingIndex;
    currentAreaSize      = areaSize;
}


ContainerArea::ContainerArea(const ContainerArea& other) {
    currentStartingIndex = other.currentStartingIndex;
    currentAreaSize      = other.currentAreaSize;
}


ContainerArea::~ContainerArea() {}


void ContainerArea::setStartingIndex(ChunkHeader::FileIndex newStartingIndex) {
    currentStartingIndex = newStartingIndex;
}


ChunkHeader::FileIndex ContainerArea::startingIndex() const {
    return currentStartingIndex;
}


bool ContainerArea::setEndingIndex(ChunkHeader::FileIndex newEndingIndex) {
    bool success;

    if (newEndingIndex >= currentStartingIndex) {
        success = true;
        currentAreaSize = newEndingIndex - currentStartingIndex;
    } else {
        success = false;
    }

    return success;
}


ChunkHeader::FileIndex ContainerArea::endingIndex() const {
    return currentStartingIndex + currentAreaSize;
}


void ContainerArea::setAreaSize(ChunkHeader::FileIndex newAreaSize) {
    currentAreaSize = newAreaSize;
}


ChunkHeader::FileIndex ContainerArea::areaSize() const {
    return currentAreaSize;
}


bool ContainerArea::reduceBy(ChunkHeader::FileIndex amount, Side side) {
    bool success = currentAreaSize >= amount;

    if (success) {
        currentAreaSize -= amount;

        if (side == Side::FROM_FRONT) {
            currentStartingIndex += amount;
        }
    }

    return success;
}


bool ContainerArea::expandBy(ChunkHeader::FileIndex amount, Side side) {
    bool success = side == Side::FROM_BACK || currentStartingIndex >= amount;

    if (success) {
        if (side == Side::FROM_FRONT) {
            currentStartingIndex -= amount;
        }

        currentAreaSize += amount;
    }

    return success;
}


ContainerArea& ContainerArea::operator=(const ContainerArea& other) {
    currentStartingIndex = other.currentStartingIndex;
    currentAreaSize      = other.currentAreaSize;

    return *this;
}


bool ContainerArea::operator==(const ContainerArea& other) const {
    return other.currentStartingIndex == currentStartingIndex && other.currentAreaSize == currentAreaSize;
}


bool ContainerArea::operator!=(const ContainerArea& other) const {
    return other.currentStartingIndex != currentStartingIndex || other.currentAreaSize != currentAreaSize;
}
