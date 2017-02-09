#include "Engine/Renderer/ParticleRenderer/ParticleEmitter.hpp"

#include "Engine/Renderer/ParticleRenderer/Particle2D.hpp"
#include "Engine/Renderer/SpriteRenderer/Sprite.hpp"
#include "Engine/Tools/Parsers/XMLUtilities.hpp"
#include "Engine/Renderer/SpriteRenderer/SpriteGameRenderer.hpp"
#include "Engine/MathUtils.hpp"

ParticleEmitterDefinition::ParticleEmitterDefinition(XMLNode topNode)
	: m_life(Range<float>(0.f, 1.f))
	, m_scale(Range<float>(0.f, 1.f))
	, m_speed(Range<Vector2>(Vector2::ZERO, Vector2(1.f,1.f)))
{
	//m_emitterDefTag = ReadXMLAttribute(topNode, "name", "");
	std::string spriteName = ReadXMLAttribute(topNode, "spriteTag", "");
	//m_spriteResource = g_spriteRenderSystem->GetSpriteResource(spriteName);

	//#TODO Material?

	m_initialSpawn = ReadXMLAttribute(topNode, "initialSpawn", 0);
	m_spawnRate = ReadXMLAttribute(topNode, "spawnRate", 0.f);

	if (m_spawnRate == 0.f)
		m_isLooping = false;
	else
		m_isLooping = true;

	//float lifeMin = ReadXMLAttribute(topNode, "lifeMin", 0.f);
	//float lifeMax = ReadXMLAttribute(topNode, "lifeMax", 1.f);

	//m_life = GetRandomFloatBetween(lifeMin, lifeMax);

	std::string colorStr = ReadXMLAttribute(topNode, "tint", "1,1,1,1");

	m_tint = Rgba(colorStr);

	std::string scaleStr = ReadXMLAttribute(topNode, "scale", "1,1,1,1");

	size_t pos = scaleStr.find(",");
	std::string scaleXStr = scaleStr.substr(0, pos);
	scaleStr.erase(0, pos + 1);


	std::string scaleYStr = scaleStr;

	//m_scale = Vector2(stof(scaleXStr), stof(scaleYStr));
}

ParticleEmitterDefinition::ParticleEmitterDefinition(std::string const & spriteName)
	: m_life(Range<float>(0.f,1.f))
	, m_scale(Range<float>(0.f, 1.f))
	, m_speed(Range<Vector2>(Vector2::ZERO, Vector2(1.f,1.f)))
{
	m_spriteName = spriteName;
}

ParticleEmitterDefinition::~ParticleEmitterDefinition()
{
	
}

ParticleEmitter::ParticleEmitter(ParticleEmitterDefinition const * definition)
	: m_emitTimer(0.f)
	, m_layerID(0)
{
	m_particleEmitterDefinition = definition;
}

ParticleEmitter::~ParticleEmitter()
{

}

Particle2D * ParticleEmitter::SpawnParticle(Vector2 const & position) const
{
	Particle2D* newParticle = new Particle2D();
	Sprite* newSprite = Sprite::Create(m_particleEmitterDefinition->m_spriteName, m_layerID);
	
	newSprite->m_color = m_particleEmitterDefinition->m_tint;
	newSprite->m_scale.x = m_particleEmitterDefinition->m_scale.get_random();
	newSprite->m_scale.y = m_particleEmitterDefinition->m_scale.get_random();

	newParticle->m_sprite = newSprite;
	newParticle->SetPosition(position);
	newParticle->m_maxAge = m_particleEmitterDefinition->m_life.get_random();
	newParticle->m_acceleration = Vector2(m_particleEmitterDefinition->m_speed.get_random());


	return newParticle;
}
