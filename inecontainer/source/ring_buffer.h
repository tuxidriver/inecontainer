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
* This header defines the \ref RingBuffer class.
***********************************************************************************************************************/

/* .. sphinx-project inecontainer */

#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <cstring>

/**
 * Template class that provides basic functions to manage a ring buffer.  The ring buffer class provides two mechanisms
 * for use.
 *
 *     * You can use the ring buffer to insert or extract single entries.
 *
 *     * You can use the ring buffer to perform bulk insertion or bulk extraction.
 *
 * Insertion can be safely performed on a different thread from extraction.
 *
 * \param[in] T The data type managed by the ring buffer.  Class T is expected to be copy assignable and must have a
 *              default constructor.
 *
 * \param[in] L The length of the ring buffer, in entries.
 */
template<typename T, unsigned L> class RingBuffer {
    public:
        /**
         * The ring buffer size, in entries.
         */
        static constexpr unsigned bufferLength = L;

        RingBuffer() {
            currentInsertionIndex  = 0;
            currentExtractionIndex = 0;
            currentEntryCount      = 0;
        }

        /**
         * Copy constructor.
         *
         * \param[in] other The instance to be copied.
         */
        RingBuffer(const RingBuffer<T,L>& other) {
            for (unsigned i=0 ; i<bufferLength ; ++i) {
                buffer[i] = other.buffer[i];
            }

            currentInsertionIndex  = other.currentInsertionIndex;
            currentExtractionIndex = other.currentExtractionIndex;
            currentEntryCount      = other.currentEntryCount;
        }

        ~RingBuffer() {};

        /**
         * Method you can use to determine if the ring buffer is empty.
         *
         * \return Returns true if the ring buffer is empty.  Returns false if the ring buffer is not empty.
         */
        bool empty() const {
            return currentEntryCount == 0;
        }

        /**
         * Method you can use to determine if the ring buffer is not empty.
         *
         * \return Returns true if the ring buffer is not empty.  Returns false if the ring buffer is empty.
         */
        bool notEmpty() const {
            return !empty();
        }

        /**
         * Method you can call to determine if the ring buffer is full.
         *
         * \return Returns true if the ring buffer is full.  Returns false if the ring buffer is not full.
         */
        bool full() const {
            return currentEntryCount == bufferLength;
        }

        /**
         * Method you can call to determine if the ring buffer is not full.
         *
         * \return Returns true if the ring buffer is not full.  Returns false if the ring buffer is full.
         */
        bool notFull() const {
            return !full();
        }

        /**
         * Method you can call to determine the number of entries in the ring buffer.
         *
         * \return Returns the number of entries in the ring buffer.
         */
        unsigned count() const {
            return currentEntryCount;
        }

        /**
         * Alternate method you can call to determine the number of entries in the ring buffer.
         *
         * \return Returns the current number of entries in the ring buffer.
         */
        unsigned length() const {
            return count();
        }

        /**
         * Method you can call to determine the available space in the ring buffer.
         *
         * \return Returns the available space in the ring buffer.
         */
        unsigned available() const {
            return bufferLength - currentEntryCount;
        }

        /**
         * Method you can call to determine the total size of the ring buffer (used + available).
         *
         * \ref RingBuffer::bufferLength
         *
         * \return Returns the total size of the ring buffer, in entries.
         */
        unsigned size() const {
            return bufferLength;
        }

        /**
         * Method that clears out the ring buffer.
         */
        void clear() {
            currentInsertionIndex  = 0;
            currentExtractionIndex = 0;
            currentEntryCount      = 0;
        }

        /**
         * Method you can use to insert a single entry into the ring buffer.
         *
         * \param[in] entry The entry to be inserted.
         *
         * \return Returns true on success, returns false if the ring buffer is full.
         */
        bool insert(const T& entry) {
            bool success;

            if (currentEntryCount < bufferLength) {
                buffer[currentInsertionIndex] = entry;

                unsigned nextInsertionIndex = currentInsertionIndex + 1;
                if (nextInsertionIndex >= bufferLength) {
                    nextInsertionIndex = 0;
                }

                currentInsertionIndex = nextInsertionIndex;
                ++currentEntryCount;

                success = true;
            } else {
                success = false;
            }

            return success;
        }

        /**
         * Method you can use to extract a single entry into the ring buffer.
         *
         * \param[in] defaultValue The default value to return if the ring buffer is empty.
         *
         * \return Returns the extracted entry.  The default value will be returned if the ring buffer is empty.
         */
        T extract(const T& defaultValue = T()) {
            T extracted;

            if (currentEntryCount > 0) {
                extracted = buffer[currentExtractionIndex];

                unsigned nextExtractionIndex = currentExtractionIndex + 1;
                if (nextExtractionIndex >= bufferLength) {
                    nextExtractionIndex = 0;
                }

                currentExtractionIndex = nextExtractionIndex;
                --currentEntryCount;
            } else {
                extracted = defaultValue;
            }

            return extracted;
        }

        /**
         * Method that can be used to peek into the ring buffer.
         *
         * \param[in] offset A positive offset from the current extraction point into the ring buffer.  A value of zero
         *                   will return a reference to the next entry to be extracted.  This value must be less than
         *                   the length of the ring buffer.
         *
         * \return Returns a reference to the requested ring buffer entry.  You can use this reference to both read and
         *         modify the ring buffer contents, in place.
         */
        T& snoop(unsigned offset) {
            unsigned snoopOffset = (currentExtractionIndex + offset) % bufferLength;
            return buffer[snoopOffset];
        }

        /**
         * Method that can be used to peek into the ring buffer.
         *
         * \param[in] offset A positive offset from the current extraction point into the ring buffer.  A value of zero
         *                   will return a reference to the next entry to be extracted.  This value must be less than
         *                   the length of the ring buffer.
         *
         * \return Returns a reference to the requested ring buffer entry.  You can use this reference to both read and
         *         modify the ring buffer contents, in place.
         */
        const T& snoop(unsigned offset) const {
            unsigned snoopOffset = (currentExtractionIndex + offset) % bufferLength;
            return buffer[snoopOffset];
        }

        /**
         * Method you can use to perform a bulk insertion.  The function provides a pair of buffer pointers and lengths
         * into the internal buffer in insertion order.
         *
         * \param[out] p1 The first buffer pointer.  A null pointer is returned if there is no space in the buffer.
         *
         * \param[out] l1 The available space tied to the first buffer pointer.  Will be non-zero if p1 is not null.
         *
         * \param[out] p2 The second buffer pointer.  A null pointer is returned if this buffer pointer does not need to
         *                be used.
         *
         * \param[out] l2 The available space tied to the second buffer pointer.  Will be non-zero if p2 is not null.
         *
         * \return Returns the total available space that can be used between the two buffer pointers.
         */
        unsigned bulkInsertionStart(T** p1, unsigned* l1, T** p2, unsigned* l2) {
            unsigned availableSpace = bufferLength - currentEntryCount;

            if (availableSpace > 0) {
                if (currentInsertionIndex >= currentExtractionIndex) {
                    // ........ddddddd..........
                    //         ^      ^
                    //         e      i
                    //
                    // .........................
                    //         ^
                    //         i
                    //         e

                    *p1 = buffer + currentInsertionIndex;
                    *l1 = bufferLength - currentInsertionIndex;

                    if (currentExtractionIndex == 0) {
                        *p2 = nullptr;
                        *l2 = 0;
                    } else {
                        *p2 = buffer;
                        *l2 = currentExtractionIndex;
                    }
                } else {
                    // dddddddd.......dddddddddd
                    //         ^      ^
                    //         i      e

                    *p1 = buffer + currentInsertionIndex;
                    *l1 = availableSpace;
                    *p2 = nullptr;
                    *l2 = 0;
                }
            } else {
                *p1 = nullptr;
                *p2 = nullptr;
                *l1 = 0;
                *l2 = 0;
            }

            return availableSpace;
        }

        /**
         * Method you can use to update pointers after a bulk insertion.  Note that you must not perform any insertions
         * into the list between calling \ref bulkInsertionStart and this method.
         *
         * \param[out] entriesInserted The number of inserted entries.
         *
         * \return Returns true if the number of inserted entries is valid, returns false if the number of inserted
         *         entries is invalid.
         */
        bool bulkInsertionFinish(unsigned entriesInserted) {
            bool success;

            if (entriesInserted <= bufferLength - currentEntryCount) {
                unsigned nextInsertionIndex = currentInsertionIndex + entriesInserted;
                if (nextInsertionIndex >= bufferLength) {
                    nextInsertionIndex -= bufferLength;
                }

                currentInsertionIndex = nextInsertionIndex;
                currentEntryCount += entriesInserted;

                success = true;
            } else {
                success = false;
            }

            return success;
        }

        /**
         * Method you can use to perform a bulk extractopm.  The function provides a pair of buffer pointers and lengths
         * into the internal buffer in extraction order.
         *
         * \param[out] p1 The first buffer pointer.  A null pointer is returned if there is no data to extract.
         *
         * \param[out] l1 The number of buffer entries accessible via p1.  This value will be non-zero if p1 is not
         *                null.
         *
         * \param[out] p2 The second buffer pointer.  A null pointer is returned if this buffer pointer does not need to
         *                be used.
         *
         * \param[out] l2 The number of buffer entries accessible via p2.  This value will be non-zero if p2 is not
         *                null.
         *
         * \return Returns the total number of available entries.
         */
        unsigned bulkExtractionStart(const T** p1, unsigned* l1, const T** p2, unsigned* l2) const {
            if (currentEntryCount > 0) {
                if (currentInsertionIndex > currentExtractionIndex) {
                    // ........ddddddd..........
                    //         ^      ^
                    //         e      i

                    *p1 = buffer + currentExtractionIndex;
                    *l1 = currentEntryCount;
                    *p2 = nullptr;
                    *l2 = 0;
                } else {
                    // dddddddd.......dddddddddd
                    //         ^      ^
                    //         i      e
                    //
                    // ddddddddddddddddddddddddd
                    //         ^
                    //         e
                    //         i

                    *p1 = buffer + currentExtractionIndex;
                    *l1 = bufferLength - currentExtractionIndex;

                    if (currentInsertionIndex == 0) {
                        *p2 = nullptr;
                        *l2 = 0;
                    } else {
                        *p2 = buffer;
                        *l2 = currentInsertionIndex;
                    }
                }
            } else {
                *p1 = nullptr;
                *p2 = nullptr;
                *l1 = 0;
                *l2 = 0;
            }

            return currentEntryCount;
        }

        /**
         * Method you can use to perform a bulk extractopm.  The function provides a pair of buffer pointers and lengths
         * into the internal buffer in extraction order.
         *
         * \param[out] p1 The first buffer pointer.  A null pointer is returned if there is no data to extract.
         *
         * \param[out] l1 The number of buffer entries accessible via p1.  This value will be non-zero if p1 is not
         *                null.
         *
         * \param[out] p2 The second buffer pointer.  A null pointer is returned if this buffer pointer does not need to
         *                be used.
         *
         * \param[out] l2 The number of buffer entries accessible via p2.  This value will be non-zero if p2 is not
         *                null.
         *
         * \return Returns the total number of available entries.
         */
        unsigned bulkExtractionStart(T** p1, unsigned* l1, T** p2, unsigned* l2) {
            if (currentEntryCount > 0) {
                if (currentInsertionIndex > currentExtractionIndex) {
                    // ........ddddddd..........
                    //         ^      ^
                    //         e      i

                    *p1 = buffer + currentExtractionIndex;
                    *l1 = currentEntryCount;
                    *p2 = nullptr;
                    *l2 = 0;
                } else {
                    // dddddddd.......dddddddddd
                    //         ^      ^
                    //         i      e
                    //
                    // ddddddddddddddddddddddddd
                    //         ^
                    //         e
                    //         i

                    *p1 = buffer + currentExtractionIndex;
                    *l1 = bufferLength - currentExtractionIndex;

                    if (currentInsertionIndex == 0) {
                        *p2 = nullptr;
                        *l2 = 0;
                    } else {
                        *p2 = buffer;
                        *l2 = currentInsertionIndex;
                    }
                }
            } else {
                *p1 = nullptr;
                *p2 = nullptr;
                *l1 = 0;
                *l2 = 0;
            }

            return currentEntryCount;
        }

        /**
         * Method you can use to update pointers after a bulk extraction.  Note that you must not perform any
         * extractions from the list between calling \ref bulkExtractionStart and this method.
         *
         * \param[out] entriesExtracted The number of removed entries.
         *
         * \return Returns true if the number of extracted entries is valid, returns false if the number of extracted
         *         entries is invalid.
         */
        bool bulkExtractionFinish(unsigned entriesExtracted) {
            bool success;

            if (entriesExtracted <= currentEntryCount) {
                unsigned nextExtractionIndex = currentExtractionIndex + entriesExtracted;
                if (nextExtractionIndex >= bufferLength) {
                    nextExtractionIndex -= bufferLength;
                }

                currentExtractionIndex = nextExtractionIndex;
                currentEntryCount -= entriesExtracted;

                success = true;
            } else {
                success = false;
            }

            return success;
        }

        /**
         * Assignment operator.
         *
         * \param[in] other The instance to be copied.
         */
        RingBuffer<T,L>& operator=(const RingBuffer<T,L>& other) {
            for (unsigned i=0 ; i<bufferLength ; ++i) {
                buffer[i] = other.buffer[i];
            }

            currentInsertionIndex  = other.currentInsertionIndex;
            currentExtractionIndex = other.currentExtractionIndex;
            currentEntryCount      = other.currentEntryCount;

            return *this;
        }

    private:
        /**
         * The internal data store for the ring buffer.
         */
        T buffer[L];

        /**
         * The current buffer insertion index.
         */
        unsigned currentInsertionIndex;

        /**
         * The current buffer extraction index.
         */
        unsigned currentExtractionIndex;

        /**
         * The number of entries in the ring buffer.
         */
        unsigned currentEntryCount;
};

#endif
