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
* This file implements the \ref Container::Container::Private class.
***********************************************************************************************************************/

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <cassert>

#include "container_status.h"
#include "container_area.h"
#include "free_space.h"
#include "chunk_header.h"
#include "chunk.h"
#include "file_header_chunk.h"
#include "fill_chunk.h"
#include "stream_start_chunk.h"
#include "container_virtual_file.h"
#include "virtual_file_impl.h"
#include "container_container.h"
#include "container_impl.h"

ContainerImpl::ContainerImpl(const std::string& fileIdentifier, bool ignoreIdentifier) {
    ignoreIdentifierOnOpen = ignoreIdentifier;
    currentFileIdentifier  = fileIdentifier;
    fileMapsPopulated      = false;
    currentMinorVersion    = static_cast<std::uint8_t>(-1);
    startingFileIndex      = ChunkHeader::invalidFileIndex;
}


ContainerImpl::~ContainerImpl() {}


void ContainerImpl::setWeakThis(std::weak_ptr<ContainerImpl> newWeakPointer) {
    weakThis = newWeakPointer;
}


void ContainerImpl::setLastStatus(const Container::Status& newStatus) {
    lastReportedStatus = newStatus;
}


Container::Status ContainerImpl::lastStatus() const {
    return lastReportedStatus;
}


std::uint8_t ContainerImpl::minorVersion() const {
    return currentMinorVersion;
}


Container::Status ContainerImpl::open() {
    Container::Status status;

    status = setPosition(0);
    if (!status) {
        std::uint8_t commonHeader[ChunkHeader::minimumChunkHeaderSizeBytes];
        status = read(commonHeader, ChunkHeader::minimumChunkHeaderSizeBytes);

        if (status.success()                                                                          &&
            Container::ReadSuccessful(status).bytesRead() == ChunkHeader::minimumChunkHeaderSizeBytes    ) {
            FileHeaderChunk fileHeader(weakThis, 0, commonHeader);
            status = fileHeader.load();

            if (!ignoreIdentifierOnOpen) {
                if (!status) {
                    if (fileHeader.identifier() != currentFileIdentifier) {
                        status = Container::HeaderIdentifierInvalid();
                    }
                }

                if (!status) {
                    if (fileHeader.majorVersion() < Container::Container::containerMajorVersion) {
                        status = Container::VersionDownlevelStatus(
                            fileHeader.majorVersion(),
                            Container::Container::containerMajorVersion
                        );
                    }
                }

                if (!status) {
                    if (fileHeader.majorVersion() > Container::Container::containerMajorVersion) {
                        status = Container::HeaderVersionInvalid(
                            fileHeader.majorVersion(),
                            Container::Container::containerMajorVersion
                        );
                    }
                }

            }

            if (!status) {
                if (!fileHeader.checkCrc()) {
                    status = Container::HeaderCrcError();
                }
            }

            if (!status) {
                currentMinorVersion = fileHeader.minorVersion();
                startingFileIndex   = ChunkHeader::toFileIndex(fileHeader.chunkSize());
            }

            if (size() == startingFileIndex) {
                fileMapsPopulated = true;
            } else {
                fileMapsPopulated = false;
            }
        } else if (size() == 0) {
            FileHeaderChunk fileHeader(weakThis, 0, currentFileIdentifier);
            status = fileHeader.save();

            if (!status) {
                currentMinorVersion = fileHeader.minorVersion();
                startingFileIndex   = ChunkHeader::toFileIndex(fileHeader.chunkSize());
                fileMapsPopulated   = true;
            }
        }
    }

    if (status) {
        currentMinorVersion = static_cast<std::uint8_t>(-1);
        startingFileIndex   = ChunkHeader::invalidFileIndex;
        fileMapsPopulated   = false;
    }

    filesByIdentifier.clear();
    fileApisByName.clear();
    filesByName.clear();

    clearFreeSpace();

    lastReportedStatus = status;
    return status;
}


Container::Status ContainerImpl::close() {
    Container::Status status;

    DirectoryMap::iterator pos = filesByName.begin();
    DirectoryMap::iterator end = filesByName.end();

    bool success = flushFreeSpace();
    if (!success) {
        status = lastReportedStatus;
    } else {
        while (!status && pos != end) {
            status = pos->second->flush();
            ++pos;
        }

        lastReportedStatus = status;
    }

    return status;
}


Container::Container::DirectoryMap ContainerImpl::directory() {
    if (!fileMapsPopulated) {
        lastReportedStatus = traverseContainer(true);
    }

    return fileApisByName;
}


Container::Status ContainerImpl::streamRead() {
    Container::Status status = traverseContainer(false);

    DirectoryMap::iterator pos = filesByName.begin();
    DirectoryMap::iterator end = filesByName.end();

    while (!status && pos != end) {
        status = pos->second->endOfFile();
        ++pos;
    }

    return status;
}


