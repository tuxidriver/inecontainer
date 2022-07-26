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
* This file implements tests of the Container::VirtualFile template class.
***********************************************************************************************************************/

#include <QDebug>
#include <QtTest/QtTest>

#include <cstdint>
#include <cstring>
#include <vector>
#include <memory>
#include <sstream>
#include <random>

#include <container_status.h>
#include <container_container.h>
#include <container_memory_container.h>
#include <container_virtual_file.h>

#include "test_virtual_file.h"

/***********************************************************************************************************************
 * VirtualFileWrapper
 */

class VirtualFileWrapper:public Container::VirtualFile {
    public:
        VirtualFileWrapper(const std::string& newName, Container::Container* container);

        VirtualFileWrapper(const VirtualFile& other);

        ~VirtualFileWrapper() override;

        std::vector<std::uint8_t> dataBuffer() const;

    protected:
        Container::Status receivedData(const std::uint8_t* buffer, unsigned bytesReceived) final;

    private:
        std::vector<std::uint8_t> currentBuffer;
};


VirtualFileWrapper::VirtualFileWrapper(
        const std::string&    newName,
        Container::Container* container
    ):Container::VirtualFile(
        newName,
        container
    ) {}


VirtualFileWrapper::VirtualFileWrapper(const VirtualFile &other):Container::VirtualFile(other) {}


VirtualFileWrapper::~VirtualFileWrapper() {}


std::vector<std::uint8_t> VirtualFileWrapper::dataBuffer() const {
    return currentBuffer;
}


Container::Status VirtualFileWrapper::receivedData(const std::uint8_t* buffer, unsigned bytesReceived) {
    const std::uint8_t* end = buffer + bytesReceived;
    while (buffer != end) {
        currentBuffer.push_back(*buffer);
        ++buffer;
    }

    return Container::NoStatus();
}

/***********************************************************************************************************************
 * ContainerWrapper
 */

class ContainerWrapper:public Container::MemoryContainer {
    public:
        ContainerWrapper(const std::string& fileIdentifier);

        ~ContainerWrapper() override;

    protected:
        ::Container::VirtualFile* createFile(const std::string& virtualFileName) final;
};


ContainerWrapper::ContainerWrapper(const std::string& fileIdentifier):MemoryContainer(fileIdentifier) {}


ContainerWrapper::~ContainerWrapper() {}


Container::VirtualFile* ContainerWrapper::createFile(const std::string& virtualFileName) {
    return new VirtualFileWrapper(virtualFileName, this);
}

/***********************************************************************************************************************
 * TestVirtualFile
 */

void TestVirtualFile::testVirtualFileCreateAndSequentialWriteRead() {
    Container::MemoryContainer container("Inesonic, LLC.\nAleph Test");

    Container::Status status = container.open();
    QVERIFY(status.success());

    std::shared_ptr<Container::VirtualFile> vf = container.newVirtualFile("test.dat");
    QVERIFY(vf->name() == "test.dat");
    QVERIFY(vf->size() == 0);
    QVERIFY(vf->position() == 0);

    std::uint8_t buffer[bufferSizeInBytes];
    std::uint8_t v = 254;

    std::mt19937 rng;

    unsigned long bytesRemaining = sequentialFileSizeInBytes;
    while (!status && bytesRemaining > 0) {
        unsigned maximumLength = bytesRemaining > bufferSizeInBytes ? bufferSizeInBytes : bytesRemaining;
        std::uniform_int_distribution<> lengthGenerator(1, maximumLength);
        unsigned thisWriteLength = lengthGenerator(rng);

        for (unsigned i=0 ; i<thisWriteLength ; ++i) {
            buffer[i] = v;
            --v;
            if (v == 0) {
                v = 254;
            }
        }

        status = vf->append(buffer, thisWriteLength);
        if (status.success() && Container::WriteSuccessful(status).bytesWritten() == thisWriteLength) {
            status = Container::NoStatus();
        }

        QVERIFY(!status);
        bytesRemaining -= thisWriteLength;
    }

    // TODO: Verify file contents.

    status = vf->setPosition(0);
    QVERIFY(status.success());

    v = 254;

    bytesRemaining = sequentialFileSizeInBytes;
    while (!status && bytesRemaining > 0) {
        unsigned maximumLength = bytesRemaining > bufferSizeInBytes ? bufferSizeInBytes : bytesRemaining;
        std::uniform_int_distribution<> lengthGenerator(1, maximumLength);
        unsigned thisReadLength = lengthGenerator(rng);

        status = vf->read(buffer, thisReadLength);
        if (status.success() && Container::ReadSuccessful(status).bytesRead() == thisReadLength) {
            status = Container::NoStatus();
        }

        QVERIFY(!status);

        for (unsigned i=0 ; i<thisReadLength ; ++i) {
            QVERIFY(buffer[i] == v);
            --v;
            if (v == 0) {
                v = 254;
            }
        }

        bytesRemaining -= thisReadLength;
    }
}


