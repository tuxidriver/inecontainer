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
* This header defines the \ref Container::MemoryContainer class.
***********************************************************************************************************************/

/* .. sphinx-project inecontainer */

#ifndef CONTAINER_MEMORY_CONTAINER_H
#define CONTAINER_MEMORY_CONTAINER_H

#include <cstdint>
#include <vector>
#include <string>
#include <memory>

#include "container_status.h"
#include "container_container.h"

namespace Container {
    class VirtualFile;

    /**
     * Container for virtual files stored in a large memory buffer.
     */
    class MemoryContainer:public Container {
        public:
            /**
             * Typedef used for memory buffers.
             */
            typedef std::vector<std::uint8_t> MemoryBuffer;
            /**
             * Constructor.
             *
             * \param[in] fileIdentifier   A string placed at a fixed location near the beginning of the file.  The
             *                             string can be used as a magic number to identifier the file type and is used
             *                             as a check when opening a new container.
             *
             * \param[in] ignoreIdentifier If true, the file identifier will be ignored when a container is opened.
             */
            MemoryContainer(const std::string& fileIdentifier, bool ignoreIdentifier = false);

            ~MemoryContainer() override;

            /**
             * Method that should be called to open the container.  If the container is empty, the method will attempt
             * to create a file header.  If the container is not empty, the method will verify that the file container
             * is valid.
             *
             * You must call this method before performing any operations on the container.  You must also be sure that
             * there are no \ref Container::VirtualFile instances instantiated for this container when this method is
             * called.
             *
             * \param[in] buffer         A shared pointer to the memory buffer used to store the container.
             *
             * \return Returns the status from the open attempt.
             */
            Status open(std::shared_ptr<MemoryBuffer> buffer = std::make_shared<MemoryBuffer>());

            /**
             * Method that should be called after all file operations are complete.  Forces all underlying virtual files
             * to be flushed and closed and forces any data contained within the container to be flushed.
             *
             * \return Returns true on success, returns false on error.
             */
            Status close();

            /**
             * Method you can use to obtain access to the raw buffer being used by this class.
             *
             * \return Returns a shared pointer to the underlying buffer.
             */
            std::shared_ptr<MemoryBuffer> buffer();

        protected:
            /**
             * Method that is called to determine the current size of the underlying data store, in bytes.
             *
             * \return Returns the size of the underlying data store, in bytes.  A negative value should be returned if
             *         an error occurs.
             */
            long long size() final;

            /**
             * Method that is called to seek to a position in the underlying data store prior to performing a call to
             * \ref Container::Container::read, \ref Container::Container::write, or
             * \ref Container::Container::truncate.
             *
             * \param[in] newOffset The new position in the underlying data store to seek to.
             *
             * \return Returns the status from this operation.
             */
            Status setPosition(unsigned long long newOffset) final;

            /**
             * Method that is called to seek to the last just past the last byte in the underlying data store.
             *
             * \return Returns the status from this operation.
             */
            Status setPositionLast() final;

            /**
             * Method that is called to determine the current byte offset from the beginning of the container.
             *
             * \return Returns the current byte offset from the beginning of the container.
             */
            virtual unsigned long long position() const final;

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
            Status read(std::uint8_t* buffer, unsigned desiredCount) final;

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
            Status write(const std::uint8_t* buffer, unsigned count) final;

            /**
             * Method you can overload to indicate whether the derived class supports file truncation.
             *
             * \return Returns true if file truncation is supported.  Returns false if file truncation is not supported.
             */
            virtual bool supportsTruncation() const final;

            /**
             * Method that is called to truncate the container at the current file position.
             *
             * \return Returns the status from the truncate operation.
             */
            Status truncate() final;

            /**
             * Method that is called to force any written data to be flushed to the media.
             *
             * \return Returns the status from the flush operation.
             */
            Status flush() final;

        private:
            /**
             * Implementation class.
             */
            class Private;

            /**
             * Pimpl.
             */
            std::unique_ptr<MemoryContainer::Private> impl;
    };
}

#endif
