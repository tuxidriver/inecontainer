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
* This file implements the \ref Container::FileContainer::Private class.
***********************************************************************************************************************/

#define _FILE_OFFSET_BITS 64 // Causes file methods to operate with 64-bit values.

#include <cstdint>
#include <cstdio>
#include <string>
#include <cassert>
#include <cerrno>

#include "container_status.h"
#include "container_file_container.h"
#include "container_file_container_private.h"

#if (defined(_WIN32) || defined(_WIN64))

    #include <io.h> // For _chsize_s function.

    static int largeSeek(std::FILE* file, long long offset, int whence) {
        return _fseeki64(file, offset, whence);
    }


    static unsigned long long largeTell(std::FILE* file) {
        return _ftelli64(file);
    }

#elif (defined(__linux__))

    #include <unistd.h>
    #include <sys/types.h>

    static int largeSeek(std::FILE* file, long long offset, int whence) {
        return fseeko64(file, offset, whence);
    }


    static unsigned long long largeTell(std::FILE* file) {
        return ftello64(file);
    }

#elif (defined(__APPLE__))

    #include <unistd.h>
    #include <sys/types.h>

    static int largeSeek(std::FILE* file, long long offset, int whence) {
        return fseeko(file, offset, whence);
    }


    static unsigned long long largeTell(std::FILE* file) {
        return ftello(file);
    }

#else

    #error Unknown platform

#endif

namespace Container {
    FileContainer::Private::Private(FileContainer* interface) {
        iface           = interface;
        currentFilename = "";
        currentOpenMode = FileContainer::OpenMode::CLOSED;
        fileHandle      = nullptr;
    }


    FileContainer::Private::~Private() {}


    Status FileContainer::Private::open(const std::string& filename, FileContainer::OpenMode openMode) {
        Status status;

        if (fileHandle != nullptr) {
            status = close();
        }

        if (!status) {
            switch(openMode) {
                case FileContainer::OpenMode::CLOSED: {
                    status = InvalidOpenMode(openMode);
                    break;
                }

                case FileContainer::OpenMode::READ_ONLY: {
                    fileHandle = std::fopen(filename.c_str(), "rb");
                    break;
                }

                case FileContainer::OpenMode::READ_WRITE: {
                    fileHandle = std::fopen(filename.c_str(), "r+b");
                    break;
                }

                case FileContainer::OpenMode::OVERWRITE: {
                    fileHandle = std::fopen(filename.c_str(), "w+b");
                    break;
                }

                default: {
                    status = InvalidOpenMode(openMode);
                    break;
                }
            }

            if (!status && fileHandle == nullptr) {
                status = FailedToOpenFile(filename, openMode, errno);
            }

            if (!status) {
                currentFilename = filename;
                currentOpenMode = openMode;

                if (openMode == FileContainer::OpenMode::OVERWRITE) {
                    currentFileSize = 0;
                } else {
                    int result = largeSeek(fileHandle, 0LL, SEEK_END);

                    if (result == 0) {
                        currentFileSize = largeTell(fileHandle);
                        result = largeSeek(fileHandle, 0LL, SEEK_SET);
                    }

                    if (result != 0) {
                        status = FailedToOpenFile(filename, openMode, errno);
                    }
                }
            }
        }

        return status;
    }


    Status FileContainer::Private::close() {
        Status status;

        if (fileHandle != nullptr) {
            int result = fclose(fileHandle);

            if (result != 0) {
                status = FileCloseError(currentFilename, errno);
            }

            fileHandle = nullptr;
            currentFilename = "";
            currentOpenMode = FileContainer::OpenMode::CLOSED;
        }

        return status;
    }


    std::string FileContainer::Private::filename() const {
        return currentFilename;
    }


    FileContainer::OpenMode FileContainer::Private::openMode() const {
        return currentOpenMode;
    }


    long long FileContainer::Private::size() {
        return currentFileSize;
    }


    Status FileContainer::Private::setPosition(unsigned long long newOffset) {
        Status status;

        if (fileHandle == nullptr) {
            status = FileContainerNotOpen();
        } else if (newOffset > currentFileSize) {
            status = SeekError(newOffset, currentFileSize);
        } else {
            int result = largeSeek(fileHandle, newOffset, SEEK_SET);

            if (result != 0) {
                status = SeekError(newOffset, newOffset);
            }
        }

        return status;
    }


    Status FileContainer::Private::setPositionLast() {
        Status status;

        if (fileHandle == nullptr) {
            status = FileContainerNotOpen();
        } else {
            int result = largeSeek(fileHandle, 0LL, SEEK_END);

            if (result != 0) {
                status = SeekError(currentFileSize, currentFileSize);
            }
        }

        return status;
    }


