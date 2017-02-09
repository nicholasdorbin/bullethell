#include "Engine/Renderer/Shaders/Material.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Shaders/Attribute.hpp"
#include "Engine/Renderer/Shaders/Uniform.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Shaders/GPUProgram.hpp"
#include "Engine/Math/Matrix4.hpp"


//-------------------------------------------------------------------------------------------------
Material::Material(GPUProgram const *program)
	: m_samplerID(0)
	, m_renderState(RenderState::BASIC_3D)
	, m_program(program)
	, m_deleteProgram(false)
{
	//Create Sampler
	m_samplerID = g_theRenderer->CreateSampler(GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT);

	//Attribute and Uniform count
	GLuint progID = m_program->GetProgramID();
	int attributeCount;
	glGetProgramiv(progID, GL_ACTIVE_ATTRIBUTES, &attributeCount);
	int uniformCount;
	glGetProgramiv(progID, GL_ACTIVE_UNIFORMS, &uniformCount);

	//Get Attribute information
	for (int aIndex = 0; aIndex < attributeCount; ++aIndex)
	{
		GLsizei length;
		GLint size;
		GLenum type;
		GLchar name[100];
		glGetActiveAttrib(progID, aIndex, sizeof(name), &length, &size, &type, name);
		GLint loc = glGetAttribLocation(progID, name);
		ASSERT_OR_DIE(loc >= 0, "Can't find bind point for Uniform.");

		std::string aName(name);
		std::size_t foundIndex = aName.find('[');

		if (foundIndex != std::string::npos)
		{
			aName = aName.substr(0, foundIndex);
		}

		size_t aHash = std::hash<std::string>{}(aName);
		switch (type)
		{
		case GL_INT:
		{
			int* temp = new int[size];
			m_attributes[aHash] = new Attribute(loc, length, size, type, aName, temp);
			break;
		}
		case GL_FLOAT:
		{
			float* temp = new float[size];
			m_attributes[aHash] = new Attribute(loc, length, size, type, aName, temp);
			break;
		}
		case GL_FLOAT_VEC2:
		{
			Vector2* temp = new Vector2[size];
			m_attributes[aHash] = new Attribute(loc, length, size, type, aName, temp);
			break;
		}
		case GL_FLOAT_VEC3:
		{
			Vector3* temp = new Vector3[size];
			m_attributes[aHash] = new Attribute(loc, length, size, type, aName, temp);
			break;
		}
		case GL_FLOAT_VEC4:
		{
			Vector4* temp = new Vector4[size];
			m_attributes[aHash] = new Attribute(loc, length, size, type, aName, temp);
			break;
		}
		case GL_INT_VEC4:
		{
			UIntVector4* temp = new UIntVector4[size];
			m_attributes[aHash] = new Attribute(loc, length, size, type, aName, temp);
			break;
		}
		case GL_UNSIGNED_INT_VEC4:
		{
			UIntVector4* temp = new UIntVector4[size];
			m_attributes[aHash] = new Attribute(loc, length, size, type, aName, temp);
			break;
		}
		//#TODO: add Matrix case
		case GL_SAMPLER_2D:
			m_attributes[aHash] = new Attribute(loc, length, size, type, aName, new unsigned int(0));
			break;
		}
	}

	//Get Uniform information
	for (int uIndex = 0; uIndex < uniformCount; ++uIndex)
	{
		GLsizei length;
		GLint size;
		GLenum type;
		GLchar name[100];
		glGetActiveUniform(progID, uIndex, sizeof(name), &length, &size, &type, name);
		GLint loc = glGetUniformLocation(progID, name);
		ASSERT_OR_DIE(loc >= 0, "Can't find bind point for Uniform.");

		std::string uName(name);
		std::size_t foundIndex = uName.find('[');

		if (foundIndex != std::string::npos)
		{
			uName = uName.substr(0, foundIndex);
		}

		size_t uHash = std::hash<std::string>{}(uName);
		switch (type)
		{
		case GL_INT:
		{
			int* temp = new int[size];
			m_uniforms[uHash] = new Uniform(loc, length, size, type, uName, temp);
			break;
		}
		case GL_FLOAT:
		{
			float* temp = new float[size];
			m_uniforms[uHash] = new Uniform(loc, length, size, type, uName, temp);
			break;
		}
		case GL_FLOAT_VEC2:
		{
			Vector2* temp = new Vector2[size];
			m_uniforms[uHash] = new Uniform(loc, length, size, type, uName, temp);
			break;
		}
		case GL_FLOAT_VEC3:
		{
			Vector3* temp = new Vector3[size];
			m_uniforms[uHash] = new Uniform(loc, length, size, type, uName, temp);
			break;
		}
		case GL_FLOAT_VEC4:
		{
			Vector4* temp = new Vector4[size];
			m_uniforms[uHash] = new Uniform(loc, length, size, type, uName, temp);
			break;
		}
		case GL_INT_VEC4:
		{
			UIntVector4* temp = new UIntVector4[size];
			m_uniforms[uHash] = new Uniform(loc, length, size, type, uName, temp);
			break;
		}
		case GL_UNSIGNED_INT_VEC4:
		{
			UIntVector4* temp = new UIntVector4[size];
			m_uniforms[uHash] = new Uniform(loc, length, size, type, uName, temp);
			break;
		}
		case GL_FLOAT_MAT4:
		{
			//mat44_fl* temp = new mat44_fl[size];
			Matrix4* temp = new Matrix4[size];
			m_uniforms[uHash] = new Uniform(loc, length, size, type, uName, temp);
			break;
		}
		case GL_SAMPLER_2D:
			m_uniforms[uHash] = new Uniform(loc, length, size, type, uName, new unsigned int(0));
			break;
		}

	}
}


