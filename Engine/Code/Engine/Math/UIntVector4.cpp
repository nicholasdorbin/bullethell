#include "Engine/Math/UIntVector4.hpp"

UIntVector4::UIntVector4()
{
	x = 0;
	y = 0;
	z = 0;
	w = 0;
}


UIntVector4::UIntVector4(const UIntVector4& v) : x(v.x), y(v.y), z(v.z), w(v.w)
{

}

UIntVector4::UIntVector4(unsigned int initialX, unsigned int initialY, unsigned int initialZ, unsigned int initialW)
{
	x = initialX;
	y = initialY;
	z = initialZ;
	w = initialW;
}



void UIntVector4::SetXYZ(unsigned int newX, unsigned int newY, unsigned int newZ, unsigned int newW)
{
	x = newX;
	y = newY;
	z = newZ;
	w = newW;
}

unsigned int UIntVector4::GetMagnitude() const
{
	return (unsigned int)sqrt(x*x + y*y + z*z + w*w);
}

void UIntVector4::SetNormalized()
{
	unsigned int magnitude = this->GetMagnitude();
	if (magnitude == 0)
	{
		x = 0;
		y = 0;
		z = 0;
		w = 0;
	}
	else
	{
		x = x / magnitude;
		y = y / magnitude;
		z = z / magnitude;
		w = w / magnitude;
	}

}

UIntVector4 UIntVector4::GetNormalized() const
{
	unsigned int magnitude = GetMagnitude();
	if (magnitude == 0)
		return UIntVector4(0, 0, 0, 0);

	UIntVector4 result;
	unsigned int invMagnitude = (1 / magnitude);
	result.x = x * invMagnitude;
	result.y = y * invMagnitude;
	result.z = z * invMagnitude;
	return result;
}



UIntVector4 UIntVector4::ZERO = UIntVector4(0, 0, 0, 0);
UIntVector4 UIntVector4::ONE = UIntVector4(1, 1, 1, 1);

UIntVector4& UIntVector4::operator+=(const UIntVector4 other)
{
	this->x = x + other.x;
	this->y = y + other.y;
	this->z = z + other.z;
	this->w = w + other.w;
	return *this;
}

const UIntVector4 UIntVector4::operator+(const UIntVector4 other) const
{
	UIntVector4 result = *this;
	result.x = x + other.x;
	result.y = y + other.y;
	result.z = z + other.z;
	result.w = w + other.w;
	return result;
}

const UIntVector4 UIntVector4::operator-(const UIntVector4 other) const
{
	UIntVector4 result = *this;
	result.x = x - other.x;
	result.y = y - other.y;
	result.z = z - other.z;
	result.w = w - other.w;
	return result;
}

UIntVector4& UIntVector4::operator=(const UIntVector4 &rhs)
{
	x = rhs.x;
	y = rhs.y;
	z = rhs.z;
	w = rhs.w;
	return *this;
}

UIntVector4 UIntVector4::operator-=(const UIntVector4 other)
{
	this->x = x - other.x;
	this->y = y - other.y;
	this->z = z - other.z;
	this->w = w - other.w;
	return *this;
}

UIntVector4 UIntVector4::operator*(unsigned int scalar)
{
	UIntVector4 result;
	result.x = x * scalar;
	result.y = y * scalar;
	result.z = z * scalar;
	result.w = w * scalar;
	return result;
}

const UIntVector4 UIntVector4::operator*(const unsigned int& scalar) const
{
	UIntVector4 result;
	result.x = x * scalar;
	result.y = y * scalar;
	result.z = z * scalar;
	result.w = w * scalar;
	return result;
}

UIntVector4 operator*(unsigned int scalar, const UIntVector4& vec)
{
	UIntVector4 result;
	result.x = vec.x * scalar;
	result.y = vec.y * scalar;
	result.z = vec.z * scalar;
	result.w = vec.w * scalar;
	return result;
}

const UIntVector4 UIntVector4::operator/(const unsigned int& scalar) const
{
	UIntVector4 result;
	result.x = x / scalar;
	result.y = y / scalar;
	result.z = z / scalar;
	result.w = w / scalar;
	return result;
}

bool operator==(const UIntVector4& lhs, const UIntVector4& rhs)
{
	if (lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z && lhs.w == rhs.w)
		return true;
	return false;
}