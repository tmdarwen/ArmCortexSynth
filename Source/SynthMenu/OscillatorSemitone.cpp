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

#include "SynthMenu/OscillatorSemitone.h"
#include "Utilities/StringUtilities.h"
#include "AudioGeneration/Oscillator.h"

OscillatorSemitone::OscillatorSemitone(Oscillator* oscillator) : oscillator_(oscillator) { }

void OscillatorSemitone::Increment()
{
    if(oscillator_->GetSemitone() < MAX_VALUE_)
    {
        oscillator_->SetSemitone(oscillator_->GetSemitone() + 1);
    }
}

void OscillatorSemitone::Decrement()
{
    if(oscillator_->GetSemitone() > MIN_VALUE_)
    {
        oscillator_->SetSemitone(oscillator_->GetSemitone() - 1);
    }
}

const char* OscillatorSemitone::GetValueAsText()
{
    if(oscillator_->GetSemitone() > 0)
    {
        text_[0] = '+';
        NumberToString(oscillator_->GetSemitone(), &(text_[1]));
    }
    else
    {
        NumberToString(oscillator_->GetSemitone(), text_);
    }
    return text_;
}
