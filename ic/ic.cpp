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
* This file contains the main entry point for ic command line tool.
***********************************************************************************************************************/

#include <string>
#include <iostream>
#include <iomanip>
#include <vector>
#include <memory>
#include <cstdint>
#include <cstring>
#include <cstdio>

#include <container_container.h>
#include <container_file_container.h>
#include <container_virtual_file.h>
#include <container_status.h>

#if (defined(_WIN32) || defined(_WIN64))

    // Why Windows needs to be just slightly different than everyone else is beyond me.

    #include <io.h>

    bool fileReadable(const std::string& filename) {
        return _access(filename.c_str(), 04) != -1;
    }

#elif (defined(__linux__) || defined(__APPLE__))

    #include <unistd.h>

    bool fileReadable(const std::string& filename) {
        return access(filename.c_str(), R_OK) == 0;
    }

#else

    #error Unknown platform

#endif

static const std::string   fileIdentifier = "Inesonic, LLC.\nAleph";
static const unsigned long bufferSize     = 65536;


int extractVirtualFile(std::shared_ptr<Container::VirtualFile> vf) {
    int                exitStatus     = 0;
    std::string        filename       = vf->name();
    unsigned long long size           = vf->size();
    unsigned long long bytesRemaining = size;

    std::uint8_t* buffer = new std::uint8_t[bufferSize];

    FILE* file = std::fopen(filename.c_str(), "wb");
    if (file == nullptr) {
        std::cerr << "*** Could not open file " << filename << " for writing." << std::endl;
        exitStatus = 1;
    }

    while (exitStatus == 0 && bytesRemaining > 0) {
        unsigned long bytesToMove = static_cast<unsigned long>(  bytesRemaining < bufferSize
                                                               ? bytesRemaining
                                                               : bufferSize
                                                              );

        Container::Status status = vf->read(buffer, bytesToMove);
        if (!status.success() || Container::ReadSuccessful(status).bytesRead() != bytesToMove) {
            std::cerr << "*** Could not read " << filename << " from container" << std::endl;
            exitStatus = 1;
        } else {
            unsigned long bytesWritten = static_cast<unsigned long>(std::fwrite(buffer, 1, bytesToMove, file));

            if (bytesWritten != bytesToMove) {
                std::cerr << "*** Could not write to file " << filename << std::endl;
                exitStatus = 1;
            } else {
                bytesRemaining -= bytesToMove;
            }
        }
    }

    if (exitStatus == 0) {
        std::cout << "Extracted " << filename << " (" << size << " bytes)" << std::endl;
    }

    std::fclose(file);

    delete[] buffer;

    return exitStatus;
}


int doList(const Container::FileContainer::DirectoryMap& directory) {
    unsigned           maximumFilenameWidth = 8;
    unsigned long long totalContentSize = 0;

    Container::FileContainer::DirectoryMap::const_iterator pos = directory.cbegin();
    Container::FileContainer::DirectoryMap::const_iterator end = directory.cend();

    while (pos != end) {
        std::string        filename     = pos->first;
        unsigned long long fileSize     = static_cast<unsigned long long>(pos->second->size());
        unsigned           filenameSize = static_cast<unsigned>(filename.size());

        totalContentSize += fileSize;
        if (filenameSize > maximumFilenameWidth) {
            maximumFilenameWidth = filenameSize;
        }

        ++pos;
    }

    unsigned i;
    std::cout << "Filename";
    for (i=8 ; i<maximumFilenameWidth ; ++i) {
        std::cout << " ";
    }
    std::cout << "        Size" << std::endl;

    for (i=0 ; i<maximumFilenameWidth ; ++i) {
        std::cout << "-";
    }

    std::cout << "  ----------" << std::endl;

    pos = directory.cbegin();
    end = directory.cend();

    while (pos != end) {
        std::string        filename     = pos->first;
        unsigned long long fileSize     = static_cast<unsigned long long>(pos->second->size());
        unsigned           filenameSize = static_cast<unsigned>(filename.size());

        std::cout << filename;
        for (unsigned i=filenameSize ; i<maximumFilenameWidth ; ++i) {
            std::cout << " ";
        }

        std::cout << "  " << std::setw(10) << fileSize << std::setw(0) << std::endl;

        ++pos;
    }

    for (i=0 ; i<maximumFilenameWidth ; ++i) {
        std::cout << " ";
    }

    std::cout << "  ----------" << std::endl;

    for (i=0 ; i<maximumFilenameWidth ; ++i) {
        std::cout << " ";
    }

    std::cout << "  " << std::setw(10) << totalContentSize << std::setw(0) << std::endl;
    return 0;
}


int doInitialize(Container::FileContainer::DirectoryMap& directory) {
    Container::Status status;

    Container::FileContainer::DirectoryMap::iterator it = directory.begin();
    while (!status && it != directory.end()) {
        std::shared_ptr<Container::VirtualFile> file = it->second;
        status = file->erase();
    }

    int exitStatus;

    if (status) {
        std::cerr << "*** " << status.description() << std::endl;
        exitStatus = 1;
    } else {
        directory.clear();
        exitStatus = 0;
    }

    return exitStatus;
}


