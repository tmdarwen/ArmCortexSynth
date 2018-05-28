#pragma once

#include <MenuSystem/MenuSystem.h>

// This class allows easy access to the 4x20 text display of the menu
class SynthMenuOutput : public MenuOutput
{
    public:
        static const unsigned int ROWS_ = 4;
        static const unsigned int COLUMNS_ = 20;

        void SetText(uint8_t line, const char* text);
        const char* GetTextLine(unsigned char line);

    private:
        char text_[ROWS_][COLUMNS_];
};
