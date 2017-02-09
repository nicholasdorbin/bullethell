#include "Engine/Renderer/SpriteRenderer/SpriteLayer.hpp"
#include "Engine/Renderer/SpriteRenderer/Sprite.hpp"
#include "Engine/Renderer/Shaders/Material.hpp"

SpriteLayer::SpriteLayer(int id)
	: m_isLayerEnabled(true)
{
	m_layerID = id;
	//#TODO remove this default fbo effect
	m_screenEffect = new Material("Data/Shaders/post.vert", "Data/Shaders/post_passthrough.frag");
}

SpriteLayer::~SpriteLayer()
{
	delete m_screenEffect;
}

void SpriteLayer::AddSprite(Sprite * newSprite)
{
	m_spritesInLayer.push_back(newSprite);
}

void SpriteLayer::RemoveSprite(Sprite * sprite)
{
	for (auto spriteIter = m_spritesInLayer.begin(); spriteIter != m_spritesInLayer.end(); ++spriteIter)
	{
		if ((*spriteIter) == sprite)
		{
			m_spritesInLayer.erase(spriteIter);
			break;
		}
	}
}

void SpriteLayer::SetScreenEffect(Material* newEffect)
{
	m_screenEffect = newEffect;
}

void SpriteLayer::RemoveScreenEffect()
{
	delete m_screenEffect;
	m_screenEffect = nullptr;
}

