#pragma once

#include <string>
#include <vector>
#include <fstream>
    
// A fairly simple class to read data from a file
class FileReader
{
    public:
        FileReader(const std::string& filename);
        virtual ~FileReader();

        std::size_t GetFileSize();

        std::vector<char> ReadData(std::size_t position, std::size_t bytes);

    private:
        std::string filename_;
        std::ifstream fileStream_;
        std::size_t fileSize_;

        void InitializeFileReading();
};
