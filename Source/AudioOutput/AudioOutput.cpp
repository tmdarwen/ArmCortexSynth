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

#include "AudioOutput/AudioOutput.h"
#include "TM4C123G/DMA.h"
#include "inc/tm4c123gh6pm.h"

#define AUDIO_BUFFER_SIZE_IN_SAMPLES  256

extern "C" void SSI0InterruptHandler();

uint16_t primaryAudioBuffer[AUDIO_BUFFER_SIZE_IN_SAMPLES];
uint16_t alternateAudioBuffer[AUDIO_BUFFER_SIZE_IN_SAMPLES];

// The callback function for getting audio data
void (*fillAudioBufferCallback)(uint16_t* buffer, uint32_t bufferSampleSize);

// Function prototypes
void InitSSI0();
void ConfigureAudioOutputDMA();
void ConfigureChannelControlStructure(uint32_t channel, enum ChannelType channelType);

void InitAudioOutput(void (*callback)(uint16_t* buffer, uint32_t bufferSampleSize))
{
	fillAudioBufferCallback = callback;
	InitSSI0();
}

// We're going to use SSI0 for communication with the MAX541 DAC.  SSI0 corresponds to pins
// PA2-thru-PA5 (see page 1351):
//    PA2: SSI0Clk
//    PA3: SSI0Fss
//    PA4: SSI0Rx <-- Don't need, we're only transferring audio data, not receiving it.
//    PA5: SSI0Tx
void InitSSI0()
{
	// From pg 965 of the datasheet on init and config of SSI:
	// "To enable and initialize the SSI, the following steps are necessary:
	//
	//  1. Enable the SSI module using the RCGCSSI register (see page 346).
	//
	//  2. Enable the clock to the appropriate GPIO module via the RCGCGPIO register (see page 340).
	//     To find out which GPIO port to enable, refer to Table 23-5 on page 1351.
	//
	//  3. Set the GPIO AFSEL bits for the appropriate pins (see page 671). To determine which GPIOs to
	//     configure, see Table 23-4 on page 1344.
	//
	//  4. Configure the PMCn fields in the GPIOPCTL register to assign the SSI signals to the appropriate
	//     pins. See page 688 and Table 23-5 on page 1351.
	//
	//  5. Program the GPIODEN register to enable the pin's digital function. In addition, the drive strength,
	//     drain select and pull-up/pull-down functions must be configured. Refer to “General-Purpose
	//     Input/Outputs (GPIOs)” on page 649 for more information.
	//     Note: Pull-ups can be used to avoid unnecessary toggles on the SSI pins, which can take the
	//           slave to a wrong state. In addition, if the SSIClk signal is programmed to steady state
	//           High through the SPO bit in the SSICR0 register, then software must also configure the
	//           GPIO port pin corresponding to the SSInClk signal as a pull-up in the GPIO Pull-Up
	//           Select (GPIOPUR) register"

	// Step 1 from above: Simple, just enable SSI0 which is setting bit 0 to 1
	SYSCTL_RCGCSSI_R |= 1;

	// Step 2 from above: Simple, just enable PortA which is setting bit 0 to 1
	SYSCTL_RCGCGPIO_R |= 1;

	while((SYSCTL_PRGPIO_R  & 0x1) == 0) { }  // Wait until it's set to configure

	// Step 3 from above: We want to use the alternate functions for pins 2, 3 and 5.
	// This corresponds to binary bits 101100 which is hex 0x2C.
	GPIO_PORTA_AFSEL_R |= 0x2C;

	// Step 4 from above: See pages 688 and 1351.  The table on page 1351 shows GPIOPCTL bit field for
	// SSI0 is 2.  So the PCTL is for pins 2, 3 and 5 need to be set to 0x2.  This corresponds to setting
	// the following bits: 0x00202200
	GPIO_PORTA_PCTL_R |= 0x00202200;

	// Step 5 from above: See pg 649, we want to digitally enable pins 2, 3 and 5.
	// This corresponds to binary bits 101100 which is hex 0x2C.
	GPIO_PORTA_DEN_R |= 0x2C;

	// Page 965 of the datasheet - on init and config of SSI - goes on to say:
	//
	// 	"For each of the frame formats, the SSI is configured using the following steps:
	//
	// 	 1. Ensure that the SSE bit in the SSICR1 register is clear before making any configuration changes.
	//
	// 	 2. Select whether the SSI is a master or slave:
	// 	     a. For master operations, set the SSICR1 register to 0x0000.0000.
	// 	     b. For slave mode (output enabled), set the SSICR1 register to 0x0000.0004.
	// 	     c. For slave mode (output disabled), set the SSICR1 register to 0x0000.000C.
	//
	// 	 3. Configure the SSI clock source by writing to the SSICC register.
	//
	// 	 4. Configure the clock prescale divisor by writing the SSICPSR register.
	//
	// 	 5. Write the SSICR0 register with the following configuration:
	// 	     - Serial clock rate (SCR)
	// 	     - Desired clock phase/polarity, if using Freescale SPI mode (SPH and SPO)
	// 	     - The protocol mode: Freescale SPI, TI SSF, MICROWIRE (FRF)
	// 	     - The data size (DSS)
	//
	// 	 6. Optionally, configure the SSI module for uDMA use with the following steps:
	//       a. Configure a uDMA for SSI use. See "Micro Direct Memory Access (uDMA)" on page 585 for
	//          more information.
	//       b. Enable the SSI Module's TX FIFO or RX FIFO by setting the TXDMAE or RXDMAE bit in the
	//          SSIDMACTL register.
	//
	// 	 7. Enable the SSI by setting the SSE bit in the SSICR1 register."

	// Step 1 from above: The SSE bit is bit1 and clearing it disables the SSI operation.  I suspect we're
	// told to do this while we're configuration the SSI, (i.e. the SSI cannot be configured while it's
	// enabled).
	SSI0_CR1_R &= ~(0x1 << 1);

	// Step 2 from above: Selecting master or slave.  Valvano's "Realtime Interfacing to ARM Cortex-M
	// Microcontrollers" provides some good info here.  On page 353: "The master initiates all data
	// communication... Typically, the master device creates the clock, and the slave device uses the
	// clock to latch the data in".  Then on page 354: "Typically, the micrcontroller is master and
	// the I/O module is the slave".  Because of these quotes, and my doubt that we want the
	// MAX541 chip initiating communication or that it even has the ability to do so or provide a clock
	// it seems pretty clear we want the TM4C to be the master.
	// Configuring it as master is simple - clear bit2 of SSI0_SSICR1
	SSI0_CR1_R &= ~(0x1 << 2);

	// Step 3 from above: I'm guessing we use the system clock, stress on the word "guess", only b/c
	// I've not read otherwise anywhere else.
	SSI0_CC_R &= ~0xF;

	// Step 4 from above: Our system clock was configured in InitSystemControl() to run at 80 MHz.
	// The MAX541 datasheet shows a max SCLK frequency of 10 MHz and no min.  Using a value of
	// 113 as the CPSR (the divisor) creates a rough approximation of a 44.1 KHz sample rate, a
	// typical sample rate of digital audio.
	//
	// TODO: Allow the user to pass in a sample rate value and then do the math to calculate
	// the closest CPSR and SCR to meet that sample rate.
	SSI0_CPSR_R &= ~0xFF;
	SSI0_CPSR_R |= 113;

	// Step 5 from above:
	//    1) We want the SCR to be zero for now just b/c we're not worried about being that specific
	//       about the sample rate playback - see the above comment about approximating a 44.1 KHz
	//       sample rate.
	//    2) It looks like capturing on the first clock edge is fine so we set SPH (bit 7) to 0.
	//    3) I think protocol mode should be freescale SPI, so I'm clearing bits 4-5
	//    4) Data size should be 16 bit, so bits 0-4 get set to 0xF
	SSI0_CR0_R &= ~(0xFF << 8);
	SSI0_CR0_R &= ~(0x01 << 7);
	SSI0_CR0_R &= ~(0x03 << 4);
	SSI0_CR0_R |= 0xF;

	// Step 6a from above:  Configure SSI for uDMA
	ConfigureAudioOutputDMA();

	// The following is for Step 6b from above.
	// See section 15.3.5 of the datasheet (pg 964) it is the section titled "DMA Operation" in the
	// SSI chapter of the datasheet.  Some important takeaways:
	//    - "The uDMA operation of the SSI is enabled through the SSI DMA Control (SSIDMACTL) register"
	//    - "For the transmit channel, a single transfer request is asserted whenever at least one
	//      empty location is in the transmit FIFO. The burst request is asserted whenever the transmit
	//      FIFO has 4 or more empty slots. The single and burst uDMA transfer requests are handled
	//      automatically by the uDMA controller depending how the uDMA channel is configured".
	//   - "To enable uDMA operation for the transmit channel, the TXDMAE bit of SSIDMACTL should
	//      be set after configuring the uDMA. If uDMA is enabled, then the uDMA controller triggers an
	//      interrupt when a transfer is complete. The interrupt occurs on the SSI interrupt vector.
	//      Therefore, if interrupts are used for SSI operation and uDMA is enabled, the SSI interrupt
	//      handler must be designed to handle the uDMA completion interrupt."
	SSI0_DMACTL_R |= (0x1 << 1);
	SSI0_IM_R &= ~0x8;  // Clear bit 3 to enable the interrupt SSI0 on transit (see pages 956 and 965)
    NVIC_PRI1_R = (NVIC_PRI1_R  & 0x00FFFFFF) | 0xA0000000;      // Set interrupt priority (See table 2-9 on page 104, UART1 is interrupt #7)
    NVIC_EN0_R |= (1 << 7);  // Enable interrupt 7 (SSI0)

	// Step 7 from above:  Enable the SSI by setting the SSE bit in the SSICR1 register.
    SSI0_CR1_R |= (0x1 << 1);
}

