#include "Engine/Renderer/SpriteRenderer/SpriteResource.hpp"

#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Shaders/Material.hpp"
#include "Engine/Renderer/Shaders/GPUProgram.hpp"
#include "Engine/Math/HashUtils.hpp"
#include "Engine/Tools/Parsers/XMLUtilities.hpp"


const std::string DEFAULT_SPRITE_VERT_SHADER = "Data/Shaders/sprite.vert";
const std::string DEFAULT_SPRITE_FRAG_SHADER = "Data/Shaders/sprite.frag";

GPUProgram* SpriteResource::s_defaultProgramShader = nullptr;


SpriteResource::SpriteResource(std::string const & id, std::string const & filename)
{
	if (SpriteResource::s_defaultProgramShader == nullptr)
	{
		SpriteResource::s_defaultProgramShader = new GPUProgram(DEFAULT_SPRITE_VERT_SHADER, DEFAULT_SPRITE_FRAG_SHADER);

	}
	


	m_name = id;
	//m_id = HashString(id);
	m_id = std::hash<std::string>{}(id);

	m_texture = Texture::CreateOrGetTexture(filename); 


	m_material = new Material(SpriteResource::s_defaultProgramShader);
	m_material->SetUniform("gDiffuseTex", m_texture);
}

SpriteResource::SpriteResource(XMLNode root)
{
	if (SpriteResource::s_defaultProgramShader == nullptr)
	{
		SpriteResource::s_defaultProgramShader = new GPUProgram(DEFAULT_SPRITE_VERT_SHADER, DEFAULT_SPRITE_FRAG_SHADER);
	}

	if (root.getAttribute("id") && root.getAttribute("filepath"))
	{
		//m_name = root.getAttribute("id");
		m_name = ReadXMLAttribute(root, "id", "");
		//m_id = HashString(m_name);
		m_id = std::hash<std::string>{}(m_name);
		std::string filename  = ReadXMLAttribute(root, "filepath", "");

		m_texture = Texture::CreateOrGetTexture(filename);

		//m_defaultProgramShader = new GPUProgram(DEFAULT_SPRITE_VERT_SHADER, DEFAULT_SPRITE_FRAG_SHADER);

		//m_material = new Material(m_defaultProgramShader);
		m_material = new Material(SpriteResource::s_defaultProgramShader);
		m_material->SetUniform("gDiffuseTex", m_texture);
	}
}

SpriteResource::~SpriteResource()
{
	delete m_material;
	if (s_defaultProgramShader != nullptr)
	{
		delete s_defaultProgramShader;
		s_defaultProgramShader = nullptr;
	}
}

std::string SpriteResource::GetID() const
{
	//return m_id;
	return m_name;
}

Material * SpriteResource::GetMaterial() const
{
	return m_material;
}



Texture const * SpriteResource::GetTexture() const
{
	return m_texture;
}
