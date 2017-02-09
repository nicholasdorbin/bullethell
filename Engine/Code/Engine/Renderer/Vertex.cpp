#include "Engine/Renderer/Vertex.hpp"


//-------------------------------------------------------------------------------------------------
Vertex_PCUTB::Vertex_PCUTB(Vector3 const &position /*= Vector3::ZERO*/, Rgba const &color /*= Rgba::WHITE*/, Vector2 const &texCoords /*= Vector2::ZERO*/, Vector3 const &tangent /*= Vector3(1.f,0.f,0.f)*/, Vector3 const &biTangent /*= Vector3(0.f,1.f,0.f) */)
	: m_position(position)
	, m_color(color)
	, m_texCoord(texCoords)
	, m_tangent(tangent)
	, m_biTangent(biTangent)
{

}


//-------------------------------------------------------------------------------------------------
Vertex_PCU::Vertex_PCU(const Vector3& position /*= Vector3::ZERO*/, const Rgba& color /*= Rgba::WHITE */, const Vector2& texCoords)
	: m_position(position)
	, m_color(color)
	, m_texCoord(texCoords)
{
}


Vertex_SPRITE::Vertex_SPRITE(const Vector2& position /*= Vector2::ZERO*/, const Rgba& color /*= Rgba::WHITE */, const Vector2& texCoords)
	: m_position2D(position)
	, m_color(color)
	, m_texCoord(texCoords)
{
}

//-------------------------------------------------------------------------------------------------
Vertex_PC::Vertex_PC(const Vector3& position /*= Vector3::ZERO*/, const Rgba& color /*= Rgba::WHITE */)
	: m_position(position)
	, m_color(color)
{
}

VertexMaster::VertexMaster(Vector3 const &position /*= Vector3::ZERO*/, Rgba const &color /*= Rgba::WHITE*/,  Vector3 const &tangent /*= Vector3(1.f, 0.f, 0.f)*/, Vector3 const &biTangent /*= Vector3(0.f, 1.f, 0.f)*/, const Vector3 &normal /*= Vector3(0.f, 1.f, 0.f)*/, Vector2 const uv0 /*= Vector2::ZERO*/, Vector2 const uv1 /*= Vector2::ZERO*/)
	: m_position(position)
	, m_color(color)
	, m_tangent(tangent)
	, m_bitangent(biTangent)
	, m_normal(normal)
	, m_uv0(uv0)
	, m_uv1(uv1)
{

}

Vertex_PCUTBN::Vertex_PCUTBN(Vector3 const &position /*= Vector3::ZERO*/, Rgba const &color /*= Rgba::WHITE*/, Vector2 const &texCoords /*= Vector2::ZERO*/, Vector3 const &tangent /*= Vector3(1.f, 0.f, 0.f)*/, Vector3 const &biTangent /*= Vector3(0.f, 0.f, 1.f)*/, Vector3 const &normal /*= Vector3(0.f, -1.f, 0.f)*/)
	: m_position(position)
	, m_color(color)
	, m_texCoord(texCoords)
	, m_tangent(tangent)
	, m_biTangent(biTangent)
	, m_normal(normal)
{

}

bool operator==(const VertexMaster& lhs, const VertexMaster& rhs)
{
	if (lhs.m_position != rhs.m_position)
	{
		return false;
	}

	if (lhs.m_color != rhs.m_color)
	{
		return false;
	}

	if (lhs.m_uv0 != rhs.m_uv0)
	{
		return false;
	}

	if (lhs.m_uv1 != rhs.m_uv1)
	{
		return false;
	}

	if (lhs.m_tangent != rhs.m_tangent)
	{
		return false;
	}

	if (lhs.m_bitangent != rhs.m_bitangent)
	{
		return false;
	}

	if (lhs.m_normal != rhs.m_normal)
	{
		return false;
	}

	return true;
}

Vertex_SKINED_MESH::Vertex_SKINED_MESH(Vector3 const &position /*= Vector3::ZERO*/, Rgba const &color /*= Rgba::WHITE*/, Vector2 const &texCoords /*= Vector2::ZERO*/, 
	Vector3 const &tangent /*= Vector3(1.f, 0.f, 0.f)*/, Vector3 const &biTangent /*= Vector3(0.f, 0.f, 1.f)*/, 
	Vector4 boneWeights /*= Vector4(1.f,0.f,0.f,0.f)*/, UIntVector4 boneIndicies /*= UIntVector4(0,0,0,0)*/)
	: m_position(position)
	, m_color(color)
	, m_texCoord(texCoords)
	, m_tangent(tangent)
	, m_biTangent(biTangent)
	, m_boneWeights(boneWeights)
	, m_boneIndices (boneIndicies)
{

}
