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

#include "MenuSystem/MenuItem.h"
#include "MenuSystem/MenuItemValue.h"
#include "Utilities/StringUtilities.h"
#include <stddef.h>

MenuItem::MenuItem() : subMenu_(0), menuItemValue_(0)
{
    text_[0] = '\0';
}

MenuItem::MenuItem(const char* text, Menu* subMenu) : subMenu_(subMenu), menuItemValue_(0)
{
    StringCopy(text, text_, TEXT_SIZE);
}

MenuItem::MenuItem(const char* text, MenuItemValue* menuItemValue) : menuItemValue_(menuItemValue), subMenu_(0)
{
    StringCopy(text, text_, TEXT_SIZE);
}

const char* MenuItem::GetText()
{
    return text_;    
}

void MenuItem::Increment()
{
    if(menuItemValue_)
    {
        menuItemValue_->Increment();    
    }
}

void MenuItem::Decrement()
{
    if(menuItemValue_)
    {
        menuItemValue_->Decrement();    
    }
}

Menu* MenuItem::GetSubMenu()
{
    return subMenu_;    
}

MenuItemValue* MenuItem::GetMenuItemValue()
{
    return menuItemValue_;
}