//-------------------------------------------------------------------------------------------------
Material::Material(std::string const &vsFilePath, std::string const &fsFilePath)
	: Material(new GPUProgram(vsFilePath, fsFilePath))
{
	m_deleteProgram = true;
}


//-------------------------------------------------------------------------------------------------
Material::~Material()
{
	if (m_deleteProgram)
	{
		delete m_program;
		m_program = nullptr;
	}

	//Clear Uniforms
	std::map< size_t, Uniform* >::iterator itUniforms = m_uniforms.begin();
	while (itUniforms != m_uniforms.end()) {
		delete(itUniforms->second);
		itUniforms = m_uniforms.erase(itUniforms);
	}

	//Clear Attributes
	std::map< size_t, Attribute* >::iterator itAttributes = m_attributes.begin();
	while (itAttributes != m_attributes.end()) {
		delete(itAttributes->second);
		itAttributes = m_attributes.erase(itAttributes);
	}
}


//-------------------------------------------------------------------------------------------------
void Material::SetUniform(std::string const &uniformName, int uniformValue)
{
	size_t nameHash = std::hash<std::string>{}(uniformName);
	auto uniformIter = m_uniforms.find(nameHash);
	ASSERT_OR_DIE(uniformIter != m_uniforms.end(), "Uniform not found.");
	ASSERT_OR_DIE(uniformIter->second->m_type == GL_INT, "Wrong uniform type.");
	Uniform* uniform = m_uniforms[nameHash];
	*((int*)uniform->m_data) = uniformValue;
	m_uniforms[nameHash] = uniform;
}


//-------------------------------------------------------------------------------------------------
void Material::SetUniform(std::string const &uniformName, float uniformValue)
{
	size_t nameHash = std::hash<std::string>{}(uniformName);
	auto uniformIter = m_uniforms.find(nameHash);
	ASSERT_OR_DIE(uniformIter != m_uniforms.end(), "Uniform not found.");
	ASSERT_OR_DIE(uniformIter->second->m_type == GL_FLOAT, "Wrong uniform type.");
	Uniform* uniform = m_uniforms[nameHash];
	*((float*)uniform->m_data) = uniformValue;
	m_uniforms[nameHash] = uniform;
}


