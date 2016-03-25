#pragma once

#include "scraps/config.h"

#include <vector>
#include <string>

#include <boost/range.hpp>

namespace scraps {

/**
* BitIterator is used to create constant forward input iterators for individual bits
*/
struct BitIterator {
    // pod. uses default constructors, destructors, and assignments

    static BitIterator Begin(const void* target, size_t len);
    static BitIterator End(const void* target, size_t len);

    const bool& operator*() const;
    BitIterator operator++();    // pre
    BitIterator operator++(int); // post
    bool operator==(const BitIterator& other);
    bool operator!=(const BitIterator& other);

    const unsigned char* target;
    size_t len;
    size_t character;
    uint8_t mask;
    bool value;
};

/**
* Encodes a number via elias omega coding to a bitfield (a vector of bools).
* Note that this vector<bool> is a standard specialization that packs each value in a single bit.
*
* @param number the number to be encoded
* @return the resulting bitfield (an elias omega code)
*/
template <typename T>
std::vector<bool> EliasOmegaEncode(const T number) {
    std::vector<bool> reverseCode(1, false); // initialize with a zero

    auto n = number;
    while (n > 1) {
        // push the number, least significant bits first (since we're writing the reverse code)
        size_t written = 0;
        while (n) {
            reverseCode.push_back(n & 1);
            n >>= 1;
            ++written;
        }
        // next, encode the number of bits written minus 1
        n = written - 1;
    }

    return std::vector<bool>(reverseCode.rbegin(), reverseCode.rend());
}

/**
* Decodes a number from an elias omega code.
*
* @param begin an iterator aimed at the first bit
* @param end an iterator aimed just past the last bit
* @param next if provided, set to an iterator aimed at the bit after the last one used in decoding
* @return the decoded number or 0 on failure
*/
template <typename T = uint64_t, typename InputIterator>
T EliasOmegaDecode(InputIterator begin, InputIterator end, InputIterator* next = nullptr) {
    T n = 1;

    for (auto it = begin; it != end; ++it) {
        if (!*it) {
            if (next) {
                *next = ++it;
            }
            return n;
        }

        T group = 1;
        for (T i = 0; i < n && ++it != end; ++i) {
            group = ((group << 1) | (*it ? 1 : 0));
        }
        n = group;
    }

    if (next) {
        *next = end;
    }
    return 0; // invalid code
}

/**
* Decodes a number from an elias omega code.
*
* @param code an iterable elias omega code
* @return the decoded number
*/
template <typename T, typename U = uint64_t>
U EliasOmegaDecode(const T& code) {
    return EliasOmegaDecode<U>(boost::begin(code), boost::end(code));
}

/**
* Encoded a bitfield to a binary string. The resulting string begins with the first bit in the bitfield,
* followed by elias omega codes that indicate the count of each run of set or unset bits.
*
* @param begin an iterator aimed at the first bit
* @param end an iterator aimed just past the last bit
* @return the resulting string
*/
template <typename InputIterator>
std::string BitfieldEncode(InputIterator begin, InputIterator end) {
    std::string ret;

    if (begin == end) {
        // nothing to encode
        return ret;
    }

    size_t count      = 0;
    bool countedValue = *begin;

    // the first bit indicates which value we start counting with
    size_t bitsBuffered = 0;
    char bitBuffer      = (countedValue << (8 - ++bitsBuffered));

    for (auto it = begin;; ++it) {
        if (it == end || *it != countedValue) {
            // done counting the value, encode the count
            auto code = EliasOmegaEncode(count);
            for (auto codeBit : code) {
                bitBuffer |= (codeBit << (8 - ++bitsBuffered));
                if (bitsBuffered == 8) {
                    ret.push_back(bitBuffer);
                    bitBuffer = bitsBuffered = 0;
                }
            }
            if (it == end) {
                // done. fill the rest of the bit buffer with 1 if it's not empty
                if (bitsBuffered) {
                    while (bitsBuffered < 8) {
                        bitBuffer |= (1 << (8 - ++bitsBuffered));
                    }
                    ret.push_back(bitBuffer);
                }
                break;
            }
            count        = 1;
            countedValue = *it;
        } else {
            ++count;
        }
    }

    return ret;
}

/**
* Encoded a bitfield to a binary string. The resulting string begins with the first bit in the bitfield,
* followed by elias omega codes that indicate the count of each run of set or unset bits.
*
* @param begin an iterator aimed at the first bit
* @param end an iterator aimed just past the last bit
* @return the resulting string
*/
template <typename T>
std::string BitfieldEncode(const T& bitfield) {
    return BitfieldEncode(boost::begin(bitfield), boost::end(bitfield));
}

/**
* Decodes a string encoded via BitfieldEncode.
*
* @param encoded the encoded string
* @param len the length of the encoded string
* @return the decoded bitfield
*/
std::vector<bool> BitfieldDecode(const void* encoded, size_t len);

} // namespace scraps
