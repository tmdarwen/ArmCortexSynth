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

class SynthMenu;

class SynthMenuInput
{
    public:
        SynthMenuInput(SynthMenu*);
        void Process();

    private:
        SynthMenu* synthMenu_;

        const static uint8_t BUTTON_NOT_SET_ = 0x00;  // No button was pressed
        const static uint8_t BUTTON_ENTER_   = 0x01;  // PD0 (0000.0001) = 0x01
        const static uint8_t BUTTON_BACK_    = 0x02;  // PD1 (0000.0010) = 0x02
        const static uint8_t BUTTON_UP_      = 0x04;  // PD2 (0000.0100) = 0x04
        const static uint8_t BUTTON_DOWN_    = 0x08;  // PD3 (0000.1000) = 0x08

};
