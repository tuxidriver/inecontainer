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
* This header defines the \ref ChunkHeader class.
***********************************************************************************************************************/

/* .. sphinx-project inecontainer */

#ifndef CHUNK_HEADER_H
#define CHUNK_HEADER_H

#include <cstdint>

/**
 * Base class for all types of chunks of data.
 */
class ChunkHeader {
    public:
        /**
         * Type used to concisely indicate an offset into the file.
         */
        typedef std::uint32_t FileIndex;

        /**
         * Value used to hold power-of-2 chunk size values.
         */
        typedef std::uint8_t  ChunkP2;

        /**
         * Type used to store and manage CRC data.
         */
        typedef std::uint16_t RunningCrc;

        /**
         * Enumeration of chunk types.
         */
        enum class Type:std::uint8_t {
            /**
             * Chunk that is used to hold the file header.
             */
            FILE_HEADER_CHUNK = 0,

            /**
             * Stream start chunk. Ties a stream to a stream identifier.
             */
            STREAM_START_CHUNK = 1,

            /**
             * Stream data chunk.  Holds data tied to a stream.
             */
            STREAM_DATA_CHUNK = 2,

            /**
             * Fill/unused chunk.
             */
            FILL_CHUNK = 3
        };

        /**
         * Value indicating the minimum size of the chunk header, in bytes.
         */
        static constexpr unsigned minimumChunkHeaderSizeBytes = 4;

        /**
         * Value indicating the ma]inimum size of a chunk, in bytes.  Value includes the header.
         */
        static constexpr unsigned minimumChunkSize = 1 << 5;

        /**
         * Value indicating the maximum size of a chunk, in bytes.  Value includes the header.
         */
        static constexpr unsigned maximumChunkSize = 1 << (7 + 5);

        /**
         * Value used to indicate an invalid file index.
         */
        static constexpr FileIndex invalidFileIndex = static_cast<FileIndex>(-1);

        /**
         * Constructor.
         *
         * \param[in] additionalChunkHeaderSizeBytes The number of additional bytes in the chunk header.
         */
        ChunkHeader(unsigned additionalChunkHeaderSizeBytes = 0);

        /**
         * Constructor.
         *
         * \param[in] commonHeader          Array holding header data common to all chunk types.
         *
         * \param[in] additionalHeaderBytes The number of additional bytes in the chunk header.
         */
        ChunkHeader(std::uint8_t commonHeader[minimumChunkHeaderSizeBytes], unsigned additionalHeaderBytes);

        /**
         * Constructor.
         *
         * \param[in] commonHeader Array holding header data common to all chunk types.  Additional header space
         *                         will be calculated from the data provided in the common header.
         */
        ChunkHeader(std::uint8_t commonHeader[minimumChunkHeaderSizeBytes]);

        /**
         * Copy constructor.
         *
         * \param[in] other The instance to be copied.
         */
        ChunkHeader(const ChunkHeader& other);

        ~ChunkHeader();

        /**
         * Method that holds the chunk type associated with this chunk.
         *
         * \return Returns the chunk type tied to this chunk.
         */
        Type type() const;

        /**
         * Method that returns the number of valid bytes of data contained in this chunk.  This value includes any
         * additional header data.
         *
         * \return Returns the number of valid bytes contained in the chunk.
         */
        unsigned numberValidBytes() const;

        /**
         * Method that returns the chunk size in the container, in bytes.
         *
         * \return Returns the actual chunk size, in bytes.
         */
        unsigned chunkSize() const;

        /**
         * Method that can be used to update the CRC value.
         *
         * \param[in] newCrcValue The new CRC value.
         */
        void setCrc(RunningCrc newCrcValue);

        /**
         * Method that returns the CRC associated with the chunk.
         *
         * \return Returns the current CRC associated with the chunk.
         */
        RunningCrc crc() const;

        /**
         * Method that converts a file index to a file position.
         *
         * \param[in] index The file index to convert.
         *
         * \return Returns the file byte offset.
         */
        static unsigned long long toPosition(FileIndex index);

        /**
         * Method that converts a file position to a file index.
         *
         * \param[in] position THe position to be converted.
         *
         * \return Returns the associated file index.
         */
        static FileIndex toFileIndex(unsigned long long position);

        /**
         * Method that converts a power-of-2 chunk size value to a size in bytes.
         *
         * \param[in] chunkP2 The power-of-2 chunk size value.  Value is expected to range between 0 and 7, inclusive.
         *
         * \return Returns the total size of the chunk, in bytes.
         */
        static unsigned toChunkSize(ChunkP2 chunkP2);

        /**
         * Method that calculates the closest chunk size less than or equal to a specified number of bytes of storage.
         * This method primarily exists to optimize fill chunks on the media.
         *
         * \param[in] spaceMaximum The maximum space available for the chunk.
         */
        static ChunkP2 toClosestSmallerChunkP2(unsigned long spaceMaximum);

