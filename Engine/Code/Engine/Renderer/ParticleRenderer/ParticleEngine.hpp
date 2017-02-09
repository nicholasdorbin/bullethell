#pragma once

#include <map>
#include <vector>
#include <string>
#include "Engine/Math/Vector2.hpp"

class ParticleEngine;
class ParticleSystem;
struct ParticleSystemDefinition;


extern ParticleEngine * g_ParticleEngine;

class ParticleEngine
{
private:
	std::map<size_t, ParticleSystemDefinition*> m_particleSystemResourceDatabase;
	std::vector<ParticleSystem*> m_activeParticleSystems;

	ParticleSystemDefinition const * GetParticleSystemResource(std::string const & resourceName);

public:
	ParticleEngine();
	~ParticleEngine();

	void Update(float deltaSeconds);

	void LoadAllParticleSystemDefinitions();
	void DestroyAllParticleSystemDefinitions();
	void PlayOnce(std::string const & particleSystemID, int layerID, Vector2 const & position = Vector2::ZERO);
	ParticleSystem * Create(std::string const & particleSystemID, int layerID, Vector2 const & position = Vector2::ZERO);
};