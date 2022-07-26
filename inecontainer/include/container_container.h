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
* This header defines the \ref Container::Container class.
***********************************************************************************************************************/

/* .. sphinx-project inecontainer */

#ifndef CONTAINER_CONTAINER_H
#define CONTAINER_CONTAINER_H

#include <cstdint>
#include <map>
#include <memory>
#include <string>

#include "container_status_base.h"

namespace Container {
    class VirtualFile;

    /**
     * Pure virtual class that manages a container of virtual files.
     */
    class Container {
        friend class VirtualFile;

        public:
            /**
             * Type used for maps of virtual files by name.
             */
            typedef std::map<std::string, std::shared_ptr<VirtualFile>> DirectoryMap;

            /**
             * Class used to construct pairs for the Map class.
             */
            typedef std::pair<std::string, std::shared_ptr<VirtualFile>> DirectoryMapPair;

            /**
             * The container major version code.
             */
            static constexpr std::uint8_t containerMajorVersion = 1;

            /**
             * The latest container minor version code.
             */
            static constexpr std::uint8_t containerMinorVersion = 0;

            /**
             * Constructor
             *
             * \param[in] fileIdentifier   A string placed at a fixed location near the beginning of the file.  The
             *                             string can be used as a magic number to identifier the file type and is used
             *                             as a check when opening a new container.
             *
             * \param[in] ignoreIdentifier If true, the file identifier will be ignored when a container is opened.
             */
            Container(const std::string& fileIdentifier, bool ignoreIdentifier = false);

            virtual ~Container();

            /**
             * Method you can use to obtain the last reported container-wide status condition.  Includes both operations
             * within the container as well as operations performed by virtual files tied to this container.
             *
             * \return Returns the last reported status.
             */
            Status lastStatus() const;

            /**
             * Method you can use to obtain the minor version code for the open container.
             *
             * \return Returns the container's minor version code.
             */
            std::uint8_t minorVersion() const;

            /**
             * Returns a directory of all the streams in the container.
             *
             * \return Returns a map, keyed by the stream name, of streams in the container.
             */
            DirectoryMap directory();

            /**
             * Method you can call to create a new virtual file in the container.  The newly created file will be
             * added to the directory.
             *
             * \param[in] newVirtualFileName The new name to assign to this file.
             *
             * \return Returns the newly created virtual file.  A null pointer is returned on error.
             */
            std::shared_ptr<VirtualFile> newVirtualFile(const std::string& newVirtualFileName);

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
            Status streamRead();

        protected:
            /**
             * Method that should be called to open the container.  If the container is empty, the method will attempt
             * to create a file header.  If the container is not empty, the method will verify that the file container
             * is valid.
             *
             * You must call this method before performing any operations on the container.  You must also be sure that
             * there are no \ref Container::VirtualFile instances instantiated for this container when this method is
             * called.
             *
             * You will generally want to create a public open method in a derived class.
             *
             * Derived classes can overload this method but must always call the base class implementation.
             *
             * \return Returns the status from the open attempt.
             */
            Status open();

            /**
             * Method that should be called after all file operations are complete.  Forces all underlying virtual files
             * to be flushed and closed and forces any data contained within the container to be flushed.
             *
             * You will generally want to create a public close method in a derived class.
             *
             * If you overload this method, be sure to also call the base class implementation.
             *
             * \return Returns true on success, returns false on error.
             */
            Status close();

            /**
             * Factory method that is called by the streaming API to create new virtual file instances.  You should
             * overload this method if you wish to use the stremaing API to instantiate classes derived from
             * \ref Container::VirtualFile.
             *
             * Upon creation, the newly created stream will be added to the directory.
             *
             * The default implementation returns an instance of \ref Container::VirtualFile.
             *
             * \param[in] virtualFileName The name of the virtual file that should be created.
             *
             * \return Returns a newly created virtual file with the assigned name.
             */
            virtual VirtualFile* createFile(const std::string& virtualFileName);

        protected:
            /**
             * Method that is called to determine the current size of the underlying data store, in bytes.
             *
             * \return Returns the size of the underlying data store, in bytes.  A negative value should be returned if
             *         an error occurs.
             */
            virtual long long size() = 0;

            /**
             * Method that is called to seek to a position in the underlying data store prior to performing a call to
             * \ref Container::Container::read, \ref Container::Container::write, or
             * \ref Container::Container::truncate.
             *
             * \param[in] newOffset The new position in the underlying data store to seek to.
             *
             * \return Returns the status from the set position operation.
             */
            virtual Status setPosition(unsigned long long newOffset) = 0;

            /**
             * Method that is called to seek to the last just past the last byte in the underlying data store.
             *
             * \return Returns the status from the set position operation.
             */
            virtual Status setPositionLast() = 0;

            /**
             * Method that is called to determine the current byte offset from the beginning of the container.
             *
             * \return Returns the current byte offset from the beginning of the container.
             */
            virtual unsigned long long position() const = 0;

            /**
             * Method that is called to read a specified number of bytes of data from the underlying data store.
             *
             * \param[in] buffer       The buffer to receive the data.  The buffer is guaranteed to be large enough to
             *                         hold all the requested data.
             *
             * \param[in] desiredCount The number of bytes that are expected to be read.
             *
             * \return Returns the status from the read operation.  An instance of \ref Container::ReadSuccessful should
             *         be returned on success.
             */
            virtual Status read(std::uint8_t* buffer, unsigned desiredCount) = 0;

            /**
             * Method that is called to write a specified number of bytes of data to the underlying data store.
             *
             * \param[in] buffer The buffer holding the data to be written.
             *
             * \param[in] count  The number of bytes to be written.
             *
             * \return Returns the status from the write operation.  An instance of \ref Container::WriteSuccessful
             *         should be returned on success.
             */
            virtual Status write(const std::uint8_t* buffer, unsigned count) = 0;

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
            virtual Status truncate() = 0;

            /**
             * Method that is called to force any written data to be flushed to the media.
             *
             * \return Returns the status from the flush operation.
             */
            virtual Status flush() = 0;

        private:
            /**
             * Implementation class.
             */
            class Private;

        #if (defined(LIBCONTAINER_TEST))

            public:

        #endif

            /**
             * Pimpl.
             */
            std::shared_ptr<Container::Private> impl;
    };
}

#endif