void ConfigureAudioOutputDMA()
{
    // See table 9-1 on page 587.  We're using channel 11, encoding zero.
    InitializeDMAChannelAssignment(11, 0);

    // Note the example in the datasheet that receives ping-pong data uses the following settings:
    // "Table 9-12. Channel Control Word Configuration for Peripheral Ping-Pong Receive Example
    // Field in DMACHCTL    Bits     Value         Description
    // -----------------------------------------------------------------------
    // DSTINC              31:30       0           8-bit destination address increment
    // DSTSIZE             29:28       0           8-bit destination data size
    // SRCINC              27:26       3           Source address does not increment
    // SRCSIZE             25:24       0           8-bit source data size
    // reserved            23:18       0           Reserved
    // ARBSIZE             17:14       3           Arbitrates after 8 transfers
    // XFERSIZE             13:4      63           Transfer 64 items
    // NXTUSEBURST             3       0           N/A for this transfer type
    // XFERMODE              2:0       3           Use Ping-Pong transfer mode
    // We need to do some settings a little different since we're not doing the exact same
    // things as the example, but it's a nice reference to figure out what these fields mean.

    uint32_t channelControlWord = 0;

    // Configure XFERMODE as Ping-Pong
    channelControlWord |= 0x03;

    // Not using NXTBURST so no need to do anything

    // XFERSIZE - Setting this to 256 items (255 + 1) see datasheet pg 613
    channelControlWord |= ((AUDIO_BUFFER_SIZE_IN_SAMPLES - 1) << 4);

    // ARBSIZE - Set to zero

    // SRCSIZE - Our audio samples and DAC are 16 bit
    channelControlWord |= (0x1 << 24);

    // SRCINC - 16 bit increments to get to the next sample
    channelControlWord |= (0x1 << 26);

    // DSTSIZE - Our audio samples and DAC are 16 bit
    channelControlWord |= (0x1 << 28);

    // DSTINC - No increment for the destination (i.e. the SSIDATA register)
    channelControlWord |= ((uint32_t)(0x3 << 30));

    SetChannelControlStructure(11, PRIMARY, channelControlWord);
    SetChannelControlStructure(11, ALTERNATE, channelControlWord);

    SetChannelSourceEndAddress(11, PRIMARY, (uint32_t)(primaryAudioBuffer + AUDIO_BUFFER_SIZE_IN_SAMPLES - 1));
    SetChannelSourceEndAddress(11, ALTERNATE,  (uint32_t)(alternateAudioBuffer + AUDIO_BUFFER_SIZE_IN_SAMPLES - 1));

    SetChannelDestinationEndAddress(11, PRIMARY, (uint32_t)(&SSI0_DR_R));
    SetChannelDestinationEndAddress(11, ALTERNATE,  (uint32_t)(&SSI0_DR_R));

    EnableDMAChannel(11);
}