//-------------------------------------------------------------------------------------------------
void Material::SetUniform(std::string const &uniformName, Vector3 const &uniformValue)
{
	size_t nameHash = std::hash<std::string>{}(uniformName);
	auto uniformIter = m_uniforms.find(nameHash);
	ASSERT_OR_DIE(uniformIter != m_uniforms.end(), "Uniform not found.");
	ASSERT_OR_DIE(uniformIter->second->m_type == GL_FLOAT_VEC3, "Wrong uniform type.");
	Uniform* uniform = m_uniforms[nameHash];
	*((Vector3*)uniform->m_data) = uniformValue;
	m_uniforms[nameHash] = uniform;
}


//-------------------------------------------------------------------------------------------------
void Material::SetUniform(std::string const &uniformName, Vector4 const &uniformValue)
{
	size_t nameHash = std::hash<std::string>{}(uniformName);
	auto uniformIter = m_uniforms.find(nameHash);
	ASSERT_OR_DIE(uniformIter != m_uniforms.end(), "Uniform not found.");
	ASSERT_OR_DIE(uniformIter->second->m_type == GL_FLOAT_VEC4, "Wrong uniform type.");
	Uniform* uniform = m_uniforms[nameHash];
	*((Vector4*)uniform->m_data) = uniformValue;
	m_uniforms[nameHash] = uniform;
}


//-------------------------------------------------------------------------------------------------
void Material::SetUniform(std::string const &uniformName, Rgba const &uniformValue)
{
	Vector4 convertedValue = uniformValue.FloatRepresentation();
	SetUniform(uniformName, convertedValue);
}


//-------------------------------------------------------------------------------------------------
void Material::SetUniform(std::string const &uniformName, std::string const &uniformValue)
{
	Texture const *generatedTexture = Texture::CreateOrGetTexture(uniformValue);
	size_t nameHash = std::hash<std::string>{}(uniformName);
	auto uniformIter = m_uniforms.find(nameHash);

	ASSERT_OR_DIE(uniformIter != m_uniforms.end(), "Uniform not found.");
	ASSERT_OR_DIE(uniformIter->second->m_type == GL_SAMPLER_2D, "Wrong uniform type.");
	Uniform* uniform = m_uniforms[nameHash];
	*((unsigned int*)uniform->m_data) = generatedTexture->m_openglTextureID;
	m_uniforms[nameHash] = uniform;
}


void Material::SetUniform(std::string const &uniformName,  Texture* &uniformValue)
{
	size_t nameHash = std::hash<std::string>{}(uniformName);
	auto uniformIter = m_uniforms.find(nameHash);

	ASSERT_OR_DIE(uniformIter != m_uniforms.end(), "Uniform not found.");
	ASSERT_OR_DIE(uniformIter->second->m_type == GL_SAMPLER_2D, "Wrong uniform type.");
	Uniform* uniform = m_uniforms[nameHash];
	*((unsigned int*)uniform->m_data) = uniformValue->m_openglTextureID;
	m_uniforms[nameHash] = uniform;
}

void Material::SetUniform(std::string const &uniformName, const Texture* &uniformValue)
{
	size_t nameHash = std::hash<std::string>{}(uniformName);
	auto uniformIter = m_uniforms.find(nameHash);

	ASSERT_OR_DIE(uniformIter != m_uniforms.end(), "Uniform not found.");
	ASSERT_OR_DIE(uniformIter->second->m_type == GL_SAMPLER_2D, "Wrong uniform type.");
	Uniform* uniform = m_uniforms[nameHash];
	*((unsigned int*)uniform->m_data) = uniformValue->m_openglTextureID;
	m_uniforms[nameHash] = uniform;
}

//-------------------------------------------------------------------------------------------------
unsigned int Material::GetGPUProgramID() const
{
	return m_program->GetProgramID();
}


//-------------------------------------------------------------------------------------------------
unsigned int Material::GetSamplerID() const
{
	return m_samplerID;
}


//-------------------------------------------------------------------------------------------------
const std::map<size_t, Uniform*>& Material::GetUniformList() const
{
	return m_uniforms;
}

std::map<size_t, Uniform*>& Material::GetUniformList()
{
	return m_uniforms;
}


