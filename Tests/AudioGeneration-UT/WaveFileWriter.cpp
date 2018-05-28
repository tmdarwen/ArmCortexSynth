#include <AudioGeneration-UT/WaveFileWriter.h>
#include <AudioGeneration-UT/Exception.h>

WaveFileWriter::WaveFileWriter(const std::string& filename, std::size_t sampleRate) :
    filename_{filename}, sampleRate_{sampleRate}, sampleCount_{0}
{
    fileStream_.open(filename_, std::ios::out | std::ios::binary);
    if(!fileStream_.is_open())
    {
        throw Exception("Failed to open wave file for writing");
    }

    // Write some blank space for the wave header. (The actual wave header will be written when the file is closed as 
    // we don't know some of the values until we're ready to close the file).
    uint8_t blankSpace[sizeof(WaveHeader)];
     fileStream_.write(reinterpret_cast<const char*>(blankSpace), sizeof(WaveHeader)); 
    if(!fileStream_.good())
    {
        throw Exception("Failed to write blank data to wave file header");
    }
}

WaveFileWriter::~WaveFileWriter()
{
    WriteWaveHeader();

    fileStream_.close();
}

void WaveFileWriter::AppendAudioData(uint16_t* buffer, std::size_t bufferSize)
{
    // Note that the audio output to the DAC of the synthesizer uses unsigned 16 bit sample values 
    // that range from 0 to 65536 but the wave file format uses signed values that range from 
    // -32768 to 32767.
    const int32_t conversionFactorFromUnsignedToSign(32768);

    if(bufferSize == 0)
    {
        return;
    }

    for(std::size_t i = 0; i < bufferSize; ++i)
    {
        int16_t convertedSampleValue = (buffer[i] - conversionFactorFromUnsignedToSign);
         fileStream_.write(reinterpret_cast<const char*>(&convertedSampleValue), sizeof(int16_t));
        if(!fileStream_.good())
        {
            throw Exception("Failed to write the wave file");
        }
    }

    sampleCount_ += bufferSize;
}