void SSI0InterruptHandler()
{
    if(CheckForTranferCompletion(11, PRIMARY))
    {
        fillAudioBufferCallback(primaryAudioBuffer, AUDIO_BUFFER_SIZE_IN_SAMPLES);
        ReSetChannelControlStructureModeAndSize(11, PRIMARY, 0x03, AUDIO_BUFFER_SIZE_IN_SAMPLES);
    }

    if(CheckForTranferCompletion(11, ALTERNATE))
    {
        fillAudioBufferCallback(alternateAudioBuffer, AUDIO_BUFFER_SIZE_IN_SAMPLES);
        ReSetChannelControlStructureModeAndSize(11, ALTERNATE, 0x03, AUDIO_BUFFER_SIZE_IN_SAMPLES);
    }

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Note the following from page 600:
	// "When transfers are performed from a FIFO of the UART or SSI using the uDMA, and any interrupt
	// is generated from the UART or SSI, the module's status bit in the DMA Channel Interrupt Status
	// (DMACHIS) register must be checked at the end of the interrupt service routine. If the status bit is
	// set, clear the interrupt by writing a 1 to it".
	//
	// ...And from pg 965 of the datasheet:
	// "When transfers are performed from a FIFO of the SSI using the uDMA, and any interrupt is generated
	// from the SSI, the SSI module's status bit in the DMA Channel Interrupt Status (DMACHIS) register
	// must be checked at the end of the interrupt service routine. If the status bit is set, clear the interrupt
	// by writing a 1 to it".

    UDMA_CHIS_R &= ~(1 << 11);
}
