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
* This file implements the \ref FreeSpace class.
***********************************************************************************************************************/

#include <map>

#include "chunk_header.h"
#include "container_area.h"
#include "free_space_data.h"
#include "free_space.h"

FreeSpace::FreeSpace(
        std::map<ChunkHeader::FileIndex, FreeSpaceData>::iterator& freeSpacePosition,
        ChunkHeader::FileIndex                                     startingIndex,
        ChunkHeader::FileIndex                                     areaSize
    ):ContainerArea(
        startingIndex,
        areaSize
    ),position(
        freeSpacePosition
    ) {
    currentlyValid = true;
}


std::map<ChunkHeader::FileIndex, FreeSpaceData>::iterator FreeSpace::iterator() const {
    return position;
}


FreeSpace::FreeSpace() {
    currentlyValid = false;
}


FreeSpace::FreeSpace(const FreeSpace& other):ContainerArea(other), position(other.position) {
    currentlyValid = other.currentlyValid;
}


FreeSpace::~FreeSpace() {}


bool FreeSpace::isValid() const {
    return currentlyValid;
}


bool FreeSpace::isInvalid() const {
    return !isValid();
}


FreeSpace& FreeSpace::operator=(const FreeSpace& other) {
    ContainerArea::operator=(other);
    position       = other.position;
    currentlyValid = other.currentlyValid;

    return *this;
}
