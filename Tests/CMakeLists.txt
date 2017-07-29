cmake_minimum_required(VERSION 3.0)

project(ArmCortexSynth-UT)

include_directories($ENV{TIVA_WARE_C_SERIES_DIR})
include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/../Source)  # For non-UT code
include_directories(${PROJECT_SOURCE_DIR}/../../Catch/single_include)

if(UNIX)
	# The UT's use C++11 and possibly some C++14
	set(CMAKE_CXX_FLAGS "-std=c++14")
endif(UNIX)


# Build AudioGeneration code into a library for easy use in UT
file(GLOB SynthMenuSourceFiles "../Source/AudioGeneration/[^.]*.h" "../Source/AudioGeneration/[^.]*.cpp")
add_library(AudioGeneration ${SynthMenuSourceFiles})


# Build the SynthMenu into a library for easy use in UT
file(GLOB SynthMenuSourceFiles
	"../Source/AudioGeneration/NoteFrequencyTable.h" 
	"../Source/AudioGeneration/NoteFrequencyTable.cpp"
	"../Source/AudioGeneration/Oscillator.h"
	"../Source/AudioGeneration/Oscillator.cpp"
	"../Source/MenuSystem/[^.]*.h" 
	"../Source/MenuSystem/[^.]*.cpp"
	"../Source/SynthMenu/[^.]*.h" 
	"../Source/SynthMenu/[^.]*.cpp" 
	"../Source/Utilities/StringUtilities.h" 
	"../Source/Utilities/StringUtilities.cpp")
add_library(SynthMenu ${SynthMenuSourceFiles})


# Add the UT projects 
add_subdirectory(AudioGeneration-UT)
add_subdirectory(SynthMenu-UT)
