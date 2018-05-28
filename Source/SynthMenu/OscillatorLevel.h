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

#include "MenuSystem/MenuItemValue.h"
#include <stdint.h>

class Oscillator;

class OscillatorLevel : public MenuItemValue
{
    public:
        OscillatorLevel(Oscillator& oscillator);
        void Increment();
        void Decrement();
        const char* GetValueAsText();
        
    private:
        static const uint16_t MIN_VALUE_ = 0;
        static const uint16_t MAX_VALUE_ = 10;
        static const uint16_t DEFAULT_VALUE_ = 5;
        static const uint16_t ADJUSTMENT_VALUE_ = 1;
        static const uint8_t TEXT_LENGTH_ = 3;

        Oscillator& oscillator_;
        char text_[3];
};
