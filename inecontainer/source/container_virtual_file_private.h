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
* This header defines the \ref Container::VirtualFile::Private class.
***********************************************************************************************************************/

/* .. sphinx-project inecontainer */

#ifndef CONTAINER_VIRTUAL_FILE_PRIVATE_H
#define CONTAINER_VIRTUAL_FILE_PRIVATE_H

#include <cstdint>
#include <memory>

#include "container_status.h"
#include "virtual_file_impl.h"
#include "container_virtual_file.h"

namespace Container {
    /**
     * Private implementatin of the \ref Container::VirtualFile class.
     */
    class VirtualFile::Private:public VirtualFileImpl {
        public:
            /**
             * Constructor
             *
             * \param[in] newName          The name tied to this virtual file.
             *
             * \param[in] streamIdentifier The identifier used to identifiy this virtual file stream.
             *
             * \param[in] container        Pointer to the container class.
             *
             * \param[in] interface        Pointer to the \ref VirtualFile interface class.
             */
            Private(
                const std::string&            newName,
                StreamChunk::StreamIdentifier streamIdentifier,
                Container*                    container,
                VirtualFile*                  interface
            );

            ~Private() override;

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
            Status receivedData(const std::uint8_t* buffer, unsigned bytesReceived) final;

            /**
             * Method that is called when the end of file is reached.
             *
             * \return Returns the status from the operation.  The default implementation returns
             *         \ref Container::NoStatus.
             */
            Status endOfFile() final;

        private:
            /**
             * Pointer to the implementation class.
             */
            VirtualFile* iface;
    };
}

#endif
