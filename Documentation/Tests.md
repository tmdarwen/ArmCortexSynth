ArmCortexSynth - Unit Tests
===========================

 

**Noteworthy Items Concerning Unit Tests**

-   The unit tests are built and run independent of the main project (i.e. the ArmCortexSynth).
-   [CMake](https://cmake.org/) is used to generate project files or makefiles for the unit tests.
-   [Catch](https://github.com/philsquared/Catch) is used as the test framework.
-   Currently, the unit tests primarly test the menuing system and audio generation (i.e. the synth sounds).
-   I've tested building these in both Windows (using VS2017 on Windows 10) and Linux (using GCC 5.4.0 on Ubuntu 16.04).

 

**Steps for Building and Running the Unit Tests**

1.   If not already installed, install [CMake](https://cmake.org/).

1.   If you haven't already done so, downloaded and install [TivaWare for C Series](http://www.ti.com/tool/sw-tm4c).

1.   Setup environment variable TIVA_WARE_C_SERIES_DIR to point to the "TivaWare for C Series" directory.

1.   Clone the [Catch](https://github.com/philsquared/Catch) project at the same directory level as the ArmCortexSynth directory.

1.   Run cmake on the Test directory

1.   Build the ArmCortexSynth-UT solution/project.  The unit tests will run at the end of compilation.

 

**Noteworthy Items Concerning AudioGeneration Unit Tests**

-   These unit tests utilize the ArmCortexSynth AudioGeneration logic to create audio wave files of various synth sounds.
-   The resulting wave files are compared with existing expected results.  Any difference results in a test failure.
-   A nice benefit of these tests are that since the audio output is saved to a wave file, any sound the synth is capable of creating can be easily generated and listened to in the typical development environment (i.e. from desktop/laptop).

 

**Noteworthy Items Concerning SynthMenu Unit Tests**

-   The SynthMenu unit tests mimic navigating the LCD menu system.
-   The tests compare the text the menuing system outputs with expected results.
