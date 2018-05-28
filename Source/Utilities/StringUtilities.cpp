/*
 * ARM Cortex-M4F Synthesizer
 *
 * Copyright (c) 2017 Terence M. Darwen - tmdarwen.com
 *
 * The MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "Utilities/StringUtilities.h"
#include <stddef.h>

char ToHex(uint32_t i) { return i <= 9 ? ('0' + i) : ('A' - 10 + i); }

void StringCopy(const char* source, char* target, uint32_t maxLength)
{
    uint32_t counter = 0;
    while(counter < maxLength && source[counter])
    {
        target[counter] = source[counter];    
        ++counter;
    }

    target[counter] = '\0';  // Place string terminator at the end
}

unsigned int StringLength(const char* theString)
{
    uint32_t counter = 0;
    while(theString[counter])
    {
        ++counter;
    }

    return counter;
}

void NumberToString(int32_t number, char* string)
{
    IntegerToString(number, string, 10);
}

void StringCat(const char* source, char* target, uint32_t maxLength)
{
    uint32_t targetLength = StringLength(target);
    StringCopy(source, &(target[targetLength]), maxLength - targetLength);
}

// The following function came from
// https://stackoverflow.com/questions/8257714/how-to-convert-an-int-to-string-in-c
char* IntegerToString(int32_t value, char *result, int32_t base)
{
    // check that the base if valid
    if (base < 2 || base > 36) { *result = '\0'; return result; }

    char* ptr = result, *ptr1 = result, tmp_char;
    int tmp_value;

    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
    } while ( value );

    // Apply negative sign
    if (tmp_value < 0) *ptr++ = '-';
    *ptr-- = '\0';
    while (ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }
    return result;
}

void ByteToHexString(uint8_t theByte, char* string)
{
    string[0] = '0';
    string[1] = 'x';
    string[2] = ToHex(((theByte & 0x00F0) >> 4));
    string[3] = ToHex((theByte & 0x000F));
    string[4] = '\0';
}
