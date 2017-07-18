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

#include "MenuSystem/MenuSystem.h"
#include "MenuSystem/Menu.h"
#include "MenuSystem/MenuItem.h"
#include "MenuSystem/MenuItemValue.h"
#include "SynthMenu/OscillatorCent.h"
#include "SynthMenu/OscillatorSemitone.h"
#include "SynthMenu/OscillatorLevel.h"
#include "SynthMenu/OscillatorType.h"
#include "AudioGeneration/Oscillator.h"

class SynthMenu
{
    public:
        SynthMenu(MenuOutput* menuOutput, Oscillator* oscillators[3]);
        void HandleAction(MenuSystem::Action action);

    private:
        MenuSystem menuSystem_;
        Menu mainMenu_;
        Menu oscillatorMenu_;
        Menu oscillator1Menu_;
        Menu oscillator2Menu_;
        Menu oscillator3Menu_;

        OscillatorType oscillator1Type_;
        OscillatorType oscillator2Type_;
        OscillatorType oscillator3Type_;

        OscillatorLevel oscillator1Level_;
        OscillatorLevel oscillator2Level_;
        OscillatorLevel oscillator3Level_;

        OscillatorCent oscillator1Cent_;
        OscillatorCent oscillator2Cent_;
        OscillatorCent oscillator3Cent_;

        OscillatorSemitone oscillator1Semitone_;
        OscillatorSemitone oscillator2Semitone_;
        OscillatorSemitone oscillator3Semitone_;
};