int doExport(const Container::FileContainer::DirectoryMap& directory) {
    int exitStatus = 0;

    Container::FileContainer::DirectoryMap::const_iterator pos = directory.cbegin();
    Container::FileContainer::DirectoryMap::const_iterator end = directory.cend();

    while (exitStatus == 0 && pos != end) {
        std::shared_ptr<Container::VirtualFile> vf = pos->second;
        exitStatus = extractVirtualFile(vf);
        ++pos;
    }

    return exitStatus;
}


int doRemove(Container::FileContainer::DirectoryMap& directory, const std::string& filename) {
    int exitStatus = 0;

    Container::FileContainer::DirectoryMap::iterator it = directory.find(filename);
    if (it != directory.end()) {
        std::shared_ptr<Container::VirtualFile> vf     = it->second;
        Container::Status                       status = vf->erase();

        if (!status) {
            directory.erase(it);
        } else {
            std::cerr << "*** Could not erase " << filename << " from container." << std::endl;
            exitStatus = 1;
        }
    }

    return exitStatus;
}


int doExtract(const Container::FileContainer::DirectoryMap& directory, const std::string& filename) {
    int exitStatus = 0;

    Container::FileContainer::DirectoryMap::const_iterator it = directory.find(filename);
    if (it != directory.end()) {
        std::shared_ptr<Container::VirtualFile> vf = it->second;
        exitStatus = extractVirtualFile(vf);
    } else {
        std::cerr << "*** Could not locate " << filename << " in container." << std::endl;
        exitStatus = 1;
    }

    return exitStatus;
}


int doImport(
        Container::FileContainer&               container,
        Container::FileContainer::DirectoryMap& directory,
        const std::string&                      filename
    ) {
    int exitStatus = 0;

    if (!fileReadable(filename)) {
        std::cerr << "*** File " << filename << " can not be read, aborting." << std::endl;
        exitStatus = 1;
    }

    if (exitStatus == 0) {
        Container::FileContainer::DirectoryMap::iterator it = directory.find(filename);
        if (it != directory.end()) {
            Container::Status status = it->second->erase();
            if (status) {
                std::cerr << "*** Could not delete existing " << filename << " from container: "
                          << status.description() << std::endl;
                exitStatus = 1;
            } else {
                directory.erase(it);
            }
        }
    }

    if (exitStatus == 0) {
        std::shared_ptr<Container::VirtualFile> vf = container.newVirtualFile(filename);
        if (!vf) {
            std::cerr << "*** Could not create container file " << filename << ": "
                      << container.lastStatus().description() << std::endl;
            exitStatus = 1;
        }

        FILE* file = nullptr;
        if (exitStatus == 0) {
            file = fopen(filename.c_str(), "rb");

            if (file == nullptr) {
                exitStatus = 1;
                std::cerr << "*** Could not open physical file " << filename << std::endl;
            }
        }

        std::uint8_t buffer[bufferSize];
        while (exitStatus == 0 && !feof(file)) {
            long bytesRead = static_cast<long>(fread(buffer, 1, bufferSize, file));

            if (bytesRead < 0) {
                std::cerr << "*** Could not read file " << filename << std::endl;
                exitStatus = 1;
            } else {
                Container::Status status = vf->write(buffer, static_cast<unsigned long>(bytesRead));

                if (!status.success()) {
                    std::cerr << "*** Could not write container " << filename << std::endl;
                    exitStatus = 1;
                }
            }
        }

        fclose(file);
    }

    directory = container.directory();

    return exitStatus;
}


