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

#include "MIDI/MIDIReceiver.h"
#include "TM4C123G/DMA.h"
#include "inc/tm4c123gh6pm.h"
#include <stdint.h>

uint32_t MIDINoteMapping[128];
uint32_t callCount = 0;

void ConfigureMIDIInputDMA();
void HandleMIDIMessage(uint8_t midiMessage);

uint8_t primaryMIDIByte = 0;
uint8_t alternateMIDIByte = 0;

void ClearMIDINoteMapping()
{
    volatile uint8_t i = 0;

    for( ; i < 128; ++i)
    {
        MIDINoteMapping[i] = 0;
    }
}

void InitializeMIDIReceiver()
{
    ClearMIDINoteMapping();

    /////////////////////////////////////////////////////////////////////////////////////////////////
    // See initialization and configuration steps on page 902.  These are listed below as we
    // step through them.

    // Step 1: "Enable the UART (UART4) module using the RCGCUART register"
    SYSCTL_RCGCUART_R |= (1<<4);

    // Step 2: "Enable the clock to the appropriate GPIO module via the RCGCGPIO register"
    SYSCTL_RCGCGPIO_R |= (1<<2);

    while((SYSCTL_PRGPIO_R & (1<<2)) == 0) { }  // Wait for it to be set before configuring

    // Step 3: "Set the GPIO AFSEL bits for the appropriate pins (see page 671). To determine
    // which GPIOs to configure, see Table 23-4 on page 1344".
    GPIO_PORTC_AFSEL_R |= ((1<<4) | (1<<5));

    // Step 4: Skipping - I don't think we care about the current level or slew rate.

    // Step 5: "Configure the PMCn fields in the GPIOPCTL register to assign the UART signals
    // to the appropriate pins (see page 688 and Table 23-5 on page 1351)".  Note that
    // U4Rx = PC4 and U4Tx = PC5 and the digital function for these is "1".
    GPIO_PORTC_PCTL_R |= ((1<<16) | (1<<20));

    // The instructions don't specifically state this, but it appears we also need to digitally
    // enable the pins as well.
    GPIO_PORTC_DEN_R |= ((1<<4) | (1<<5));


    /////////////////////////////////////////////////////////////////////////////////////////////////
    // For configuring the UART, see initialization and configuration steps on page 903.

    // Step 1: "Disable the UART by clearing the UARTEN bit in the UARTCTL register"
    UART4_CTL_R &= ~0x01;

    // Steps 2 and 3: Set the baud rate for the UART transfer
    // See pg 914 for IBRD register info
    // See pg 915 for FBRD register info
    // See pg 896 for baud rate calculations
    // Google tells me the baud for MIDI is 31250
    // BRD = 16,000,000 / (16 * 31250) = 32.0
    // BRD = 80,000,000 / (16 * 31250) = 160.0
    UART4_IBRD_R = 160;
    UART4_FBRD_R = 0;  // Fraction part is zero

    // Step 4: "Write the desired serial parameters to the UARTLCRH register"
    // See pg 916 for details of this register.  All we do here is set the UART word length
    // to 8 bits (0x3).
    UART4_LCRH_R = (0x3<<5);

    // Step 5: "Configure the UART clock source by writing to the UARTCC register"
    // We're using the system clock.
    UART4_CC_R &= ~0x0F;

    // Step 6: "Configure the uDMA channel"
    ConfigureMIDIInputDMA();

    // Step 7: "Enable the UART by setting the UARTEN bit in the UARTCTL register"
    // See pg 918.  Setting bit zero enables the UART.  Setting bit 9 enables it for recieving (bit 9).
    UART4_CTL_R = ((1<<0) | (1<<9));

    // Enable UART4 DMA Receive
    UART4_DMACTL_R |= 0x1;

    // See page 142 and the table on page 104.  We want to enable interrupt 60 (UART4) in NVIC.
    // According to the documentation on page 142 bit 31 on EN1 corresponds to interrupt 63 so
    // bit 28 must correspond to interrupt 60.
    NVIC_EN1_R |= (1 << 28);
}


