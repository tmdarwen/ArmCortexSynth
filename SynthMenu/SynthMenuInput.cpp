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

#include "SynthMenu/SynthMenuInput.h"
#include "SynthMenu/SynthMenu.h"
#include "inc/tm4c123gh6pm.h"
#include "Utilities/HelperFunctions.h"

extern "C" void PortDInterrupt(void);

extern uint64_t timerCounter;
char buttonsPressed;
uint32_t interruptCount = 0;

void PortDInterrupt()
{
    static uint64_t lastTimerCounter = 0;
    static bool buttonDown = false;

    ++interruptCount;

    // Protecting against button bouncing here.
    if(timerCounter - 200 < lastTimerCounter)
    {
        GPIO_PORTD_ICR_R |= 0x0F;
        return;
    }

    if(buttonDown == false)
    {
        buttonsPressed = GPIO_PORTD_RIS_R;
        buttonDown = true;
    }
    else if(buttonDown == true)
    {
        buttonDown = false;
    }
    GPIO_PORTD_ICR_R |= 0x0F; // Clear the interrupt

    lastTimerCounter = timerCounter;
}

SynthMenuInput::SynthMenuInput(SynthMenu* synthMenu) : synthMenu_(synthMenu)
{
    // Step 1: We want to enable port D (so set bit 0 = 0000.1000 = 0x08)
    SYSCTL_RCGCGPIO_R |= 0x08;

    while((SYSCTL_PRGPIO_R  & 0x08) == 0) { }  // Wait for it to be set before configuring

    // Step 2: Set PD0-PD3 to input
    GPIO_PORTD_DIR_R &= ~0x0F;

    // Digitally enable the pins
    GPIO_PORTD_DEN_R |= 0x0F;

    GPIO_PORTD_IM_R &= ~0x0F;    // Disable the interrupt while we configure it
    GPIO_PORTD_IS_R &= ~0x0F;    // Detect edges
    GPIO_PORTD_IBE_R |= 0x0F;    // Use the GPIO Interrupt Event register for edge configuration
    GPIO_PORTD_IEV_R |= 0x0F;    // A rising edge triggers the interrupt
    GPIO_PORTD_PDR_R |= 0x0F;
    NVIC_PRI0_R = (NVIC_PRI0_R & 0x00FFFFFF) | 0xA0000000;  // Set port D interrupt to priority level 5
    GPIO_PORTD_IM_R |= 0x0F;   // We're done configuring, so unmask the interrupt

    // Enable the Port D interrupt
    NVIC_EN0_R |= 1 << 3;
}

void SynthMenuInput::Process()
{
    if(buttonsPressed == BUTTON_NOT_SET_)   return;
    else if(buttonsPressed & BUTTON_BACK_)  synthMenu_->HandleAction(MenuSystem::BACK);
    else if(buttonsPressed & BUTTON_UP_)    synthMenu_->HandleAction(MenuSystem::UP);
    else if(buttonsPressed & BUTTON_DOWN_)  synthMenu_->HandleAction(MenuSystem::DOWN);
    else if(buttonsPressed & BUTTON_ENTER_) synthMenu_->HandleAction(MenuSystem::ENTER);

    buttonsPressed = BUTTON_NOT_SET_; // Clear the button status since we're already read it
}
