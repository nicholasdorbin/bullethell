#include "Engine/Renderer/ParticleRenderer/ParticleSystem.hpp"
#include "Engine/MathUtils.hpp"

ParticleSystemDefinition::ParticleSystemDefinition(XMLNode systemNode)
{
	if (!systemNode.IsContentEmpty())
	{
		int emitterCount = systemNode.nChildNode(); //# of Emitter nodes

		for (int i = 0; i < emitterCount; i++)
		{
			XMLNode thisNode = systemNode.getChildNode(i);
			std::string name = thisNode.getName();
			if (name != "Emitter")
			{
				//Not valid node, skip
				continue;
			}
			ParticleEmitterDefinition* newEmitter = new ParticleEmitterDefinition(thisNode);
			if (newEmitter != nullptr)
			{
				m_emitterResources.push_back(newEmitter);
				//m_particleEmitterDefinitionsDatabase.insert(std::pair< std::string, ParticleEmitterDefinition* >(newEmitter->m_emitterDefTag, newEmitter));
			}

		}
	}
	
}

ParticleSystemDefinition::ParticleSystemDefinition(std::string const & resourceID)
{
	id = resourceID;
}

ParticleSystemDefinition::~ParticleSystemDefinition()
{
	for each (const ParticleEmitterDefinition* def in m_emitterResources)
	{
		delete def;
	}
}

void ParticleSystemDefinition::AddEmitter(ParticleEmitterDefinition const * emitter)
{
	m_emitterResources.push_back(emitter);
}

ParticleSystem::ParticleSystem(ParticleSystemDefinition const * resource, int layer, Vector2 const & position)
	: m_particleSystemResource(nullptr)
	, m_isDead(false)
	, m_position(Vector2::ZERO)
{
	m_particleSystemResource = resource;
	m_position = position;


	//Get all emitter defs and make em emitters
	//Make the emitters be on this layer
	for each (const ParticleEmitterDefinition* emitterDef in resource->m_emitterResources)
	{
		ParticleEmitter* newEmitter = new ParticleEmitter(emitterDef);
		newEmitter->m_layerID = layer;
		m_emitters.push_back(newEmitter);
	}

	
}

ParticleSystem::~ParticleSystem()
{
	std::vector<Particle2D*>::iterator removeIter = m_particles.begin();
	while (removeIter != m_particles.end()) {
		Particle2D* particle = *removeIter;
		delete particle;
		removeIter = m_particles.erase(removeIter);
	}
}

void ParticleSystem::Update(float deltaSeconds)
{
	UpdateParticles(deltaSeconds);
	CleanUpParticles();
	SpawnParticles(deltaSeconds);
}

void ParticleSystem::UpdateParticles(float deltaSeconds)
{
	for each (Particle2D* particle in m_particles)
	{
		particle->m_age += deltaSeconds;
		particle->m_velocity += particle->m_acceleration * deltaSeconds;

		particle->m_position += particle->m_velocity * deltaSeconds;

		//float alpha = particle->m_sprite->m_color.GetAFloat();

		float percentComplete = particle->m_age / particle->m_maxAge;
		float newAlpha = Lerp(particle->m_tint.GetAFloat(), 0.f, percentComplete);

		
		particle->m_sprite->m_color.SetAFloat(newAlpha);

		particle->SetPosition(particle->m_position);
	}
}

void ParticleSystem::CleanUpParticles()
{
	std::vector<Particle2D*>::iterator removeIter = m_particles.begin();
	while (removeIter != m_particles.end()) {
		Particle2D* particle = *removeIter;
		if (particle->m_age > particle->m_maxAge)
		{
			delete particle;
			removeIter = m_particles.erase(removeIter);
		}
		else
			++removeIter;
	}
}

void ParticleSystem::SpawnParticles(float deltaSeconds)
{
	for each (ParticleEmitter* emitter in m_emitters)
	{
		if (!emitter->m_particleEmitterDefinition->m_isLooping)
			continue;
		emitter->m_emitTimer += deltaSeconds;
		while (emitter->m_emitTimer >= emitter->m_particleEmitterDefinition->m_spawnRate)
		{
			//We do it this way so we can spawn more than one particle per frame
			m_particles.push_back(emitter->SpawnParticle(m_position));
			emitter->m_emitTimer -= emitter->m_particleEmitterDefinition->m_spawnRate;
		}

	}
}

void ParticleSystem::Initialize()
{
	//Do emitter's initial spawning
	for each (ParticleEmitter* emitter in m_emitters)
	{
		for (int i = 0; i < emitter->m_particleEmitterDefinition->m_initialSpawn; i++)
		{
			m_particles.push_back(emitter->SpawnParticle(m_position));
		}
	}
}

void ParticleSystem::SetSpawnPosition(Vector2 const & position)
{
	m_position = position;
}

void ParticleSystem::Destroy()
{
	m_isDead = true;
// 	for each (ParticleEmitter* emitter in m_emitters)
// 	{
// 		
// 	}
}

bool ParticleSystem::IsFinished()
{
	return m_isDead;
}