void TestVirtualFile::testVirtualFileCreateAndRandomWriteReadFlush() {
    std::mt19937 rng;

    for (unsigned testIteration=0 ; testIteration<numberRandomWriteReadTests ; ++testIteration) {
        Container::MemoryContainer container("Inesonic, LLC.\nAleph Test");

        Container::Status status = container.open();
        QVERIFY(status.success());

        // Generate a file we can modify.

        std::shared_ptr<Container::VirtualFile> vf = container.newVirtualFile("test.dat");
        QVERIFY(vf->name() == "test.dat");
        QVERIFY(vf->size() == 0);
        QVERIFY(vf->position() == 0);

        std::uint8_t buffer[bufferSizeInBytes];
        std::memset(buffer, 0xFF, bufferSizeInBytes);

        unsigned long bytesRemaining = randomFileSizeInBytes;
        while (bytesRemaining > 0) {
            unsigned writeLength = bytesRemaining > bufferSizeInBytes ? bufferSizeInBytes : bytesRemaining;
            status = vf->append(buffer, writeLength);
            QVERIFY(status.success());
            QVERIFY(Container::WriteSuccessful(status).bytesWritten() == writeLength);

            bytesRemaining -= writeLength;
        }

        // Randomly pick a range to munge by writing zeros.

        for (unsigned i=0 ; i<bufferSizeInBytes ; ++i) {
            buffer[i] = static_cast<std::uint8_t>(i % 254); // Don't allow 0xFF or 0xFE to be written.
        }

        std::uniform_int_distribution<> lengthGenerator(1, bufferSizeInBytes);
        std::uniform_int_distribution<> startOffsetGenerator(0, randomFileSizeInBytes);

        unsigned long writeStartOffset = startOffsetGenerator(rng);
        unsigned      writeLength = lengthGenerator(rng);

        status = vf->setPosition(writeStartOffset);
        QVERIFY(!status);

        status = vf->write(buffer, writeLength);
        QVERIFY(status.success());
        QVERIFY(Container::WriteSuccessful(status).bytesWritten() == writeLength);

        // Sometimes we flush, sometimes we do not flush.  Tests should pass no matter what happens.

        if ((testIteration % 2) == 0) {
            status = vf->flush();
            QVERIFY(!status);
        }

        // Now randomly pick a range to read.

        std::memset(buffer, 0xFE, bufferSizeInBytes);

        unsigned long readStartOffset;
        unsigned      readLength;

        do {
            readStartOffset = startOffsetGenerator(rng);
            readLength      = lengthGenerator(rng);
        } while (readStartOffset + readLength >= static_cast<unsigned long>(vf->size()));

        status = vf->setPosition(readStartOffset);
        QVERIFY(!status);

        status = vf->read(buffer, readLength);

        QVERIFY(status.success());
        QVERIFY(Container::ReadSuccessful(status).bytesRead() == readLength);

        for (unsigned i=0 ; i<readLength ; ++i) {
            unsigned long position     = readStartOffset + i;
            std::uint8_t  v            = buffer[i];
            std::uint8_t expectedValue;

            if (position < writeStartOffset || position >= writeStartOffset + writeLength) {
                expectedValue = 0xFF;
            } else {
                unsigned offsetIntoWrite = position - writeStartOffset;
                expectedValue = static_cast<std::uint8_t>(offsetIntoWrite % 254);
            }

            if (v != expectedValue) {
                QVERIFY(v == expectedValue);
            }
        }
    }
}


