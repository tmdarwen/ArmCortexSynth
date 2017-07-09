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

#include "SynthMenu/SynthMenu.h"

SynthMenu::SynthMenu(MenuOutput* menuOutput, Oscillator* oscillators[3]) :
    menuSystem_(&mainMenu_, menuOutput),
    oscillator1Type_(oscillators[0]), oscillator1Level_(oscillators[0]), oscillator1Cent_(oscillators[0]), oscillator1Semitone_(oscillators[0]),
    oscillator2Type_(oscillators[1]), oscillator2Level_(oscillators[1]), oscillator2Cent_(oscillators[1]), oscillator2Semitone_(oscillators[1]),
    oscillator3Type_(oscillators[2]), oscillator3Level_(oscillators[2]), oscillator3Cent_(oscillators[2]), oscillator3Semitone_(oscillators[2])
{
    oscillator1Menu_.AddItem(MenuItem("Waveform", &oscillator1Type_));
    oscillator1Menu_.AddItem(MenuItem("Level", &oscillator1Level_));
    oscillator1Menu_.AddItem(MenuItem("Cent", &oscillator1Cent_));
    oscillator1Menu_.AddItem(MenuItem("Semitone", &oscillator1Semitone_));

    oscillator2Menu_.AddItem(MenuItem("Waveform", &oscillator2Type_));
    oscillator2Menu_.AddItem(MenuItem("Level", &oscillator2Level_));
    oscillator2Menu_.AddItem(MenuItem("Cent", &oscillator2Cent_));
    oscillator2Menu_.AddItem(MenuItem("Semitone", &oscillator2Semitone_));

    oscillator3Menu_.AddItem(MenuItem("Waveform", &oscillator3Type_));
    oscillator3Menu_.AddItem(MenuItem("Level", &oscillator3Level_));
    oscillator3Menu_.AddItem(MenuItem("Cent", &oscillator3Cent_));
    oscillator3Menu_.AddItem(MenuItem("Semitone", &oscillator3Semitone_));

    mainMenu_.AddItem(MenuItem("Oscillator 1", &oscillator1Menu_));
    mainMenu_.AddItem(MenuItem("Oscillator 2", &oscillator2Menu_));
    mainMenu_.AddItem(MenuItem("Oscillator 3", &oscillator3Menu_));

    menuSystem_.Reset();
}

void SynthMenu::HandleAction(MenuSystem::Action  action)
{
    menuSystem_.HandleAction(action);
}

