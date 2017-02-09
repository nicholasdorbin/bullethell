#include "Engine/Math/AABB3.hpp"

AABB3::AABB3()
{
	m_mins = Vector3();
	m_maxs = Vector3();
}

AABB3::AABB3(Vector3 vecMins, Vector3 vecMax)
{
	m_mins = vecMins;
	m_maxs = vecMax;
}

bool DoBoxesOverlap(const AABB3& a, const AABB3& b)
{
	if (a.m_maxs.x < b.m_mins.x)
		return false;
	if (a.m_mins.x > b.m_maxs.x)
		return false;

	if (a.m_maxs.y < b.m_mins.y)
		return false;
	if (a.m_mins.y > b.m_maxs.y)
		return false;

	if (a.m_maxs.z < b.m_mins.z)
		return false;
	if (a.m_mins.z > b.m_maxs.z)
		return false;

	return true;
}
