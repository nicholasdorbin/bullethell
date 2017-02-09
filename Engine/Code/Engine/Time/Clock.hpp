#pragma once
#include <vector>

class Clock
{
public: 
	bool m_paused;
	double m_scale;
	Clock();
	Clock(double startTime);
	Clock(Clock* parentClock);
	~Clock();
	std::vector<Clock*> m_children;
	double m_currentTime;
	double m_previousTime;

	double GetCurrent() const;
	double GetDelta() const;
	void Update(double deltaSeconds);
	void AddChild(Clock* childClock);
};