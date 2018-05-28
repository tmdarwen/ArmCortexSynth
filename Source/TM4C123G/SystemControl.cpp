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

#include <stdint.h>
#include "SystemControl.h"
#include "inc/tm4c123gh6pm.h"

void InitSystemControl()
{
    // From pg 231 of the datasheet on init and config of System Control:

    // "The PLL is configured using direct register writes to the RCC/RCC2 register. If the RCC2 register
    //  is being used, the USERCC2 bit must be set and the appropriate RCC2 bit/field is used. The steps
    //  required to successfully change the PLL-based system clock are:
    //
    //  1. Bypass the PLL and system clock divider by setting the BYPASS bit and clearing the USESYS
    //     bit in the RCC register, thereby configuring the microcontroller to run off a "raw" clock source
    //     and allowing for the new PLL configuration to be validated before switching the system clock
    //     to the PLL.
    //
    //  2. Select the crystal value (XTAL) and oscillator source (OSCSRC), and clear the PWRDN bit in
    //     RCC/RCC2. Setting the XTAL field automatically pulls valid PLL configuration data for the
    //     appropriate crystal, and clearing the PWRDN bit powers and enables the PLL and its output.
    //
    //  3. Select the desired system divider (SYSDIV) in RCC/RCC2 and set the USESYS bit in RCC. The
    //     SYSDIV field determines the system frequency for the microcontroller.
    //
    //  4. Wait for the PLL to lock by polling the PLLLRIS bit in the Raw Interrupt Status (RIS) register.
    //
    //  5. Enable use of the PLL by clearing the BYPASS bit in RCC/RCC2".

    // Pre-Step 1 - See above: We're using RCC2 register so we set bit 31 (USERCC2)
    SYSCTL_RCC2_R |= 0x80000000;

    // Step 1 from above: We bypass the PLL by setting the BYPASS bit (bit 11)
    SYSCTL_RCC_R |= 0x400;

    // Step 1 from above: "Clear the USESYS bit in the RCC register" there is no "USESYS" bit but there
    // is a USESYSDIV bit in bit 22, I suspect that is what the datasheet means.
    SYSCTL_RCC_R &= ~0x400000;

    // Step 2 from above: "Select the crystal value (XTAL)"  Note that XTAL only exists in RCC, it's
    // bits 6-to-10.  We use a 16 MHz clock for no other reason than that is what the Valvano books
    // usually use :)  ...Note that we first clear bits 6-to-10 in case they are not already clear
    // and then we set them to 0x15 which corresponds to 16.0 MHz
    SYSCTL_RCC_R &= ~0x1F;
    SYSCTL_RCC_R |= (0x15 << 6);

    // Step 2 from above: "Select the oscillator source (OSCSRC)".  The OSCSRC corresponds to bits 4-5
    // of RCC.  I'm selecting the Main Oscillator for no real reason other than it seems like the
    // standard ("I don't need anything special") choice.
    SYSCTL_RCC2_R &= ~(0x3 << 4);

    // Step 2 from above: "clear the PWRDN bit in RCC/RCC2"
    SYSCTL_RCC_R &= ~(0x1 << 13);
    SYSCTL_RCC2_R &= ~(0x1 << 13);

    // Step 3 from above: "Select the desired system divider (SYSDIV) in RCC/RCC2 and set the USESYS
    // bit in RCC. The SYSDIV field determines the system frequency for the microcontroller."  To get
    // the 80 MHz clock frequency (which I believe is max speed of the TM4C123G) see table 5-6 on
    // page 224 of the datasheet.  In order to get a clock frequency of 80 MHz we need the following
    // settings:
    //   1) DIV400 to equal 1     (Bit 30 of RCC2)
    //   2) SYSDIV2 to equal 0x02 (Bits 23-28 of RCC2)
    //   3) SYSDIV2LSB to equal 0 (Bit 22 of RCC2)
    SYSCTL_RCC2_R |= (0x01 << 30);
    SYSCTL_RCC2_R &= ~(0x3F << 23);
    SYSCTL_RCC2_R |= (0x02 << 23);
    SYSCTL_RCC2_R &= ~(0x01 << 22);

    // Step 4 from above: "Wait for the PLL to lock"
    while((SYSCTL_RIS_R & (0x01 << 6)) == 0) { };

    // Step 5 from above: "Enable use of the PLL"
    SYSCTL_RCC_R &= ~(0x01 << 11);
    SYSCTL_RCC2_R &= ~(0x01 << 11);
}
