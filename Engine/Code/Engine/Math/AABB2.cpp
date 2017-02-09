#include "Engine/Math/AABB2.hpp"

AABB2::AABB2()
{
	m_mins = Vector2();
	m_maxs = Vector2();
}

AABB2::AABB2(Vector2 vecMins, Vector2 vecMax)
{
	m_mins = vecMins;
	m_maxs = vecMax;
}
