#pragma once
#include <math.h>
#include <string>

class IntVector2
{
public:
	IntVector2();
	IntVector2(int initialX, int initialY);
	IntVector2(const IntVector2& v);
	IntVector2(const std::string vectorAsString);
	void SetXY(int newX, int newY);
	float GetMagnitude();
	std::string ToString();

	const IntVector2 IntVector2::operator+(const IntVector2 other) const;
	const IntVector2 IntVector2::operator-(const IntVector2 other) const;
	IntVector2& IntVector2::operator=(const IntVector2 &rhs);
	IntVector2& IntVector2::operator+=(const IntVector2 other);
	IntVector2 IntVector2::operator-=(const IntVector2 other);
	IntVector2 operator* (int scalar);
	const IntVector2 operator*(const int& scalar) const;
	friend IntVector2 operator*(int scalar, const IntVector2& vec);
	friend bool operator==(const IntVector2& lhs, const IntVector2& rhs);
	friend bool operator!=(const IntVector2& lhs, const IntVector2& rhs);
	bool IntVector2::operator < (const IntVector2& rhs);
	friend bool operator<(const IntVector2& lhs, const IntVector2& rhs);

	int x;
	int y;
};


