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

#include "LCD/LCD.h"
#include "inc/tm4c123gh6pm.h"

struct LCDActionRecorded
{
    LCDAction lcdAction_;
    uint64_t tick_;
};

LCD::LCD()
{
    InitPinsForDigitalOutput();
    DatasheetInit();
}

void LCD::ClearScreen()
{
    // See page 6 of the NHD-0420H1Z-FL-GBW-33V3  datasheet.  This clears the screen and
    // sets the cursor to "home".

    fifo_.Push(LCDAction(LCDAction::Command, 0x01));
    fifo_.Push(LCDAction(LCDAction::Wait, 123));  // 1.53 milliseconds = 1,530,000 / 12.5 = 122,400 ticks
    fifo_.Push(LCDAction(LCDAction::Command, 0x02));
    fifo_.Push(LCDAction(LCDAction::Wait, 123));  // 1.53 milliseconds = 1,530,000 / 12.5 = 122,400 ticks
}

void LCD::WriteText(const char* text)
{
    while(*text)
    {
        fifo_.Push(LCDAction(LCDAction::Write, *text));
        fifo_.Push(LCDAction(LCDAction::Wait, 4));  // See page 6 of the NewHaven datasheet.  The execution 
                                                    // time for a write is 43 microseconds (43,000 nanoseconds).
                                                    // So tick wise, 43000/12.5 = 3440.
        ++text;
    }
}

void LCD::WriteChar(uint8_t row, uint8_t column, char theChar)
{
    // To understand what we're doing here with "baseDDRAMAddress" see pg 10 of the
    // NHD-0420H1Z-FL-GBW-33V3 datasheet.
    char baseDDRAMAddress = 0;
    if(row == 1) baseDDRAMAddress = 0x40;
    else if(row == 2) baseDDRAMAddress = 0x14;
    else if(row == 3) baseDDRAMAddress = 0x54;

    // See pg 6 of the NHD-0420H1Z-FL-GBW-33V3 datasheet.  A command of 0x80 will allow
    // us to set the DDRAM address (the row/column of where the next character).
    char command = 0x80 + (baseDDRAMAddress + column);

    // 0010.0001 = 0x21  <-- Set DDRAM to row=0, column=1
    fifo_.Push(LCDAction(LCDAction::Command, command));
    fifo_.Push(LCDAction(LCDAction::Wait, 4));
    fifo_.Push(LCDAction(LCDAction::Write, theChar));
    fifo_.Push(LCDAction(LCDAction::Wait, 4));
}

void LCD::Process(uint64_t ticks)
{
    if(fifo_.GetCount() == 0)
    {
        return;
    }

    LCDAction action(fifo_.Peek());
    switch(action.GetType())
    {
        case LCDAction::Command:
            Command(action.GetValue());
            fifo_.Pop();
            lastExecutedActionTickCount_ = ticks;
            break;
        case LCDAction::Write:
            Write(action.GetValue());
            fifo_.Pop();
            lastExecutedActionTickCount_ = ticks;
            break;
        case LCDAction::Wait:
            if(static_cast<uint64_t>(action.GetValue()) < (ticks - lastExecutedActionTickCount_))
            {
                fifo_.Pop();
                lastExecutedActionTickCount_ = ticks;
            }
            break;
        case LCDAction::Empty:
            break;
    }    
}

void LCD::WriteTextBlocking(const char* text)
{
    while(*text)
    {
        Write(*text);
        ++text;

        // See page 6 of the NewHaven datasheet.  The execution time for a write is 43
        // microseconds (43,000 nanoseconds). So tick wise, 43000/12.5 = 3440.
        SpinDelay(3440);
    }
}

void LCD::ClearScreenBlocking()
{
    // See page 6 of the NHD-0420H1Z-FL-GBW-33V3  datasheet.  This clears the screen and
    // sets the cursor to "home".
    Command(0x01);
    SpinDelay(122400);  // 1.53 milliseconds = 1,530,000 / 12.5 = 122,400 ticks
    Command(0x02);
    SpinDelay(122400);  // 1.53 milliseconds = 1,530,000 / 12.5 = 122,400 ticks
}

