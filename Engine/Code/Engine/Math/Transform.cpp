#include "Engine/Math/Transform.hpp"



//-------------------------------------------------------------------------------------------------
Transform::Transform()
	: m_position(Vector3::ZERO)
	, m_rotation(Matrix4::IDENTITY)
	, m_scale(Vector3::ONE)
{
}


//-------------------------------------------------------------------------------------------------
Transform::Transform(Vector3 const &position, Matrix4 const &rotation, Vector3 const &scale)
	: m_position(position)
	, m_rotation(rotation)
	, m_scale(scale)
{
}


//-------------------------------------------------------------------------------------------------
Transform::~Transform()
{
}


//-------------------------------------------------------------------------------------------------
Matrix4 Transform::GetModelMatrix() const
{
	Matrix4 scale = Matrix4::IDENTITY;
	//scale.MakeScale(m_scale);
	scale.Scale(m_scale);
	//MatrixMakeScale(&scale, m_scale.x);
	Matrix4 model = Matrix4::IDENTITY;
	model = m_rotation;// .GetRotationMatrix( );
	model.ChangePosition(m_position);
// 	model.data[12] = m_position.x;
// 	model.data[13] = m_position.y;
// 	model.data[14] = m_position.z;
	return scale * model;
}

