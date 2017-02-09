#pragma once
#include <math.h>
class Vector3;

class Vector4
{
public:
	Vector4();
	Vector4(float initialX, float initialY, float initialZ, float initialW);
	Vector4(const Vector4& v);
	Vector4(const Vector3& v, const float newW);
	void SetXYZ(float newX, float newY, float newZ, float newW);
	float GetMagnitude() const;
	void SetNormalized();
	Vector4 GetNormalized() const;
	Vector3 GetXYZ();

	const Vector4 Vector4::operator+(const Vector4 other) const;
	const Vector4 Vector4::operator-(const Vector4 other) const;
	Vector4& Vector4::operator=(const Vector4 &rhs);
	Vector4& Vector4::operator+=(const Vector4 other);
	Vector4 Vector4::operator-=(const Vector4 other);
	Vector4 operator* (float scalar);
	const Vector4 operator*(const float& scalar) const;
	friend Vector4 operator*(float scalar, const Vector4& vec);
	const Vector4 operator/(const float& scalar) const;
	friend bool operator==(const Vector4& lhs, const Vector4& rhs);
	friend bool operator!=(const Vector4& lhs, const Vector4& rhs);
	const Vector4 operator-() const;

	float x;
	float y;
	float z;
	float w;

	static Vector4 ZERO;
	static Vector4 ONE;
};

