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

#include "MenuSystem/MenuSystem.h"
#include "MenuSystem/Menu.h"
#include "MenuSystem/MenuItemValue.h"
#include "Utilities/StringUtilities.h"
#include <stddef.h>

MenuSystem::MenuSystem() :
    mainMenu_(0),
    currentMenu_(0),
    currentRow_(0),
    output_(0),
    valueTweaking_(false) { }

MenuSystem::MenuSystem(Menu* mainMenu, MenuOutput* output) :
    mainMenu_(mainMenu),
    output_(output),
	currentMenu_(mainMenu_),
	currentRow_(0),
    valueTweaking_(false) { }

// Resets screen to main menu
void MenuSystem::Reset()
{
	currentMenu_ = mainMenu_;
	currentRow_ = 0;
	valueTweaking_ = false;

	UpdateText();
}

void MenuSystem::SetMainMenu(Menu* mainMenu)
{
	mainMenu_ = mainMenu;	
}

void MenuSystem::SetOutput(MenuOutput* output)
{
	output_ = output;	
}
	
uint32_t MenuSystem::GetRowCount()
{
	return currentMenu_->GetRowCount();	
}
	
void MenuSystem::UpdateText()
{
    unsigned int currentRow = 0;
	for( ; currentRow < currentMenu_->GetRowCount(); ++currentRow)
	{
		char text[TEXT_SIZE + NULL_CHAR_SIZE];
		text[0] = '\0';
        if(currentRow_ == currentRow) StringCat("> ", text, TEXT_SIZE);
        else StringCat("  ", text, TEXT_SIZE);

        // Copy the menu item text
        StringCopy(currentMenu_->GetMenuItemText(currentRow), &(text[2]), 18);

        // Copy the menu item's value (if it exists)
        MenuItemValue* menuItemValue = currentMenu_->GetItem(currentRow).GetMenuItemValue();
        if(menuItemValue)
        {
            StringCat(": ", text, TEXT_SIZE);
            StringCat(menuItemValue->GetValueAsText(), text, TEXT_SIZE);
        }

        output_->SetText(currentRow, text);
	}

	// Set any remaining rows to empty
	while(currentRow < MENU_ROWS)
	{
	    output_->SetText(currentRow, "");
	    ++currentRow;
	}
}

// TODO: Refactor using command pattern?
void MenuSystem::HandleAction(MenuSystem::Action action)
{
	if(action == DOWN)
	{
		if(valueTweaking_)
		{
			currentMenu_->GetItem(currentRow_).Decrement();
			UpdateText();
		}
		else if(currentRow_ + 1 < currentMenu_->GetRowCount())
		{
			++currentRow_;	
			UpdateText();
		}
	}
	else if(action == UP)
	{
		if(valueTweaking_)
		{
			currentMenu_->GetItem(currentRow_).Increment();
            UpdateText();
		}
		else if(currentRow_ > 0)
		{
			--currentRow_;	
			UpdateText();
		}
	}
	else if(action == ENTER)
	{
		if(currentMenu_->GetItem(currentRow_).GetSubMenu())
		{
			currentMenu_ = currentMenu_->GetItem(currentRow_).GetSubMenu();
			currentRow_ = 0;
			UpdateText();
		}
		else if(valueTweaking_)
		{
			valueTweaking_ = false;
		}
		else
		{
			valueTweaking_ = true;
		}
	}
	else if(action == BACK)
	{
		if(valueTweaking_)
		{
			valueTweaking_ = false;	
		}
		else if(currentMenu_->GetParentMenu())
		{
			currentMenu_ = currentMenu_->GetParentMenu();
			currentRow_ = 0;
			UpdateText();
		}
	}
}
