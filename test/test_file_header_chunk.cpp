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
* This file implements tests of the FileHeaderChunk class.
***********************************************************************************************************************/

#include <QDebug>
#include <QtTest/QtTest>

#include <cstdint>
#include <memory>

#define LIBCONTAINER_TEST // Makes the implementation accessible from the public API.

#include <container_container.h>
#include <container_memory_container.h>
#include <container_container_private.h> // temporary
#include <container_impl.h>
#include <file_header_chunk.h>

#include "test_file_header_chunk.h"

/***********************************************************************************************************************
 * FileHeaderChunkWrapper
 */

class FileHeaderChunkWrapper:public FileHeaderChunk {
    public:
        FileHeaderChunkWrapper(
            std::weak_ptr<ContainerImpl> container,
            FileIndex                    fileIndex,
            const std::string&           identifier);

        FileHeaderChunkWrapper(
            std::weak_ptr<ContainerImpl> container,
            FileIndex                    fileIndex,
            std::uint8_t                 commonHeader[Chunk::minimumChunkHeaderSizeBytes]
        );

        ~FileHeaderChunkWrapper();

        std::uint8_t* additionalHeader() const;

        unsigned additionalHeaderSizeBytes() const;
};


FileHeaderChunkWrapper::FileHeaderChunkWrapper(
        std::weak_ptr<ContainerImpl> container,
        FileIndex                    fileIndex,
        const std::string&           identifier
    ):FileHeaderChunk(
        container,
        fileIndex,
        identifier
    ) {}


FileHeaderChunkWrapper::FileHeaderChunkWrapper(
        std::weak_ptr<ContainerImpl> container,
        FileIndex                    fileIndex,
        std::uint8_t                 commonHeader[Chunk::minimumChunkHeaderSizeBytes]
    ):FileHeaderChunk(
        container,
        fileIndex,
        commonHeader
    ) {}


FileHeaderChunkWrapper::~FileHeaderChunkWrapper() {}


std::uint8_t* FileHeaderChunkWrapper::additionalHeader() const {
    return Chunk::additionalHeader();
}


unsigned FileHeaderChunkWrapper::additionalHeaderSizeBytes() const {
    return Chunk::additionalHeaderSizeBytes();
}

/***********************************************************************************************************************
 * TestFileHeaderChunk
 */

void TestFileHeaderChunk::testAccessors() {
    Container::MemoryContainer container("Inesonic, LLC./nAleph");
    FileHeaderChunkWrapper chunk(
        dynamic_cast<Container::Container&>(container).impl,
        0,
        "Inesonic, LLC.\nAleph"
    );

    QVERIFY(chunk.majorVersion() == Container::Container::containerMajorVersion);
    QVERIFY(chunk.minorVersion() == Container::Container::containerMinorVersion);
    QVERIFY(chunk.identifier() == "Inesonic, LLC.\nAleph");
}


void TestFileHeaderChunk::testSaveLoadMethods() {
    Container::MemoryContainer container("Inesonic, LLC./nAleph");
    Container::Status status = container.open();
    QVERIFY(!status);

    FileHeaderChunkWrapper chunk1(
        dynamic_cast<Container::Container&>(container).impl,
        0,
        "Inesonic, LLC.\nAleph"
    );

    status = chunk1.save();
    QVERIFY(status.success());

    FileHeaderChunkWrapper chunk2(
        dynamic_cast<Container::Container&>(container).impl,
        0,
        "Inesonic, LLC.\nAleph"
    );
    status = chunk2.load(true);
    QVERIFY(status.success());

    QVERIFY(chunk1.type() == chunk2.type());
    QVERIFY(chunk1.numberValidBytes() == chunk2.numberValidBytes());
    QVERIFY(chunk1.additionalHeaderSizeBytes() == chunk2.additionalHeaderSizeBytes());
    QVERIFY(chunk1.majorVersion() == chunk2.majorVersion());
    QVERIFY(chunk1.minorVersion() == chunk2.minorVersion());
    QVERIFY(chunk1.identifier() == chunk2.identifier());

    for (unsigned i=0 ; i<chunk1.additionalHeaderSizeBytes() ; ++i) {
        QVERIFY(chunk1.additionalHeader()[i] == chunk2.additionalHeader()[i]);
    }
}
