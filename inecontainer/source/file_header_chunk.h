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
* This header defines the \ref FileHeaderChunk class.
***********************************************************************************************************************/

/* .. sphinx-project inecontainer */

#ifndef FILE_HEADER_CHUNK_H
#define FILE_HEADER_CHUNK_H

#include <cstdint>
#include <string>

#include "container_impl.h"
#include "chunk.h"

/**
 * Class that provides support for file header chunks.
 */
class FileHeaderChunk:public Chunk {
    public:
        /**
         * Constructor.
         *
         * \param[in] container  The container used to access the requested data.
         *
         * \param[in] fileIndex  The file index where the chunk starts.
         *
         * \param[in] identifier A string used to identify the file type.
         */
        FileHeaderChunk(std::weak_ptr<ContainerImpl> container, FileIndex fileIndex, const std::string& identifier);

        /**
         * Constructor.
         *
         * \param[in] container            The container used to access the requested data.
         *
         * \param[in] fileIndex            The file index where the chunk starts.
         *
         * \param[in] commonHeader         Array holding header data common to all chunk types.
         */
        FileHeaderChunk(
            std::weak_ptr<ContainerImpl> container,
            FileIndex                    fileIndex,
            std::uint8_t                 commonHeader[Chunk::minimumChunkHeaderSizeBytes]
        );

        ~FileHeaderChunk() override;

        /**
         * Method that returns the format major version.
         *
         * \return Returns the container format major version number.
         */
        std::uint8_t majorVersion() const;

        /**
         * Method that returns the format major version.
         *
         * \return Returns the container format major version number.
         */
        std::uint8_t minorVersion() const;

        /**
         * Method that returns the identifier string tied to this file header.
         *
         * \return Returns the identifier string tied to this header.
         */
        std::string identifier() const;

        /**
         * Method that checks if this file header is valid.  Note that this method does not check the CRC.
         *
         * \param[in] expectedIdentifier The identifier we are expecting to see.
         *
         * \return Returns true if the file header is valid.  Returns false if the file header is invalid.
         */
        bool isValid(const std::string& expectedIdentifier) const;
};

#endif
