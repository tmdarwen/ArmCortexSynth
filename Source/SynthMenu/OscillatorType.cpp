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

#include "SynthMenu/OscillatorType.h"
#include "AudioGeneration/Oscillator.h"

OscillatorType::OscillatorType(Oscillator& oscillator) : oscillator_(oscillator) { }

void OscillatorType::Increment()
{
    if(oscillator_.GetWaveformType() == None) { oscillator_.SetWaveformType(Sawtooth); }
    else if(oscillator_.GetWaveformType() == Square) { oscillator_.SetWaveformType(None); }
    else if(oscillator_.GetWaveformType() == Sawtooth) { oscillator_.SetWaveformType(Square); }
}

void OscillatorType::Decrement()
{
    if(oscillator_.GetWaveformType() == None) { oscillator_.SetWaveformType(Square); }
    else if(oscillator_.GetWaveformType() == Square) { oscillator_.SetWaveformType(Sawtooth); }
    else if(oscillator_.GetWaveformType() == Sawtooth) { oscillator_.SetWaveformType(None); }
}

const char* OscillatorType::GetValueAsText()
{
    if(oscillator_.GetWaveformType() == None) { return "None"; }
    else if(oscillator_.GetWaveformType() == Square) { return "Square"; }
    return "Sawtooth";
}

WaveformType OscillatorType::GetType()
{
    return oscillator_.GetWaveformType();
}
