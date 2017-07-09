ArmCortexSynth
==============

Turns an ARM Cortex-M4F Processor into a Syntheizer

Video Demo: <https://youtu.be/UTizmNIYIBM>

 

**Main Hardware**

-   T[exas Instruments Tiva C Series TM4C123G
    LaunchPad](http://www.ti.com/tool/ek-tm4c123gxl)

-   MIDI Keyboard ([Example](https://www.youtube.com/watch?v=7zAPMpPD-n4))

-   Typical Computer Speakers

-   [Maxim
    MAX541](https://datasheets.maximintegrated.com/en/ds/MAX541-MAX542.pdf)
    (Audio DAC)

-   [Recom ROE-3.305S DC/DC
    Converter](http://www.mouser.com/ds/2/468/ROE-766139.pdf)

-   [Texas Instruments LM4041 ](http://www.ti.com/lit/ds/symlink/lm4041c.pdf)

-   [Newhaven
    NHD-0420H1Z-FL-GBW-33V3](http://www.newhavendisplay.com/nhd0420h1zflgbw33v3-p-5163.html)
    (LCD Screen)

-   [Digikey
    CP-2350-ND](http://www.digikey.com/product-detail/en/SDS-50J/CP-2350-ND/97033)
    (MIDI Connector)

-   [Digikey1N4148-TAPCT-ND](http://www.digikey.com/product-detail/en/1N4148-TAP/1N4148-TAPCT-ND/3104296)
    (Diode)

-   [Digikey
    751-1263-5-ND](http://www.digikey.com/product-detail/en/6N138/751-1263-5-ND/1731496)
    (6N138 Optocoupler)

-   Typical MIDI Cable
    ([Example](https://www.amazon.com/gp/product/B009GUP89S/ref=s9_acsd_topr_hd_bw_boEpr_c_x_2_w?pf_rd_m=ATVPDKIKX0DER&pf_rd_s=merchandised-search-4&pf_rd_r=1DZD7NG6RAR0H224K6TV&pf_rd_t=101&pf_rd_p=99ef2030-b386-50e5-bf67-36b72f770aed&pf_rd_i=11973431))

-   Miscellaneous resistors and capacitors, see the schematics images in the
    documents directory.

 

**The Code**

-   Development was done with TI's Code Composer Studio (CCS) 7.0.

-   Currently, only source files are in the repo. There are currently no
    scripts, etc to generate makefiles or project files for quick/easy
    compilation.

-   Creating/importing into a new project in CCS should not be very difficult.
    Note that I used a both stack and heap sizes of 4096 and also make sure to
    add your Tiva C Series directory into the include path.

-   Unfortunately TI's C/C++ compiler support is only up to C++03. I would have
    very much liked to have used some C++11 and later features :(

-   Future plans include allowing this to also compile under the GNU ARM
    Toolchain and possibly other popular embedded environments.

 

**Upcoming Plans​**

-   Allow for easy makefile and/or project file instantiation using CMake or
    something similar.

-   Support GNU ARM Toolchain and additional popular embedded environments.

-   Support additional development boards (not just the TI TM4C123G series)