    unsigned long long FileContainer::Private::position() const {
        unsigned long long currentPosition;

        if (fileHandle == nullptr) {
            currentPosition = 0;
        } else {
            currentPosition = largeTell(fileHandle);
        }

        return currentPosition;
    }


    Status FileContainer::Private::read(std::uint8_t* buffer, unsigned desiredCount) {
        Status status;

        if (fileHandle == nullptr) {
            status = FileContainerNotOpen();
        } else {
            unsigned long long currentPosition = largeTell(fileHandle);
            unsigned bytesRead = static_cast<unsigned>(std::fread(buffer, 1, desiredCount, fileHandle));

            if (bytesRead != desiredCount && !feof(fileHandle)) {
                status = FileReadError(currentFilename, currentPosition, errno);
            } else {
                status = ReadSuccessful(bytesRead);
            }
        }

        return status;
    }


    Status FileContainer::Private::write(const std::uint8_t* buffer, unsigned count) {
        Status status;

        if (fileHandle == nullptr) {
            status = FileContainerNotOpen();
        } else {
            unsigned long long currentPosition = largeTell(fileHandle);
            unsigned bytesWritten = static_cast<unsigned>(std::fwrite(buffer, 1, count, fileHandle));

            if (bytesWritten != count) {
                status = FileWriteError(currentFilename, currentPosition, errno);
            } else {
                status = WriteSuccessful(bytesWritten);

                unsigned long long endingPosition = currentPosition + bytesWritten;
                if (endingPosition > currentFileSize) {
                    currentFileSize = endingPosition;
                }
            }
        }

        return status;
    }


    bool FileContainer::Private::supportsTruncation() const {
        return true;
    }


    Status FileContainer::Private::truncate() {
        Status status;

        if (fileHandle == nullptr) {
            status = FileContainerNotOpen();
        } else {
            #if (defined(_WIN32) || defined(_WIN64))

                unsigned long long currentPosition = largeTell(fileHandle);
                int result = _chsize_s(fileno(fileHandle), currentPosition);

                if (result != 0) {
                    status = FileTruncateError(currentFilename, result, errno);
                }

            #elif (defined(__linux__))

                unsigned long long currentPosition = largeTell(fileHandle);
                int result = std::fflush(fileHandle);

                if (result == 0) {
                    result = ftruncate(fileno(fileHandle), currentPosition);
                }

                if (result == 0) {
                    FILE* newFileHandle;

                    switch(currentOpenMode) {
                        case FileContainer::OpenMode::READ_ONLY: {
                            newFileHandle = std::freopen(currentFilename.c_str(), "rb", fileHandle);
                            break;
                        }

                        case FileContainer::OpenMode::READ_WRITE:
                        case FileContainer::OpenMode::OVERWRITE: {
                            newFileHandle = std::freopen(currentFilename.c_str(), "r+b", fileHandle);
                            break;
                        }

                        default: {
                            newFileHandle = nullptr;
                            break;
                        }
                    }

                    if (newFileHandle == nullptr) {
                        result = -1;
                    } else {
                        fileHandle = newFileHandle;
                        status = setPosition(currentPosition);
                    }
                }

                if (result != 0) {
                    status = FileTruncateError(currentFilename, currentPosition, errno);
                }

            #elif (defined(__APPLE__))

                unsigned long long currentPosition = largeTell(fileHandle);
                int result = std::fflush(fileHandle);

                if (result == 0) {
                    result = ftruncate(fileno(fileHandle), currentPosition);
                }

                if (result == 0) {
                    result = std::fclose(fileHandle);
                }

                if (result == 0) {
                    FILE* newFileHandle;
                    switch(currentOpenMode) {
                        case FileContainer::OpenMode::READ_ONLY: {
                            newFileHandle = std::fopen(currentFilename.c_str(), "rb");
                            break;
                        }

                        case FileContainer::OpenMode::READ_WRITE:
                        case FileContainer::OpenMode::OVERWRITE: {
                            newFileHandle = std::fopen(currentFilename.c_str(), "r+b");
                            break;
                        }

                        default: {
                            newFileHandle = nullptr;
                            break;
                        }
                    }

                    if (newFileHandle == nullptr) {
                        result = -1;
                    } else {
                        fileHandle = newFileHandle;
                        status = setPosition(currentPosition);
                    }

                    if (result != 0) {
                        status = FileTruncateError(currentFilename, currentPosition, errno);
                    }
                }

            #endif
        }

        return status;
    }


    Status FileContainer::Private::flush() {
        Status status;

        if (fileHandle == nullptr) {
            status = FileContainerNotOpen();
        } else {
            int result = std::fflush(fileHandle);

            if (result != 0) {
                status = FileFlushError(currentFilename, errno);
            }
        }

        return status;
    }
}
