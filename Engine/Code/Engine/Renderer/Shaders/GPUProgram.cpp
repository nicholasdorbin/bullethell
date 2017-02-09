#include "Engine/Renderer/Shaders/GPUProgram.hpp"

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"


//-------------------------------------------------------------------------------------------------
GPUProgram::GPUProgram(std::string const &vsFilePath, std::string const &fsFilePath)
{
	//Build Vert and Frag Shader
	m_vertexShaderID = g_theRenderer->CreateShader(vsFilePath, GL_VERTEX_SHADER);
	m_fragmentShaderID = g_theRenderer->CreateShader(fsFilePath, GL_FRAGMENT_SHADER);
	ASSERT_OR_DIE(m_vertexShaderID != NULL && m_fragmentShaderID != NULL, "Vertex or Fragment File not loaded correctly.");

	//Build Program
	m_programID = g_theRenderer->CreateAndLinkProgram(m_vertexShaderID, m_fragmentShaderID, fsFilePath);
	ASSERT_OR_DIE(m_programID != NULL, "Program did not link.");

	//Release Vert and Frag Shader
	glDeleteShader(m_vertexShaderID);
	glDeleteShader(m_fragmentShaderID);
}


//-------------------------------------------------------------------------------------------------
GPUProgram::~GPUProgram()
{
	glDeleteProgram(m_programID);
}
