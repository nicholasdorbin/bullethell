#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "io.h"
#include <stdio.h>

bool SaveBufferToBinaryFile(const std::vector<unsigned char>& buffer, const std::string& filepath)
{
	FILE* file = nullptr;
	errno_t didFileOpen = fopen_s(&file, filepath.c_str(), "wb");
	if (didFileOpen != 0)
		return false;
	fwrite(&buffer[0], sizeof(unsigned char), buffer.size(), file);
	fclose(file);
	return true;
}

bool LoadBufferFromBinaryFile(std::vector<unsigned char>& out_buffer, const std::string& filepath)
{
	FILE* file = nullptr;
	errno_t didFileOpen = fopen_s(&file, filepath.c_str(), "rb");
	if (didFileOpen != 0)
		return false;
	fseek(file, 0, SEEK_END);
	size_t size = ftell(file);
	rewind(file);
	out_buffer.resize(size);
	fread(&out_buffer[0], sizeof(unsigned char), size, file);
	fclose(file);
	return true;
}

bool LoadBufferFromBinaryFile(std::string& out_buffer, const std::string& filepath)
{
	FILE* file = nullptr;
	errno_t didFileOpen = fopen_s(&file, filepath.c_str(), "rb");
	if (didFileOpen != 0)
		return false;
	fseek(file, 0, SEEK_END);
	size_t size = ftell(file);
	rewind(file);
	out_buffer.resize(size);
	fread(&out_buffer[0], sizeof(unsigned char), size, file);
	fclose(file);
	return true;
}

void ByteSwap(void *data, size_t const dataSize)
{
	data;
	dataSize;
}

//-----------------------------------------------------------------------------------------------
std::vector< std::string > EnumerateFilesInFolder(const std::string& relativeDirectoryPath, const std::string& filePattern)
{
	// Don't use full file path, always use relative

	// 1. find first
	// 2. build relative path
	// 3. check to see if hidden or subdirectory
	// 4. add to vector
	// 5. error = findnext
	// 6. as long as don't have error and search handle != -1, loop 2 - 5
	// 7. _findclose( searchHandle );

	std::string					searchPathPattern = relativeDirectoryPath + "/" + filePattern;
	std::vector< std::string > foundFiles;

	int error = 0;
	struct _finddata_t fileInfo;
	intptr_t searchHandle = _findfirst(searchPathPattern.c_str(), &fileInfo);
	while (searchHandle != -1 && !error)
	{
		std::string relativePathToFile = Stringf("%s/%s", relativeDirectoryPath.c_str(), fileInfo.name);
		bool		isDirectory = fileInfo.attrib & _A_SUBDIR ? true : false;
		bool		isHidden = fileInfo.attrib & _A_HIDDEN ? true : false;

		if (!isDirectory && !isHidden)
			foundFiles.push_back(relativePathToFile);

		error = _findnext(searchHandle, &fileInfo);
	}
	_findclose(searchHandle);

	return foundFiles;
	
}

std::vector< std::string > EnumerateFilesInFolder(const std::string& relativeDirectoryPath, const std::string& filePattern, bool recurseSubFolders)
{
	// Don't use full file path, always use relative

	// 1. find first
	// 2. build relative path
	// 3. check to see if hidden or subdirectory
	// 4. add to vector
	// 5. error = findnext
	// 6. as long as don't have error and search handle != -1, loop 2 - 5
	// 7. _findclose( searchHandle );

	std::string					searchPathPattern = relativeDirectoryPath + "/" + filePattern;
	std::vector< std::string > foundFiles;

	int error = 0;
	struct _finddata_t fileInfo;
	intptr_t searchHandle = _findfirst(searchPathPattern.c_str(), &fileInfo);
	while (searchHandle != -1 && !error)
	{
		std::string relativePathToFile = Stringf("%s/%s", relativeDirectoryPath.c_str(), fileInfo.name);
		bool		isDirectory = fileInfo.attrib & _A_SUBDIR ? true : false;
		bool		isHidden = fileInfo.attrib & _A_HIDDEN ? true : false;

		if (!isDirectory && !isHidden)
			foundFiles.push_back(relativePathToFile);

		if (isDirectory && !isHidden && recurseSubFolders)
		{
			if (relativePathToFile != "." && relativePathToFile != "..")
			{
				std::vector<std::string> additionalFiles;

				additionalFiles = EnumerateFilesInFolder(relativePathToFile, filePattern, true);

				for each (std::string str in additionalFiles)
				{
					foundFiles.push_back(str);
				}
			}
			
		}

		error = _findnext(searchHandle, &fileInfo);
	}
	_findclose(searchHandle);

	if (recurseSubFolders)
	{
		
		std::vector<std::string> foundSubDirs = GetSubDirectories(relativeDirectoryPath, true);
		for each (std::string subDirStr in foundSubDirs)
		{
			std::vector<std::string> foundSubFiles = EnumerateFilesInFolder(subDirStr, filePattern, true);
			for each (std::string subFileStr in foundSubFiles)
			{
				foundFiles.push_back(subFileStr);
			}
		}
		
	}

	return foundFiles;
}