StreamChunk::StreamIdentifier ContainerImpl::newStreamIdentifier(bool *ok) {
    StreamChunk::StreamIdentifier newIdentifier = StreamChunk::invalidStreamIdentifier;
    bool                          isOK          = true;

    if (!fileMapsPopulated) {
        lastReportedStatus = traverseContainer(true);
        if (lastReportedStatus) {
            isOK = false;
        }
    }

    if (isOK) {
        do {
            if (newIdentifier == StreamChunk::invalidStreamIdentifier) {
                newIdentifier = 0;
            } else {
                ++newIdentifier;
                assert(newIdentifier != StreamChunk::invalidStreamIdentifier);
            }
        } while (filesByIdentifier.find(newIdentifier) != filesByIdentifier.end());
    }

    if (ok != nullptr) {
        *ok = isOK;
    }

    return newIdentifier;
}


std::shared_ptr<Container::VirtualFile> ContainerImpl::newVirtualFile(const std::string& newVirtualFileName) {
    std::shared_ptr<Container::VirtualFile> result;

    if (!fileMapsPopulated) {
        lastReportedStatus = traverseContainer(true);
    }

    Container::Container::DirectoryMap::iterator pos = fileApisByName.find(newVirtualFileName);
    if (pos == fileApisByName.end()) {
        Container::VirtualFile* virtualFile = createFile(newVirtualFileName);
        if (virtualFile != nullptr) {
            result.reset(virtualFile);
            fileApisByName.insert(Container::Container::DirectoryMapPair(newVirtualFileName, result));
        }
    }

    return result;
}


bool ContainerImpl::containerScanNeeded() const {
    return !fileMapsPopulated;
}


Container::Status ContainerImpl::scanContainer() {
    Container::Status status;

    if (!fileMapsPopulated) {
        status = traverseContainer(true);
    }

    lastReportedStatus = status;
    return status;
}


void ContainerImpl::registerFileImplementation(std::shared_ptr<VirtualFileImpl> virtualFile) {
    assert(filesByName.find(virtualFile->name()) == filesByName.end());
    filesByName.insert(DirectoryMapPair(virtualFile->name(), virtualFile));
    filesByIdentifier.insert(IdentifierMapPair(virtualFile->streamIdentifier(), virtualFile));
}


bool ContainerImpl::fileRenamed(const std::string& oldName, const std::string& newName) {
    bool success;

    Container::Container::DirectoryMap::iterator apiNameIterator = fileApisByName.find(oldName);
    DirectoryMap::iterator                       nameIterator = filesByName.find(oldName);

    if (apiNameIterator != fileApisByName.end() && nameIterator != filesByName.end()) {
        fileApisByName.insert(
            Container::Container::DirectoryMapPair(newName, apiNameIterator->second)
        );

        filesByName.insert(DirectoryMapPair(newName, nameIterator->second));

        fileApisByName.erase(apiNameIterator);
        filesByName.erase(nameIterator);

        success = true;
    } else {
        success = false;
    }

    return success;
}


bool ContainerImpl::fileErased(const std::string& name) {
    bool success;

    Container::Container::DirectoryMap::iterator apiNameIterator = fileApisByName.find(name);
    DirectoryMap::iterator                       nameIterator = filesByName.find(name);

    if (apiNameIterator != fileApisByName.end() && nameIterator != filesByName.end()) {
        fileApisByName.erase(apiNameIterator);
        filesByName.erase(nameIterator);

        success = true;
    } else {
        success = false;
    }

    return success;
}


bool ContainerImpl::flushArea(const ContainerArea& area) {
    Container::Status status;

    unsigned long long containerSize = static_cast<unsigned long long>(size());
    if (supportsTruncation() && ChunkHeader::toPosition(area.endingIndex()) >= containerSize) {
        // If the area to flush and mark as free goes all the way to the end of the file and the container object
        // supports file truncation, truncate the file.

        status = setPosition(ChunkHeader::toPosition(area.startingIndex()));
        if (!status) {
            status = truncate();
        }
    } else {
        // Else, write the areas as free.

        ContainerArea remainingArea = area;

        while (!status && remainingArea.areaSize() > 0) {
            FillChunk chunk(
                weakThis,
                remainingArea.startingIndex(),
                static_cast<unsigned>(ChunkHeader::toPosition(remainingArea.areaSize()))
            );

            status = chunk.save();

            if (!status) {
                remainingArea.reduceBy(ChunkHeader::toFileIndex(chunk.chunkSize()), ContainerArea::Side::FROM_FRONT);
            }
        }
    }

    lastReportedStatus = status;
    return status.success();
}


