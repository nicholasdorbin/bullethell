#pragma once
#include <math.h>

class Vector2
{
public:
	Vector2();
	Vector2( float initialX, float initialY );
	Vector2(const Vector2& v);
	void SetXY(float newX, float newY);
	float GetMagnitude();
	void SetNormalized();
	Vector2 GetNormalized();

	const Vector2 Vector2::operator+(const Vector2 other) const;
	const Vector2 Vector2::operator-(const Vector2 other) const;
	Vector2& Vector2::operator=(const Vector2 &rhs);
	Vector2& Vector2::operator+=(const Vector2 other);
	Vector2 Vector2::operator-=(const Vector2 other);
	Vector2 Vector2::operator*=(const float& scalar);
	
	Vector2 operator* (float scalar);
	const Vector2 operator*(const float& scalar) const;
	friend Vector2 operator*(float scalar, const Vector2& vec);
	const Vector2 Vector2::operator*(const Vector2 other) const;
	const Vector2 operator/(const float& scalar) const;
	friend bool operator==(const Vector2& lhs, const Vector2& rhs);
	friend bool operator!=(const Vector2& lhs, const Vector2& rhs);
	
	const Vector2 operator-() const;
	friend bool operator<(const Vector2& lhs, const Vector2& rhs);

	float x;
	float y;

	static Vector2 ZERO;
};