        /**
         * Method that calculates the closest chunk size less than or equal to a specified number of bytes of storage.
         * This method primarily exists to optimize fill chunks on the media.
         *
         * \param[in] spaceMinimum The minimum space required to store the chunk.
         */
        static ChunkP2 toClosestLargerChunkP2(unsigned long spaceMinimum);

    protected:
        /**
         * Method that returns a pointer to the raw header data.v
         *
         * \return Returns an array of bytes to the raw chunk header data.
         */
        std::uint8_t* fullHeader() const;

        /**
         * Method that returns the total size of the header, in bytes.
         *
         * \return Returns the size of the header, in bytes.
         */
        unsigned fullHeaderSizeBytes() const;

        /**
         * Method that returns a pointer to the additional header data.
         *
         * \return Returns an array of bytes to the additional allocated chunk header space.
         */
        std::uint8_t* additionalHeader() const;

        /**
         * Method that returns the total size of the additional header, in bytes.
         *
         * \return Returns the size of the header, in bytes.
         */
        unsigned additionalHeaderSizeBytes() const;

        /**
         * Method that returns the total available space in the chunk based on the current chunk size that can be used
         * by derived classes.  The value takes into account and subtracts out space currently allocated for the header,
         * including additional header bytes.
         *
         * \return Returns the total available space in the chunk.
         */
        unsigned additionalAvailableSpace() const;

        /**
         * Method that sets the type of this chunk.
         *
         * \param[in] newType The new chunk type.
         */
        void setType(Type newType);

        /**
         * Method that sets number of valid bytes tracked in the chunk.
         *
         * \param[in] newValidByteCount The desired number of bytes to store into this chunk.
         *
         * \param[in] canGrowChunkSize  If true, the chunk size is allowed to grow.  If false, the chunk size must
         *                              remain fixed or shrink in size.
         *
         * \param[in] chunkSizeChanged  A pointer to a boolean value that can be used to determine if the chunk size
         *                              changed with this change in the number of valid bytes.
         *
         * \return Returns the actual number of valid bytes that can be contained in this chunk.  The value will be
         *         equal to or less than the value specified.
         */
        unsigned setNumberValidBytes(
            unsigned newValidByteCount,
            bool     canGrowChunkSize = false,
            bool*    chunkSizeChanged = nullptr
        );

        /**
         * Method that finds an optimal chunk size that will fit within a specified amount of space.  The chunk will be
         * configured such that all data is considered valid.
         *
         * You can use this method when either you have a significant amount of data and want the largest chunk possible
         * that can fit within a specific amount of space.  You can also use this method to create fill chunks to fit
         * within a specified amount of space.  Note that the created chunk may be less than the space desired.
         *
         * \param[in] availableSpace The available space for the chunk, in bytes.
         *
         * \return Returns the largest chunk that can fit within the specified amount of space.  The value will always
         *         be greater or equal to the minimum chunk size (currently 32 bytes) and will be equal to or less than
         *         the available space in bytes.  If the available space is less than the minimum chunk size, a value of
         *         zero is returned.
         */
        unsigned setBestFitSize(unsigned availableSpace);

        /**
         * Method that indicates that all bytes in the chunk should be marked as valid.  This method will not adjust the
         * chunk size.
         */
        void setAllBytesValid();

        /**
         * Method that can be used to initialize a \ref ChunkHeader::RunningCrc value.  Calculates the CRC over the
         * header.
         *
         * \return Returns an initial CRC value.
         */
        RunningCrc initializeCrc() const;

        /**
         * Method that can be used to compute a running CRC value.
         *
         * \param[in] currentCrc  The current CRC value calculated by \ref ChunkHeader::initializeCrc or a previous call
         *                        to this method.
         *
         * \param[in] data        The data to calculate the CRC over.
         *
         * \param[in] dataLength The number of valid bytes of data.
         *
         * \return Returns an updated CRC value.
         */
        static RunningCrc calculateCrc(RunningCrc currentCrc, const std::uint8_t* data, unsigned dataLength);

        /**
         * Method that calculates the power of 2 of a number, rounding down.
         *
         * \param[in] x The value to calculate the power of 2 of.
         *
         * \return Returns the power of 2 of the MSB of the number.
         */
        static unsigned log2(std::uint32_t x);

    private:
        /**
         * Table used to do fast log2 computations.
         */
        static const unsigned char mulDeBruijnBitTable[32];

        /**
         * Table used to perform fast CRC computations.
         */
        static const std::uint16_t crcTable[256];

        /**
         * The raw header data.
         */
        std::uint8_t* header;

        /**
         * The total allocated chunk header size.
         */
        std::uint16_t headerSize;
};

#endif
