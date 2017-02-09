#pragma once
#include "Engine/Math/IntVector2.hpp"

class IntAABB2
{
public:
	IntAABB2();
	IntAABB2(IntVector2 vecMins, IntVector2 vecMax);
	IntVector2 m_mins;
	IntVector2 m_maxs;

	void ShiftX(int val);
	void ShiftY(int val);
};