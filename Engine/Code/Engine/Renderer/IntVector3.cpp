#include "Engine/Math/IntVector3.hpp"

IntVector3::IntVector3()
{
	x = 0;
	y = 0;
	z = 0;
}

IntVector3::IntVector3(int initialX, int initialY, int initialZ)
{
	x = initialX;
	y = initialY;
	z = initialZ;
}
IntVector3::IntVector3(const IntVector3& v) : x(v.x), y(v.y), z(v.z)
{

}

void IntVector3::SetXY(int newX, int newY, int newZ)
{
	x = newX;
	y = newY;
	z = newZ;
}

float IntVector3::GetMagnitude()
{
	float xf = (float)x;
	float yf = (float)y;
	float zf = (float)z;
	return sqrt(xf*xf + yf*yf + zf*zf);
}

IntVector3& IntVector3::operator=(const IntVector3 &rhs)
{
	x = rhs.x;
	y = rhs.y;
	z = rhs.z;
	return *this;
}

const IntVector3 IntVector3::operator+(const IntVector3 other) const
{
	IntVector3 result = *this;
	result.x = x + other.x;
	result.y = y + other.y;
	result.z = z + other.z;
	return result;
}

IntVector3& IntVector3::operator+=(const IntVector3 other)
{
	this->x = x + other.x;
	this->y = y + other.y;
	this->z = z + other.z;
	return *this;
}

const IntVector3 IntVector3::operator-(const IntVector3 other) const
{
	IntVector3 result = *this;
	result.x = x - other.x;
	result.y = y - other.y;
	result.z = z - other.z;
	return result;
}

IntVector3 IntVector3::operator-=(const IntVector3 other)
{
	this->x = x - other.x;
	this->y = y - other.y;
	return *this;
}

IntVector3 IntVector3::operator*(int scalar)
{
	IntVector3 result;
	result.x = x * scalar;
	result.y = y * scalar;
	result.z = z * scalar;
	return result;
}

const IntVector3 IntVector3::operator*(const int& scalar) const
{
	IntVector3 result;
	result.x = x * scalar;
	result.y = y * scalar;
	result.z = z * scalar;
	return result;
}
IntVector3 operator*(int scalar, const IntVector3& vec)
{
	IntVector3 result;
	result.x = vec.x * scalar;
	result.y = vec.y * scalar;
	result.z = vec.z * scalar;
	return result;
}


bool operator==(const IntVector3& lhs, const IntVector3& rhs)
{
	if (lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z)
		return true;
	return false;
}

bool operator!=(const IntVector3& lhs, const IntVector3& rhs)
{
	if (lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z)
		return false;
	return true;
}