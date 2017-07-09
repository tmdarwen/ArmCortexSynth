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

#include "SynthMenu/OscillatorLevel.h"
#include "Utilities/StringUtilities.h"
#include "AudioGeneration/Oscillator.h"

OscillatorLevel::OscillatorLevel(Oscillator* oscillator) : oscillator_(oscillator) { }

void OscillatorLevel::Increment()
{
    if(oscillator_->GetLevel() < MAX_VALUE_)
    {
        oscillator_->SetLevel(oscillator_->GetLevel() + 1);
    }
}

void OscillatorLevel::Decrement()
{
    if(oscillator_->GetLevel() > MIN_VALUE_)
    {
        oscillator_->SetLevel(oscillator_->GetLevel() - 1);
    }
}

const char* OscillatorLevel::GetValueAsText()
{
    NumberToString(oscillator_->GetLevel(), text_);
    return text_;
}
