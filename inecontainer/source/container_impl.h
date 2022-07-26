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
* This header defines the \ref ContainerImpl class.
***********************************************************************************************************************/

/* .. sphinx-project inecontainer */

#ifndef CONTAINER_IMPL_H
#define CONTAINER_IMPL_H

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <utility>

#include "container_status.h"
#include "free_space_tracker.h"
#include "chunk_header.h"
#include "chunk.h"
#include "stream_chunk.h"
#include "container_container.h"

class VirtualFileImpl;
class VirtualFile;

/**
 * Pure virtual container implementation class.  You should derive from this class to create a pimpl for the public
 * container class.
 *
 * Any PIMPL defined as part of the API must be private to the API which then makes the container implementation
 * inaccessible to other classes.  We work around this by making the PIMPL class derived from another class with virtual
 * functions.  This approach also has the side-effect of more fully segregating the public API from the implementation.
 */
class ContainerImpl:public FreeSpaceTracker {
    public:
        /**
         * Type used for maps of virtual files by name.
         */
        typedef std::map<std::string, std::shared_ptr<VirtualFileImpl>> DirectoryMap;

        /**
         * Class used to construct pairs for the Map class.
         */
        typedef std::pair<std::string, std::shared_ptr<VirtualFileImpl>> DirectoryMapPair;

        /**
         * Constructor
         *
         * \param[in] fileIdentifier   A string placed at a fixed location near the beginning of the file.  The string
         *                             can be used as a magic number to identifier the file type and is used as a check
         *                             when opening a new container.
         *
         * \param[in] ignoreIdentifier If true, the file identifier will be ignored when a container is opened.
         */
        ContainerImpl(const std::string& fileIdentifier, bool ignoreIdentifier = false);

        ~ContainerImpl();

        /**
         * Provides this class with a weak pointer to itself that it can then issue to other classes it creates.  Allows
         * the API to maintain a shared pointer to this instance.
         *
         * \param[in] newWeakPointer The weak pointer reference to this.
         */
        void setWeakThis(std::weak_ptr<ContainerImpl> newWeakPointer);

        /**
         * Method that can be called to update the container status.
         *
         * \param[in] newStatus The new status to be reported.
         */
        void setLastStatus(const Container::Status& newStatus);

        /**
         * Method you can use to obtain the last reported container-wide status condition.  Includes both operations
         * within the container as well as operations performed by virtual files tied to this container.
         *
         * \return Returns the last reported status.
         */
        Container::Status lastStatus() const;

        /**
         * Method you can use to obtain the minor version code for this container.
         *
         * \return Returns the container's minor version code.
         */
        std::uint8_t minorVersion() const;

        /**
         * Method that should be called to open the container.  If the container is empty, the method will attempt
         * to create a file header.  If the container is not empty, the method will verify that the file container
         * is valid.
         *
         * You must call this method before performing any operations on the container.
         *
         * \return Returns the status from the open attempt.
         */
        Container::Status open();

        /**
         * Method that should be called after all file operations are complete.  Forces all underlying virtual files
         * to be flushed and closed and forces any data contained within the container to be flushed.
         *
         * \return Returns true on success, returns false on error.
         */
        Container::Status close();

        /**
         * Returns a directory of all the streams in the container.
         *
         * \return Returns a map, keyed by the stream name, of streams in the container.
         */
        Container::Container::DirectoryMap directory();

        /**
         * Method you can call to perform a sequential read across the container.
         *
         * Upon calling this method, calls will be performed to the \ref Container::Container::createFile method for
         * each virtual file defined in the container.  You can overload this method to create classes derived from
         * \ref Container::VirtualFile with key methods overloaded.
         *
         * Each \ref Container::VirtualFile will then receive calls to \ref Container::VirtualFile::receivedData
         * for portions of the virtual file.  The method will be called in file data order.
         *
         * After the last byte of data in the file is read, the \ref Container::VirtualFile::endOfFile method is
         * called.
         *
         * \return Returns the status from the operation.
         */
        Container::Status streamRead();

        /**
         * Method that obtains a new and unused stream ID.  Note that the method may require that the container be
         * scanned to locate all of the existing streams.
         *
         * \param[out] ok An optional pointer to a boolean value that will hold true on success, false on failure.
         *
         * \return Returns the selected stream ID.  A value of 0 is returned on error.
         */
        StreamChunk::StreamIdentifier newStreamIdentifier(bool* ok = nullptr);

        /**
         * Method you can call to create a new virtual file in the container.  The newly created file will be
         * added to the directory.
         *
         * \param[in] newVirtualFileName The new name to assign to this file.
         *
         * \return Returns the newly created virtual file.  A null pointer is returned on error.
         */
        std::shared_ptr<Container::VirtualFile> newVirtualFile(const std::string& newVirtualFileName);

        /**
         * Method you can call to create a new virtual file in the container using the API's implementation of
         * \ref Container::Container::newVirtualFile.  The newly created file will be added to the directory.
         *
         * \param[in] newVirtualFileName The new name to assign to this file.
         *
         * \return Returns the newly created virtual file.  A null pointer is returned on error.
         */
        virtual std::shared_ptr<Container::VirtualFile> callNewVirtualFile(const std::string& newVirtualFileName) = 0;

        /**
         * Method that indicates if the container needs to be scanned.
         *
         * \return Returns true if the container needs to be scanned, returns false if all the maps are believed to be
         *         fully populated.
         */
        bool containerScanNeeded() const;

        /**
         * Method that scans the container to build up the directory, chunk map, and free list.
         *
         * \return Returns the status from the operation.
         */
        Container::Status scanContainer();

        /**
         * Method that adds an \ref VirtualFileImpl instance to the filesByName dictionary.
         *
         * \param[in] virtualFile The virtual file to be added to the internal dictionary.
         */
        void registerFileImplementation(std::shared_ptr<VirtualFileImpl> virtualFile);

