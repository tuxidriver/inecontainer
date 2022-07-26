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
* This header defines the \ref Container::VirtualFile class.
***********************************************************************************************************************/

/* .. sphinx-project inecontainer */

#ifndef CONTAINER_VIRTUAL_FILE_H
#define CONTAINER_VIRTUAL_FILE_H

#include <cstdint>
#include <memory>

#include "container_status.h"

namespace Container {
    class Container;

    /**
     * Class that provides an API to a single virtual file in the container.  The class provides several modes of
     * operation:
     *
     *     * A random access mode that models traditional file access.  For this mode, use the methods:
     *         - \ref Container::VirtualFile::open
     *         - \ref Container::VirtualFile::close
     *         - \ref Container::VirtualFile::size
     *         - \ref Container::VirtualFile::setPosition
     *         - \ref Container::VirtualFile::setPositionLast
     *         - \ref Container::VirtualFile::position
     *         - \ref Container::VirtualFile::read
     *         - \ref Container::VirtualFile::write
     *         - \ref Container::VirtualFile::truncate
     *         - \ref Container::VirtualFile::flush
     *
     *     * You can also use this class to provide a streaming API to write data to a newly created container.  For
     *       the write streaming API, you should use the methods:
     *
     *         - \ref Container::Container::newVirtualFile
     *         - \ref Container::VirtualFile::write
     *
     *     * You can use this class to provide a read streaming API to read data from the container.  In this mode, the
     *       \ref Container::Container class must control the order that data is presented to the application.  You must
     *       overload the \ref Container::VirtualFile class, overloading the method
     *       \ref Container::VirtualFile::receivedData.  The \ref Container::VirtualFile::receivedData method will be
     *       called as data for this file is discovered.  Note that data will be presented in order.  You must also
     *       overload the factory method \ref Container::Container::createFile method to create instances of the
     *       proper derived class for each file discovered in the archive.  You can optionally overload the
     *       \ref Container::VirtualFile::endOfFile method to receive notification of the end of file.
     *
     *     * You can also perform virtual file level access using the methods:
     *         - \ref Container::VirtualFile::rename
     *         - \ref Container::VirtualFile::erase
     */
    class VirtualFile {
        friend class Container;

        protected:
            /**
             * Protected constructor used by the \ref Container::Container class to instantiate virtual file instances.
             *
             * \param[in] newName   The name tied to this virtual file.
             *
             * \param[in] container The private \ref Container::Container class implementation.
             */
            VirtualFile(const std::string& newName, Container* container);

        public:
            /**
             * Copy constructor.  Note that copies of this virtual file will operate on the same underlying file and
             * will remain in sync with each other.
             *
             * \param[in] other The instance to be copied.
             */
            VirtualFile(const VirtualFile& other);

            virtual ~VirtualFile();

            /**
             * Method that returns the name of this virtual file.
             *
             * \return Returns the virtual file name.
             */
            std::string name() const;

            /**
             * Method that determines the current size of the virtual file.  Note that this operation must scan the
             * container and is, therefore, rather slow.
             *
             * \return Returns the size of the virtual file.  A negative value is returned on error.
             */
            long long size();

            /**
             * Method that seeks to a relative offset in the virtual file.  Note that this operation must scan the
             * container and is, therefore, rather slow.
             *
             * \param[in] newOffset The new offset into the virtual file.
             *
             * \return Returns the status from the seek operation.
             */
            Status setPosition(unsigned long long newOffset);

            /**
             * Method that seeks to the end of the virtual file.  Note that this operation must scan the container to
             * locate the last entry and is therefore rather slow.
             *
             * \return Returns the status from the seek operation.
             */
            Status setPositionLast();

            /**
             * Method that determines the current virtual file pointer into this file.
             *
             * \return Returns the current location.  A negative value is returned on error.
             */
            long long position() const;

            /**
             * Method that returns the number of bytes of data currently cached for this virtual file.
             *
             * \return Returns the number of cached bytes.
             */
            unsigned long long bytesInWriteCache() const;

            /**
             * Method that reads a specified number of bytes of data from the container, if available.
             *
             * \param[in] buffer       The buffer to receive the data.  The buffer must be large enough to hold all the
             *                         requested information.
             *
             * \param[in] desiredCount The number of bytes that should be read, if possible.
             *
             * \return Returns the status from the read operation.  On success an instance of
             *         \ref Container::ReadSuccessful is returned.
             */
            Status read(std::uint8_t* buffer, unsigned desiredCount);

            /**
             * Method that writes a specified number of bytes of data.  You can use this method either during
             * random access or when streaming data to a virtual file in a container.
             *
             * \param[in] buffer The buffer holding the data to write.
             *
             * \param[in] count  The number of bytes to be written.
             *
             * \return Returns the status from the write operation.  On success an instance of
             *         \ref Container::WriteSuccessful is returned.
             */
            Status write(const std::uint8_t* buffer, unsigned count);

            /**
             * Method that appends a specified number of bytes of data to the end of the virtual file.  Calling this
             * method is the same as calling \ref Container::VirtualFile::setPositionLast followed by
             * \ref Container::VirtualFile::write.
             *
             * \param[in] buffer The buffer holding the data to write.
             *
             * \param[in] count  The number of bytes to be written.
             *
             * \return Returns the status from the write operation.  On success an instance of
             *         \ref Container::WriteSuccessful is returned.
             */
            Status append(const std::uint8_t* buffer, unsigned count);

            /**
             * Method that truncates the file at the current position.  All data after the current position will be
             * discarded.
             *
             * \return Returns the status from the truncation operation.
             */
            Status truncate();

            /**
             * Method that flushes any pending write data to the media.
             *
             * \return Returns the status from the flush operation.
             */
            Status flush();

            /**
             * Method that deletes this file.  This virtual file object will no longer be valid after calling this
             * method.
             *
             * \return Returns the status from the erase operation.
             */
            Status erase();

            /**
             * Method that renames this file.
             *
             * \param[in] newName The new name to assign to this virtual file.
             *
             * \return Returns the status from the rename operation.
             */
            Status rename(const std::string& newName);

            /**
             * Method that can be used to make a shallow copy of this virtual file.  Like the copy constructor, copies
             * of this virtual file will operate on the same underlying file and will remain in sync with each other.
             *
             * \param[in] other The instance to be copied.
             *
             * \return Returns a reference to this object.
             */
            VirtualFile& operator=(const VirtualFile& other);

        protected:
            /**
             * Method you can overload to receive data from the streaming API.  Note that, to avoid multiple instances
             * incorrectly operating on the same data, only the instance that was instantiated by
             * \ref Container::Container::createFile will receive calls to this method.
             *
             * \param[in] buffer        A pointer to the buffer holding the read data.
             *
             * \param[in] bytesReceived A count of the number of bytes received in the buffer.
             *
             * \return Returns the status from the operation.  You should return \ref Container::NoStatus on success or
             *         an instance of a class derived from \ref Container::Status on error.  Note that you can extend
             *         the \ref Container::Status class to create your own error types, if desired.
             */
            virtual Status receivedData(const std::uint8_t* buffer, unsigned bytesReceived);

            /**
             * Method that is called when the end of file is reached.
             *
             * \return Returns the status from the operation.  The default implementation returns
             *         \ref Container::NoStatus.
             */
            virtual Status endOfFile();

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
            std::shared_ptr<VirtualFile::Private> impl;
    };
}

#endif
