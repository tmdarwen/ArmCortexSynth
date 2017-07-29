cmake_minimum_required(VERSION 3.0)

file(GLOB source_files ../main.cpp [^.]*.cpp [^.]*.h)
add_executable(SynthMenu-UT ${source_files})
target_link_libraries(SynthMenu-UT SynthMenu)
add_custom_command(TARGET SynthMenu-UT POST_BUILD COMMAND SynthMenu-UT)
