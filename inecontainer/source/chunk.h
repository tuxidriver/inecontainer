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
* This header defines the \ref Chunk class.
***********************************************************************************************************************/

/* .. sphinx-project inecontainer */

#ifndef CHUNK_H
#define CHUNK_H

#include <cstdint>
#include <memory>

#include "container_status.h"
#include "chunk_header.h"

class ContainerImpl;

/**
 * Class that extends \ref ChunkHeader to include virtual load and store method.
 */
class Chunk:public ChunkHeader {
    public:
        /**
         * Constructor.
         *
         * \param[in] container                      The container used to access the requested data.
         *
         * \param[in] fileIndex                      The file index where the chunk starts.
         *
         * \param[in] additionalChunkHeaderSizeBytes The number of additional bytes in the chunk header.
         */
        Chunk(std::weak_ptr<ContainerImpl> container, FileIndex fileIndex, unsigned additionalChunkHeaderSizeBytes = 0);

        /**
         * Constructor.
         *
         * \param[in] container            The container used to access the requested data.
         *
         * \param[in] fileIndex            The file index where the chunk starts.
         *
         * \param[in] commonHeader         Array holding header data common to all chunk types.
         *
         * \param[in] additionalHeaderBytes The number of additional bytes in the chunk header.
         */
        Chunk(
            std::weak_ptr<ContainerImpl> container,
            FileIndex                    fileIndex,
            std::uint8_t                 commonHeader[Chunk::minimumChunkHeaderSizeBytes],
            unsigned                     additionalHeaderBytes
        );

        /**
         * Constructor.
         *
         * \param[in] container    The container used to access the requested data.
         *
         * \param[in] fileIndex    The file index where the chunk starts.
         *
         * \param[in] commonHeader Array holding header data common to all chunk types.
         */
        Chunk(
            std::weak_ptr<ContainerImpl> container,
            FileIndex                    fileIndex,
            std::uint8_t                 commonHeader[Chunk::minimumChunkHeaderSizeBytes]
        );

        virtual ~Chunk();

        /**
         * Method that sets the file index for this chunk.
         *
         * \param[in] newFileIndex The file index where the chunk begins.
         */
        void setFileIndex(FileIndex newFileIndex);

        /**
         * Method that determines the file index where this chunk resides.
         *
         * \return Returns the file index of the chunk.
         */
        FileIndex fileIndex() const;

        /**
         * Method that returns the container where this chunk resides.
         *
         * \return Returns a pointer to the container associated with this chunk.
         */
        std::weak_ptr<ContainerImpl> container() const;

        /**
         * Method that loads a chunk into memory from the container.  The default implementation loads the additional
         * chunk header into memory.
         *
         * To improve speed, the CRC is not checked on the loaded chunk after being loaded.  You can do this as an
         * additional operation, if desired.
         *
         * \param[in] includeCommonHeader If true, the portion of the header common to all chunk types will be loaded.
         *                                If false, only the additional header and payload will be loaded.
         *
         * \return Returns the status from the load operation.
         */
        virtual Container::Status load(bool includeCommonHeader = false);

        /**
         * Method that writes the chunk to the container.  The default implementation assumes all valid data is
         * contained in the header.  Note that a save operation might adjust the chunk size downward so be sure to
         * verify the chunk size after each save operation.
         *
         * The method will automatically update the CRC prior to writing the chunk to the container.
         *
         * \param[in] padToChunkSize If true, additional bytes will be written at the end of the chunk, if needed to
         *                           pad the chunk to the correct total byte size.
         *
         * \return Returns the status from the save operation.
         */
        virtual Container::Status save(bool padToChunkSize = true);

        /**
         * Method that checks if the CRC is valid.  This version assumes that the entire chunk contents are contained
         * within the additional header data.
         */
        virtual bool checkCrc() const;

    protected:
        /**
         * Method that is called by the \ref Chunk::save method to calculate the CRC.  The default implementation sets
         * the CRC to the initialized CRC.
         */
        virtual void updateCrc();

        /**
         * Method that can be called by the \ref Chunk::save method and overloaded versions of that method to write
         * random data at the end of the chunk to fill the chunk out to full size.
         *
         * \param[in] additionalBytes The number of additional bytes to write.  A value of 0, the default, will cause
         *                            this method to calculate the number of bytes that must be written.
         *
         * \return Returns the status from the write operation.
         */
        Container::Status writeTail(unsigned additionalBytes = 0);

    private:
        /**
         * Knuth-Lewis PRNG multiplier
         */
        static constexpr std::uint32_t kla = 1664525;

        /**
         * Knuth-Lewis PRNG offset.
         */
        static constexpr std::uint32_t klc = 1013904223;

        /**
         * Seed used to generate random data for chunk tails.
         */
        static std::uint32_t randomSeed;

        /**
         * The file idnex where the chunk begins.
         */
        FileIndex currentFileIndex;

        /**
         * Container used for file access.
         */
        std::weak_ptr<ContainerImpl> currentContainer;

        /**
         * The number of bytes of the chunk that are currently loaded.
         */
        unsigned numberLoadedBytes;
};

#endif
