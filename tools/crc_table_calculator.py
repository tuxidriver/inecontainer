#!/usr/bin/python
#-*-python-*-##################################################################
# Copyright 2017 - 2022 Inesonic, LLC
#
# MIT License:
#   Permission is hereby granted, free of charge, to any person obtaining a
#   copy of this software and associated documentation files (the "Software"),
#   to deal in the Software without restriction, including without limitation
#   the rights to use, copy, modify, merge, publish, distribute, sublicense,
#   and/or sell copies of the Software, and to permit persons to whom the
#   Software is furnished to do so, subject to the following conditions:
#   
#   The above copyright notice and this permission notice shall be included in
#   all copies or substantial portions of the Software.
#   
#   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
#   DEALINGS IN THE SOFTWARE.
###############################################################################

"""
Program that calculates CRC tables for the generator polynomial:

    x^16 + x^15 + x^2 + 1

The algorithm assumes a 256 byte table of 16-bit words.  To employ, you should
define a shift register that is as long as the generator polynomial order plus
as long as the unit being shifted in.  For a CRC-16 polynomial and byte wide
computation, the shift register should be 24 bits.

In the case above, shift in the received byte and extract the upper 8-bit.
With a Galois LFSR, the shifted 8-bits would have been applied to the bits as
they're shifted in, modifying

"""

###############################################################################
# Import:
#

import math
import random

###############################################################################
# Globals:
#

POLYNOMIAL = 0x18005;
"""
The generator polymomial represented as a hexidecimal number.

"""

POLYNOMIAL_ORDER = 16;
"""
The the order of the polynomial above.

"""

WORD_SIZE = 8;
"""
The size of the inserted word, in bits.

"""

NUMBER_TEST_ITERATIONS = 10
"""
The number of iterations of test to perform.  A value of 0 disables testing.

"""

MAXIMUM_LINE_LENGTH = 120
"""
The maximum line width.  The value will be used to optimize formatting of the
table.

"""

INDENTATION = 4
"""
Indentation to place at the left size of each line of values.

"""

###############################################################################
# Functions:
#

def galois_lfsr_bit(
    order,
    polynomial,
    initial_lfsr_state,
    inserted_data,
    inserted_length):
    """
    Shifts data into a Galois LFSR on bit at a time.  The data is inserted MSB
    first into the LSB of the LFSR.

    :param order:
        The LFSR length, in bits.

    :param polynomial:
        A value representing the generator polynomial.  The CRC-16 polynomial
        :matn:`x^{16} + x^{15} + x^2 + 1` would be represented by the value
        0x18005.

    :param initial_lfsr_state:
        A value representing the initial state of the LFSR before the byte is
        shifted in.

    :param inserted_data:
        The byte to be inserted into the LFSR, MSB first.

    :param inserted_length:
        The length of the inserted data, in bits.

    :type order:              int or long
    :type polynomial:         int or long
    :type initial_lfsr_state: int or long
    :type inserted_data:      int or long
    :type inserted_length:    int or long

    :return:
        Returns the final LFSR state.

    :rtype: long

    """

    lfsr = long(initial_lfsr_state)
    lfsr_mask = long(1) << order
    insert_mask = 1 << (inserted_length - 1)

    while insert_mask != 0:
        if inserted_data & insert_mask != 0:
            lfsr = (lfsr << 1) | 1
        else:
            lfsr = lfsr << 1

        if lfsr & lfsr_mask != 0:
            lfsr ^= polynomial

        insert_mask = insert_mask >> 1

    return lfsr


def galois_lfsr_word(
    order,
    xor_table,
    initial_lfsr_state,
    inserted_word,
    inserted_length):
    """
    Shifts data into a Galois LFSR a word at a time.  The data is inserted MSB
    first into the LSB of the LFSR.

    :param order:
        The LFSR length, in bits.  The LFSR must be at least 2 times the
        inserted word length.

    :param table:
        A table of XOR values to apply.  The index to the table is the upper
        word of the LFSR.  Values are XOR'd against the entire LFSR after the
        new word is inserted.  The table length must be
        :math:`2^{\inserted\_length}` entries long.

    :param initial_lfsr_state:
        A value representing the initial state of the LFSR before the byte is
        shifted in.

    :param inserted_word:
        The word to be inserted into the LFSR, MSB first.

    :param inserted_length:
        The length of the inserted word, in bytes.

    :type order:              int or long
    :type table:              list or tuple
    :type initial_lfsr_state: int or long
    :type inserted_word:      int or long
    :type inserted_length:    int or long

    :return:
        Returns the final LFSR state.

    :rtype: long

    """

    lfsr = long(initial_lfsr_state)
    table_index = (
          (lfsr >> (order - inserted_length))
        & ((1 << inserted_length) - 1)
    )

    lfsr = (lfsr << inserted_length) | inserted_word
    lfsr ^= xor_table[table_index]

    return lfsr & ((1 << order) - 1)