void LCD::WriteCharBlocking(uint8_t row, uint8_t column, char theChar)
{
    // To understand what we're doing here with "rowTransformed" see pg 10 of the ST7066U
    // datasheet.  It shows that the data addresses are setup such as:
    // >>Line0<<
    // >>Line2<<
    // >>Line1<<
    // >>Line3<<
    char rowTransformed = row;
    if(row == 1) rowTransformed = 2;
    else if(row == 2) rowTransformed = 1;

    // See pg 6 of the NHD-0420H1Z-FL-GBW-33V3 datasheet.  A command of 0x80 will allow
    // us to set the DDRAM address (the row/column of where the next character).
    char command = 0x80 + (rowTransformed * 20 + column);
    Command(command);
    Write(theChar);
    SpinDelay(3440);
}

void LCD::InitPinsForDigitalOutput()
{
    // See page 3 of the NewHaven Display NHD-0420H1Z-FL-GBW-33V3 datasheet for the pin assignments.
    // I'm hooking the display to the TM4C123G using the following pins - Display pin on left, Tiva
    // pin on right: RS=PE1, R/W=PE2, E=PE3, DB0=PB0, DB1=PB1, DB2=PB2, DB3=PB3, DB4=PB4, DB5=PB5,
    // DB6=PB6, DB7=PB7.

    // See pg 656 of the TM4C123G datasheet for the following instructions.  We want to setup all the
    // GPIO pins (PE1-PE3 and PB0-PB7) to digital outputs.
    //
    // To configure the GPIO pins of a particular port, follow these steps:
    // 1. Enable the clock to the port by setting the appropriate bits in the RCGCGPIO register (see
    //    page 340). In addition, the SCGCGPIO and DCGCGPIO registers can be programmed in the
    //    same manner to enable clocking in Sleep and Deep-Sleep modes.
    // 2. Set the direction of the GPIO port pins by programming the GPIODIR register. A write of a 1
    //    indicates output and a write of a 0 indicates input.
    // 3. Configure the GPIOAFSEL register to program each bit as a GPIO or alternate pin. If an alternate
    //    pin is chosen for a bit, then the PMCx field must be programmed in the GPIOPCTL register for
    //    the specific peripheral required. There are also two registers, GPIOADCCTL and GPIODMACTL,
    //    which can be used to program a GPIO pin as a ADC or DMA trigger, respectively.
    // 4. Set the drive strength for each of the pins through the GPIODR2R, GPIODR4R, and GPIODR8R
    //    registers.
    // 5. Program each pad in the port to have either pull-up, pull-down, or open drain functionality through
    //    the GPIOPUR, GPIOPDR, GPIOODR register. Slew rate may also be programmed, if needed,
    //    through the GPIOSLR register.
    // 6. To enable GPIO pins as digital I/Os, set the appropriate DEN bit in the GPIODEN register. To
    //    enable GPIO pins to their analog function (if available), set the GPIOAMSEL bit in the
    //    GPIOAMSEL register.
    // 7. Program the GPIOIS, GPIOIBE, GPIOEV, and GPIOIM registers to configure the type, event,
    //    and mask of the interrupts for each port.
    //    Note: To prevent false interrupts, the following steps should be taken when re-configuring
    //    GPIO edge and interrupt sense registers:
    //        a. Mask the corresponding port by clearing the IME field in the GPIOIM register.
    //        b. Configure the IS field in the GPIOIS register and the IBE field in the GPIOIBE
    //           register.
    //        c. Clear the GPIORIS register.
    //        d. Unmask the port by setting the IME field in the GPIOIM register.
    // 8. Optionally, software can lock the configurations of the NMI and JTAG/SWD pins on the GPIO
    //    port pins, by setting the LOCK bits in the GPIOLOCK register.
    //    When the internal POR signal is asserted and until otherwise configured, all GPIO pins are configured
    //    to be undriven (tristate): GPIOAFSEL=0, GPIODEN=0, GPIOPDR=0, and GPIOPUR=0, except for
    //    the pins shown in Table 10-1 on page 650. Table 10-3 on page 657 shows all possible configurations
    //    of the GPIO pads and the control register settings required to achieve them. Table 10-4 on page 658
    //    shows how a rising edge interrupt is configured for pin 2 of a GPIO port.

    // Step 1: We want to enable ports E and B (so set bits 4 and 1 = 0001.0010 = 0x12)
    SYSCTL_RCGCGPIO_R |= 0x12;

    while((SYSCTL_PRGPIO_R  & 0x12) == 0) { }  // Wait for it to be set before configuring

    // Step 2: Set port E pins 1, 2 and 3 (bits 1, 2, 3) to output.  0000.1110 = 0x0E
    GPIO_PORTE_DIR_R |= 0x0E;

    // Step 2: Set all port B pins to output.
    GPIO_PORTB_DIR_R |= 0xFF;

    // Step 3: Skip - We're not using alternate functions for the GPIO pins here.

    // Step 4: Skip - As far as I know, the default drive strength is fine.

    // Step 5: Skip - As far as I know, the default is fine.

    // Step 6: Digital enable pins our port E pins (bits 1,2,3 = 0000.1110 binary = 0x0E)
    GPIO_PORTE_DEN_R |= 0x0E;

    // Step 6: Digital enable all pins on port B.
    GPIO_PORTB_DEN_R |= 0xFF;

    // Step 7: Skip - Not necessary

    // Step 8: Skip - Not necessary
}

