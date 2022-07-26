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
* This header defines the \ref ScatterGatherListSegment class.
***********************************************************************************************************************/

/* .. sphinx-project inecontainer */

#ifndef SCATTER_GATHER_LIST_SEGMENT_H
#define SCATTER_GATHER_LIST_SEGMENT_H

#include <cstdint>

/**
 * Trivial class that can be used to manage an entry in a scatter-gather list.
 */
class ScatterGatherListSegment {
    public:
        /**
         * Constructor.
         *
         * \param[in] base           Pointer to the base address for data in this segment.
         *
         * \param[in] length         The length of the segment, in bytes.
         *
         * \param[in] processedCount The number of bytes to indicate as processed in this segment.
         */
        ScatterGatherListSegment(std::uint8_t* base = nullptr, unsigned length = 0, unsigned processedCount = 0);

        /**
         * Constructor.
         *
         * \param[in] base           Pointer to the base address for data in this segment.
         *
         * \param[in] end            Pointer just past the end of the segment.
         *
         * \param[in] processedCount The number of bytes to indicate as processed in this segment.
         */
        ScatterGatherListSegment(std::uint8_t* base, std::uint8_t* end, unsigned processedCount = 0);

        /**
         * Copy constructor.
         *
         * \param[in] other The instance to be copied.
         */
        ScatterGatherListSegment(const ScatterGatherListSegment& other);

        ~ScatterGatherListSegment();

        /**
         * Method that can be used to set the base pointer for this segment.
         *
         * \param[in] newBase   The new base pointer.
         *
         * \param[in] newLength The new base pointer.
         */
        void update(std::uint8_t* newBase, unsigned newLength);

        /**
         * Method that can be used to set the base pointer for this segment.
         *
         * \param[in] newBase The new base pointer.
         *
         * \param[in] newEnd  The new end pointer.
         */
        void update(std::uint8_t* newBase, std::uint8_t* newEnd);

        /**
         * Method that can be used to set the base pointer for this segment.
         *
         * \param[in] newBase The new base pointer.
         */
        void setBase(std::uint8_t* newBase);

        /**
         * Method that can be used to obtain the base pointer for this segment.
         *
         * \return Returns a pointer to the start of the segment.
         */
        std::uint8_t* base() const;

        /**
         * Method that can be used to set length of this segment.
         *
         * \param[in] newLength The new base pointer.
         */
        void setLength(unsigned newLength);

        /**
         * Method that can be used to obtain the length of this segment.
         *
         * \return Returns the length of this segment.
         */
        unsigned length() const;

        /**
         * Method that can be used to set the end pointer for this segment.
         *
         * \param[in] newEnd The new end pointer value.
         */
        void setEnd(std::uint8_t* newEnd);

        /**
         * Method that can be used to obtain the current end pointer.
         *
         * \return Returns the current end pointer.
         */
        std::uint8_t* end() const;

        /**
         * Method that sets the processed byte count.
         *
         * \param[in] newProcessedCount The new processed count for this segment.
         */
        void setProcessedCount(unsigned newProcessedCount);

        /**
         * Method that returns the processed byte count for this segment.
         *
         * \return Returns the processed byte count.
         */
        unsigned processedCount() const;

        /**
         * Copy operator.
         *
         * \param[in] other The instance to be copied.
         *
         * \return Returns a reference to this object.
         */
        ScatterGatherListSegment& operator=(const ScatterGatherListSegment& other);

        /**
         * Comparison operator.
         *
         * \param[in] other The instance to be compared against.
         *
         * \return Returns true if the segments are identical.  Returns false if the segments are different.
         */
        bool operator==(const ScatterGatherListSegment& other) const;

        /**
         * Comparison operator.
         *
         * \param[in] other The instance to be compared against.
         *
         * \return Returns true if the segments are different.  Returns false if the segments are identical.
         */
        bool operator!=(const ScatterGatherListSegment& other) const;

    private:
        /**
         * The current base pointer.
         */
        std::uint8_t* currentBase;

        /**
         * The current length.
         */
        unsigned currentLength;

        /**
         * The reported number of processed bytes.
         */
        unsigned currentProcessedCount;
};

#endif