Container::Status ContainerImpl::traverseContainer(bool buildMapsOnly) {
    Container::Status status;

    fileMapsPopulated = true;

    unsigned long long currentPosition = ChunkHeader::toPosition(startingFileIndex);
    unsigned long long fileSize        = size();

    std::uint8_t* buffer;

    if (buildMapsOnly) {
        buffer = nullptr;
    } else {
        buffer = new std::uint8_t[ChunkHeader::maximumChunkSize];
    }

    while (!status && currentPosition < fileSize) {
        std::uint8_t commonHeader[ChunkHeader::minimumChunkHeaderSizeBytes];

        status = setPosition(currentPosition);

        if (!status) {
            status = read(commonHeader, ChunkHeader::minimumChunkHeaderSizeBytes);

            if (status.success()                                                                          &&
                Container::ReadSuccessful(status).bytesRead() == ChunkHeader::minimumChunkHeaderSizeBytes    ) {
                status = Container::NoStatus();
            }
        }

        unsigned chunkSize = 0;
        if (!status) {
            ChunkHeader header(commonHeader);

            ChunkHeader::Type type = header.type();
            chunkSize = header.chunkSize();

            switch(type) {
                case Chunk::Type::FILL_CHUNK: {
                    newFreeSpaceArea(
                        ChunkHeader::toFileIndex(currentPosition),
                        ChunkHeader::toFileIndex(chunkSize),
                        false
                    );

                    break;
                }

                case Chunk::Type::STREAM_START_CHUNK: {
                    StreamStartChunk streamStartChunk(weakThis, Chunk::toFileIndex(currentPosition), commonHeader);
                    status = streamStartChunk.load(false);

                    std::string                   virtualFilename;
                    StreamChunk::StreamIdentifier identifier = StreamChunk::invalidStreamIdentifier;

                    if (!status) {
                        virtualFilename = streamStartChunk.virtualFilename();
                        identifier      = streamStartChunk.streamIdentifier();

                        if (!status && filesByName.find(virtualFilename) != filesByName.end()) {
                            status = Container::FilenameMismatch(virtualFilename, "", currentPosition);
                        }
                    }

                    if (!status) {
                        std::shared_ptr<Container::VirtualFile> vf = callNewVirtualFile(virtualFilename);

                        if (!vf) {
                            status = Container::FileCreationError(virtualFilename, currentPosition);
                        }

                        DirectoryMap::iterator pos = filesByName.find(virtualFilename);
                        assert(pos != filesByName.end());

                        std::shared_ptr<VirtualFileImpl> vfi = pos->second;

                        // The virtual file will automatically assign an identifier and it may be incorrect. We
                        // check if the identifier is incorrect and change it here, if needed.

                        StreamChunk::StreamIdentifier guessIdentifier = vfi->streamIdentifier();

                        if (guessIdentifier != identifier) {
                            IdentifierMap::iterator posByIdentifier = filesByIdentifier.find(guessIdentifier);
                            assert(posByIdentifier != filesByIdentifier.end());

                            vfi->setStreamIdentifier(identifier);

                            filesByIdentifier.insert(IdentifierMapPair(identifier, posByIdentifier->second));
                            filesByIdentifier.erase(posByIdentifier);
                        }

                        vfi->setStreamStartIndex(streamStartChunk.fileIndex());
                    }

                    break;
                }

                case Chunk::Type::STREAM_DATA_CHUNK: {
                    StreamDataChunk streamDataChunk(weakThis, Chunk::toFileIndex(currentPosition), commonHeader);

                    if (buildMapsOnly) {
                        status = streamDataChunk.loadHeader(false);
                    } else {
                        streamDataChunk.addScatterGatherListSegment(buffer, ChunkHeader::maximumChunkSize);
                        status = streamDataChunk.load(false);
                    }

                    if (!status) {
                        StreamChunk::StreamIdentifier identifier = streamDataChunk.streamIdentifier();

                        IdentifierMap::iterator pos = filesByIdentifier.find(identifier);
                        if (pos == filesByIdentifier.end()) {
                            status = Container::StreamIdentifierMismatch(identifier, 0, currentPosition);
                        } else {
                            std::shared_ptr<VirtualFileImpl> vf = pos->second;
                            vf->addChunkLocation(
                                streamDataChunk.fileIndex(),
                                streamDataChunk.chunkOffset(),
                                streamDataChunk.payloadSize()
                            );

                            if (!buildMapsOnly) {
                                status = vf->receivedData(
                                    buffer,
                                    streamDataChunk.scatterGatherListSegment(0).processedCount()
                                );
                            }
                        }
                    }

                    break;
                }

                case Chunk::Type::FILE_HEADER_CHUNK: {
                    status = Container::ContainerDataError(currentPosition);
                    break;
                }
            }
        }

        if (!status) {
            currentPosition += chunkSize;
        }
    }

    if (buffer != nullptr) {
        delete[] buffer;
    }

    return status;
}
