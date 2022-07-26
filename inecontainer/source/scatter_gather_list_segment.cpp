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
* This file implements the \ref ScatterGatherListSegment class.
***********************************************************************************************************************/

#include <cstdint>
#include <cassert>

#include "scatter_gather_list_segment.h"

ScatterGatherListSegment::ScatterGatherListSegment(std::uint8_t* base, unsigned length, unsigned processedCount) {
    assert(processedCount <= length);

    currentBase           = base;
    currentLength         = length;
    currentProcessedCount = processedCount;
}


ScatterGatherListSegment::ScatterGatherListSegment(std::uint8_t* base, std::uint8_t* end, unsigned processedCount) {
    assert(end - base >= 0);
    assert(processedCount <= static_cast<unsigned>(end - base));

    currentBase           = base;
    currentLength         = static_cast<unsigned>(end-base);
    currentProcessedCount = processedCount;
}


ScatterGatherListSegment::ScatterGatherListSegment(const ScatterGatherListSegment& other) {
    currentBase           = other.currentBase;
    currentLength         = other.currentLength;
    currentProcessedCount = other.currentProcessedCount;
}


ScatterGatherListSegment::~ScatterGatherListSegment() {}


void ScatterGatherListSegment::update(std::uint8_t* newBase, unsigned newLength) {
    currentBase   = newBase;
    currentLength = newLength;
}


void ScatterGatherListSegment::update(std::uint8_t* newBase, std::uint8_t* newEnd) {
    assert(newEnd - newBase >= 0);
    currentBase   = newBase;
    currentLength = static_cast<unsigned>(newEnd - newBase);
}


void ScatterGatherListSegment::setBase(std::uint8_t* newBase) {
    currentBase = newBase;
}


std::uint8_t* ScatterGatherListSegment::base() const {
    return currentBase;
}


void ScatterGatherListSegment::setLength(unsigned newLength) {
    currentLength = newLength;
}


unsigned ScatterGatherListSegment::length() const {
    return currentLength;
}


void ScatterGatherListSegment::setEnd(std::uint8_t* newEnd) {
    assert(newEnd - currentBase >= 0);
    currentLength = static_cast<unsigned>(newEnd - currentBase);
}


std::uint8_t* ScatterGatherListSegment::end() const {
    return currentBase + currentLength;
}


void ScatterGatherListSegment::setProcessedCount(unsigned newProcessedCount) {
    assert(newProcessedCount <= currentLength);
    currentProcessedCount = newProcessedCount;
}


unsigned ScatterGatherListSegment::processedCount() const {
    return currentProcessedCount;
}


ScatterGatherListSegment& ScatterGatherListSegment::operator=(const ScatterGatherListSegment& other) {
    currentBase           = other.currentBase;
    currentLength         = other.currentLength;
    currentProcessedCount = other.currentProcessedCount;

    return *this;
}


bool ScatterGatherListSegment::operator==(const ScatterGatherListSegment& other) const {
    return (
           other.currentBase           == currentBase
        && other.currentLength         == currentLength
        && other.currentProcessedCount == currentProcessedCount
    );
}


bool ScatterGatherListSegment::operator!=(const ScatterGatherListSegment& other) const {
    return (
           other.currentBase           != currentBase
        || other.currentLength         != currentLength
        || other.currentProcessedCount != currentProcessedCount
    );
}
