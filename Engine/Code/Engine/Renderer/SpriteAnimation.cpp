#include "Engine/Renderer/SpriteAnimation.hpp"

SpriteAnimation::SpriteAnimation(SpriteSheet& spriteSheet, float durationSeconds, SpriteAnimMode mode, int startSpriteIndex, int endSpriteIndex)
{
	m_spriteSheet = &spriteSheet;
	m_mode = mode;
	m_startIndex = startSpriteIndex;
	m_curIndex = m_startIndex;
	m_endIndex = endSpriteIndex;
	m_isFinished = false;
	m_isPlaying = true;
	m_durationSeconds = durationSeconds;
	m_elapsedSeconds = 0.f;
}

//NOTE: Update must be called before Render is done
void SpriteAnimation::Update(float deltaSeconds)
{
	if (m_isPlaying)
	{

		m_elapsedSeconds += deltaSeconds;
		CalcCurrentFrame();
		if (m_mode == SPRITE_ANIM_MODE_PLAY_TO_END)
		{
			if (m_curIndex > m_endIndex || m_durationSeconds < m_elapsedSeconds)
			{
				Pause();
				//SetSecondsElapsed(0.f);
				m_isFinished = true;
			}
		}
		else if (m_mode == SPRITE_ANIM_MODE_LOOPING)
		{
			if (m_curIndex > m_endIndex || m_durationSeconds < m_elapsedSeconds)
			{
				Reset();
			}
		}
	}
}

AABB2 SpriteAnimation::GetCurrentTexCoords() const
{
	return m_spriteSheet->GetTexCoordsForSpriteIndex(m_curIndex);
}

Texture* SpriteAnimation::GetTexture() const
{
	return m_spriteSheet->m_spriteSheetTexture;
}

void SpriteAnimation::Pause()
{
	m_isPlaying = false;
}

void SpriteAnimation::Resume()
{
	m_isPlaying = true;
}

void SpriteAnimation::Reset()
{
	m_curIndex = m_startIndex;
	SetSecondsElapsed(0.f);
}

void SpriteAnimation::CalcCurrentFrame()
{
	float framesPerSecond = (m_endIndex - m_startIndex + 1) / m_durationSeconds;
	float cur = m_elapsedSeconds * framesPerSecond;
	cur = floor(cur);
	m_curIndex = (int)cur + m_startIndex;
}

float SpriteAnimation::GetSecondsRemaining() const
{
	float elapsed = m_durationSeconds - m_elapsedSeconds;
	return elapsed;
}

float SpriteAnimation::GetFractionElapsed() const
{
	float elapsedFrac = m_durationSeconds / m_elapsedSeconds;
	return elapsedFrac;
}

float SpriteAnimation::GetFractionRemaining() const
{
	float elapsedFrac = GetFractionElapsed();
	float remainingFrac = 1 - elapsedFrac;
	return remainingFrac;
}

float SpriteAnimation::SetSecondsElapsed(float secondsElapsed)
{
	m_elapsedSeconds = secondsElapsed;
	return m_elapsedSeconds;
}

float SpriteAnimation::SetFractionElapsed(float fractionElapsed)
{
	float seconds = m_durationSeconds * fractionElapsed;
	m_elapsedSeconds = seconds;
	return m_elapsedSeconds;
}

