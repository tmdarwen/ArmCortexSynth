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

#include "Utilities/FIFO.h"
#include <stdint.h>

class LCDAction
{
	public:
		enum Type { Empty, Command, Write, Wait};
		LCDAction() : type_(Empty), value_(0) { }
		LCDAction(Type type, int16_t value) : type_(type), value_(value) { }

		Type GetType() { return type_; }
		int16_t GetValue() { return value_; }

	private:
		Type type_;
		int16_t value_; // Holds the command, the write char or the delay amount
};

class LCD
{
	public:
		LCD();

		void ClearScreen();
		void WriteText(const char*);
		void WriteChar(uint8_t row, uint8_t column, char theChar);

        void ClearScreenBlocking();
        void WriteTextBlocking(const char*);
        void WriteCharBlocking(uint8_t row, uint8_t column, char theChar);

		void Process(uint64_t ticks);

	private:
		void InitSysTick();
		void InitPinsForDigitalOutput();
		void DatasheetInit();

		void Command(char i);
		void Write(char i);
		
		void SpinDelay(uint32_t ticks);

		static const uint16_t FIFO_MAX_SIZE_ = 256;
		FIFO<LCDAction, FIFO_MAX_SIZE_> fifo_;

		uint64_t lastExecutedActionTickCount_;
};
