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

#include "AudioGeneration/AudioMixer.h"
#include "AudioGeneration/Oscillator.h"
#include "AudioGeneration/NoteFrequencyTable.h"
#include <math.h>
#include <stddef.h>

AudioMixer::AudioMixer() : sampleCount_(NULL), midiNoteIndex_(NO_MIDI_NOTE_), oscillator1_(NULL), oscillator2_(NULL), oscillator3_(NULL)
{
    InitializeNoteFrequencyTable();
}

void AudioMixer::SetOscillator1(Oscillator* oscillator)
{
    oscillator1_ = oscillator;
}

void AudioMixer::SetOscillator2(Oscillator* oscillator)
{
    oscillator2_ = oscillator;
}

void AudioMixer::SetOscillator3(Oscillator* oscillator)
{
    oscillator3_ = oscillator;
}

void AudioMixer::SetMIDINote(uint8_t midiNoteIndex)
{
    if(midiNoteIndex < MIDI_NOTE_COUNT_)
    {
        midiNoteIndex_ = midiNoteIndex;
    }
    else
    {
        midiNoteIndex_ = MIDI_NOTE_COUNT_ - 1;
    }
}

int AudioMixer::GetActiveOscillatorCount()
{
    int oscillatorCount = 0;

    if(oscillator1_->GetWaveformType() != None) { ++oscillatorCount; }
    if(oscillator2_->GetWaveformType() != None) { ++oscillatorCount; }
    if(oscillator3_->GetWaveformType() != None) { ++oscillatorCount; }

    return oscillatorCount;
}

void AudioMixer::GetAudioData(uint16_t buffer[], uint32_t bufferSampleSize)
{
    if(midiNoteIndex_ == NO_MIDI_NOTE_)
    {
        return;
    }

    int totalOscillatorCount = GetActiveOscillatorCount();

    oscillator1_->MixInOscillatorAudio(buffer, bufferSampleSize, totalOscillatorCount, midiNoteIndex_, sampleCount_);
    oscillator2_->MixInOscillatorAudio(buffer, bufferSampleSize, totalOscillatorCount, midiNoteIndex_, sampleCount_);
    oscillator3_->MixInOscillatorAudio(buffer, bufferSampleSize, totalOscillatorCount, midiNoteIndex_, sampleCount_);

    sampleCount_ += bufferSampleSize;
}
