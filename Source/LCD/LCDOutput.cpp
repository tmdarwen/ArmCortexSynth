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

#include "LCDOutput.h"

LCDOutput::LCDOutput()
{
    for(uint8_t i = 0; i < ROWS_; ++i)
    {
        for(uint8_t j = 0; j < COLUMNS_; ++j)
        {
            existingText_[i][j] = ' ';
        }
    }

    lcd_.ClearScreen();
}

LCDOutput::~LCDOutput() { }

void LCDOutput::Process(uint64_t ticks)
{
    lcd_.Process(ticks);
}

void LCDOutput::SetText(uint8_t line, const char* text)
{
    uint8_t columnCounter = 0;
    while(text[columnCounter])
    {
        if(existingText_[line][columnCounter] != text[columnCounter])
        {
            lcd_.WriteChar(line, columnCounter, text[columnCounter]);
            existingText_[line][columnCounter] = text[columnCounter];
        }
        ++columnCounter;
    }

    while(columnCounter < COLUMNS_)
    {
        if(existingText_[line][columnCounter] != ' ')
        {
            lcd_.WriteChar(line, columnCounter, ' ');
            existingText_[line][columnCounter] = ' ';
        }
        ++columnCounter;
    }
}
