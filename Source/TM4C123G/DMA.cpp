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

#include "TM4C123G/DMA.h"
#include "inc/tm4c123gh6pm.h"

// See section 9.2.5 (pg 589) of the datasheet for DMA channel configuration information.  Note that it
// says "The control table can be located anywhere in system memory, but it must be contiguous and aligned
// on a 1024-byte boundary".  Looking at the TI's ARM C/C++ Compiler documentation I see that alignment can
// be done using #pragma DATA_ALIGN ().  The reason it's a structure of 256 uint32_t is because there are
// 32 primary and 32 alternate channels in the control structure and each channel has four 4 byte registers.
// See tables 9-3 and 9-4 on page 590 of the datasheet for more info on this.
#define CHANNEL_CONTROL_STRUCTURE_SIZE 256
#pragma DATA_ALIGN (1024)
uint32_t uDMAChannelControl[CHANNEL_CONTROL_STRUCTURE_SIZE];

void ConfigureDMAChannelMap(uint8_t channel, uint8_t encoding);

void InitializeDMA()
{
    static bool DMAInitialized = false;
    volatile uint32_t i;

    if(DMAInitialized)
    {
        return;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // From page 966 of the datasheet for configuring DMA for SSI:
    //
    //      "6. Optionally, configure the SSI module for uDMA use with the following steps:
    //       a. Configure a uDMA for SSI use. See "Micro Direct Memory Access (uDMA)" on page 585 for
    //          more information.


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Step 6a from above, page 585 is the beginning of chapter 9 (Micro DMA).  Section 9.3 contains
    // "Initialization and Configuration", this is on page 600 and contains the following:
    //
    // "Before the uDMA controller can be used, it must be enabled in the System Control block and in the
    // peripheral. The location of the channel control structure must also be programmed. The following
    // steps should be performed one time during system initialization:
    //
    // 1. Enable the uDMA clock using the RCGCDMA register (see page 342).
    //
    // 2. Enable the uDMA controller by setting the MASTEREN bit of the DMA Configuration (DMACFG)
    //    register.
    //
    // 3. Program the location of the channel control table by writing the base address of the table to the
    //    DMA Channel Control Base Pointer (DMACTLBASE) register. The base address must be
    //    aligned on a 1024-byte boundary.

    // From step 1 above: Enabling DMA is as simple as setting bit 0
    SYSCTL_RCGCDMA_R |= 0x1;

    while((SYSCTL_RCGCDMA_R  & 0x1) == 0) { }  // Wait for it to be set before configuring

    // From step 2 above: Simple, just set bit 0
    UDMA_CFG_R |= 0x1;

    // From step 3 above: Bits 10-31 of the DMA_CTL_BASE register need to be set to the channel control
    // base address.  We first clear bits 10-31 which corresponds to 0xFFFF.FC00.
    UDMA_CTLBASE_R &= ~0xFFFFFC00;
    UDMA_CTLBASE_R |= (uint32_t)uDMAChannelControl;

    // I don't believe the documentation states this, but I think it's probably a good idea to clear out the
    // uDMAChannelControl structure before configuring it.
    for(i = 0; i < CHANNEL_CONTROL_STRUCTURE_SIZE; ++i)
    {
        uDMAChannelControl[i] = 0;
    }

    DMAInitialized = true;
}

void InitializeDMAChannelAssignment(uint8_t channel, uint8_t encoding)
{
    volatile uint32_t i;

    InitializeDMA();

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // From page 966 of the datasheet for configuring DMA for SSI:
    //
    //      "6. Optionally, configure the SSI module for uDMA use with the following steps:
    //       a. Configure a uDMA for SSI use. See "Micro Direct Memory Access (uDMA)" on page 585 for
    //          more information.
    //       b. Enable the SSI Module's TX FIFO or RX FIFO by setting the TXDMAE or RXDMAE bit in the
    //          SSIDMACTL register."

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Before continuing on with the configuration, we need to understand what type of transfer mode is
    // needed and what configuration using that transfer mode entails.
    //
    // Note on page 591 of the datatsheet it states: "Ping-Pong mode is used to support a continuous
    // data flow to or from a peripheral".  Since audio is a continuous data flow, we want to use the
    // ping-pong transfer mode.
    //
    // The datasheet goes on to say: "To use Ping-Pong mode, both the primary and alternate data
    // structures must be implemented... The transfer is started using the primary control structure. When
    // the transfer using the primary control structure is complete, the uDMA controller reads the alternate
    // control structure for that channel to continue the transfer.  Each time this happens, an interrupt is
    // generated, and the processor can reload the control structure for the just-completed transfer. Data
    // flow can continue indefinitely this way, using the primary and alternate control structures to switch
    // back and forth between buffers as the data flows to or from the peripheral".
    //
    // So, based on that last paragraph, it sounds like we're going to need to use both the primary and
    // alternate control structures.  Also, it sounds like we need to setup an interrupt.  Checkout the
    // following from page 605 of the datasheet explaining an example of setting up ping-pong transfer
    // mode:
    //
    // "An interrupt handler should be configured when using uDMA Ping-Pong mode, it is best to use an
    // interrupt handler... The interrupt handler is triggered after each buffer is complete".
    //
    // So, to summarize what we've come to decide from this comment chunk is that we will be using
    // Ping-Pong mode for transferring data and both primary and alternate control structures off channel 11
    // (SSI0 TX) in the channel control structure and we'll need an interrupt to tell us when the control
    // structures need to be serviced.


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // See section 9.3.4 on pg 604 of the datasheet.  This shows an example of configuring for ping-pong
    // receive, we need ping pong transferring (not receiving) and will also be using channel 11 and not channel
    // 8 but I imagine most settings to be similar:
    //
    // "First, configure the channel attributes:
    //  1. Configure bit 8 of the DMA Channel Priority Set (DMAPRIOSET) or DMA Channel Priority
    //     Clear (DMAPRIOCLR) registers to set the channel to High priority or Default priority.
    //
    //  2. Set bit 8 of the DMA Channel Primary Alternate Clear (DMAALTCLR) register to select the
    //     primary channel control structure for this transfer.
    //
    //  3. Set bit 8 of the DMA Channel Useburst Clear (DMAUSEBURSTCLR) register to allow the
    //     uDMA controller to respond to single and burst requests.
    //
    //  4. Set bit 8 of the DMA Channel Request Mask Clear (DMAREQMASKCLR) register to allow
    //     the uDMA controller to recognize requests for this channel."


    // Step 1 from above: Configure the DMA channel priority.  Doing nothing should leave the priority low.

    // Step 2 above
    UDMA_ALTCLR_R |= (1 << channel);

    // Step 3 above
    UDMA_USEBURSTCLR_R |= (1 << channel);

    // Step 4 above
    UDMA_REQMASKCLR_R |= (1 << channel);

    // Interestingly enough it appears this step is missing from the documentation, see the function
    // below for more info.
    ConfigureDMAChannelMap(channel, encoding);
}

void ConfigureDMAChannelMap(uint8_t channel, uint8_t encoding)
{
    // The example steps appear to be missing a step: Configuring the channel assignments in the DMACHMAP register.
    // This is briefly mentioned in the datasheet in section 9.3.5 on page 606.  This is where we select the
    // encoding we want to use from table 9-1 on page 587.

    uint32_t dmaChannelMapAddress = (uint32_t)&UDMA_CHMAP0_R;
    if(channel >= 8 && channel <= 15) { dmaChannelMapAddress = (uint32_t)&UDMA_CHMAP1_R; }
    else if(channel >= 16 && channel <= 23) { dmaChannelMapAddress = (uint32_t)&UDMA_CHMAP2_R; }
    else if(channel >= 24 && channel <= 31) { dmaChannelMapAddress = (uint32_t)&UDMA_CHMAP3_R; }

    uint32_t bitShift = (channel % 8) * 4;

    // First clear the endcoding value...
    (*((volatile uint32_t *)dmaChannelMapAddress)) &= ~(0xF << bitShift);

    // ...then set it.
    if(encoding > 0)
    {
        (*((volatile uint32_t *)dmaChannelMapAddress)) |= (encoding << bitShift);
    }
}

void EnableDMAChannel(uint8_t channel)
{
    // From page 606 of the datasheet, the last step (aside from handling interrupts during processing) is to
    // "Enable the uDMA Channel" - "Enable the channel by setting bit 8 of the DMA Channel Enable Set (DMAENASET)
    // register" - Of course this is the example where they're using channel 8 and we're using channel 11.
    UDMA_ENASET_R |= (1 << channel);
}

uint32_t GetChannelControlStructureBaseAddress(uint32_t channel, enum ChannelType channelType)
{
    // See section 9.2.5 (pg 589) of the datasheet for how we calculate the baseAddress
    uint32_t baseAddress = (uint32_t)(uDMAChannelControl + (channel * 4));
    if(channelType == ALTERNATE)
    {
        baseAddress += 0x200;
    }

    return baseAddress;
}

void SetChannelControlStructure(uint32_t channel, enum ChannelType channelType, uint32_t channelControlWord)
{
    // See pg 607 for definitions of these registers
    const uint32_t DMACHCTL_OFFSET = 0x008;    // DMA Channel Control Word

    uint32_t baseAddress = GetChannelControlStructureBaseAddress(channel, channelType);

    // Note that bits 18-thru-23 are reserved, so we clear all bits in the register except 
    // for 18-thru-23 and then or-equal it with our given value.
    (*((volatile uint32_t *)(baseAddress + DMACHCTL_OFFSET))) &= ~0xFF03FFFF;
    (*((volatile uint32_t *)(baseAddress + DMACHCTL_OFFSET))) |= channelControlWord;
}

void SetChannelSourceEndAddress(uint32_t channel, enum ChannelType channelType, uint32_t sourceAddressValue)
{
    // See pg 607 for definitions of these registers
    const uint32_t DMASRCENDP_OFFSET = 0x000;  // DMA Channel Source Address End Pointer

    uint32_t baseAddress = GetChannelControlStructureBaseAddress(channel, channelType);

    (*((volatile uint32_t *)(baseAddress + DMASRCENDP_OFFSET))) = sourceAddressValue;
}

void SetChannelDestinationEndAddress(uint32_t channel, enum ChannelType channelType, uint32_t destinationAddressValue)
{
    // See pg 607 for definitions of these registers
    const uint32_t DMADSTENDP_OFFSET = 0x004;  // DMA Channel Destination Address End Pointer

    uint32_t baseAddress = GetChannelControlStructureBaseAddress(channel, channelType);

    (*((volatile uint32_t *)(baseAddress + DMADSTENDP_OFFSET))) = destinationAddressValue;
}

bool CheckForTranferCompletion(uint32_t channel, enum ChannelType channelType)
{
    const uint32_t DMACHCTL_OFFSET = 0x008;  // DMA Channel Control Word.  See pg 607.

    uint32_t baseAddress = GetChannelControlStructureBaseAddress(channel, channelType);

    uint32_t channelControlRegisterValue = (*((volatile uint32_t*)(baseAddress + DMACHCTL_OFFSET)));

    // From page 591: "At the end of a transfer, the transfer size indicates 0, and the transfer
    // mode indicates 'stopped'.  Because the control word is modified by the uDMA controller, it must be
    // reconfigured before each new transfer. The source and destination end pointers are not modified,
    // so they can be left unchanged if the source or destination addresses remain the same".
    if(((channelControlRegisterValue & 0x7) == 0) && ((channelControlRegisterValue & (0x3FF << 4)) == 0))
    {
        return true;
    }

    return false;
}

void ReSetChannelControlStructureModeAndSize(uint32_t channel, enum ChannelType channelType, uint8_t xFerMode, uint32_t xFerSize)
{
    // See pg 607 for definitions of these registers
    const uint32_t DMACHCTL_OFFSET = 0x008;    // DMA Channel Control Word

    uint32_t baseAddress = GetChannelControlStructureBaseAddress(channel, channelType);

    (*((volatile uint32_t *)(baseAddress + DMACHCTL_OFFSET))) |= xFerMode;  // Reset XFERMODE
    (*((volatile uint32_t *)(baseAddress + DMACHCTL_OFFSET))) |= ((xFerSize - 1) << 4);  // Reset XFERSIZE
}