void LCD::DatasheetInit()
{
    // The following comments come straight from the NewHaven Display
    // NHD-0420H1Z-FL-GBW-33V3 datasheet - page 10.

    // E = 0;
    // Delay(100);        // Wait >40 msec after power is applied
    // command(0x30);    // command 0x30 = Wake up
    // Delay(30);        // must wait 5ms, busy flag not available
    // command(0x30);    // command 0x30 = Wake up #2
    // Delay(10);        // must wait 160us, busy flag not available
    // command(0x30);    // command 0x30 = Wake up #3
    // Delay(10);        // must wait 160us, busy flag not available
    // command(0x38);    // Function set: 8-bit/2-line
    // command(0x10);    // Set cursor
    // command(0x0c);    // Display ON; Cursor ON
    // command(0x06);    // Entry mode set

    // So, going off of the comments above, we first want to set E low.
    // From comments above (see InitPinsForDigitalOutput) we know E=PE3.
    // See page 654 of the TM4C123 datasheet for determining how write addresses
    // work.  You'll see the mask goes from bits 2-9 corresponding to pins we
    // want to write to.  Since we just want to write to pin 3 this will be:
    // 00.0010.0000 = 0x020
    (*(volatile uint32_t *)((uint32_t)(GPIO_PORTE_DATA_BITS_R) + 0x020)) = 0x00;

    // Our processor is running at 80MHz.  This is 12.5 ns per tick.  We want to
    // wait for at least 40 milliseconds (40,000,000 ns).  This means we want to
    // wait 40,000,000 / 12.5 ticks = 3,200,000.
    SpinDelay(3200000);

    // Send the "wake up" command
    Command(0x30);

    // Delay for 5 milliseconds (5,000,000 ns).  This means 5,000,000 / 12.5 =
    // 400,000 ticks.
    SpinDelay(400000);

    // Send the "wake up" command
    Command(0x30);

    // Delay for 160 microseconds (160,000 ns).  This means 160,000 / 12.5 =
    // 12,800 ticks.
    SpinDelay(12800);

    // Send the "wake up" command
    Command(0x30);

    // Delay for 160 microseconds (160,000 ns).  This means 160,000 / 12.5 =
    // 12,800 ticks.
    SpinDelay(12800);

    // Then write the reminder of the initialization commands.  Note that the example code
    // doesn't show it, but on page 6 of the datasheet it shows how long the execution time
    // needs to be for each instruction.  This explains the 39 microsend displays below.
    Command(0x38);
    SpinDelay(3120);  // 39 micro delay = 39,000/12.5 = 3120 ticks
    Command(0x10);
    SpinDelay(3120);  // 39 micro delay = 39,000/12.5 = 3120 ticks
    Command(0x0C);
    Command(0x06);
    SpinDelay(3120);  // 39 micro delay = 39,000/12.5 = 3120 ticks

    // Let's clear the screen too.
    ClearScreenBlocking();
}