std::vector < std::string > GetSubDirectories(const std::string& relativeDirectoryPath, bool recurseSubFolders)
{
	std::string					searchPathPattern = relativeDirectoryPath + "/" + "*";
	std::vector< std::string > foundFiles;

	int error = 0;
	struct _finddata_t fileInfo;
	intptr_t searchHandle = _findfirst(searchPathPattern.c_str(), &fileInfo);
	while (searchHandle != -1 && !error)
	{
		std::string relativePathToFile = Stringf("%s/%s", relativeDirectoryPath.c_str(), fileInfo.name);
		std::string fileName = Stringf("%s", fileInfo.name);
		bool		isDirectory = fileInfo.attrib & _A_SUBDIR ? true : false;
		bool		isHidden = fileInfo.attrib & _A_HIDDEN ? true : false;

			

		if (isDirectory && !isHidden)
		{
			if (fileName != "." && fileName != "..")
			{
				foundFiles.push_back(relativePathToFile);
			}

		}

		error = _findnext(searchHandle, &fileInfo);
	}
	_findclose(searchHandle);


	if (recurseSubFolders)
	{
		std::vector<std::string> subDirsToAdd;
		for each (std::string  subDirStr in foundFiles)
		{
			std::vector<std::string> foundSubDirs = GetSubDirectories(subDirStr, true);
			for each (std::string  superSubDirStr in foundSubDirs)
			{
				//add to temp
				subDirsToAdd.push_back(superSubDirStr);
			}
		}

		for each (std::string  subDirToAddStr in subDirsToAdd)
		{
			foundFiles.push_back(subDirToAddStr);
		}
	}
	//add temp to found
	return foundFiles;
}

FileBinaryReader::FileBinaryReader()
{

}

bool FileBinaryReader::Open(std::string const &filename)
{
	char const *mode = "rb";

	errno_t error = fopen_s(&fileHandle, filename.c_str(), mode);
	if (error != 0) {
		return false;
	}

	return true;
}

void FileBinaryReader::Close()
{
	if (fileHandle != nullptr) {
		fclose(fileHandle);
		fileHandle = nullptr;
	}
}

size_t FileBinaryReader::ReadBytes(void *out_buffer, size_t const numBytes)
{
	return fread(out_buffer, sizeof(byte_t), numBytes, fileHandle);
}

FileBinaryWriter::FileBinaryWriter()
{

}

bool FileBinaryWriter::Open(std::string const &filename, bool append /*= false*/)
{
	char const *mode;
	if (append) {
		mode = "ab"; // append binary mode (adds to end of file)
	}
	else {
		mode = "wb"; // write binary mode (new file) (overwrites file)
	}

	errno_t error = fopen_s(&fileHandle, filename.c_str(), mode);
	if (error != 0) {
		return false;
	}

	return true;
}

void FileBinaryWriter::Close()
{
	if (fileHandle != nullptr) {
		fclose(fileHandle);
		fileHandle = nullptr;
	}
}

size_t FileBinaryWriter::WriteBytes(void const *src, size_t const numBytes)
{
	return fwrite(src, sizeof(byte_t), numBytes, fileHandle);
}



bool IBinaryWriter::WriteString(std::string const &string)
{
	size_t length = string.size();
	size_t bufferLength = length + 1;
	//Write how much data I'm going to write, then write the data
	return Write<uint32_t>(bufferLength) && (WriteBytes(&string[0], bufferLength) == bufferLength);
}

bool IBinaryWriter::WriteFloats(std::vector<float> const &floats)
{
	size_t len = floats.size();
	if (len == 0)
	{
		return Write<uint32_t>(len);
	}
	return Write<uint32_t>(len) && (WriteBytes(&floats[0], len) == len);
}

bool IBinaryWriter::WriteInts(std::vector<int> const &ints)
{
	size_t len = ints.size();
	if (len == 0)
	{
		return Write<uint32_t>(len);
	}
	return Write<uint32_t>(len) && (WriteBytes(&ints[0], len) == len);
}

size_t IBinaryReader::ReadString(std::string *out_buffer)
{
	uint32_t numBytes;
	Read<uint32_t>(&numBytes);
	out_buffer->resize(numBytes);
	return ReadBytes(&(*out_buffer)[0], numBytes);
}

size_t IBinaryReader::ReadFloats(std::vector<float> *out_buffer)
{
	uint32_t len = 0;
	Read(&len);

	if (len != 0)
	{
		out_buffer->resize(len);
		ReadBytes(&(*out_buffer)[0], len);
	}

	return len;
}

size_t IBinaryReader::ReadInts(std::vector<int> *out_buffer)
{
	uint32_t len = 0;
	Read(&len);
	
	if (len != 0)
	{
		out_buffer->resize(len);
		ReadBytes(&(*out_buffer)[0], len);
	}

	return len;
}
