#pragma once
#include "Engine/Renderer/SpriteSheet.hpp"
enum SpriteAnimMode
{
	SPRITE_ANIM_MODE_PLAY_TO_END,	// Play from time=0 to durationSeconds, then finish
	SPRITE_ANIM_MODE_LOOPING,		// Play from time=0 to end then repeat (never finish)
	//  SPRITE_ANIM_MODE_PINGPONG, 	// optional, play forwards, backwards, forwards...
	NUM_SPRITE_ANIM_MODES
};

class SpriteAnimation
{
public:
	SpriteAnimation(SpriteSheet& spriteSheet, float durationSeconds,
		SpriteAnimMode mode, int startSpriteIndex, int endSpriteIndex);

	SpriteSheet* m_spriteSheet;
	SpriteAnimMode m_mode;
	int m_startIndex;
	int m_curIndex;
	int m_endIndex;
	bool m_isFinished;
	bool m_isPlaying;
	float m_durationSeconds;
	float m_elapsedSeconds;

	void Update(float deltaSeconds);
	AABB2 GetCurrentTexCoords() const;	// Based on the current elapsed time
	Texture* GetTexture() const;
	void Pause();					// Starts paused by default
	void Resume();				// Resume after pausing
	void Reset();					// Rewinds to time 0 and starts (re)playing
	bool IsFinished() const			{ return m_isFinished; }
	bool IsPlaying() const			{ return m_isPlaying; }
	void CalcCurrentFrame();
	float GetDurationSeconds() const	{ return m_durationSeconds; }
	float GetSecondsElapsed() const	{ return m_elapsedSeconds; }
	float GetSecondsRemaining() const;
	float GetFractionElapsed() const;
	float GetFractionRemaining() const;
	float SetSecondsElapsed(float secondsElapsed);	    // Jump to specific time
	float SetFractionElapsed(float fractionElapsed);    // e.g. 0.33f for one-third in
};
