#include "Engine/Math/EulerAngles.hpp"
#include "Engine/MathUtils.hpp"
#include "Engine/Math/Vector3.hpp"

const EulerAngles EulerAngles::ZERO = EulerAngles(0.f, 0.f,0.f);

void EulerAngles::NegateYaw()
{
	m_yawDegreesAboutZ = ClampFloatCircular(-m_yawDegreesAboutZ, 0.f, 360.f);
}

Vector3 EulerAngles::GetUnitVector()
{
	Vector3 result;

	EulerAngles flipedYaw = *this;
	flipedYaw.NegateYaw();


	float yaw = flipedYaw.m_yawDegreesAboutZ;
	float pitch = flipedYaw.m_pitchDegreesAboutX;

	float cosYaw = CosDegrees(flipedYaw.m_yawDegreesAboutZ);
	float sinYaw = SinDegrees(flipedYaw.m_yawDegreesAboutZ);
	float cosPitch = CosDegrees(flipedYaw.m_pitchDegreesAboutX);
	float sinPitch = SinDegrees(flipedYaw.m_pitchDegreesAboutX);

	result.x = SinDegrees(yaw) * CosDegrees(pitch);
	result.y = CosDegrees(yaw) * CosDegrees(pitch);
	result.z = SinDegrees(pitch);
	return Vector3((sinYaw * cosPitch),(cosYaw*cosPitch) , -sinPitch);
	//result = result - Vector3::FORWARD;
	//return result;
}

EulerAngles::EulerAngles(float pitchDegreesAboutX, float rollDegreesAboutY, float yawDegreesAboutZ)
{
	m_pitchDegreesAboutX = pitchDegreesAboutX;
	m_rollDegreesAboutY = rollDegreesAboutY;
	m_yawDegreesAboutZ = yawDegreesAboutZ;
}

EulerAngles::EulerAngles()
{
	m_pitchDegreesAboutX = 0.f;
	m_rollDegreesAboutY = 0.f;
	m_yawDegreesAboutZ = 0.f;
}

