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
#include "AudioGeneration/WaveformTypes.h"

class Oscillator
{
    public:
        Oscillator();
        Oscillator(WaveformType waveformType, uint8_t level, int8_t cent, int8_t semitone);

        void MixInOscillatorAudio(uint16_t buffer[], uint32_t bufferSampleSize, uint8_t totalOscillatorCount, uint8_t noteIndex, uint32_t currentSample);

        WaveformType GetWaveformType();
        uint8_t GetLevel();
        int8_t GetCent();
        int8_t GetSemitone();

        void SetWaveformType(WaveformType waveformType);
        void SetLevel(uint8_t level);
        void SetCent(int8_t cent);
        void SetSemitone(int8_t semitone);

    private:
        float GetSamplesPerCycle(uint8_t noteIndex);

        WaveformType waveformType_;
        uint8_t level_;
        int8_t cent_;
        int8_t semitone_;
};
