#pragma once
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Math/Matrix4.hpp"

class Camera3D
{
private:
	Matrix4 m_projection;
	float m_fov;
public:
	Camera3D();
	Vector3 GetForwardXYZ();
	Vector3 GetForwardXY();
	Vector3 GetLeftXY();
	void FixAndClampAngles();

	Matrix4 GetViewMatrix() const;
	Vector3 m_position;
	EulerAngles m_orientation;
	Matrix4 GetProjectionMatrix() const { return m_projection; }
	float GetFOV() { return m_fov; }
	void ChangeFov(float fov);
	void UpdateProjectionMatrix();
};