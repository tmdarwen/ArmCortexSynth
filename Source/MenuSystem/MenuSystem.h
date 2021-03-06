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

class MenuOutput
{
    public:
        virtual ~MenuOutput() { }
        virtual void SetText(uint8_t line, const char* text) = 0;
};


class Menu;

class MenuSystem
{
    public:
        enum Action
        {
            UP,
            DOWN,
            BACK,
            ENTER
        };

        static const uint8_t MENU_ROWS = 4;
        static const uint8_t TEXT_SIZE = 20;
        static const uint8_t NULL_CHAR_SIZE = 1;

        MenuSystem();
        MenuSystem(Menu* mainMenu, MenuOutput* output);

        void SetMainMenu(Menu* mainMenu);
        void SetOutput(MenuOutput* output);
        void Reset();  // Resets screen to main menu
        void HandleAction(Action action);
        uint32_t GetRowCount();
    
    private:
        void Init();
        void UpdateText();

        Menu* mainMenu_;
        Menu* currentMenu_;
        uint8_t currentRow_;
        MenuOutput* output_;
        bool valueTweaking_;
};
