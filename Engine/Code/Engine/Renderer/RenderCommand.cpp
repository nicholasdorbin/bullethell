#include "Engine/Renderer/RenderCommand.hpp"

RenderCommand::RenderCommand() : m_type(RenderCommand::POINT)
, m_drawMode(DEPTH_TEST_OFF)
, m_startPostion(Vector3::ZERO)
, m_endPosition(Vector3::ZERO)
, m_mins(Vector3::ZERO)
, m_maxs(Vector3::ZERO)
, m_radius(0.f)
, m_color(Rgba::WHITE)
, m_seconds(0.f)
{};

void RenderCommand::Update(float deltaSeconds)
{
	m_seconds -= deltaSeconds;
}
