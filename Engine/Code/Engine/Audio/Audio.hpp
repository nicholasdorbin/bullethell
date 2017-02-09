#ifndef INCLUDED_AUDIO
#define INCLUDED_AUDIO
#pragma once

//---------------------------------------------------------------------------
#pragma comment( lib, "Thirdparty/fmod/fmodex_vc" ) // Link in the fmodex_vc.lib static library

//---------------------------------------------------------------------------
#include "ThirdParty/fmod/fmod.hpp"
#include <string>
#include <vector>
#include <map>


//---------------------------------------------------------------------------
typedef unsigned int SoundID;
typedef void* AudioChannelHandle;
const unsigned int MISSING_SOUND_ID = 0xffffffff;


/////////////////////////////////////////////////////////////////////////////
class AudioSystem
{
public:
	AudioSystem::AudioSystem();
	virtual AudioSystem::~AudioSystem();
	SoundID AudioSystem::CreateOrGetSound(const std::string& soundFileName);
	void AudioSystem::PlaySound(SoundID soundID, float volumeLevel = 1.f, bool looping = false);
	void AudioSystem::StopChannel(AudioChannelHandle channel);
	void AudioSystem::StopSound(SoundID soundID);
	void AudioSystem::Update(float deltaSeconds); // Must be called at regular intervals (e.g. every frame)

protected:
	void AudioSystem::InitializeFMOD();
	void AudioSystem::ValidateResult(FMOD_RESULT result);

protected:
	FMOD::System*						m_fmodSystem;
	std::map< std::string, SoundID >	m_registeredSoundIDs;
	std::vector< FMOD::Sound* >			m_registeredSounds;
	std::map< SoundID, FMOD::Channel* >			m_registeredChannels;
};


//---------------------------------------------------------------------------
void InitializeAudio();


#endif // INCLUDED_AUDIO
