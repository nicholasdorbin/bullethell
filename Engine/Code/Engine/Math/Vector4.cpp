#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/Vector3.hpp"
Vector4::Vector4()
{
	x = 0.f;
	y = 0.f;
	z = 0.f;
	w =  0.f;
}


Vector4::Vector4(const Vector4& v) : x(v.x), y(v.y), z(v.z), w(v.w)
{

}

Vector4::Vector4(float initialX, float initialY, float initialZ, float initialW)
{
	x = initialX;
	y = initialY;
	z = initialZ;
	w = initialW;
}

Vector4::Vector4(const Vector3& v, const float newW)
{
	x = v.x;
	y = v.y;
	z = v.z;
	w = newW;
}

void Vector4::SetXYZ(float newX, float newY, float newZ, float newW)
{
	x = newX;
	y = newY;
	z = newZ;
	w = newW;
}

float Vector4::GetMagnitude() const
{
	return (float)sqrt(x*x + y*y + z*z + w*w);
}

void Vector4::SetNormalized()
{
	float magnitude = this->GetMagnitude();
	if (magnitude == 0.f)
	{
		x = 0.f;
		y = 0.f;
		z = 0.f;
		w = 0.f;
	}
	else
	{
		x = x / magnitude;
		y = y / magnitude;
		z = z / magnitude;
		w = w / magnitude;
	}

}

Vector4 Vector4::GetNormalized() const
{
	float magnitude = GetMagnitude();
	if (magnitude == 0.f)
		return Vector4(0.f, 0.f, 0.f, 0.f);

	Vector4 result;
	float invMagnitude = (1.f / magnitude);
	result.x = x * invMagnitude;
	result.y = y * invMagnitude;
	result.z = z * invMagnitude;
	return result;
}

Vector3 Vector4::GetXYZ()
{
	Vector3 result;
	result.x = x;
	result.y = y;
	result.z = z;
	return result;
}

const Vector4 Vector4::operator-() const
{
	Vector4 vec;
	vec.x = -x;
	vec.y = -y;
	vec.z = -z;
	vec.w = -w;
	return vec;
}

Vector4 Vector4::ZERO = Vector4(0.f, 0.f, 0.f, 0.f);
Vector4 Vector4::ONE = Vector4(1.f, 1.f, 1.f, 1.f);

Vector4& Vector4::operator+=(const Vector4 other)
{
	this->x = x + other.x;
	this->y = y + other.y;
	this->z = z + other.z;
	this->w = w + other.w;
	return *this;
}

const Vector4 Vector4::operator+(const Vector4 other) const
{
	Vector4 result = *this;
	result.x = x + other.x;
	result.y = y + other.y;
	result.z = z + other.z;
	result.w = w + other.w;
	return result;
}

const Vector4 Vector4::operator-(const Vector4 other) const
{
	Vector4 result = *this;
	result.x = x - other.x;
	result.y = y - other.y;
	result.z = z - other.z;
	result.w = w - other.w;
	return result;
}

Vector4& Vector4::operator=(const Vector4 &rhs)
{
	x = rhs.x;
	y = rhs.y;
	z = rhs.z;
	w = rhs.w;
	return *this;
}

Vector4 Vector4::operator-=(const Vector4 other)
{
	this->x = x - other.x;
	this->y = y - other.y;
	this->z = z - other.z;
	this->w = w - other.w;
	return *this;
}

Vector4 Vector4::operator*(float scalar)
{
	Vector4 result;
	result.x = x * scalar;
	result.y = y * scalar;
	result.z = z * scalar;
	result.w = w * scalar;
	return result;
}

const Vector4 Vector4::operator*(const float& scalar) const
{
	Vector4 result;
	result.x = x * scalar;
	result.y = y * scalar;
	result.z = z * scalar;
	result.w = w * scalar;
	return result;
}

Vector4 operator*(float scalar, const Vector4& vec)
{
	Vector4 result;
	result.x = vec.x * scalar;
	result.y = vec.y * scalar;
	result.z = vec.z * scalar;
	result.w = vec.w * scalar;
	return result;
}

const Vector4 Vector4::operator/(const float& scalar) const
{
	Vector4 result;
	result.x = x / scalar;
	result.y = y / scalar;
	result.z = z / scalar;
	result.w = w / scalar;
	return result;
}

bool operator==(const Vector4& lhs, const Vector4& rhs)
{
	if (lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z && lhs.w == rhs.w)
		return true;
	return false;
}