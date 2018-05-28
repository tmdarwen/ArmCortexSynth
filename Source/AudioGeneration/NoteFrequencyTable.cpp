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

#include "AudioGeneration/NoteFrequencyTable.h"
#include <math.h>

// For more info about the following values please see https://en.wikipedia.org/wiki/MIDI_tuning_standard
float twoToTheOne12th_ = 1.059463094f;
float a440Hz_ = 440.0;
int a440Index_ = 57;

// See MIDI standard, it seems to be generally agreed that MIDI notes range in index from 
// 0-to-127 with middle C at 60.
const int indexCount = 128;

float noteFrequency = 0.0;
float noteFrequencyTable_[indexCount];

void InitializeNoteFrequencyTable()
{
    volatile int i = 0;

    for(; i < indexCount; ++i)
    {
        noteFrequencyTable_[i] = a440Hz_ * powf(twoToTheOne12th_, static_cast<float>(i - a440Index_));
    }
}

float GetFrequency(uint8_t noteIndex)
{
    if(noteIndex > indexCount)
    {
        // I keep error handling simple here and just return a frequency of zero if the given 
        // note index is out of range.
        return 0.0;
    }

    return noteFrequencyTable_[noteIndex];
}
