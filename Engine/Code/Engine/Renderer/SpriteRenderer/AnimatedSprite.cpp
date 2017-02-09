#include "Engine/Renderer/SpriteRenderer/AnimatedSprite.hpp"
#include "Engine/Renderer/SpriteRenderer/SpriteResource.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Tools/Parsers/XMLUtilities.hpp"
#include "Engine/Renderer/SpriteRenderer/SpriteGameRenderer.hpp"
#include "Engine/Renderer/SpriteRenderer/SpriteLayer.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Time/Clock.hpp"
#include "Engine/Core/EngineCommon.hpp"

AnimationSequence::AnimationSequence(XMLNode rootNode)
{
	if (rootNode.getAttribute("id") && rootNode.getAttribute("endTime"))
	{
		//m_name = root.getAttribute("id");
		m_name = ReadXMLAttribute(rootNode, "id", "");
		m_endTime = ReadXMLAttribute(rootNode, "endTime", 0.f);

		std::string loopMode = ReadXMLAttribute(rootNode, "loopMode", "");

		if (loopMode == "loop")
			m_loopMode = ANIM_SEQUENCE_MODE_LOOPING;
		else if (loopMode == "none")
		{
			m_loopMode = ANIM_SEQUENCE_MODE_PLAY_TO_END;
		}
		else if (loopMode == "pingpong")
		{
			m_loopMode = ANIM_SEQUENCE_MODE_PINGPONG;
		}

		int frameCount = rootNode.nChildNode(); //# of Frame nodes

		for (int i = 0; i < frameCount; i++)
		{
			XMLNode frameNode = rootNode.getChildNode(i);
			std::string name = frameNode.getName();
			if (name != "Frame")
			{
				//Not valid node, skip
				continue;
			}
			frame_t newFrame;
			std::string resourceName = ReadXMLAttribute(frameNode, "resourceID", "");
			frameNode;
			const SpriteResource* frameResource = g_spriteRenderSystem->GetSpriteResource(resourceName);
			if (frameResource == nullptr)
				ERROR_AND_DIE("Could not load Sprite Anim Resource: " + resourceName);

			newFrame.m_spriteResource = frameResource;
			newFrame.m_startTime = ReadXMLAttribute(frameNode, "time", 0.f);

			m_frames.push_back(newFrame);
		}
	}
}

AnimationSequence::~AnimationSequence()
{
}

const SpriteResource* AnimationSequence::GetFrame(float currentTime)
{
	frame_t currentFrame = m_frames[0];
	int i = 0;
	for each (frame_t frame in m_frames)
	{
		if (frame.m_startTime <= currentTime)
		{
			currentFrame = frame; 
			i++;
		}
		else
		{
			break;
		}
	}

	return currentFrame.m_spriteResource;
}

float AnimationSequence::UpdateTime(float t, float dt)
{
	float nextTime = t + dt;

	if (m_loopMode == ANIM_SEQUENCE_MODE_LOOPING)
	{
		if (nextTime > m_endTime)
			nextTime = 0;
	}
	else if (m_loopMode == ANIM_SEQUENCE_MODE_PLAY_TO_END)
	{
		if (nextTime > m_endTime)
		{
			nextTime = m_endTime;
		}
	}
	return nextTime;
}


AnimatedSprite::AnimatedSprite(AnimationSequence* sequence, int layer /*= 0*/)
	:Sprite(sequence->m_frames[0].m_spriteResource->GetID(), layer)
	, m_sequence(sequence)
	, m_currentTime(0)
	, m_isFinished(false)
	, m_isPlaying(false)
	, m_playRate(1.f)
{

}

AnimatedSprite::~AnimatedSprite()
{
	//AnimSequences get deleted via the database
	//delete m_sequence;
}

AnimatedSprite * AnimatedSprite::Create(std::string const & spriteAnimID, int layer /*= 0*/)
{
	//AnimationSequence* newSprite = new AnimatedSprite(spriteID, layer);

	AnimationSequence* sequence = g_spriteRenderSystem->GetAnimationSequence(spriteAnimID);
	if (sequence == nullptr)
		ERROR_AND_DIE("Could not load sprite: " + spriteAnimID);

	AnimatedSprite* newSprite = new AnimatedSprite(sequence, layer);
	newSprite->m_spriteResource = g_spriteRenderSystem->GetSpriteResource(sequence->m_frames[0].m_spriteResource->GetID());
	newSprite->m_material = newSprite->m_spriteResource->GetMaterial();
	//Add it to Game Renderer's list
	newSprite->m_animClock = new Clock(g_systemClock);
	g_systemClock->AddChild(newSprite->m_animClock);
	g_spriteRenderSystem->CreateOrGetLayer(layer)->AddSprite(newSprite);

	return newSprite;
}

void AnimatedSprite::Update()
{
	float dt = (float)m_animClock->GetDelta() * m_playRate;
	if (m_isPlaying)
	{
		m_currentTime = m_sequence->UpdateTime(m_currentTime, dt);
		m_spriteResource = m_sequence->GetFrame(m_currentTime);
		if (m_spriteResource == nullptr)
			ERROR_AND_DIE("Could not load spriteAnim");
		if (m_sequence->m_loopMode == ANIM_SEQUENCE_MODE_PLAY_TO_END)
		{
			if (m_currentTime > m_sequence->m_endTime)
			{
				m_isFinished = true;
				m_isPlaying = false;
			}
		}
		m_material = m_spriteResource->GetMaterial();
	}
}