void TestVirtualFile::testVirtualFileRename() {
    Container::MemoryContainer container("Inesonic, LLC.\nAleph Test");

    Container::Status status = container.open();
    QVERIFY(status.success());

    // Generate a file we can modify.

    std::shared_ptr<Container::VirtualFile> vf1 = container.newVirtualFile("test.dat");
    vf1->rename("test2.dat");

    QVERIFY(vf1->name() == "test2.dat");

    vf1->flush();

    vf1->rename("test3.dat");

    // TODO: Close and re-open container and verify the renamed file is correct.

    QVERIFY(vf1->name() == "test3.dat");
}


void TestVirtualFile::testVirtualFileErase() {
    Container::MemoryContainer container("Inesonic, LLC.\nAleph Test");

    Container::Status status = container.open();
    QVERIFY(status.success());

    std::shared_ptr<Container::VirtualFile> vf1 = container.newVirtualFile("test1.dat");
    std::shared_ptr<Container::VirtualFile> vf2 = container.newVirtualFile("test2.dat");

    std::uint8_t buffer[bufferSizeInBytes];
    std::uint8_t v1 = 254;
    std::uint8_t v2 = 0;

    std::mt19937                    rng;
    std::uniform_int_distribution<> fileSelector(1, 2);

    unsigned long bytesRemaining = sequentialFileSizeInBytes;
    while (!status && bytesRemaining > 0) {
        unsigned maximumLength = bytesRemaining > bufferSizeInBytes ? bufferSizeInBytes : bytesRemaining;
        std::uniform_int_distribution<> lengthGenerator(1, maximumLength);
        unsigned thisWriteLength = lengthGenerator(rng);
        unsigned fileSelection = fileSelector(rng);

        if (fileSelection == 1) {
            for (unsigned i=0 ; i<thisWriteLength ; ++i) {
                buffer[i] = v1;
                --v1;
                if (v1 == 0) {
                    v1 = 254;
                }
            }

            status = vf1->append(buffer, thisWriteLength);
        } else {
            for (unsigned i=0 ; i<thisWriteLength ; ++i) {
                buffer[i] = v2;
                ++v2;
                if (v2 == 255) {
                    v2 = 0;
                }
            }

            status = vf2->append(buffer, thisWriteLength);
        }

        if (status.success() && Container::WriteSuccessful(status).bytesWritten() == thisWriteLength) {
            status = Container::NoStatus();
        }

        QVERIFY(!status);
        bytesRemaining -= thisWriteLength;
    }

    status = vf1->flush();
    QVERIFY(!status);

    status = vf2->flush();
    QVERIFY(!status);

    status = vf2->erase();
    QVERIFY(!status);
}


