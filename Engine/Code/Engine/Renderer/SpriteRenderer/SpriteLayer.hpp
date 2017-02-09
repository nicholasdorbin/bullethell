#pragma once
#include <vector>
//-------------------------------------------------------------------------------------------------
class Sprite;

class Material;


//-------------------------------------------------------------------------------------------------
class SpriteLayer
{
public:
	int m_layerID;
	//Sprite * m_listStart;
	std::vector<Sprite*> m_spritesInLayer;
	bool m_isLayerEnabled;
	Material* m_screenEffect;

public:
	SpriteLayer(int id);
	~SpriteLayer();

	void AddSprite(Sprite * newSprite);
	void RemoveSprite(Sprite * sprite);
	void SetScreenEffect(Material* newEffect);
	void RemoveScreenEffect();
};