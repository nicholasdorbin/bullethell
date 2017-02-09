#include "Engine/CinematicCamera/SceneKeyframe.hpp"

SceneKeyframe::SceneKeyframe()
	: position(Vector3::ZERO)
	, rotation(Quaternion::IDENTITY_POS)
	, timeAt(0.f)
	, fov(70.f)
	, lookAtState(LOOKAT_NOCHANGE)
	, lookAtTargetPosition(Vector3::ZERO)
{

}

SceneKeyframe::SceneKeyframe(Vector3 pos, Quaternion rot, float time)
{
	position = pos;
	rotation = rot;
	timeAt = time;
	entryVelocity = Vector3::ZERO;
	exitVelocity = Vector3::ZERO;
	fov = 70.f;
	lookAtState = LOOKAT_NOCHANGE;
	lookAtTargetPosition = Vector3::ZERO;
}

SceneKeyframe::SceneKeyframe(Vector3 pos, Quaternion rot, float time, Vector3 entryVel, Vector3 exitVel)
{
	position = pos;
	rotation = rot;
	timeAt = time;
	entryVelocity = entryVel;
	exitVelocity = exitVel;
	fov = 70.f;
	lookAtState = LOOKAT_NOCHANGE;
	lookAtTargetPosition = Vector3::ZERO;
}

void SceneKeyframe::SetBothVelocities(Vector3 velocity)
{
	exitVelocity = velocity;
	entryVelocity = velocity;
}