void LCD::Command(char i)
{
    // The code/comments below come straight from the NewHaven Display
    // NHD-0420H1Z-FL-GBW-33V3 datasheet - page 10.
    // P1 = i;        //put data on output Port
    // D_I = 0;        //D/I=LOW : send instruction  <-- I believe D_I corresponds to "RS"
    // R_W = 0;        //R/W=LOW : Write
    // E = 1;
    // Delay(1);    //enable pulse width >= 300ns
    // E = 0;        //Clock enable: falling edge

    // See page 654 of the TM4C123 datasheet for determining how write addresses
    // work.  We want to write an entire byte to port B.  The mask goes from
    // bit 2-9: 11.1111.1100 = 0x3FC.
    (*(volatile uint32_t *)((uint32_t)(GPIO_PORTB_DATA_BITS_R) + 0x3FC)) = i;

    // From comments above (see InitPinsForDigitalOutput), RS=PE1, R/W=PE2, E=PE3.
    // This corresponds to mask 00.0011.1000 which equals 0x038.  We want to set
    // RS low, R/W low and E high.  This corresponds to 0000.1000 binary = 0x08.
    (*(volatile uint32_t *)((uint32_t)(GPIO_PORTE_DATA_BITS_R) + 0x038)) = 0x08;

    // Wait for 300ns.  300 / 12.5 = 24 ticks
    SpinDelay(24);

    // Then set E low and leave R/W and RS low.  This corresponds to 0000.0000 binary = 0x00.
    (*(volatile uint32_t *)((uint32_t)(GPIO_PORTE_DATA_BITS_R) + 0x038)) = 0x00;

    // See page 7 of the NewHave datasheet.  The cycle time for writing from the MPU
    // to the LCD is 1200 ns.  1200 / 12.5 = 96 ticks
    SpinDelay(96);
}

void LCD::Write(char i)
{
    // The code/comments below come straight from the NewHaven Display
    // NHD-0420H1Z-FL-GBW-33V3 datasheet - page 10.
    // P1 = i;        //put data on output Port
    // D_I = 1;        //D/I=HIGH : send data  <-- I believe D_I corresponds to "RS"
    // R_W = 0;        //R/W=LOW : Write
    // E = 1;
    // Delay(1);    //enable pulse width >= 300ns
    // E = 0;        //Clock enable: falling edge

    // See page 654 of the TM4C123 datasheet for determining how write addresses
    // work.  We want to write an entire byte to port B.  The mask goes from
    // bit 2-9: 11.1111.1100 = 0x3FC.
    (*(volatile uint32_t *)((uint32_t)(GPIO_PORTB_DATA_BITS_R) + 0x3FC)) = i;

    // From comments above (see InitPinsForDigitalOutput), RS=PE1, R/W=PE2, E=PE3.
    // This corresponds to mask 00.0011.1000 which equals 0x038.  We want to set
    // RS high, R/W low and E high.  This corresponds to 0000.1010 binary = 0x0A.
    (*(volatile uint32_t *)((uint32_t)(GPIO_PORTE_DATA_BITS_R) + 0x038)) = 0x0A;

    // Wait for at least 300ns.  300 / 12.5 = 24 ticks
    SpinDelay(24);

    // Then set E low and leave RS high and R/W low.  This corresponds to 0000.0010 binary = 0x02.
    (*(volatile uint32_t *)((uint32_t)(GPIO_PORTE_DATA_BITS_R) + 0x038)) = 0x02;
}

void LCD::SpinDelay(uint32_t ticks)
{
    volatile uint32_t i(ticks);
    while(--i);
}
