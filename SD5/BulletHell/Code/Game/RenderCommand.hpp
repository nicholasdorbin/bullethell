#pragma once
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Rgba.hpp"
/*DebugRenderArrow(vec3_fl pos, vec3_fl toward, color_fl const &color,

 float duration, eDepthMode depth_mode = eDepthMode_ZTest)
 */

class RenderCommand
{
public:
	RenderCommand();
	RenderCommand(Vector3 pos, Vector3 fwd, Rgba color, float seconds, bool isDepthTest);

	Vector3		m_postion;
	Vector3		m_forward;
	Rgba		m_color;
	float		m_seconds;
	bool		m_depthTest;
};