int main(int argumentCount, char* argumentValues[]) {
    int exitStatus = 0;

    unsigned argumentNumber = 1;

    while (argumentNumber < static_cast<unsigned>(argumentCount)      &&
           std::strcmp(argumentValues[argumentNumber], "-h") != 0     &&
           std::strcmp(argumentValues[argumentNumber], "--help") != 0 &&
           std::strcmp(argumentValues[argumentNumber], "-?") != 0        ) {
        ++argumentNumber;
    }

    if (argumentNumber < static_cast<unsigned>(argumentCount)) {
        std::cout
            << "Inesonic Container Tool, Version 1.0" << std::endl
            << "Copyright 2016 - 2022 Inesonic, LLC." << std::endl
            << std::endl
            << "Usage:" << std::endl
            << "    ic [ -h | --help | -? |" << std::endl
            << "         <container>" << std::endl
            << "                     [ -l | --list ]" << std::endl
            << "                     [ -I | --initialize ]" << std::endl
            << "                     [ -X | --export ]" << std::endl
            << "                     [ -r <filename> | --remove <filename> ]" << std::endl
            << "                     [ -x <filename> | --export <filename> ]" << std::endl
            << "                     [ -i <filename> | --import <filename> ]" << std::endl
            << "                     . . .                                     ]" << std::endl
            << std::endl
            << "    -h | --help | -?" << std::endl
            << "      Displays this help screen, then exits.  All other switches and options" << std::endl
            << "      will be ignored." << std::endl
            << std::endl
            << "    <container>" << std::endl
            << "      The container file to perform all operations on.  The container file" << std::endl
            << "      is required for all switches except \"--help\" and must precede all" << std::endl
            << "      switches." << std::endl
            << std::endl
            << "    -l | --list" << std::endl
            << "      Lists the current contents of the container." << std::endl
            << std::endl
            << "    -I | --initialize" << std::endl
            << "      Erases any content in the container and restores the container to an" << std::endl
            << "      empty state." << std::endl
            << std::endl
            << "    -X | --export" << std::endl
            << "      Exports the entire contents of the container into the current directory." << std::endl
            << std::endl
            << "    -r <filename> | --remove <filename>" << std::endl
            << "      Removes the specified file from the container." << std::endl
            << std::endl
            << "    -x <filename> | --extract <filename>" << std::endl
            << "      Exports the specified file from the container.  The container is" << std::endl
            << "      unchanged." << std::endl
            << std::endl
            << "    -i <filename> | --import <filename>" << std::endl
            << "      Imports the specified file into the container." << std::endl
            << std::endl
            << "Notes:" << std::endl
            << "    With the exception of the \"--help\" switch, you can include multiple" << std::endl
            << "    switches on the command line and in arbitrary order.  This allows you to" << std::endl
            << "    perform complex operations on a container with a single command line.  The" << std::endl
            << "    tool will exit on the first error." << std::endl;
    } else {
        argumentNumber = 1;

        unsigned containerFilePosition = 0;
        while (argumentNumber < static_cast<unsigned>(argumentCount) && argumentValues[argumentNumber][0] == '-') {
            ++argumentNumber;
        }

        if (argumentNumber < static_cast<unsigned>(argumentCount)) {
            containerFilePosition = argumentNumber;
            std::string              containerFilename(argumentValues[argumentNumber]);
            Container::FileContainer container(fileIdentifier, true);

            Container::Status status = container.open(
                containerFilename,
                Container::FileContainer::OpenMode::READ_WRITE
            );

            if (status) {
                std::cerr << "*** " << status.description() << std::endl;
                exitStatus = 1;
            }

            Container::FileContainer::DirectoryMap directory;
            if (exitStatus == 0) {
                directory = container.directory();

                if (container.lastStatus()) {
                    std::cerr << "*** " << container.lastStatus().description() << std::endl;
                    exitStatus = 1;
                }
            }

            argumentNumber = 1;
            while (exitStatus == 0 && argumentNumber < static_cast<unsigned>(argumentCount)) {
                if (argumentNumber != containerFilePosition) {
                    std::string switchArgument(argumentValues[argumentNumber]);

                    if (switchArgument == "-l" || switchArgument == "--list") {
                        exitStatus = doList(directory);
                    } else if (switchArgument == "-I" || switchArgument == "--initialize") {
                        exitStatus = doInitialize(directory);
                    } else if (switchArgument == "-X" || switchArgument == "--export") {
                        exitStatus = doExport(directory);
                    } else {
                        unsigned nextArgument = argumentNumber;
                        do {
                            ++nextArgument;
                        } while (nextArgument < static_cast<unsigned>(argumentCount) &&
                                 nextArgument == containerFilePosition                  );

                        if (nextArgument <= static_cast<unsigned>(argumentCount)) {
                            std::string parameterArgument(argumentValues[nextArgument]);

                            if (switchArgument == "-r" || switchArgument == "--remove") {
                                exitStatus = doRemove(directory, parameterArgument);
                                ++argumentNumber;
                            } else if (switchArgument == "-e" || switchArgument == "--export") {
                                exitStatus = doExtract(directory, parameterArgument);
                                ++argumentNumber;
                            } else if (switchArgument == "-i" || switchArgument == "--import") {
                                exitStatus = doImport(container, directory, parameterArgument);
                                ++argumentNumber;
                            } else {
                                std::cerr << "*** Unknown switch \"" << switchArgument << "\"." << std::endl;
                                exitStatus = 1;
                            }
                        } else {
                            std::cerr << "*** Switch " << switchArgument << " expects a parameter." << std::endl;
                            exitStatus = 1;
                        }
                    }
                }

                ++argumentNumber;
            }

            if (container.openMode() != Container::FileContainer::OpenMode::CLOSED) {
                Container::Status status = container.close();
                if (status) {
                    std::cerr << "*** Could not close container: " << status.description() << std::endl;
                    exitStatus = 1;
                }
            }
        } else {
            std::cerr << "*** You must specify a container file." << std::endl;
            exitStatus = 1;
        }
    }

    return exitStatus;
}
