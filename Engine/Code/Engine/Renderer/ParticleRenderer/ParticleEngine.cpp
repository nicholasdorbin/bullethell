#include "Engine/Renderer/ParticleRenderer/ParticleEngine.hpp"

#include "Engine/Core/FileUtils.hpp"
#include "Engine/Tools/Parsers/XMLUtilities.hpp"

#include "Engine/Renderer/ParticleRenderer/ParticleSystem.hpp"
#include "Engine/Renderer/ParticleRenderer/ParticleEmitter.hpp"
#include "Engine/Renderer/ParticleRenderer/Particle2D.hpp"
#include "Engine/MathUtils.hpp"
#include "Engine/Math/Vector4.hpp"


ParticleEngine * g_ParticleEngine = nullptr;

ParticleSystemDefinition const * ParticleEngine::GetParticleSystemResource(std::string const & resourceName)
{
	size_t nameHash = std::hash<std::string>{}(resourceName);
	auto foundSystem = m_particleSystemResourceDatabase.find(nameHash);
	if (foundSystem != m_particleSystemResourceDatabase.end())
	{
		return foundSystem->second;
	}
	else
	{
		return nullptr;
	}
}

ParticleEngine::ParticleEngine()
{

}

ParticleEngine::~ParticleEngine()
{

}

void ParticleEngine::Update(float deltaSeconds)
{
	for each (ParticleSystem* system in m_activeParticleSystems)

	{
		system->Update(deltaSeconds);
	}
}

void ParticleEngine::LoadAllParticleSystemDefinitions()
{


	ParticleSystemDefinition* spark = new ParticleSystemDefinition("spark");
	ParticleEmitterDefinition* sparks = new ParticleEmitterDefinition("soft-particle");
	//sparks->material = AdditiveMaterial; // SRC:  SrcAlpha.  DEST: One
	sparks->m_initialSpawn = 100;
	sparks->m_spawnRate = 0.0f;
	sparks->m_life = Range<float>(1.0f, 1.5f);
	//float minXScale = GetRandomFloatBetween(0.5f, 0.7f);
	//float minYScale = GetRandomFloatBetween(0.5f, 0.7f);
	sparks->m_scale = Range<float>(0.5f, 0.7f);
	sparks->m_tint = Rgba(Vector4(.8f, .8f, .1f, .2f));
	sparks->m_isLooping = false;
	spark->AddEmitter(sparks);

	size_t nameHashSpark = std::hash<std::string>{}(spark->id);
	m_particleSystemResourceDatabase.insert(std::pair<size_t, ParticleSystemDefinition*>(nameHashSpark, spark));
	

	ParticleSystemDefinition *smoke = new ParticleSystemDefinition("smoke");

	// Add an emitter to it
	ParticleEmitterDefinition* smokey = new ParticleEmitterDefinition("soft-particle");
	//smokey->material = AlphaBlended; // SRC: SrcAlpha.  DEST:  InvSrcAlpha
	smokey->m_initialSpawn = 10;
	smokey->m_spawnRate = .2f;
	smokey->m_life = Range<float>(2.f, 3.0f);
	smokey->m_scale = Range<float>(0.1f, 0.1f);
	smokey->m_speed = Range<Vector2>(Vector2(-2.f, 2.f), Vector2(2.f, 2.f));
	smokey->m_tint = Rgba(Vector4(.4f, .4f, .42f, 1.f));
	//ASSERT(smoke->is_looping()); // since this one has a spawn rate.
	smokey->m_isLooping = true;
	smoke->AddEmitter(smokey);


	size_t nameHashSmoke = std::hash<std::string>{}(smoke->id);
	m_particleSystemResourceDatabase.insert(std::pair<size_t, ParticleSystemDefinition*>(nameHashSmoke, smoke));
}

void ParticleEngine::DestroyAllParticleSystemDefinitions()
{
	std::map< size_t, ParticleSystemDefinition* >::iterator it = m_particleSystemResourceDatabase.begin();
	while (it != m_particleSystemResourceDatabase.end()) {
		delete(it->second);
		it = m_particleSystemResourceDatabase.erase(it);
	}
}

void ParticleEngine::PlayOnce(std::string const & particleSystemID, int layerID, Vector2 const & position /*= Vector2::ZERO*/)
{
	const ParticleSystemDefinition* thisSystemDef = GetParticleSystemResource(particleSystemID);
	if (thisSystemDef == nullptr)
		return;
	ParticleSystem* newSystem = new ParticleSystem(thisSystemDef, layerID, position);
	newSystem->Initialize();
	newSystem->Destroy();
}

ParticleSystem * ParticleEngine::Create(std::string const & particleSystemID, int layerID, Vector2 const & position /*= Vector2::ZERO*/)
{

	const ParticleSystemDefinition* thisSystemDef = GetParticleSystemResource(particleSystemID);
	if (thisSystemDef == nullptr)
		return nullptr;

	ParticleSystem* newSystem = new ParticleSystem(thisSystemDef, layerID, position);

	m_activeParticleSystems.push_back(newSystem);
	return newSystem;
}
