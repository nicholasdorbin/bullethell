#include "Engine/Time/Clock.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"


Clock::Clock(double startTime)
{
	m_currentTime = startTime;
	m_previousTime = startTime;
	m_paused = false;
	m_scale = 1.f;
}

Clock::Clock()
{
	m_currentTime = g_systemClock->GetCurrent();
	m_previousTime = m_currentTime;
	m_paused = false;
	m_scale = 1.f;
}

Clock::Clock(Clock* parentClock)
{
	m_currentTime = parentClock->m_currentTime;
	m_previousTime = parentClock->m_previousTime;
	m_paused = false;
	m_scale = 1.f;
}

Clock::~Clock()
{
	for each(Clock* childClock in m_children)
	{
		delete childClock;
	}
}

double Clock::GetCurrent() const
{
	return m_currentTime;
}

double Clock::GetDelta() const
{
	return (m_currentTime - m_previousTime);
}

void Clock::Update(double deltaSeconds)
{
	double dt = deltaSeconds;
	if(m_paused)
		dt = 0;
	else
	{
		dt *= m_scale;
	}
	m_previousTime = m_currentTime;
	m_currentTime += dt;



	for each(Clock* childClock in m_children)
	{
		childClock->Update(dt);
	}
}

void Clock::AddChild(Clock* childClock)
{
	m_children.push_back(childClock);
}
