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

#include <TM4C123G/TimedInterrupt.h>
#include "inc/tm4c123gh6pm.h"

void Timer0AInit(uint32_t startPeriod)
{
    // See page 722 (section 11.4) of the datasheet for initialization and configuration steps.

    // Pre-Step: "To use a GPTM, the appropriate TIMERn bit must be set in the RCGCTIMER"
    SYSCTL_RCGCTIMER_R |= 0x01;

    // Step 1: "Ensure the timer is disabled"
    TIMER0_CTL_R = 0x00000000;

    // Step 2: "Write the GPTM Configuration Register (GPTMCFG) with a value of 0x0000.0000"
    TIMER0_CFG_R = 0x00000000;

    // Step 3: "Write a value of 0x2 for Periodic mode"
    TIMER0_TAMR_R = 0x00000002;

    // Step 4: Skipping - Don't need to capture values of the timer

    // Step 5: "Load the start value into the GPTM"
    TIMER0_TAILR_R = startPeriod;

    // Step 6: "Set the appropriate bits in the GPTM Interrupt Mask Register"
    TIMER0_IMR_R = 0x00000001;

    // Step 7: "Set the TnEN bit in the GPTMCTL register to enable the timer"
    TIMER0_CTL_R = 0x00000001;

    // Timer0A is interrupt #19.  Set the priority of this interrupt to a lower priority.
    NVIC_PRI4_R = (NVIC_PRI4_R & 0x00FFFFFF) | 0xA0000000;

    // Enable the Timer 0A interrupt.  See page 104 of the datasheet.  Timer 0A is interrupt #19.
    NVIC_EN0_R = 1 << 19;
}