void TestVirtualFile::testFileOpenCloseEraseAndRandomAccess() {
    for (unsigned iteration=0 ; iteration<numberOpenCloseEraseAndRandomAccessTests ; ++iteration) {
        // Write a bunch of files randomly into a memory container.

        typedef Container::MemoryContainer::MemoryBuffer MemoryBuffer;
        std::shared_ptr<MemoryBuffer> containerBuffer = std::make_shared<MemoryBuffer>();

        Container::MemoryContainer writeContainer("Inesonic, LLC.\nAleph Test");

        Container::Status status = writeContainer.open(containerBuffer);
        QVERIFY(status.success());

        std::vector<std::shared_ptr<Container::VirtualFile>> virtualFiles;
        std::vector<std::uint8_t>                            accumulators;
        std::vector<long long>                               sizes;

        for (unsigned i=0 ; i<numberVirtualFiles ; ++i) {
            std::stringstream stream;
            stream << "test" << i << ".dat";

            virtualFiles.push_back(writeContainer.newVirtualFile(stream.str()));
            accumulators.push_back(0);
            sizes.push_back(0);
        }

        std::mt19937                    rng;
        std::uniform_int_distribution<> fileSelector(0, numberVirtualFiles - 1);

        std::uint8_t buffer[bufferSizeInBytes];

        unsigned long bytesRemaining = sequentialFileSizeInBytes;
        while (!status && bytesRemaining > 0) {
            unsigned maximumLength = bytesRemaining > bufferSizeInBytes ? bufferSizeInBytes : bytesRemaining;
            std::uniform_int_distribution<> lengthGenerator(1, maximumLength);
            unsigned thisWriteLength = lengthGenerator(rng);
            unsigned fileSelection = fileSelector(rng);

            std::shared_ptr<Container::VirtualFile> vf  = virtualFiles.at(fileSelection);
            std::uint8_t                            a   = accumulators.at(fileSelection);
            std::uint8_t                            max = static_cast<std::uint8_t>(254 - fileSelection);

            for (unsigned i=0 ; i<thisWriteLength ; ++i) {
                buffer[i] = a;
                ++a;
                if (a == max) {
                    a = 0;
                }
            }

            status = vf->append(buffer, thisWriteLength);

            if (status.success() && Container::WriteSuccessful(status).bytesWritten() == thisWriteLength) {
                status = Container::NoStatus();
            }

            accumulators[fileSelection]  = a;
            sizes[fileSelection]        += thisWriteLength;

            bytesRemaining -= thisWriteLength;
        }

        QVERIFY(!status);

        for (unsigned i=0 ; i<numberVirtualFiles ; ++i) {
            std::shared_ptr<Container::VirtualFile> vf  = virtualFiles.at(i);
            QVERIFY(vf->size() == sizes.at(i));
        }

        status = writeContainer.close();
        QVERIFY(!status);

        // Now open the container using just the file contents and delete a file.
        unsigned erasedFile = fileSelector(rng);

        {
            Container::MemoryContainer readContainer("Inesonic, LLC.\nAleph Test");

            status = readContainer.open(containerBuffer);
            QVERIFY(!status);

            Container::MemoryContainer::DirectoryMap directory = readContainer.directory();

            if (readContainer.lastStatus()) {
                qDebug() << "Failure: " << readContainer.lastStatus().description().c_str();
            }

            QVERIFY(!readContainer.lastStatus());
            QVERIFY(directory.size() == numberVirtualFiles);

            std::stringstream stream;
            stream << "test" << erasedFile << ".dat";
            Container::Container::DirectoryMap::iterator pos = directory.find(stream.str());
            QVERIFY(pos != directory.end());

            std::shared_ptr<Container::VirtualFile> vf = pos->second;
            status = vf->erase();
            QVERIFY(!status);

            status = readContainer.close();
        }

        // Now open the container again using just the file contents and verify the file map and file sizes.  Then
        // select a file, at random and write into the file at an arbitrary location.  Close the container, re-open the
        // container and confirm the written contents.

        {
            Container::MemoryContainer container1("Inesonic, LLC.\nAleph Test");

            status = container1.open(containerBuffer);
            QVERIFY(!status);

            Container::MemoryContainer::DirectoryMap directory = container1.directory();
            QVERIFY(directory.size() == numberVirtualFiles - 1);

            for (unsigned i=0 ; i<numberVirtualFiles ; ++i) {
                std::stringstream stream;
                stream << "test" << i << ".dat";
                Container::Container::DirectoryMap::iterator pos = directory.find(stream.str());

                if (i != erasedFile) {
                    QVERIFY(pos != directory.end());
                    QVERIFY(pos->second->size() == sizes.at(i));
                } else {
                    QVERIFY(pos == directory.end());
                }
            }

            unsigned fileSelection;
            do {
                fileSelection = fileSelector(rng);
            } while (fileSelection == erasedFile);

            std::stringstream stream;
            stream << "test" << fileSelection << ".dat";
            Container::Container::DirectoryMap::iterator pos = directory.find(stream.str());
            QVERIFY(pos != directory.end());

            std::shared_ptr<Container::VirtualFile> vf = pos->second;

            unsigned maximumWriteLength =   bufferSizeInBytes < vf->size()
                                          ? bufferSizeInBytes
                                          : static_cast<unsigned>(vf->size());

            std::uniform_int_distribution<> lengthGenerator(1, maximumWriteLength);
            unsigned long thisWriteLength = lengthGenerator(rng);

            std::uniform_int_distribution<> startGenerator(0, vf->size() - thisWriteLength);
            unsigned long thisWriteStart = startGenerator(rng);

            std::memset(buffer, 0xFF, thisWriteLength);

            status = vf->setPosition(thisWriteStart);
            QVERIFY(!status);

            status = vf->write(buffer, thisWriteLength);
            QVERIFY(status.success());
            QVERIFY(Container::WriteSuccessful(status).bytesWritten() == thisWriteLength);

            status = container1.close();
            QVERIFY(!status);

            Container::MemoryContainer container2("Inesonic, LLC.\nAleph Test");

            status = container2.open(containerBuffer);
            QVERIFY(!status);

            directory = container2.directory();
            QVERIFY(directory.size() == numberVirtualFiles - 1);

            pos = directory.find(stream.str());
            QVERIFY(pos != directory.end());

            vf = pos->second;

            status = vf->setPosition(0);
            QVERIFY(!status);

            std::uint8_t       max           = static_cast<std::uint8_t>(254 - fileSelection);
            unsigned long long fileSize      = static_cast<unsigned long long>(vf->size());
            unsigned long long thisReadStart = 0;

            while (thisReadStart < fileSize) {
                unsigned long long bytesRemaining = fileSize - thisReadStart;
                unsigned           readLength     =   bufferSizeInBytes < bytesRemaining
                                                    ? bufferSizeInBytes
                                                    : static_cast<unsigned>(bytesRemaining);

                status = vf->read(buffer, readLength);
                QVERIFY(status.success());
                QCOMPARE(Container::ReadSuccessful(status).bytesRead(), readLength);

                for (unsigned i=0 ; i<readLength ; ++i) {
                    unsigned long long position = thisReadStart + i;
                    std::uint8_t       v;
                    if (position < thisWriteStart || position >= (thisWriteStart + thisWriteLength)) {
                        v = static_cast<std::uint8_t>(position % max);
                    } else {
                        v = 0xFF;
                    }

                    QVERIFY(buffer[i] == v);
                }

                thisReadStart += readLength;
            }

            status = container2.close();
            QVERIFY(!status);
        }
    }
}


