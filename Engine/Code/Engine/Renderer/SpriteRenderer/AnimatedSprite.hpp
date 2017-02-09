#pragma once
#include "Engine/Renderer/SpriteRenderer/Sprite.hpp"

#include <vector>
#include <string>
class SpriteResource;
struct XMLNode;
class Clock;

enum eLoopMode
{
	ANIM_SEQUENCE_MODE_PLAY_TO_END,	// Play from time=0 to durationSeconds, then finish
	ANIM_SEQUENCE_MODE_LOOPING,		// Play from time=0 to end then repeat (never finish)
	ANIM_SEQUENCE_MODE_PINGPONG, 	// optional, play forwards, backwards, forwards...
	NUM_ANIM_SEQUENCE_MODES
};

struct frame_t
{
	const SpriteResource* m_spriteResource;
	float m_startTime;
};

class AnimationSequence
{
public:
	AnimationSequence(XMLNode rootNode);
	~AnimationSequence();
	std::string m_name;
 	std::vector<frame_t> m_frames;
 	eLoopMode m_loopMode;
 	float m_endTime; //duration of anim
 
 	const SpriteResource* GetFrame(float currentTime);
 	float UpdateTime(float t, float dt);

};

class AnimatedSprite : public Sprite
{
private:
	AnimatedSprite();
	AnimatedSprite(AnimationSequence* sequence, int layer = 0);
public:
	~AnimatedSprite();
	AnimationSequence* m_sequence;
	Clock* m_animClock;
	float m_currentTime;
	float m_playRate;
	bool m_isPlaying;
	bool m_isFinished;


	static AnimatedSprite * Create(std::string const & spriteAnimID, int layer = 0);
	void Update();
	/*
	void Update(float dt)
	{
		
	}
	*/
};
