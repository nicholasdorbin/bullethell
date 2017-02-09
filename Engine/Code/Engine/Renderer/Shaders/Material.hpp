#pragma once

#include <string>
#include <map>
#include "Engine/Renderer/RenderState.hpp"
#include "Engine/Math/Rgba.hpp"
#include "Engine/Renderer/Shaders/Light.hpp"


//-------------------------------------------------------------------------------------------------
class Texture;
class GPUProgram;
class Rgba;
class Vector3;
class Vector4;
struct Uniform;
struct Attribute;


//-------------------------------------------------------------------------------------------------
class Material
{
private:
	unsigned int m_samplerID;
	RenderState m_renderState;
	GPUProgram const *m_program;
	bool m_deleteProgram;

	//Uniforms
	std::map<size_t, Attribute*> m_attributes;
	std::map<size_t, Uniform*> m_uniforms;

public:
	Material(GPUProgram const *program);
	Material(std::string const &vsFilePath, std::string const &fsFilePath);
	~Material();

	//Get Uniforms
	std::map<size_t, Uniform*>& GetUniformList();
	const std::map<size_t, Uniform*>& GetUniformList() const;

	//Set Uniforms
	void SetUniform(std::string const &uniformName, int uniformValue);
	void SetUniform(std::string const &uniformName, float uniformValue);
	void SetUniform(std::string const &uniformName, Vector3 const &uniformValue);
	void SetUniform(std::string const &uniformName, Vector4 const &uniformValue);
	void SetUniform(std::string const &uniformName, Rgba const &uniformValue);
	void SetUniform(std::string const &uniformName, std::string const &uniformValue);
	void SetUniform(std::string const &uniformName, const Texture* &uniformValue);

	void SetUniform(std::string const &uniformName, Texture* &uniformValue);
	//Get IDs
	unsigned int GetGPUProgramID() const;
	unsigned int GetSamplerID() const;
	std::map<size_t, Attribute*>const&  GetAttributeList() const { return m_attributes; }; //size_t is really the hash of the name just hashed
	std::map<size_t, Attribute*>&  GetAttributeList()  { return m_attributes; }; 
	//std::map<size_t, Uniform*> GetUniformList() const { return m_uniforms; };

	RenderState GetRenderState() const { return m_renderState; }
};