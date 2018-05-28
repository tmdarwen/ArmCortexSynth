#include "catch.hpp"
#include <AudioGeneration/AudioMixer.h>
#include <AudioGeneration/WaveformTypes.h>
#include <AudioGeneration-UT/WaveFileWriter.h>
#include <AudioGeneration-UT/FileMatch.h>

const std::size_t samplesPerSecond{44100};
const std::size_t secondsPerAudioSnippet{2};
const std::size_t samplesPerAudioSnippet{samplesPerSecond * secondsPerAudioSnippet};

void WriteWaveFile(const std::string& waveFilename, uint16_t* audioData)
{
    WaveFileWriter waveFileWriter(waveFilename, samplesPerSecond);
    waveFileWriter.AppendAudioData(audioData, samplesPerAudioSnippet);
}

TEST_CASE("Audio Generation")
{
    // See MIDI message documentation for note index
    const std::size_t c3NoteIndex{48};
    const std::size_t c4NoteIndex{60};

    AudioMixer audioMixer;
    uint16_t audioData[samplesPerAudioSnippet];

    SECTION("Default Audio C4")
    {
        audioMixer.SetMIDINote(c4NoteIndex);
        audioMixer.GetAudioData(audioData, samplesPerAudioSnippet);
        WriteWaveFile("C4Default.wav", audioData);
        REQUIRE(FilesMatch("C4Default.wav", "C4DefaultExpected.wav"));
    }

    SECTION("Default Audio C3")
    {
        audioMixer.SetMIDINote(c3NoteIndex);
        audioMixer.GetAudioData(audioData, samplesPerAudioSnippet);
        WriteWaveFile("C3Default.wav", audioData);
        REQUIRE(FilesMatch("C3Default.wav", "C3DefaultExpected.wav"));
    }

    SECTION("Sawtooth Audio C3")
    {
        Oscillator& oscillator1{audioMixer.GetOscillator1()};
        Oscillator& oscillator2{audioMixer.GetOscillator2()};
        Oscillator& oscillator3{audioMixer.GetOscillator3()};

        oscillator1.SetWaveformType(Sawtooth);
        oscillator2.SetWaveformType(Sawtooth);
        oscillator3.SetWaveformType(Sawtooth);

        audioMixer.SetMIDINote(c3NoteIndex);
        audioMixer.GetAudioData(audioData, samplesPerAudioSnippet);
        WriteWaveFile("C3Sawtooth.wav", audioData);
        REQUIRE(FilesMatch("C3Sawtooth.wav", "C3SawtoothExpected.wav"));
    }

    SECTION("Sawtooth Octave Audio C3")
    {
        Oscillator& oscillator1{audioMixer.GetOscillator1()};
        Oscillator& oscillator2{audioMixer.GetOscillator2()};
        Oscillator& oscillator3{audioMixer.GetOscillator3()};

        oscillator1.SetWaveformType(Sawtooth);
        oscillator2.SetWaveformType(Sawtooth);
        oscillator2.SetSemitone(12);
        oscillator3.SetWaveformType(Sawtooth);
        oscillator3.SetSemitone(-12);

        audioMixer.SetMIDINote(c3NoteIndex);
        audioMixer.GetAudioData(audioData, samplesPerAudioSnippet);
        WriteWaveFile("C3SawtoothOctaves.wav", audioData);
        REQUIRE(FilesMatch("C3SawtoothOctaves.wav", "C3SawtoothOctavesExpected.wav"));
    }

    SECTION("Square Fifths Audio C3")
    {
        Oscillator& oscillator1{audioMixer.GetOscillator1()};
        Oscillator& oscillator2{audioMixer.GetOscillator2()};
        Oscillator& oscillator3{audioMixer.GetOscillator3()};

        oscillator2.SetSemitone(7);
        oscillator3.SetSemitone(7);

        audioMixer.SetMIDINote(c3NoteIndex);
        audioMixer.GetAudioData(audioData, samplesPerAudioSnippet);
        WriteWaveFile("C3SquareFifths.wav", audioData);
        REQUIRE(FilesMatch("C3SquareFifths.wav", "C3SquareFifthsExpected.wav"));
    }
}
