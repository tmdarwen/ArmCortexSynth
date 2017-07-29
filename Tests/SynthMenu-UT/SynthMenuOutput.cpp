#include <SynthMenu-UT/SynthMenuOutput.h>
#include <Utilities/StringUtilities.h>
#include <cstddef> // For NULL

void SynthMenuOutput::SetText(uint8_t line, const char* text)
{
	StringCopy(text, text_[line], COLUMNS_);
}

const char* SynthMenuOutput::GetTextLine(unsigned char line)
{
	if(line >= ROWS_)
	{
		return NULL;
	}

	return text_[line];
}
