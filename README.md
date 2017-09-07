ArmCortexSynth
==============

A fully functioning MIDI synthesizer using an ARM Cortex-M4F processor ([Tiva LaunchPad TM4C123G](http://www.ti.com/tool/ek-tm4c123gxl)).  
Video Demo: <https://youtu.be/UTizmNIYIBM>

 

**Hardware**

-   [Texas Instruments Tiva C Series TM4C123G LaunchPad](http://www.ti.com/tool/ek-tm4c123gxl)

-   MIDI Keyboard ([Example](https://www.youtube.com/watch?v=7zAPMpPD-n4))

-   Typical MIDI Cable
    ([Example](https://www.amazon.com/gp/product/B009GUP89S/ref=s9_acsd_topr_hd_bw_boEpr_c_x_2_w?pf_rd_m=ATVPDKIKX0DER&pf_rd_s=merchandised-search-4&pf_rd_r=1DZD7NG6RAR0H224K6TV&pf_rd_t=101&pf_rd_p=99ef2030-b386-50e5-bf67-36b72f770aed&pf_rd_i=11973431))

-   Typical Computer Speakers

-   [Maxim MAX541](https://datasheets.maximintegrated.com/en/ds/MAX541-MAX542.pdf) (Audio DAC)

-   [Recom ROE-3.305S DC/DC Converter](http://www.mouser.com/ds/2/468/ROE-766139.pdf)

-   [Texas Instruments LM4041 ](http://www.ti.com/lit/ds/symlink/lm4041c.pdf)

-   [Newhaven NHD-0420H1Z-FL-GBW-33V3](http://www.newhavendisplay.com/nhd0420h1zflgbw33v3-p-5163.html) (LCD Screen)

-   [Digikey CP-2350-ND](http://www.digikey.com/product-detail/en/SDS-50J/CP-2350-ND/97033) (MIDI Connector)

-   [Digikey1N4148-TAPCT-ND](http://www.digikey.com/product-detail/en/1N4148-TAP/1N4148-TAPCT-ND/3104296) (Diode)

-   [Digikey 751-1263-5-ND](http://www.digikey.com/product-detail/en/6N138/751-1263-5-ND/1731496) (6N138 Optocoupler)

-   Miscellaneous resistors, capacitors and interconnect wires. See the schematic images in the documentation directory for details.

 


**Important**

For historical compatibility reasons it seems TI has tied two pins on port B of the TM4C123G directly to port D.  Please see [this TM4C123G support forum thread](https://e2e.ti.com/support/microcontrollers/tiva_arm/f/908/t/605750) for how to effectively deal with this.

 


**Tests**

The unit tests are separate projects that build and run on a typical desktop platform.  For information on how to build and run tests please see [this document](Documentation/Tests.md).

 


**Steps for Building on Windows**

1.   If you haven't already done so, downloaded and install [TivaWare for C Series](http://www.ti.com/tool/sw-tm4c).

1.   Download and install [TI's Code Composer Studio](http://www.ti.com/tool/ccstudio) (CCS) v7.0 or later.

1.   Setup environment variable TIVA_WARE_C_SERIES_DIR to point to the "TivaWare for C Series" directory.

1.   Setup environment variable ARM_CORTEX_SYNTH_ROOT_DIR to point to the directory at which you've cloned this repo.

1.   After starting CCS, select "File -> Import" and select "CCS Projects" and press the "Next" button.

1.   Set the "Select search-directory" to the directory at which you've cloned this repo.  It should automatically find the "ArmCortexSynth" project file.  Click the "Finish" button.

1.   A new project (ArmCortexSynth) should now be imported into your workspace.

1.   You should be able to build (either debug or release).  This will generate an ArmCortexSynth.bin file that can be flashed to the Tiva LaunchPad (TM4C123G) development board using TI's [LM Flash Programmer](http://www.ti.com/tool/lmflashprogrammer).

 

**Miscellaneous Notes Concerning the Project**

-   Unfortunately TI's C++ compiler support is only up to C++03. I would have very much liked to have used some C++11 (and later) features :(

-   Currently, the only (easy) way to build this is through TI's Code Composer Studio (see instructions above).  Ideally, I would have functionality in place to allow a build tool (CMake?) to generate a makefile or other project files.

-   Doesn't currently use [CMSIS](https://developer.arm.com/embedded/cmsis).  With this project I was most interested in working directly with the MCU and therefore interface directly with the TM4C123G registers.

 

**To Do**

-   Allow for easy makefile and/or project file instantiation using CMake or something similar.

-   Support GNU ARM Toolchain and additional popular embedded development toolchains.

-   Support additional development boards (not just the TI TM4C123G series)

 

**Licensing**

The MIT License applies to this software and its supporting documentation:

*Copyright (c) 2017 Terence M. Darwen - tmdarwen.com*

*Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:*

*The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.*

*THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*
