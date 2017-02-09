#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Matrix4.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/Vector2.hpp"

Vector3::Vector3()
{
	x = 0.f;
	y = 0.f;
	z = 0.f;
}


Vector3::Vector3(const Vector3& v) : x(v.x), y(v.y), z(v.z)
{

}

Vector3::Vector3(float initialX, float initialY, float initialZ)
{
	x = initialX;
	y = initialY;
	z = initialZ;
}

Vector3::Vector3(float value)
{
	x = value;
	y = value;
	z = value;
}

Vector3::Vector3(const Vector2& v, float initialZ /*= 0.f*/)
{
	x = v.x;
	y = v.y;
	z = initialZ;
}

void Vector3::SetXYZ(float newX, float newY, float newZ)
{
	x = newX;
	y = newY;
	z = newZ;
}

float Vector3::GetMagnitude() const
{
	return (float)sqrt(x*x + y*y + z*z);
}

void Vector3::SetNormalized()
{
	float magnitude = this->GetMagnitude();
	if (magnitude == 0.f)
	{
		x = 0.f;
		y = 0.f;
		z = 0.f;
	}
	else
	{
		x = x / magnitude;
		y = y / magnitude;
		z = z / magnitude;
	}
	
}

Vector3 Vector3::GetNormalized() const
{
	float magnitude = GetMagnitude();
	if (magnitude == 0.f)
		return Vector3(0.f, 0.f, 0.f);

	Vector3 result;
	float invMagnitude = (1.f / magnitude);
	result.x = x * invMagnitude;
	result.y = y * invMagnitude;
	result.z = z * invMagnitude;
	return result;
}

const Vector3 Vector3::operator-() const
{
	Vector3 vec;
	vec.x = -x;
	vec.y = -y;
	vec.z = -z;
	return vec;
}

Vector3 Vector3::ZERO = Vector3(0.f, 0.f, 0.f);
Vector3 Vector3::ONE = Vector3(1.f, 1.f, 1.f);

Vector3 Vector3::UP = Vector3(0.f, 0.f, 1.f);
Vector3 Vector3::RIGHT = Vector3(1.f, 0.f, 0.f);
Vector3 Vector3::FORWARD = Vector3(0.f, 1.f, 0.f);

Vector3& Vector3::operator+=(const Vector3& other)
{
	this->x = x + other.x;
	this->y = y + other.y;
	this->z = z + other.z;
	return *this;
}

const Vector3 Vector3::operator+(const Vector3& other) const
{
	Vector3 result = *this;
	result.x = x + other.x;
	result.y = y + other.y;
	result.z = z + other.z;
	return result;
}

const Vector3 Vector3::operator-(const Vector3& other) const
{
	Vector3 result = *this;
	result.x = x - other.x;
	result.y = y - other.y;
	result.z = z - other.z;
	return result;
}

Vector3& Vector3::operator=(const Vector3& rhs)
{
	x = rhs.x;
	y = rhs.y;
	z = rhs.z;
	return *this;
}

Vector3 Vector3::operator-=(const Vector3& other)
{
	this->x = x - other.x;
	this->y = y - other.y;
	this->z = z - other.z;
	return *this;
}

Vector3 Vector3::operator*(float scalar)
{
	Vector3 result;
	result.x = x * scalar;
	result.y = y * scalar;
	result.z = z * scalar;
	return result;
}

const Vector3 Vector3::operator*(float scalar) const
{
	Vector3 result;
	result.x = x * scalar;
	result.y = y * scalar;
	result.z = z * scalar;
	return result;
}

const Vector3 Vector3::operator*(const Matrix4& mat) const
{
	Vector4 vec4 = Vector4(x, y, z, 0.f);

	Vector4 result = vec4 * mat;

	return Vector3(result.x, result.y, result.z);
}

Vector3 operator*(float scalar, const Vector3& vec)
{
	Vector3 result;
	result.x = vec.x * scalar;
	result.y = vec.y * scalar;
	result.z = vec.z * scalar;
	return result;
}

const Vector3 Vector3::operator/(float scalar) const
{
	Vector3 result;
	result.x = x / scalar;
	result.y = y / scalar;
	result.z = z / scalar;
	return result;
}

bool operator==(const Vector3& lhs, const Vector3& rhs)
{
	if (lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z)
		return true;
	return false;
}

bool operator!=(const Vector3& lhs, const Vector3& rhs)
{
	if (lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z)
		return false;
	return true;
}