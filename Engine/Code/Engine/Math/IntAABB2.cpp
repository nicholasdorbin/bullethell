#include "Engine/Math/IntAABB2.hpp"

IntAABB2::IntAABB2()
{
	m_mins = IntVector2();
	m_maxs = IntVector2();
}

IntAABB2::IntAABB2(IntVector2 vecMins, IntVector2 vecMax)
{
	m_mins = vecMins;
	m_maxs = vecMax;
}

void IntAABB2::ShiftX(int val)
{
	m_mins.x += val;
	m_maxs.x += val;
}

void IntAABB2::ShiftY(int val)
{
	m_mins.y += val;
	m_maxs.y += val;
}
