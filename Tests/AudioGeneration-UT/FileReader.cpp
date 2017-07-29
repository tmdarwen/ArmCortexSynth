#include <AudioGeneration-UT/FileReader.h>
#include <AudioGeneration-UT/Exception.h>

FileReader::FileReader(const std::string& filename) : filename_{filename}, fileStream_(filename_, std::ios::in | std::ios::binary)
{
	InitializeFileReading();
}

FileReader::~FileReader()
{
	fileStream_.close();
}

std::size_t FileReader::GetFileSize()
{
	return fileSize_;
}

std::vector<char> FileReader::ReadData(std::size_t position, std::size_t bytes)
{
	std::vector<char> data(bytes);

	fileStream_.seekg(position, std::ios_base::beg);

	fileStream_.read(data.data(), bytes);
	if(!fileStream_.good())
	{
		throw Exception("Failed to read audio data from file");
	}

	return data;
 }

void FileReader::InitializeFileReading()
{
	if(!fileStream_.is_open())
	{
		throw Exception("Failed to open file");
	}

	fileStream_.seekg(0, std::ios_base::end);
	fileSize_ = static_cast<std::size_t>(fileStream_.tellg());
	fileStream_.seekg(0, std::ios_base::beg);
}
