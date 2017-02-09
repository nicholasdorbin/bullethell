#pragma once
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Quaternion.hpp"

enum eLookAtState
{
	LOOKAT_ENABLE = 0,
	LOOKAT_DISABLE,
	LOOKAT_NOCHANGE
};

struct SceneKeyframe
{
	Vector3 position;
	Quaternion rotation;
	float timeAt;
	Vector3 entryVelocity;
	Vector3 exitVelocity;
	float fov;
	eLookAtState lookAtState;
	Vector3 lookAtTargetPosition;

	SceneKeyframe();
	SceneKeyframe(Vector3 pos, Quaternion rot, float time);
	SceneKeyframe(Vector3 pos, Quaternion rot, float time, Vector3 entryVel, Vector3 exitVel);

	void SetBothVelocities(Vector3 velocity);
};