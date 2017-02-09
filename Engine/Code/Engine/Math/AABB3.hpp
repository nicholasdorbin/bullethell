#pragma once
#include "Engine/Math/Vector3.hpp"

class AABB3
{
public:
	AABB3();
	AABB3(Vector3 vecMins, Vector3 vecMax);
	Vector3 m_mins;
	Vector3 m_maxs;
};

bool DoBoxesOverlap(const AABB3& a, const AABB3& b);