void TestVirtualFile::testTruncate() {
    std::mt19937                    rng;
    std::uniform_int_distribution<> positionSelector(0, bufferSizeInBytes);

    for (unsigned iteration=0 ; iteration < numberTruncateTests ; ++iteration) {
        Container::MemoryContainer container("Inesonic, LLC.\nAleph Test");

        Container::Status status = container.open();
        QVERIFY(status.success());

        // Create a file containing a sequence of numbers less than 255.

        std::shared_ptr<Container::VirtualFile> vf = container.newVirtualFile("test.dat");
        std::uint8_t                            buffer[bufferSizeInBytes];

        for (unsigned i=0 ; i<bufferSizeInBytes ; ++i) {
            buffer[i] = static_cast<std::uint8_t>(i % 255);
        }

        status = vf->write(buffer, bufferSizeInBytes);
        QVERIFY(status.success());
        QVERIFY(Container::WriteSuccessful(status).bytesWritten() == bufferSizeInBytes);

        // Truncate the file at a randomly selected position.

        unsigned long long truncatePosition = positionSelector(rng);
        status = vf->setPosition(truncatePosition);
        if (status) {
            qDebug() << status.description().c_str();
            QVERIFY(!status);
        }

        status = vf->truncate();
        QVERIFY(!status);

        // Verify file truncated at the correct length.

        QVERIFY(vf->size() == static_cast<long long>(truncatePosition));

        // Write a bunch of 0xFF (255) values to the file.

        std::memset(buffer, 0xFF, bufferSizeInBytes);

        status = vf->write(buffer, bufferSizeInBytes);
        QVERIFY(status.success());
        QVERIFY(Container::WriteSuccessful(status).bytesWritten() == bufferSizeInBytes);

        // Verify the new file size.

        QVERIFY(vf->size() == static_cast<long long>(truncatePosition + bufferSizeInBytes));

        // And verify the file contents.

        status = vf->setPosition(0);
        unsigned long long readPosition = 0;
        unsigned long long fileSize     = static_cast<unsigned long long>(vf->size());

        while (readPosition < fileSize) {
            unsigned bytesToEndOfFile = static_cast<unsigned>(fileSize - readPosition);
            unsigned bytesToRead      = bufferSizeInBytes < bytesToEndOfFile ? bufferSizeInBytes : bytesToEndOfFile;

            status = vf->read(buffer, bytesToRead);

            QVERIFY(status.success());
            QVERIFY(Container::ReadSuccessful(status).bytesRead() == bytesToRead);

            for (unsigned i=0 ; i<bytesToRead ; ++i) {
                unsigned long long position = readPosition + i;
                std::uint8_t       v;
                if (position < truncatePosition) {
                    v = static_cast<std::uint8_t>(position % 255);
                } else {
                    v = 0xFF;
                }

                QVERIFY(buffer[i] == v);
            }

            readPosition += bytesToRead;
        }
    }
}


