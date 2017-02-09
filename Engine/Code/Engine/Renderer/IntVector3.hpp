#pragma once
#include <math.h>

class IntVector3
{
public:
	IntVector3();
	IntVector3(int initialX, int initialY, int initialZ);
	IntVector3(const IntVector3& v);
	void SetXY(int newX, int newY, int newZ);
	float GetMagnitude();

	const IntVector3 IntVector3::operator+(const IntVector3 other) const;
	const IntVector3 IntVector3::operator-(const IntVector3 other) const;
	IntVector3& IntVector3::operator=(const IntVector3 &rhs);
	IntVector3& IntVector3::operator+=(const IntVector3 other);
	IntVector3 IntVector3::operator-=(const IntVector3 other);
	IntVector3 operator* (int scalar);
	const IntVector3 operator*(const int& scalar) const;
	friend IntVector3 operator*(int scalar, const IntVector3& vec);
	friend bool operator==(const IntVector3& lhs, const IntVector3& rhs);
	friend bool operator!=(const IntVector3& lhs, const IntVector3& rhs);

	int x;
	int y;
	int z;
};