/*
The following info comes from http://soundfile.sapp.org/doc/WaveFormat/
The canonical WAVE format starts with the RIFF header:

0         4   ChunkID          Contains the letters "RIFF" in ASCII form
                               (0x52494646 big-endian form).
4         4   ChunkSize        36 + SubChunk2Size, or more precisely:
                               4 + (8 + SubChunk1Size) + (8 + SubChunk2Size)
                               This is the size of the rest of the chunk 
                               following this number.  This is the size of the 
                               entire file in bytes minus 8 bytes for the
                               two fields not included in this count:
                               ChunkID and ChunkSize.
8         4   Format           Contains the letters "WAVE"
                               (0x57415645 big-endian form).

The "WAVE" format consists of two subchunks: "fmt " and "data":
The "fmt " subchunk describes the sound data's format:

12        4   Subchunk1ID      Contains the letters "fmt "
                               (0x666d7420 big-endian form).
16        4   Subchunk1Size    16 for PCM.  This is the size of the
                               rest of the Subchunk which follows this number.
20        2   AudioFormat      PCM = 1 (i.e. Linear quantization)
                               Values other than 1 indicate some 
                               form of compression.
22        2   NumChannels      Mono = 1, Stereo = 2, etc.
24        4   SampleRate       8000, 44100, etc.
28        4   ByteRate         == SampleRate * NumChannels * BitsPerSample/8
32        2   BlockAlign       == NumChannels * BitsPerSample/8
                               The number of bytes for one sample including
                               all channels. I wonder what happens when
                               this number isn't an integer?
34        2   BitsPerSample    8 bits = 8, 16 bits = 16, etc.
          2   ExtraParamSize   if PCM, then doesn't exist
          X   ExtraParams      space for extra parameters

The "data" subchunk contains the size of the data and the actual sound:

36        4   Subchunk2ID      Contains the letters "data"
                               (0x64617461 big-endian form).
40        4   Subchunk2Size    == NumSamples * NumChannels * BitsPerSample/8
                               This is the number of bytes in the data.
                               You can also think of this as the size
                               of the read of the subchunk following this 
                               number.
44        *   Data             The actual sound data.
*/
void WaveFileWriter::WriteWaveHeader()
{
    WaveHeader waveHeader;

    waveHeader.chunkID_[0] = 'R';
    waveHeader.chunkID_[1] = 'I';
    waveHeader.chunkID_[2] = 'F';
    waveHeader.chunkID_[3] = 'F';

    uint32_t subChunk2Size{static_cast<uint32_t>(sampleCount_) * static_cast<uint32_t>(sizeof(int16_t))};

    uint32_t chunkSize{4 + (8 + SIZE_OF_SUBCHUNK1) + (8 + subChunk2Size)};

    waveHeader.chunkSize_[0] = chunkSize & 0xFF;
    waveHeader.chunkSize_[1] = (chunkSize >> 8) & 0xFF;
    waveHeader.chunkSize_[2] = (chunkSize >> 16) & 0xFF;
    waveHeader.chunkSize_[3] = (chunkSize >> 24) & 0xFF;

    waveHeader.format_[0] = 'W';
    waveHeader.format_[1] = 'A';
    waveHeader.format_[2] = 'V';
    waveHeader.format_[3] = 'E';

    waveHeader.subChunk1ID_[0] = 'f';
    waveHeader.subChunk1ID_[1] = 'm';
    waveHeader.subChunk1ID_[2] = 't';
    waveHeader.subChunk1ID_[3] = ' ';

    waveHeader.subChunk1Size_[0] = SIZE_OF_SUBCHUNK1 & 0xFF;
    waveHeader.subChunk1Size_[1] = (SIZE_OF_SUBCHUNK1 >> 8) & 0xFF;
    waveHeader.subChunk1Size_[2] = (SIZE_OF_SUBCHUNK1 >> 16) & 0xFF;
    waveHeader.subChunk1Size_[3] = (SIZE_OF_SUBCHUNK1 >> 24) & 0xFF;

    waveHeader.audioFormat_[0] = PCM_AUDIO_FORMAT & 0xFF;
    waveHeader.audioFormat_[1] = (PCM_AUDIO_FORMAT >> 8) & 0xFF;

    waveHeader.channels_[0] = CHANNELS & 0xFF;
    waveHeader.channels_[1] = 0;

    waveHeader.sampleRate_[0] = sampleRate_ & 0xFF;
    waveHeader.sampleRate_[1] = (sampleRate_ >> 8) & 0xFF;
    waveHeader.sampleRate_[2] = (sampleRate_ >> 16) & 0xFF;
    waveHeader.sampleRate_[3] = (sampleRate_ >> 24) & 0xFF;

    uint32_t byteRate{static_cast<uint32_t>(sampleRate_) * static_cast<uint32_t>(sizeof(int16_t))};

    waveHeader.byteRate_[0] = byteRate & 0xFF;
    waveHeader.byteRate_[1] = (byteRate >> 8) & 0xFF;
    waveHeader.byteRate_[2] = (byteRate >> 16) & 0xFF;
    waveHeader.byteRate_[3] = (byteRate >> 24) & 0xFF;

    uint32_t blockAlign{static_cast<uint32_t>(sizeof(int16_t))};

    waveHeader.blockAlign_[0] = blockAlign & 0xFF;
    waveHeader.blockAlign_[1] = (blockAlign >> 8) & 0xFF;

    uint16_t bitsPerSample = sizeof(int16_t) * 8;

    waveHeader.bitsPerSample_[0] = bitsPerSample & 0xFF;
    waveHeader.bitsPerSample_[1] = (bitsPerSample >> 8) & 0xFF;

    waveHeader.subChunk2ID_[0] = 'd';
    waveHeader.subChunk2ID_[1] = 'a';
    waveHeader.subChunk2ID_[2] = 't';
    waveHeader.subChunk2ID_[3] = 'a';

    waveHeader.subChunk2Size_[0] = subChunk2Size & 0xFF;
    waveHeader.subChunk2Size_[1] = (subChunk2Size >> 8) & 0xFF;
    waveHeader.subChunk2Size_[2] = (subChunk2Size >> 16) & 0xFF;
    waveHeader.subChunk2Size_[3] = (subChunk2Size >> 24) & 0xFF;

    fileStream_.seekp(0);
     fileStream_.write(reinterpret_cast<const char*>(&waveHeader), sizeof(WaveHeader));
    if(!fileStream_.good())
    {
        throw Exception("Failed to write wave file header");
    }
}
