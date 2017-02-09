#pragma once
#include "Engine/Math/Matrix4.hpp"
#include "Engine/Math/Vector3.hpp"



//-------------------------------------------------------------------------------------------------
class Transform
{
private:
	Vector3 m_position;
	Matrix4 m_rotation;
	Vector3 m_scale;

public:
	Transform();
	Transform(Vector3 const &position, Matrix4 const &rotation, Vector3 const &scale);
	~Transform();

	Vector3 GetPosition() const { return m_position; }
	Matrix4 GetRotation() const { return m_rotation; }
	Vector3 GetScale() const { return m_scale; }
	void SetPosition(Vector3 const &position) { m_position = position; }
	void SetRotation(Matrix4 const &rotation) { m_rotation = rotation; }
	void SetScale(Vector3 const &scale) { m_scale = scale; }

	Matrix4 GetModelMatrix() const;
};