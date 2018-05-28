#include <AudioGeneration-UT/FileMatch.h>
#include <AudioGeneration-UT/FileReader.h>

bool FilesMatch(const std::string& fileA, const std::string& fileB)
{
    FileReader fileReaderA{fileA};
    FileReader fileReaderB{fileB};

    if(fileReaderA.GetFileSize() != fileReaderB.GetFileSize())
    {
        return false;
    }

    std::size_t currentPosition{0};
    const std::size_t readSize{1024};
    while(currentPosition < fileReaderA.GetFileSize())
    {
        std::size_t readAmount{readSize};
        if((currentPosition + readAmount) > fileReaderA.GetFileSize())
        {
            readAmount = fileReaderA.GetFileSize() - currentPosition;    
        }

        auto bufferA{fileReaderA.ReadData(currentPosition, readAmount)};
        auto bufferB{fileReaderB.ReadData(currentPosition, readAmount)};

        for(std::size_t i{0}; i < readAmount; ++i)
        {
            if(bufferA[i] != bufferB[i])
            {
                return false;
            }
        }

        currentPosition += readAmount;
    }

    return true;
}

