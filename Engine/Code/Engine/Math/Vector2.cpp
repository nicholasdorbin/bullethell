#include "Engine/Math/Vector2.hpp"

Vector2::Vector2()
{
	x = 0.f;
	y = 0.f;
}

Vector2::Vector2(float initialX, float initialY)
{
	x = initialX;
	y = initialY;
}
Vector2::Vector2(const Vector2& v) : x(v.x), y(v.y)
{

}

void Vector2::SetXY(float newX, float newY)
{
	x = newX;
	y = newY;
}

float Vector2::GetMagnitude()
{
	return (float)sqrt(x*x + y*y);
}

void Vector2::SetNormalized()
{
	float magnitude = this->GetMagnitude();
	x = x / magnitude;
	y = y / magnitude;
}

Vector2 Vector2::GetNormalized()
{
	float magnitude = this->GetMagnitude();
	Vector2 result;
	result.x = x / magnitude;
	result.y = y / magnitude;
	return result;
}

Vector2& Vector2::operator=(const Vector2 &rhs)
{
	x = rhs.x;
	y = rhs.y;
	return *this;
}

const Vector2 Vector2::operator+(const Vector2 other) const
{
	Vector2 result = *this;
	result.x = x + other.x;
	result.y = y + other.y;
	return result;
}

Vector2& Vector2::operator+=(const Vector2 other)
{
	this->x = x + other.x;
	this->y = y + other.y;
	return *this;
}

const Vector2 Vector2::operator-(const Vector2 other) const
{
	Vector2 result = *this;
	result.x = x - other.x;
	result.y = y - other.y;
	return result;
}

 Vector2 Vector2::operator-=(const Vector2 other)
{
	 this->x = x - other.x;
	 this->y = y - other.y;
	 return *this;
}

 const Vector2 Vector2::operator-() const
 {
	 Vector2 vec;
	 vec.x = -x;
	 vec.y = -y;
	 return vec;
 }


 const Vector2 Vector2::operator*(const Vector2 other) const
 {
	 Vector2 result = *this;
	 result.x = x * other.x;
	 result.y = y * other.y;
	 return result;
 }

 Vector2 Vector2::operator*(float scalar)
 {
	 Vector2 result;
	 result.x = x * scalar;
	 result.y = y * scalar;
	 return result;
 }
 
 Vector2 Vector2::operator*=(const float& scalar)
 {
	 Vector2 result;
	 result.x = x * scalar;
	 result.y = y * scalar;
	 return result;

 }

 const Vector2 Vector2::operator*(const float& scalar) const
 {
	 Vector2 result;
	 result.x = x * scalar;
	 result.y = y * scalar;
	 return result;
 }
 Vector2 operator*(float scalar, const Vector2& vec)
 {
	Vector2 result;
	 result.x = vec.x * scalar;
	 result.y = vec.y * scalar;
	 return result;
 }


 const Vector2 Vector2::operator/(const float& scalar) const
 {
	 Vector2 result;
	 result.x = x / scalar;
	 result.y = y / scalar;
	 return result;
 }



 bool operator==(const Vector2& lhs, const Vector2& rhs)
 {
	 if (lhs.x == rhs.x && lhs.y == rhs.y)
		return true;
	 return false;
 }

 bool operator!=(const Vector2& lhs, const Vector2& rhs)
 {
	 if (lhs.x == rhs.x && lhs.y == rhs.y)
		 return false;
	 return true;
 }

 bool operator<(const Vector2& lhs, const Vector2& rhs)
 {
	 if (lhs.y < rhs.y)
		 return true;
	 if (rhs.y < lhs.y)
		 return false;
	 return lhs.x < rhs.x;
 }

 Vector2 Vector2::ZERO = Vector2(0.f, 0.f);