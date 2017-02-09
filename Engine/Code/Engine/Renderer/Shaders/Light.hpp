#pragma once

#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"


//-----------------------------------------------------------------------------------------------
class Light
{
public:
	Light();
	Light(const Vector4& color, const Vector3& position, const Vector3& direction, float minLightDist, float maxLightDist,
		float powerAtMin, float powerAtMax, float dirInterp,
		float thetaInner, float thetaOuter,
		float thetaInnerPower, float thetaOuterPower);

	Light( const Vector4& color, const Vector3& pos, float radius);
public:
	Vector3 m_position;
	Vector3 m_direction;
	Vector4 m_color;

	float m_minLightDistance;
	float m_maxLightDistance;
	float m_powerAtMin;
	float m_powerAtMax;
	float m_directionalInterp;
	float m_thetaInner;
	float m_thetaOuter;
	float m_thetaInnerPower;
	float m_thetaOuterPower;
};