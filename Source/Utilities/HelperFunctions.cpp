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

#include "HelperFunctions.h"

#define TO_HEX(i) (i <= 9 ? '0' + i : 'A' - 10 + i)

// Converts the given byte to a string value.  Note that the size of the given string parameter must
// be at least five characters in length.
void ByteToHexString(char theByte, char* string)
{
    string[0] = '0';
    string[1] = 'x';
    string[2] = TO_HEX(((theByte & 0x00F0) >> 4));
    string[3] = TO_HEX((theByte & 0x000F));
    string[4] = '\0';
}

void DisableInterrupts()
{
    __asm(" CPSID  I\n");
}

void EnableInterrupts()
{
    __asm(" CPSIE  I\n");
}

void WaitForInterrupt()
{
    __asm(" WFI\n");
}
