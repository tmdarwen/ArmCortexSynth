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

#include "AudioOutput/AudioOutput.h"
#include "AudioGeneration/AudioMixer.h"
#include "AudioGeneration/Oscillator.h"
#include "LCD/LCDOutput.h"
#include "MIDI/MIDIReceiver.h"
#include "SynthMenu/SynthMenu.h"
#include "SynthMenu/SynthMenuInput.h"
#include "TM4C123G/SystemControl.h"
#include "TM4C123G/TimedInterrupt.h"
#include "Utilities/Logger.h"
#include "Utilities/HelperFunctions.h"
#include <stdint.h>
#include "inc/tm4c123gh6pm.h"

uint64_t timerCounter = 0;

AudioMixer* pAudioMixer;

// This function gets called when AudioOutput needs new audio samples
void FillAudioBufferCallback(uint16_t* buffer, uint32_t bufferSampleSize)
{
    static uint32_t lastMIDICallCount = 0;
    volatile uint32_t i = 0;

    // Get the latest MIDI info to the Audio Mixer
    if(lastMIDICallCount != GetMIDICallCount())
    {
        lastMIDICallCount = GetMIDICallCount();
        pAudioMixer->SetMIDINote(GetMostRecentMIDIKey());
    }

    // Silent the audio buffer before adding audio to it
    for( ; i < bufferSampleSize; ++i)
    {
        buffer[i] = 0;
    }

    pAudioMixer->GetAudioData(buffer, bufferSampleSize);
}

LCDOutput* pLCDOutput = 0;
SynthMenuInput* pSynthMenuInput = 0;

extern "C" void Timer0AHandler(void)
{
    TIMER0_ICR_R = TIMER_ICR_TATOCINT;  // Clear the timer interrupt
    pLCDOutput->Process(timerCounter);
    pSynthMenuInput->Process();
    ++timerCounter;
}

void main()
{
    DisableInterrupts();  // Disable interrupts during configuration/initialization

    InitSystemControl();  // Setup system clock speed (80MHz)

    Logger::PrintNewLine();
    Logger::PrintStringWithNewLine("============================================================");
    Logger::PrintStringWithNewLine("ARM Cortex-M4F Synth");
    Logger::PrintStringWithNewLine("Version: x.xx");
    Logger::PrintStringWithNewLine("Build Number: xxx");
    Logger::PrintString("Build Date: ");
    Logger::PrintStringWithNewLine(__DATE__);
    Logger::PrintString("Build Time: ");
    Logger::PrintStringWithNewLine(__TIME__);

    Logger::PrintStringWithNewLine("Initializing Audio Mixer");
    AudioMixer audioMixer;
    pAudioMixer = &audioMixer;

    Logger::PrintStringWithNewLine("Initializing Oscillators");
    Oscillator oscillator1(Square, 10, 0, 0);
    Oscillator oscillator2(Square, 10, 8, 0);
    Oscillator oscillator3(Square, 10, -8, 0);

    audioMixer.SetOscillator1(&oscillator1);
    audioMixer.SetOscillator2(&oscillator2);
    audioMixer.SetOscillator3(&oscillator3);

    Oscillator* oscillators[3];
    oscillators[0] = &oscillator1;
    oscillators[1] = &oscillator2;
    oscillators[2] = &oscillator3;

    Logger::PrintStringWithNewLine("Initializing LCD Menu");
    LCDOutput lcdOutput;

    Logger::PrintStringWithNewLine("Initializing Synth Menu");
    SynthMenu synthMenu(&lcdOutput, oscillators);

    Logger::PrintStringWithNewLine("Initializing Synth Menu Inputs");
    SynthMenuInput synthMenuInput(&synthMenu);

    pLCDOutput = &lcdOutput;
    pSynthMenuInput = &synthMenuInput;

    Logger::PrintStringWithNewLine("Initializing LCD/Menu Timer");
    Timer0AInit();

    Logger::PrintStringWithNewLine("Initializing Audio Output");
    InitAudioOutput(FillAudioBufferCallback);

    Logger::PrintStringWithNewLine("Initializing MIDI Inputs");
    InitializeMIDIReceiver();

    Logger::PrintStringWithNewLine("Enabling Interrupts");
    EnableInterrupts();  // We're done with initialization, so enable interrupts

    while(1)
    {
       WaitForInterrupt();
    }
}
