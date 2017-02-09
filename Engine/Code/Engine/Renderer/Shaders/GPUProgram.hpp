#pragma once

#include <string>


//-------------------------------------------------------------------------------------------------
class GPUProgram
{
private:
	unsigned int m_vertexShaderID;
	unsigned int m_fragmentShaderID;
	unsigned int m_programID;
public:
	GPUProgram(std::string const &vsFilePath, std::string const &fsFilePath);
	~GPUProgram();

	unsigned int GetProgramID() const { return m_programID; }
};