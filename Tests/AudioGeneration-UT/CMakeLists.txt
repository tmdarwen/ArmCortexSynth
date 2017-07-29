cmake_minimum_required(VERSION 3.0)

file(GLOB source_files ../main.cpp [^.]*.cpp [^.]*.h)
add_executable(AudioGeneration-UT ${source_files})
target_link_libraries(AudioGeneration-UT AudioGeneration)
file(GLOB WAV_FILES ${CMAKE_CURRENT_SOURCE_DIR}/ExpectedAudioResults/*.wav)
file(COPY ${WAV_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
add_custom_command(TARGET AudioGeneration-UT POST_BUILD COMMAND AudioGeneration-UT)