def generate_xor_table(order, polynomial, inserted_length):
    """
    Shifts data into a Galois LFSR on bit at a time.  The data is inserted MSB
    first into the LSB of the LFSR.

    :param order:
        The LFSR length, in bits.  The LFSR must be at least 2 times the
        inserted word length.

    :param polynomial:
        A value representing the generator polynomial.  The CRC-16 polynomial
        :matn:`x^{16} + x^{15} + x^2 + 1` would be represented by the value
        0x18005.

    :param inserted_length:
        The length of the inserted word, in bytes.

    :type order:              int or long
    :type polynomial:         int or long
    :type inserted_length:    int or long

    :return:
        Returns a list containing :math:`2^{inserted\_length}` entries that can
        be used by the :meth:`galois_lfsr_word` method.

    :rtype: list

    """

    table_length = 1 << inserted_length
    xor_table = []
    for inserted_word in range(table_length):
        initial_lfsr_state = inserted_word << (order - inserted_length)
        xor_value = galois_lfsr_bit(
            order,
            polynomial,
            initial_lfsr_state,
            0,
            inserted_length
        )

        xor_table.append(xor_value)

    return xor_table

###############################################################################
# Functions:
#

xor_table = generate_xor_table(POLYNOMIAL_ORDER, POLYNOMIAL, WORD_SIZE)
xor_table_size = len(xor_table)

entry_size_bytes = int(math.ceil(POLYNOMIAL_ORDER / 8))
entry_format_string = "0x%%0%dX"%(2 * entry_size_bytes)

row_field_size = len("%d"%xor_table_size)
ending_comment_size = 4 + row_field_size
ending_comment_string = "// %%%ds"%(row_field_size + 1)

entry_width_chars = 2 + 2 * entry_size_bytes + 2
available_characters = MAXIMUM_LINE_LENGTH - INDENTATION - ending_comment_size
ideal_number_entries = available_characters / entry_width_chars
number_columns = int(2 ** math.floor(math.log(ideal_number_entries, 2)))
number_rows = int(math.ceil(xor_table_size / number_columns))
line_width = INDENTATION + number_columns * entry_width_chars

first_row = True

s = "std::uint%d_t ChunkHeader::crcTable[%d] = {\n"%(
    int(8 * math.ceil(POLYNOMIAL_ORDER / 8)),
    xor_table_size
)

s += "//" + " " * (INDENTATION - 2 + entry_width_chars / 2 - 2)
for column in range(number_columns):
    s += "+%-2d"%column

    if column != number_columns - 1:
        s += " " * (entry_width_chars - 3)

s += "\n//" + " " * (INDENTATION - 2)

for column in range(number_columns):
    s += "-" * (entry_width_chars - 2)

    if column != number_columns - 1:
        s += "  "

s += "\n"

for row in range(number_rows):
    l = " " * INDENTATION

    for column in range(number_columns):
        index = row * number_columns + column

        if index < xor_table_size:
            xor_value = xor_table[index]
            l += entry_format_string%xor_value

            if index == xor_table_size - 1:
                l += ""
            else:
                l += ", "

    if len(l) < line_width:
        l += " " * (line_width - len(l))

    l += ending_comment_string%("+%d"%(number_columns * row))

    if first_row:
        s += l
        first_row = False
    else:
        s += "\n" + l

s += "\n};"

print s;

if NUMBER_TEST_ITERATIONS > 0:
    bit_lfsr = 0;
    byte_lfsr = 0;

    print "Word      Bit LFSR          Word LFSR"
    print "----  ----------------  ----------------"
    for i in range(NUMBER_TEST_ITERATIONS):
        random_word = random.randint(0, xor_table_size - 1)

        new_bit_lfsr = galois_lfsr_bit(
            POLYNOMIAL_ORDER,
            POLYNOMIAL,
            bit_lfsr,
            random_word,
            WORD_SIZE
        )

        new_byte_lfsr = galois_lfsr_word(
            POLYNOMIAL_ORDER,
            xor_table,
            byte_lfsr,
            random_word,
            WORD_SIZE
        )

        print "0x%02X  0x%04X -> 0x%04X  0x%04X -> 0x%04X"%(
            random_word,
            bit_lfsr,
            new_bit_lfsr,
            byte_lfsr,
            new_byte_lfsr
        )

        assert(new_bit_lfsr == new_byte_lfsr)

        bit_lfsr = new_bit_lfsr
        byte_lfsr = new_byte_lfsr
