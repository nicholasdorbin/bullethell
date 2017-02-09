#pragma once
#include "Engine/MathUtils.hpp"
#include <algorithm>

template <typename T>
class Range
{
public:
	T min;
	T max;
	Range(T _min, T _max)
	{
		//min = std::min(_min, _max);
		if (_min < _max)
		{
			min = _min;
			max = _max;
		}
		else
		{
			min = _max;
			max = _min;
		}
		//max = std::max(_min, _max);
	}
	T get_random() const { return  get(GetRandomFloatBetween(0.f,1.f)); }
	const T get(float t) const
	{
		return Lerp(min, max, ClampFloat(t,0.f,1.f));
	}
};
