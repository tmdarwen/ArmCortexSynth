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

#include "UARTLogger/UARTLogger.h"
#include "inc/tm4c123gh6pm.h"

const char NEW_LINE[] = "\n\r";

// See "TM4C123 Tutorial: UART to PC Communication" at https://www.youtube.com/watch?v=Vy-k7u5djM4
// for a great tutorial of how this works.

UARTLogger::UARTLogger() : systemClockFrequency_(DEFAULT_SYSTEM_CLOCK_FREQUENCY_), baudRate_(DEFAULT_BAUD_RATE_)
{
    //Enable UART0.  See page 345.
    SYSCTL_RCGCUART_R |= 0x01;

    // Enable UART for port A.  See page 340.
    SYSCTL_RCGCGPIO_R |= 0x01;

    while((SYSCTL_PRGPIO_R  & 0x1) == 0) { }  // Wait for it to be set before configuring

    // See page 671.  Here we set the Alternate Function Select for GPIO pins AP0 and AP1.
    // See the table on page 1351.  These pins are used for UART0.
    GPIO_PORTA_AFSEL_R = (1<<1) | (1<<0);

    // See pg 688
    GPIO_PORTA_PCTL_R = (1<<4) | (1<<0);

    // Digital enable the pins
    GPIO_PORTA_DEN_R = (1<<1) | (1<<0);

    // Disable the UART by clearing the bit
    UART0_CTL_R &= ~0x01;

    // Pg 914: This is the "integer baud rate divisor".  The IBRD register holds the integer
    // portion of the baud rate divisor and FBRD holds the fractional part of the baud rate
    // divisor.  Page 896 has more on the configuration including this calculation:
    //     BRD = BRDI + BRDF = UARTSysClk / (ClkDiv * Baud Rate)
    // So, to calculate the BRDI we use the integer portion of the following equation and
    // and we use the modulo portion (plus some math - see below) for FBRD:
    //     BaudRateDivisor = UARTSysClk / (16 * BaudRate)
    // Page 896 continues to say: "The 6-bit fractional number (that is to be loaded into the
    // DIVFRAC bit field in the UARTFBRD register) can be calculated by taking the fractional
    // part of the baud-rate divisor, multiplying it by 64, and adding 0.5 to account for
    // rounding errors:
    //    UARTFBRD[DIVFRAC] = integer(BRDF * 64 + 0.5) = 0
    uint32_t brd = systemClockFrequency_ / (16 * baudRate_);
    UART0_IBRD_R = brd;
    UART0_FBRD_R = static_cast<uint32_t>(((static_cast<float>(systemClockFrequency_) / (16.0 * baudRate_)) - brd) * 64.0 + 0.5);

    // See pg 916
    UART0_LCRH_R = (0x3<<5);

    // Source for the UART clock will be the system clock
    UART0_CC_R = 0x0;

    // See pg 918.  Here we enable UART1 and also enable it for transmission (bit 8) and recieve (bit 9)
    // these three bits must all be set at once.s
    UART0_CTL_R = ((1<<0) | (1<<8) | (1<<9));
}

void UARTLogger::TransmitCharacter(char dataChar)
{
	// See pg 911, bit 5 - we add data into the transmit FIFO as long as it's not full
    while((UART0_FR_R & (1<<5)) != 0) { }
    UART0_DR_R = dataChar;
}

void UARTLogger::PrintString(const char* string)
{
	// Keep sending characters until we hit a NULL
	while(*string)
	{
		TransmitCharacter(*(string++));
	}
}

void UARTLogger::PrintStringWithNewLine(const char* string)
{
    PrintString(string);
    PrintString(NEW_LINE);
}

void UARTLogger::PrintNewLine()
{
    PrintString(NEW_LINE);
}
