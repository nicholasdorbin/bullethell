#pragma once
#include "Engine/Math/Vector2.hpp"

class AABB2
{
public:
	AABB2();
	AABB2(Vector2 vecMins, Vector2 vecMax);
	Vector2 m_mins;
	Vector2 m_maxs;
};