#pragma once
#include <string>


//-------------------------------------------------------------------------------------------------
class Material;
class Texture;
class GPUProgram;
struct XMLNode;

//-------------------------------------------------------------------------------------------------
class SpriteResource
{
private:
	std::string m_name;
	size_t m_id;
	Texture const * m_texture;
	Material * m_material;
	

public:
	static GPUProgram* s_defaultProgramShader;
	SpriteResource(std::string const & id, std::string const & filename);
	SpriteResource(XMLNode root);
	SpriteResource(SpriteResource const & copy) = delete;
	~SpriteResource();

	std::string GetID() const;
	Material * GetMaterial() const;
	Texture const * GetTexture() const;
};