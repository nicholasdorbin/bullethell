#include "Game/RenderCommand.hpp"

RenderCommand::RenderCommand()
{
	m_postion = Vector3(0.f,0.f,0.f);
	m_forward = Vector3(0.f, 0.f, 0.f);
	m_color = Rgba::WHITE;
	m_seconds = 0.f;
	m_depthTest = false;
}

RenderCommand::RenderCommand(Vector3 pos, Vector3 fwd, Rgba color, float seconds, bool isDepthTest)
{
	m_postion = pos;
	m_forward = fwd;
	m_color = color;
	m_seconds = seconds;
	m_depthTest = isDepthTest;
}
