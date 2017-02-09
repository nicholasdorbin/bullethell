#pragma once
#include <math.h>

class Matrix4;
class Vector2;

class Vector3
{
public:
	Vector3();
	Vector3(float value);
	Vector3(float initialX, float initialY, float initialZ);
	Vector3(const Vector3& v);
	Vector3(const Vector2& v, float initialZ = 0.f);
	void SetXYZ(float newX, float newY, float newZ);
	float GetMagnitude() const;
	void SetNormalized();
	Vector3 GetNormalized() const;

	const Vector3 Vector3::operator+(const Vector3& other) const;
	const Vector3 Vector3::operator-(const Vector3& other) const;
	Vector3& Vector3::operator=(const Vector3& rhs);
	Vector3& Vector3::operator+=(const Vector3& other);
	Vector3 Vector3::operator-=(const Vector3& other);
	Vector3 operator* (float scalar);
	const Vector3 operator*(float scalar) const;
	const Vector3 operator*(const Matrix4& mat) const;
	friend Vector3 operator*(float scalar, const Vector3& vec);
	const Vector3 operator/(float scalar) const;
	friend bool operator==(const Vector3& lhs, const Vector3& rhs);
	friend bool operator!=(const Vector3& lhs, const Vector3& rhs);
	const Vector3 operator-() const;

	float x;
	float y;
	float z;

	static Vector3 ZERO;
	static Vector3 ONE;
	static Vector3 UP;
	static Vector3 RIGHT;
	static Vector3 FORWARD;
};

