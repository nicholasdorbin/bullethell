#pragma once

#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Matrix4.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include <math.h>
#include "Engine/MathUtils.hpp"

class Quaternion {
public:
	Quaternion() : w(0.f), x(0.f), y(0.f), z(0.f) { }
	Quaternion(float angle, Vector3 axis)
	{
		Vector3 normalAxis = axis.GetNormalized();
		w = CosDegrees(angle / 2.f);
		

		x = SinDegrees(angle / 2.f) * normalAxis.x;
		y = SinDegrees(angle / 2.f) * normalAxis.y;
		z = SinDegrees(angle / 2.f) * normalAxis.z;

		//Test code for conversions
// 		EulerAngles angles;
// 		this->ToEulerAngles(angles);
// 		angles.NegateYaw();
// 
// 		Quaternion testQuat = Quaternion(angles);


	}
	Quaternion(float w_in, float x_in, float y_in, float z_in)
		: w(w_in)
		, x(x_in)
		, y(y_in)
		, z(z_in)
	{ }
	Quaternion(EulerAngles euler)
	{
		float heading = euler.m_yawDegreesAboutZ;
		float pitch = euler.m_pitchDegreesAboutX;
		float bank = euler.m_rollDegreesAboutY;

		heading = ClampFloatCircular(-heading, 0.f, 360.f);


		// Assuming the angles are in radians.
		float c1 = CosDegrees(heading / 2);
		float s1 = SinDegrees(heading / 2);
		float c2 = CosDegrees(pitch / 2);
		float s2 = SinDegrees(pitch / 2);
		float c3 = CosDegrees(bank / 2);
		float s3 = SinDegrees(bank / 2);
		float c1c2 = c1*c2;
		float s1s2 = s1*s2;
		w = c1c2*c3 - s1s2*s3;
		y = c1c2*s3 + s1s2*c3;
		z = s1*c2*c3 + c1*s2*s3;
		x = c1*s2*c3 - s1*c2*s3;
		x = -x;
	}

	friend Quaternion operator*(const Quaternion& q, float scalar) {
		return Quaternion(q.w * scalar, q.x * scalar, q.y * scalar, q.z * scalar);
	}
	friend Quaternion operator*(float scalar, const Quaternion& q) {
		return Quaternion(q.w * scalar, q.x * scalar, q.y * scalar, q.z * scalar);
	}

	friend Quaternion operator+(const Quaternion& q_a, const Quaternion& q_b) {
		return Quaternion(q_a.w + q_b.w, q_a.x + q_b.x, q_a.y + q_b.y, q_a.z + q_b.z);
	}

	float DotProduct(const Quaternion& other) const;

	void ToRotationMatrix(Matrix4& m) const;
	void ToEulerAngles(EulerAngles& euler) const;
	Vector3 GetUnitVector();



	static Quaternion IDENTITY_POS;
	static Quaternion IDENTITY_NEG;

private:
	float w;
	float x;
	float y;
	float z;
};