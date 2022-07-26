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
* This file implements the \ref FileHeaderChunk class.
***********************************************************************************************************************/

#include <cstdint>
#include <memory>
#include <cstring>
#include <cassert>

#include "container_impl.h"
#include "file_header_chunk.h"

FileHeaderChunk::FileHeaderChunk(
        std::weak_ptr<ContainerImpl> container,
        FileIndex                    fileIndex,
        const std::string&           identifier
    ):Chunk(
        container,
        fileIndex,
        static_cast<unsigned>(identifier.size() + 4)
    ) {
    setType(ChunkHeader::Type::FILE_HEADER_CHUNK);
    setNumberValidBytes(additionalHeaderSizeBytes());

    std::uint8_t* header = additionalHeader();
    header[0] = Container::Container::containerMajorVersion;
    header[1] = Container::Container::containerMinorVersion;
    header[2] = 0;
    header[3] = 0;

    char* s = reinterpret_cast<char*>(header + 4);
    for (std::string::const_iterator it=identifier.cbegin(),end=identifier.cend() ; it!=end ; ++it) {
        *s = *it;
        ++s;
    }
}


FileHeaderChunk::FileHeaderChunk(
        std::weak_ptr<ContainerImpl> container,
        FileIndex                    fileIndex,
        std::uint8_t                 commonHeader[Chunk::minimumChunkHeaderSizeBytes]
    ):Chunk(
        container,
        fileIndex,
        commonHeader
    ) {}


FileHeaderChunk::~FileHeaderChunk() {}


std::uint8_t FileHeaderChunk::majorVersion() const {
    return additionalHeader()[0];
}


std::uint8_t FileHeaderChunk::minorVersion() const {
    return additionalHeader()[1];
}


std::string FileHeaderChunk::identifier() const {
    unsigned    validBytes = numberValidBytes();
    std::string identifierString;

    if (validBytes > 4) {
        unsigned stringLength = validBytes - 4;

        char* s = reinterpret_cast<char*>(additionalHeader() + 4);
        for (unsigned i=0 ; i<stringLength ; ++i) {
            identifierString += s[i];
        }
    }

    return identifierString;
}


bool FileHeaderChunk::isValid(const std::string& expectedIdentifier) const {
    return (
           expectedIdentifier == identifier()
        && majorVersion() == Container::Container::containerMajorVersion
        && minorVersion() == Container::Container::containerMinorVersion
    );
}