void ConfigureMIDIInputDMA()
{
    // See table 9-1 on page 587.  We're using channel 18, encoding two (this is UART4 RX).
    InitializeDMAChannelAssignment(18, 2);

    // Not missing MIDI notes is a must, so I'm setting the priority to high.
    UDMA_PRIOSET_R |= (1 << 18);

    // See page 611 for details on the DMACHCTL register
    //
    // Note the example in the datasheet that receives ping-pong data uses the following settings:
    // "Table 9-10. Channel Control Word Configuration for Peripheral Transmit Example"
    //
    // Field in DMACHCTL    Bits     Value         Description
    // -----------------------------------------------------------------------
    // DSTINC              31:30       3           Destination address does not increment
    // DSTSIZE             29:28       0           8-bit destination data size
    // SRCINC              27:26       0           8-bit source address increment
    // SRCSIZE             25:24       0           8-bit source data size
    // reserved            23:18       0           Reserved
    // ARBSIZE             17:14       2           Arbitrates after 4 transfers
    // XFERSIZE             13:4      63           Transfer 64 items
    // NXTUSEBURST             3       0           N/A for this transfer type
    // XFERMODE              2:0       1           Use Basic transfer mode
    //
    // We need to do some settings a little different since we're not doing the exact same
    // things as the example, but it's a nice reference to figure out what these fields mean.
    // The following is the configuration I think we need:
    //
    // Field in DMACHCTL    Bits     Value         Description
    // -----------------------------------------------------------------------
    // DSTINC              31:30       3           No increment necessary on destination - just writing 1 byte
    // DSTSIZE             29:28       0           8-bit destination data size
    // SRCINC              27:26       3           No increment on the source (i.e. the MIDI device)
    // SRCSIZE             25:24       0           8-bit source data size
    // reserved            23:18       0           Reserved
    // ARBSIZE             17:14       0           Arbitrates after 1 transfer
    // XFERSIZE             13:4       0           Transfer 1 item
    // NXTUSEBURST             3       0           N/A for this transfer type
    // XFERMODE              2:0       3           Use ping pong transfer mode
    //

    uint32_t channelControlWord = 0;

    // Configure Ping Pong XFERMODE
    channelControlWord |= 0x03;

    // Not using NXTBURST - Do nothing

    // XFERSIZE - Setting this to 1 byte (a single MIDI message is one byte).  The value
    // for the register is zero, so do nothing.

    // ARBSIZE - Set to zero

    // SRCSIZE - Source size is a single byte (i.e. a single MIDI message).  Again, the
    // value for the register is zero.

    // SRCINC - No source increment
    channelControlWord |= (0x3 << 26);

    // SRCSIZE - Source size is a single byte (i.e. a single MIDI message).  Again, the
    // value for the register is zero.

    // DSTINC - No destination increment
    channelControlWord |= 0xC0000000;


    SetChannelControlStructure(18, PRIMARY, channelControlWord);
    SetChannelControlStructure(18, ALTERNATE, channelControlWord);

    SetChannelSourceEndAddress(18, PRIMARY, (uint32_t)(&UART4_DR_R));
    SetChannelSourceEndAddress(18, ALTERNATE,  (uint32_t)(&UART4_DR_R));

    SetChannelDestinationEndAddress(18, PRIMARY, (uint32_t)(&primaryMIDIByte));
    SetChannelDestinationEndAddress(18, ALTERNATE,  (uint32_t)(&alternateMIDIByte));

    EnableDMAChannel(18);
}

extern "C" void MIDIReceiverCallback()
{
    if(CheckForTranferCompletion(18, PRIMARY))
    {
        HandleMIDIMessage(primaryMIDIByte);
        ReSetChannelControlStructureModeAndSize(18, PRIMARY, 0x03, 1);
    }

    if(CheckForTranferCompletion(18, ALTERNATE))
    {
        HandleMIDIMessage(alternateMIDIByte);
        ReSetChannelControlStructureModeAndSize(18, ALTERNATE, 0x03, 1);
    }

    UDMA_CHIS_R &= ~(1 << 18);  // Clear the interrupt
}

// Returns zero if no key was pressed
int GetMostRecentMIDIKey()
{
    volatile uint8_t i = 0;
    uint32_t maxCallCount = 0;
    uint32_t lastKeyPressed = 0;

    for( ; i < 128; ++i)
    {
        if(MIDINoteMapping[i] > maxCallCount)
        {
            lastKeyPressed = i;
            maxCallCount = MIDINoteMapping[i];
        }
    }

    return lastKeyPressed;
}

uint32_t GetMIDICallCount()
{
    return callCount;
}

// Check out a MIDI specification to understand the commands and the sequence of commands.
// Just search for "MIDI specification" and you will find a lot of great online resources.
void HandleMIDIMessage(uint8_t midiMessage)
{
    static bool noteOn = false;
    static bool noteOff = false;
    static uint8_t theNote = 0;
    static uint8_t messagesAfterNoteCommand = 0;

    // Ignore typical MIDI heartbeat/status messages
    if(midiMessage == 0xFF || midiMessage == 0xFE)
    {
        return;
    }

    if(noteOn == true || noteOff == true)
    {
        messagesAfterNoteCommand++;
        if(messagesAfterNoteCommand == 1)
        {
            theNote = midiMessage;
        }
        else if(messagesAfterNoteCommand == 2)
        {
            // A velocity of 0 means the note is off
            if(noteOff == true || midiMessage == 0x00)
            {
                ++callCount;
                MIDINoteMapping[theNote] = 0;
            }
            else if(noteOn == true)
            {
                ++callCount;
                MIDINoteMapping[theNote] = callCount;
            }

            noteOn = false;
            noteOff = false;
            messagesAfterNoteCommand = 0;
        }
    }
    else
    {
        if((midiMessage & 0xF0) == 0x90)
        {
            noteOn = true;
            noteOff = false;
            messagesAfterNoteCommand = 0;
        }
        else if((midiMessage & 0xF0) == 0x80)
        {
            noteOn = false;
            noteOff = true;
            messagesAfterNoteCommand = 0;
        }
    }
}
