#include "catch.hpp"
#include <SynthMenu/SynthMenu.h>
#include <AudioGeneration/Oscillator.h>
#include <Utilities/StringUtilities.h>
#include <SynthMenu-UT/SynthMenuOutput.h>
#include <string.h>

bool RawStringsEqual(const char* s1, const char* s2)
{
    if(strncmp(s1, s2, 19) == 0)
    {
        return true;
    }

    return false;
}

TEST_CASE("Menu System")
{
    SynthMenuOutput testOutput;

    Oscillator oscillator1(Square, 10, 0, 0);
    Oscillator oscillator2(Square, 10, 8, 0);
    Oscillator oscillator3(Square, 10, -8, 0);
    
    SynthMenu synthMenu(&testOutput, oscillator1, oscillator2, oscillator3);

    SECTION("Initial Menu")
    {
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(0), "> Oscillator 1"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(1), "  Oscillator 2"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(2), "  Oscillator 3"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(3), ""));
    }

    SECTION("Test Navigating Down")
    {
        synthMenu.HandleAction(MenuSystem::DOWN);

        REQUIRE(RawStringsEqual(testOutput.GetTextLine(0), "  Oscillator 1"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(1), "> Oscillator 2"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(2), "  Oscillator 3"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(3), ""));

        synthMenu.HandleAction(MenuSystem::DOWN);

        REQUIRE(RawStringsEqual(testOutput.GetTextLine(0), "  Oscillator 1"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(1), "  Oscillator 2"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(2), "> Oscillator 3"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(3), ""));

        synthMenu.HandleAction(MenuSystem::DOWN);

        REQUIRE(RawStringsEqual(testOutput.GetTextLine(0), "  Oscillator 1"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(1), "  Oscillator 2"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(2), "> Oscillator 3"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(3), ""));
    }    

    SECTION("Test Navigating Back Up")
    {
        synthMenu.HandleAction(MenuSystem::DOWN);
        synthMenu.HandleAction(MenuSystem::DOWN);
        synthMenu.HandleAction(MenuSystem::UP);

        REQUIRE(RawStringsEqual(testOutput.GetTextLine(0), "  Oscillator 1"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(1), "> Oscillator 2"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(2), "  Oscillator 3"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(3), ""));

        synthMenu.HandleAction(MenuSystem::UP);

        REQUIRE(RawStringsEqual(testOutput.GetTextLine(0), "> Oscillator 1"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(1), "  Oscillator 2"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(2), "  Oscillator 3"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(3), ""));

        synthMenu.HandleAction(MenuSystem::UP);

        REQUIRE(RawStringsEqual(testOutput.GetTextLine(0), "> Oscillator 1"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(1), "  Oscillator 2"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(2), "  Oscillator 3"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(3), ""));
    }    

    SECTION("Test Navigating To Oscillator")
    {
        synthMenu.HandleAction(MenuSystem::ENTER);

        REQUIRE(RawStringsEqual(testOutput.GetTextLine(0), "> Waveform: Square"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(1), "  Level: 10"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(2), "  Cent: 0"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(3), "  Semitone: 0"));
    }

    SECTION("Test Changing Oscillator Waveform")
    {
        synthMenu.HandleAction(MenuSystem::ENTER);

        REQUIRE(RawStringsEqual(testOutput.GetTextLine(0), "> Waveform: Square"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(1), "  Level: 10"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(2), "  Cent: 0"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(3), "  Semitone: 0"));

        synthMenu.HandleAction(MenuSystem::ENTER);
        synthMenu.HandleAction(MenuSystem::DOWN);

        REQUIRE(RawStringsEqual(testOutput.GetTextLine(0), "> Waveform: Sawtooth"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(1), "  Level: 10"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(2), "  Cent: 0"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(3), "  Semitone: 0"));

        synthMenu.HandleAction(MenuSystem::DOWN);

        REQUIRE(RawStringsEqual(testOutput.GetTextLine(0), "> Waveform: None"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(1), "  Level: 10"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(2), "  Cent: 0"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(3), "  Semitone: 0"));

        synthMenu.HandleAction(MenuSystem::DOWN);

        REQUIRE(RawStringsEqual(testOutput.GetTextLine(0), "> Waveform: Square"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(1), "  Level: 10"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(2), "  Cent: 0"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(3), "  Semitone: 0"));

        synthMenu.HandleAction(MenuSystem::UP);

        REQUIRE(RawStringsEqual(testOutput.GetTextLine(0), "> Waveform: None"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(1), "  Level: 10"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(2), "  Cent: 0"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(3), "  Semitone: 0"));

        synthMenu.HandleAction(MenuSystem::UP);

        REQUIRE(RawStringsEqual(testOutput.GetTextLine(0), "> Waveform: Sawtooth"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(1), "  Level: 10"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(2), "  Cent: 0"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(3), "  Semitone: 0"));

        synthMenu.HandleAction(MenuSystem::UP);

        REQUIRE(RawStringsEqual(testOutput.GetTextLine(0), "> Waveform: Square"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(1), "  Level: 10"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(2), "  Cent: 0"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(3), "  Semitone: 0"));
    }

    SECTION("Test Changing Oscillator Level")
    {
        synthMenu.HandleAction(MenuSystem::ENTER);
        synthMenu.HandleAction(MenuSystem::DOWN);

        REQUIRE(RawStringsEqual(testOutput.GetTextLine(0), "  Waveform: Square"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(1), "> Level: 10"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(2), "  Cent: 0"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(3), "  Semitone: 0"));

        synthMenu.HandleAction(MenuSystem::ENTER);
        synthMenu.HandleAction(MenuSystem::UP);

        REQUIRE(RawStringsEqual(testOutput.GetTextLine(0), "  Waveform: Square"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(1), "> Level: 10"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(2), "  Cent: 0"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(3), "  Semitone: 0"));

        synthMenu.HandleAction(MenuSystem::DOWN);

        REQUIRE(RawStringsEqual(testOutput.GetTextLine(0), "  Waveform: Square"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(1), "> Level: 9"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(2), "  Cent: 0"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(3), "  Semitone: 0"));

        synthMenu.HandleAction(MenuSystem::DOWN);
        synthMenu.HandleAction(MenuSystem::DOWN);
        synthMenu.HandleAction(MenuSystem::DOWN);
        synthMenu.HandleAction(MenuSystem::DOWN);
        synthMenu.HandleAction(MenuSystem::DOWN);
        synthMenu.HandleAction(MenuSystem::DOWN);
        synthMenu.HandleAction(MenuSystem::DOWN);
        synthMenu.HandleAction(MenuSystem::DOWN);
        synthMenu.HandleAction(MenuSystem::DOWN);
        synthMenu.HandleAction(MenuSystem::DOWN);

        REQUIRE(RawStringsEqual(testOutput.GetTextLine(0), "  Waveform: Square"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(1), "> Level: 0"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(2), "  Cent: 0"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(3), "  Semitone: 0"));

        synthMenu.HandleAction(MenuSystem::UP);
        synthMenu.HandleAction(MenuSystem::UP);
        synthMenu.HandleAction(MenuSystem::UP);
        synthMenu.HandleAction(MenuSystem::UP);
        synthMenu.HandleAction(MenuSystem::UP);
        synthMenu.HandleAction(MenuSystem::UP);
        synthMenu.HandleAction(MenuSystem::UP);
        synthMenu.HandleAction(MenuSystem::UP);
        synthMenu.HandleAction(MenuSystem::UP);
        synthMenu.HandleAction(MenuSystem::UP);
        synthMenu.HandleAction(MenuSystem::UP);

        REQUIRE(RawStringsEqual(testOutput.GetTextLine(0), "  Waveform: Square"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(1), "> Level: 10"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(2), "  Cent: 0"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(3), "  Semitone: 0"));
    }

    SECTION("Test Changing Oscillator Cent")
    {
        synthMenu.HandleAction(MenuSystem::ENTER);
        synthMenu.HandleAction(MenuSystem::DOWN);
        synthMenu.HandleAction(MenuSystem::DOWN);

        REQUIRE(RawStringsEqual(testOutput.GetTextLine(0), "  Waveform: Square"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(1), "  Level: 10"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(2), "> Cent: 0"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(3), "  Semitone: 0"));

        synthMenu.HandleAction(MenuSystem::ENTER);
        synthMenu.HandleAction(MenuSystem::UP);

        REQUIRE(RawStringsEqual(testOutput.GetTextLine(0), "  Waveform: Square"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(1), "  Level: 10"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(2), "> Cent: +1"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(3), "  Semitone: 0"));

        synthMenu.HandleAction(MenuSystem::DOWN);
        synthMenu.HandleAction(MenuSystem::DOWN);

        REQUIRE(RawStringsEqual(testOutput.GetTextLine(0), "  Waveform: Square"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(1), "  Level: 10"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(2), "> Cent: -1"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(3), "  Semitone: 0"));

        for(std::size_t i{0}; i < 100; ++i)
        {
            synthMenu.HandleAction(MenuSystem::DOWN);
        }

        REQUIRE(RawStringsEqual(testOutput.GetTextLine(0), "  Waveform: Square"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(1), "  Level: 10"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(2), "> Cent: -100"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(3), "  Semitone: 0"));

        for(std::size_t i{0}; i < 200; ++i)
        {
            synthMenu.HandleAction(MenuSystem::UP);
        }

        REQUIRE(RawStringsEqual(testOutput.GetTextLine(0), "  Waveform: Square"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(1), "  Level: 10"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(2), "> Cent: +100"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(3), "  Semitone: 0"));
    }

    SECTION("Test Changing Oscillator Semitone")
    {
        synthMenu.HandleAction(MenuSystem::ENTER);
        synthMenu.HandleAction(MenuSystem::DOWN);
        synthMenu.HandleAction(MenuSystem::DOWN);
        synthMenu.HandleAction(MenuSystem::DOWN);

        REQUIRE(RawStringsEqual(testOutput.GetTextLine(0), "  Waveform: Square"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(1), "  Level: 10"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(2), "  Cent: 0"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(3), "> Semitone: 0"));

        synthMenu.HandleAction(MenuSystem::ENTER);
        synthMenu.HandleAction(MenuSystem::UP);

        REQUIRE(RawStringsEqual(testOutput.GetTextLine(0), "  Waveform: Square"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(1), "  Level: 10"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(2), "  Cent: 0"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(3), "> Semitone: +1"));

        synthMenu.HandleAction(MenuSystem::DOWN);
        synthMenu.HandleAction(MenuSystem::DOWN);

        REQUIRE(RawStringsEqual(testOutput.GetTextLine(0), "  Waveform: Square"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(1), "  Level: 10"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(2), "  Cent: 0"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(3), "> Semitone: -1"));

        for(std::size_t i{0}; i < 13; ++i)
        {
            synthMenu.HandleAction(MenuSystem::UP);
        }

        REQUIRE(RawStringsEqual(testOutput.GetTextLine(0), "  Waveform: Square"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(1), "  Level: 10"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(2), "  Cent: 0"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(3), "> Semitone: +12"));

        for(std::size_t i{0}; i < 24; ++i)
        {
            synthMenu.HandleAction(MenuSystem::DOWN);
        }

        REQUIRE(RawStringsEqual(testOutput.GetTextLine(0), "  Waveform: Square"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(1), "  Level: 10"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(2), "  Cent: 0"));
        REQUIRE(RawStringsEqual(testOutput.GetTextLine(3), "> Semitone: -12"));
    }
}
