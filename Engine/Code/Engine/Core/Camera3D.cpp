#include "Engine/Core/Camera3D.hpp"
#include "Engine/MathUtils.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Math/Quaternion.hpp"


Camera3D::Camera3D()
	 :m_projection(Matrix4::IDENTITY)
	, m_fov(70.f)
{
	m_position.SetXYZ(0.f, 0.f, 0.f);
	m_orientation = EulerAngles::ZERO;
	float aspect = 16.f / 9.f;
	m_projection.ChangeToPerspectiveProjection(m_fov, aspect, 0.1f, 100.f);


	Matrix4 COB = Matrix4();
	COB.SetBasis(Vector3::RIGHT, Vector3::UP, Vector3::FORWARD);

	Matrix4 newProj = COB * m_projection;

	m_projection = newProj;


	//MatrixMakePerspective(&m_projection, 70.f, aspect, 0.1f, 100.f);
	//MatrixMakeProjPerspective(&m_projection, aspect, DegToRad(60.f), 0.01f, 1000.f);
	//m_projection.MakePerspective(60.f, aspect, 0.01f, 1000.f);
}

Vector3 Camera3D::GetForwardXYZ()
{
	//Yaw = theta
	//phi = pitch nose down
	//(cos theta)*(cos phi),(sin theta)*(cos phi),(-sin phi)

	float cosYaw = CosDegrees(m_orientation.m_yawDegreesAboutZ);
	float sinYaw = SinDegrees(m_orientation.m_yawDegreesAboutZ);
	float cosPitch = CosDegrees(m_orientation.m_pitchDegreesAboutX);
	float sinPitch = SinDegrees(m_orientation.m_pitchDegreesAboutX);
	//return Vector3((cosYaw*cosPitch), (sinYaw * cosPitch), -sinPitch);//Simple Miner
	return Vector3((sinYaw * cosPitch), (cosYaw*cosPitch), sinPitch);

}

Vector3 Camera3D::GetForwardXY()
{
	float cosYaw = CosDegrees(m_orientation.m_yawDegreesAboutZ);
	float sinYaw = SinDegrees(m_orientation.m_yawDegreesAboutZ);
	return Vector3(sinYaw, cosYaw, 0.f);
}

Vector3 Camera3D::GetLeftXY()
{
	Vector3 forwardXY = GetForwardXY();
	return Vector3(-forwardXY.y, forwardXY.x, 0.f);
}

void Camera3D::FixAndClampAngles()
{

	m_orientation.m_pitchDegreesAboutX = ClampFloat(m_orientation.m_pitchDegreesAboutX, -90.f, 90.f);
	m_orientation.m_rollDegreesAboutY = ClampFloatCircular(m_orientation.m_rollDegreesAboutY, 0.f, 360.f);
	m_orientation.m_yawDegreesAboutZ = ClampFloatCircular(m_orientation.m_yawDegreesAboutZ, 0.f, 360.f);

}

//-------------------------------------------------------------------------------------------------
Matrix4 Camera3D::GetViewMatrix() const
{


	Matrix4 view(Matrix4::IDENTITY);

	
// 	Quaternion quatFaceRight = Quaternion(90.f, Vector3::UP);
// 
// 	quatFaceRight.ToRotationMatrix(view);
// 	view.Inverse();

	

	view.MatrixMakeRotationEuler(-m_orientation.m_yawDegreesAboutZ, m_orientation.m_pitchDegreesAboutX, m_orientation.m_rollDegreesAboutY);
	//view.MatrixMakeRotationEuler(-lookAtAngles.m_yawDegreesAboutZ, lookAtAngles.m_pitchDegreesAboutX, lookAtAngles.m_rollDegreesAboutY);

	

	view.TransposeRotation();
	view.NegateTranslation(m_position);

	
	return view;
}

void Camera3D::ChangeFov(float fov)
{
	m_fov = fov;
	UpdateProjectionMatrix();
}

void Camera3D::UpdateProjectionMatrix()
{
	float aspect = 16.f / 9.f;
	m_projection.ChangeToPerspectiveProjection(m_fov, aspect, 0.1f, 100.f);


	Matrix4 COB = Matrix4();
	COB.SetBasis(Vector3::RIGHT, Vector3::UP, Vector3::FORWARD);

	Matrix4 newProj = COB * m_projection;

	m_projection = newProj;;
}
