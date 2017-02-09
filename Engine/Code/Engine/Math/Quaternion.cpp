#include "Engine/Math/Quaternion.hpp"
#include "Engine/MathUtils.hpp"

float Quaternion::DotProduct(const Quaternion& other) const
{
	return (w * other.w + x * other.x + y * other.y + z * other.z);
}

void Quaternion::ToRotationMatrix(Matrix4& out_matrix) const
{
	float qx2 = x * x;
	float qy2 = y * y;
	float qz2 = z * z;

	float qw = w;
	float qx = x;
	float qy = y;
	float qz = z;

	out_matrix.matrix[0] = 1.f - 2.f * qy2 - 2.f * qz2;
	out_matrix.matrix[1] = 2.f * qx*qy + 2.f * qz*qw;
	out_matrix.matrix[2] = 2.f * qx*qz - 2.f * qy*qw;
	out_matrix.matrix[3] = 0.f;

	out_matrix.matrix[4] = 2.f * qx*qy - 2.f * qz*qw;
	out_matrix.matrix[5] = 1.f - 2.f * qx2 - 2.f * qz2;
	out_matrix.matrix[6] = 2.f * qy*qz + 2.f * qx*qw;
	out_matrix.matrix[7] = 0.f;

	out_matrix.matrix[8] = 2.f * qx*qz + 2.f * qy*qw;
	out_matrix.matrix[9] = 2.f * qy*qz - 2.f * qx*qw;
	out_matrix.matrix[10] = 1.f - 2.f * qx2 - 2.f * qy2;
	out_matrix.matrix[11] = 0.f;

	out_matrix.matrix[12] = 0.f;
	out_matrix.matrix[13] = 0.f;
	out_matrix.matrix[14] = 0.f;
	out_matrix.matrix[15] = 1.f;


	//In column major
	// 	1 - 2 * qy2 - 2 * qz2	2 * qx*qy - 2 * qz*qw	2 * qx*qz + 2 * qy*qw
	// 	2 * qx*qy + 2 * qz*qw	1 - 2 * qx2 - 2 * qz2	2 * qy*qz - 2 * qx*qw
	// 	2 * qx*qz - 2 * qy*qw	2 * qy*qz + 2 * qx*qw	1 - 2 * qx2 - 2 * qy2
}


void Quaternion::ToEulerAngles(EulerAngles& euler) const
{
	/*
	Matrix4 mat = Matrix4::IDENTITY;
	ToRotationMatrix(mat);
	euler = mat.GetEulerAngles();
	*/
	float pitch;
	float roll;
	float yaw;

	float sinPitch = -2.f * (y*z + w*x);

	if (fabs(sinPitch) > 0.9999f)
	{
		pitch = sinPitch * (pi / 2.f);

		roll = (float)atan2(-x*z - w*y, 0.5f - y*y - z*z);

		yaw = 0;
	}
	else
	{
		pitch = (float)asin(sinPitch);
		roll = (float)atan2(x*z - w*y, 0.5f - x*x - y*y);
		yaw = (float)atan2(x*y - w*z, 0.5f - x*x - z*z);
	}


	euler.m_pitchDegreesAboutX = RadToDeg(pitch);
	euler.m_yawDegreesAboutZ = RadToDeg(-yaw);
	euler.m_rollDegreesAboutY = RadToDeg(roll);
}

Vector3 Quaternion::GetUnitVector()
{
	EulerAngles angles;
	ToEulerAngles(angles);
	angles.NegateYaw();

	return angles.GetUnitVector();
}

Quaternion Quaternion::IDENTITY_POS = Quaternion(1.f, 0.f, 0.f, 0.f);
Quaternion Quaternion::IDENTITY_NEG = Quaternion(-1.f, 0.f, 0.f, 0.f);