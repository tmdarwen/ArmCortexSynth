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

AudioMixer::AudioMixer() : sampleCount_(0), midiNoteIndex_(NO_MIDI_NOTE_)
{
    InitializeNoteFrequencyTable();
	SetupDefaultOscillatorValues();
}

void AudioMixer::SetupDefaultOscillatorValues()
{
	oscillator2_.SetCent(8);
	oscillator3_.SetCent(-8);
}

Oscillator& AudioMixer::GetOscillator1()
{
    return oscillator1_;
}

Oscillator& AudioMixer::GetOscillator2()
{
    return oscillator2_;
}

Oscillator& AudioMixer::GetOscillator3()
{
    return oscillator3_;
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

uint8_t AudioMixer::GetActiveOscillatorCount()
{
    int oscillatorCount = 0;

    if(oscillator1_.GetWaveformType() != None) { ++oscillatorCount; }
    if(oscillator2_.GetWaveformType() != None) { ++oscillatorCount; }
    if(oscillator3_.GetWaveformType() != None) { ++oscillatorCount; }

    return oscillatorCount;
}

void AudioMixer::GetAudioData(uint16_t buffer[], uint32_t bufferSampleSize)
{
    // Silent the audio buffer before adding audio to it through the oscillators
	for(unsigned int i = 0 ; i < bufferSampleSize; ++i)
    {
        buffer[i] = 0;
    }

	// If no note is being played, no audio needed
    if(midiNoteIndex_ == NO_MIDI_NOTE_)
    {
        return;
    }

    uint8_t totalOscillatorCount = GetActiveOscillatorCount();

    oscillator1_.MixInOscillatorAudio(buffer, bufferSampleSize, totalOscillatorCount, midiNoteIndex_, sampleCount_);
    oscillator2_.MixInOscillatorAudio(buffer, bufferSampleSize, totalOscillatorCount, midiNoteIndex_, sampleCount_);
    oscillator3_.MixInOscillatorAudio(buffer, bufferSampleSize, totalOscillatorCount, midiNoteIndex_, sampleCount_);

    sampleCount_ += bufferSampleSize;
}