        /**
         * Method that is called by a \ref VirtualFileImpl to inform the container that the file has been renamed.
         *
         * \param[in] oldName The previous name of this virtual file.
         *
         * \param[in] newName The new name of the virtual file.
         *
         * \return Returns true on success, returns false on error.
         */
        bool fileRenamed(const std::string& oldName, const std::string& newName);

        /**
         * Method that is called by a \ref VirtualFileImpl to inform the container that the file has been erased.
         *
         * \param[in] name The name of the file.
         *
         * \return Returns true on success, returns false on error.
         */
        bool fileErased(const std::string& name);

        /**
         * Method that performs a call to the overloaded \ref Container::Container::createFile method defined by the
         * public API.
         *
         * \param[in] virtualFileName The name of the virtual file to be created.
         *
         * \return Returns a pointer to the newly created virtual file.  The virtual file will be managed by a shared
         *         pointer instantiated by the container.
         */
        virtual Container::VirtualFile* createFile(const std::string& virtualFileName) = 0;

        /**
         * Method that is called to determine the current size of the underlying data store, in bytes.
         *
         * \return Returns the size of the underlying data store, in bytes.  A negative value should be returned if
         *         an error occurs.
         */
        virtual long long size() = 0;

        /**
         * Method that calls the overloaded \ref Container::Container::setPosition method defined by the public API.
         *
         * \param[in] newOffset The new byte offset into the container to set the file pointer to.  Subsequent calls to
         *                      \ref Container::Container::read or \ref Container::Container::write will perform their
         *                      read or write operation from this offset and will automatically advance the pointer.
         *
         * \return Returns the status from the operation.
         */
        virtual Container::Status setPosition(unsigned long long newOffset) = 0;

        /**
         * Method that calls the overloaded \ref Container::Container::setPositionLast method defined by the public API.
         *
         * \return Returns the status from the operation.
         */
        virtual Container::Status setPositionLast() = 0;

        /**
         * Method that is called to determine the current byte offset from the beginning of the container.
         *
         * \return Returns the current byte offset from the beginning of the container.
         */
        virtual unsigned long long position() const = 0;

        /**
         * Method that calls the overloaded \ref Container::Container::read method defined by the public API.
         *
         * \param[in] buffer       The buffer to hold the read information.
         *
         * \param[in] desiredCount The number of bytes that the method should attempt to read into the buffer.
         *
         * \return Returns the status from the operation.
         */
        virtual Container::Status read(std::uint8_t* buffer, unsigned desiredCount) = 0;

        /**
         * Method that calls the overloaded \ref Container::Container::write method defined by the public API.
         *
         * \param[in] buffer The buffer holding the information to be written.
         *
         * \param[in] count  The number of bytes that the method should attempt to write into the container.
         *
         * \return Returns the actual number of bytes written.  A negative value is returned if an error is detected.
         */
        virtual Container::Status write(const std::uint8_t* buffer, unsigned count) = 0;

        /**
         * Method you can overload to indicate whether the derived class supports file truncation.
         *
         * \return Returns true if file truncation is supported.  Returns false if file truncation is not supported.
         */
        virtual bool supportsTruncation() const = 0;

        /**
         * Method that is called to truncate the container at the current file position.
         *
         * \return Returns the status from the truncate operation.
         */
        virtual Container::Status truncate() = 0;

        /**
         * Method that calls the overloaded \ref Container::Container::flush method defined by the public API.
         *
         * \return Returns true on success, returns false on error.
         */
        virtual Container::Status flush() = 0;

    protected:
        /**
         * Method that is called to trigger an area of the container to be written as fill area.
         *
         * Detailed failure status will be handled through lastReportedStatus.
         *
         * \param[in] area The area or region to be flushed.
         *
         * \return Returns true on success, returns false on error.
         */
        bool flushArea(const ContainerArea& area) final;

    private:
        /**
         * Flag that indicates that the identifier in the file header should be ignored when the container is opened.
         */
        bool ignoreIdentifierOnOpen;

        /**
         * The last reported status for this container.
         */
        Container::Status lastReportedStatus;

        /**
         * Type used for maps of virtual files by name.
         */
        typedef std::map<StreamChunk::StreamIdentifier, std::shared_ptr<VirtualFileImpl>> IdentifierMap;

        /**
         * Class used to construct pairs for the Map class.
         */
        typedef std::pair<StreamChunk::StreamIdentifier, std::shared_ptr<VirtualFileImpl>> IdentifierMapPair;

        /**
         * Method that is called to build file maps, if needed.
         *
         * \param[in] buildMapsOnly If true, this method will just build up the various maps.  If false, the method will
         *                          report read data to each created virtual file.
         *
         * \return Returns the status from the operation.
         */
        Container::Status traverseContainer(bool buildMapsOnly);

        /**
         * Weak pointer reference to this.
         */
        std::weak_ptr<ContainerImpl> weakThis;

        /**
         * The file identifier passed to the constructor.  Validated on a call to \ref ContainerImpl::open.
         */
        std::string currentFileIdentifier;

        /**
         * Holds the container minor version code.
         */
        std::uint8_t currentMinorVersion;

        /**
         * The file index where actual data begins.
         */
        ChunkHeader::FileIndex startingFileIndex;

        /**
         * Map of virtual files by stream ID.
         */
        IdentifierMap filesByIdentifier;

        /**
         * Map of known files by name.
         */
        Container::Container::DirectoryMap fileApisByName;

        /**
         * Map of virtual file implementations by name.
         */
        DirectoryMap filesByName;

        /**
         * Flag that indicates if the file maps are fully populated.
         */
        bool fileMapsPopulated;
};

#endif