void TestVirtualFile::testStreamRead() {
    typedef Container::MemoryContainer::MemoryBuffer MemoryBuffer;
    std::shared_ptr<MemoryBuffer> containerBuffer = std::make_shared<MemoryBuffer>();

    Container::MemoryContainer writeContainer("Inesonic, LLC.\nAleph Test");

    Container::Status status = writeContainer.open(containerBuffer);
    QVERIFY(status.success());

    std::vector<std::shared_ptr<Container::VirtualFile>> virtualFiles;
    std::vector<std::uint8_t>                            accumulators;
    std::vector<long long>                               sizes;

    for (unsigned i=0 ; i<numberVirtualFiles ; ++i) {
        std::stringstream stream;
        stream << "test" << i << ".dat";

        virtualFiles.push_back(writeContainer.newVirtualFile(stream.str()));
        accumulators.push_back(0);
        sizes.push_back(0);
    }

    std::mt19937                    rng;
    std::uniform_int_distribution<> fileSelector(0, numberVirtualFiles - 1);

    std::uint8_t buffer[bufferSizeInBytes];

    unsigned long bytesRemaining = sequentialFileSizeInBytes;
    while (!status && bytesRemaining > 0) {
        unsigned maximumLength = bytesRemaining > bufferSizeInBytes ? bufferSizeInBytes : bytesRemaining;
        std::uniform_int_distribution<> lengthGenerator(1, maximumLength);
        unsigned thisWriteLength = lengthGenerator(rng);
        unsigned fileSelection = fileSelector(rng);

        std::shared_ptr<Container::VirtualFile> vf  = virtualFiles.at(fileSelection);
        std::uint8_t                            a   = accumulators.at(fileSelection);
        std::uint8_t                            max = static_cast<std::uint8_t>(254 - fileSelection);

        for (unsigned i=0 ; i<thisWriteLength ; ++i) {
            buffer[i] = a;
            ++a;
            if (a == max) {
                a = 0;
            }
        }

        status = vf->append(buffer, thisWriteLength);

        if (status.success() && Container::WriteSuccessful(status).bytesWritten() == thisWriteLength) {
            status = Container::NoStatus();
        }

        accumulators[fileSelection]  = a;
        sizes[fileSelection]        += thisWriteLength;

        bytesRemaining -= thisWriteLength;
    }

    QVERIFY(!status);

    status = writeContainer.close();
    QVERIFY(!status);

    // Now read the files using the streaming interface.

    ContainerWrapper readContainer("Inesonic, LLC.\nAleph Test");

    status = readContainer.open(containerBuffer);
    QVERIFY(!status);

    status = readContainer.streamRead();
    QVERIFY(!status);

    ContainerWrapper::DirectoryMap directory = readContainer.directory();
    QVERIFY(directory.size() == numberVirtualFiles);

    for (unsigned i=0 ; i<numberVirtualFiles ; ++i) {
        std::stringstream stream;
        stream << "test" << i << ".dat";

        ContainerWrapper::DirectoryMap::iterator pos = directory.find(stream.str());
        QVERIFY(pos != directory.end());

        std::shared_ptr<VirtualFileWrapper> vf = std::dynamic_pointer_cast<VirtualFileWrapper>(pos->second);
        std::vector<std::uint8_t> receivedData = vf->dataBuffer();

        unsigned long length = static_cast<unsigned>(receivedData.size());
        std::uint8_t  max    = static_cast<std::uint8_t>(254 - i);

        QVERIFY(length == static_cast<unsigned long>(sizes.at(i)));
        for (unsigned j=0 ; j<length ; ++j) {
            std::uint8_t a = j % max;
            QVERIFY(receivedData.at(j) == a);
        }
    }
}
