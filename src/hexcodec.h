/*
 *  Copyright 2015 Masatoshi Teruya. All rights reserved.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to
 *  deal in the Software without restriction, including without limitation the
 *  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 *  sell copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 *  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 *
 *  hexcodec.h
 *  Created by Masatoshi Teruya on 15/03/14.
 *
 */

#ifndef HEXCODEC_H
#define HEXCODEC_H

#include <errno.h>
#include <stddef.h>

// dest length must be greater than len*2
static inline size_t hex_encode(char *dest, size_t dest_len, char *src,
                                size_t src_len)
{
    static const char dec2hex[16] = "0123456789abcdef";
    unsigned char *d              = (unsigned char *)dest;
    unsigned char *s              = (unsigned char *)src;
    size_t k                      = src_len;

    // clamp consumed src length to current dest capacity
    if (k > dest_len / 2) {
        k = dest_len / 2;
    }

    // destination buffer is too small
    if (k == 0 && src_len > 0) {
        errno = EINVAL;
        return -1;
    }

    // encode src to hex-encoded dest and return consumed src length
    for (size_t i = 0; i < k; i++) {
        *d++ = dec2hex[s[i] >> 4];
        *d++ = dec2hex[s[i] & 0xf];
    }
    return k;
}

// src length must be multiples of two
// dest length must be greater than len/2
static inline size_t hex_decode(char *dest, size_t dest_len, char *src,
                                size_t src_len)
{
    static const signed char hex2dec[256] = {
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        //  0  1  2  3  4  5  6  7  8  9
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1, -1, -1, -1, -1,
        //  A   B   C   D   E   F
        10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        //  a   b   c   d   e   f
        10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
    unsigned char *d = (unsigned char *)dest;
    unsigned char *s = (unsigned char *)src;
    size_t k         = src_len;

    // clamp consumed src length to current dest capacity
    if (k / 2 > dest_len) {
        k = dest_len * 2;
    }

    // destination buffer is too small
    if (k == 0 && src_len > 0) {
        errno = EINVAL;
        return -1;
    }

    for (size_t i = 0; i < k; i += 2) {
        // illegal byte sequence
        if (hex2dec[s[i]] == -1 || hex2dec[s[i + 1]] == -1) {
            errno = EILSEQ;
            return -1;
        }
        *d++ = hex2dec[s[i]] << 4 | hex2dec[s[i + 1]];
    }

    // return consumed src length
    return k;
}

#endif
