#pragma once
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Rgba.hpp"
/*DebugRenderArrow(vec3_fl pos, vec3_fl toward, color_fl const &color,

float duration, eDepthMode depth_mode = eDepthMode_ZTest)
*/

class RenderCommand
{
public:

	enum CommandType
	{
		POINT,
		LINE,
		ARROW,
		AABB3,
		SPHERE
	};

	enum DrawMode
	{
		DEPTH_TEST_OFF,
		DEPTH_TEST_ON,
		XRAY
	};

	RenderCommand();
	

	CommandType m_type;
	DrawMode	m_drawMode;
	Vector3		m_startPostion;
	Vector3		m_endPosition;
	Vector3		m_mins;
	Vector3		m_maxs;
	float		m_radius;
	Rgba		m_color;
	float		m_seconds;

	void Update(float deltaSeconds);
};