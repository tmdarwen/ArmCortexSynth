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

#pragma once

#include <stdint.h>

class Oscillator;

class AudioMixer
{
    public:
        AudioMixer();
        void SetOscillator1(Oscillator*);
        void SetOscillator2(Oscillator*);
        void SetOscillator3(Oscillator*);

        void SetMIDINote(uint8_t midiNoteIndex);
        void GetAudioData(uint16_t buffer[], uint32_t bufferSampleSize);

    private:
        void CalculateNoteFrequencyTable();
        int GetActiveOscillatorCount();

        Oscillator* oscillator1_;
        Oscillator* oscillator2_;
        Oscillator* oscillator3_;

        uint8_t midiNoteIndex_;
        static const uint8_t MIDI_NOTE_COUNT_ = 128;
        static const uint8_t NO_MIDI_NOTE_ = 0;
        float noteFrequencyTable_[MIDI_NOTE_COUNT_];

        uint64_t sampleCount_;
};
