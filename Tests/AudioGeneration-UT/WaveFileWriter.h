#pragma once

#include <string>
#include <fstream>
#include <stdint.h>


struct WaveHeader
{
    uint8_t chunkID_[4];
    uint8_t chunkSize_[4];
    uint8_t format_[4];
    uint8_t subChunk1ID_[4];
    uint8_t subChunk1Size_[4];
    uint8_t audioFormat_[2];
    uint8_t channels_[2];
    uint8_t sampleRate_[4];
    uint8_t byteRate_[4];
    uint8_t blockAlign_[2];
    uint8_t bitsPerSample_[2];
    uint8_t subChunk2ID_[4];
    uint8_t subChunk2Size_[4];
};


class WaveFileWriter
{
    public:
        WaveFileWriter(const std::string& filename, std::size_t sampleRate);
        ~WaveFileWriter();

        void AppendAudioData(uint16_t* buffer, std::size_t bufferSize);

    private:
        void WriteWaveHeader();

        std::string filename_;
        std::size_t sampleRate_;
        std::size_t sampleCount_;
        std::ofstream fileStream_;

        static const uint32_t SIZE_OF_SUBCHUNK1{16};
        static const uint16_t PCM_AUDIO_FORMAT{1};
        static const uint8_t CHANNELS{1};
        //static const std::size_t WAVE_HEADER_SIZE{44};
};
