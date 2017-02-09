#pragma once

#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/UIntVector4.hpp"
#include "Engine/Math/Rgba.hpp"


//-------------------------------------------------------------------------------------------------
enum VertexType
{
	VertexType_PC,
	VertexType_PCU,
	VertexType_PCUTB,
	VertexType_PCUTBN,
	VertexType_SKINED_MESH,
	VertexType_SPRITE

};


struct Vertex_PCUTBN
{
public:
	Vector3 m_position;
	Rgba m_color;
	Vector2 m_texCoord;
	Vector3 m_tangent;
	Vector3 m_biTangent;
	Vector3 m_normal;

public:
	Vertex_PCUTBN(Vector3 const &position = Vector3::ZERO, Rgba const &color = Rgba::WHITE, Vector2 const &texCoords = Vector2::ZERO, Vector3 const &tangent = Vector3(1.f, 0.f, 0.f), Vector3 const &biTangent = Vector3(0.f, 0.f, 1.f), Vector3 const &normal = Vector3(0.f, -1.f, 0.f));
};

//-------------------------------------------------------------------------------------------------
struct Vertex_PCUTB
{
public:
	Vector3 m_position;
	Rgba m_color;
	Vector2 m_texCoord;
	Vector3 m_tangent;
	Vector3 m_biTangent;

public:
	Vertex_PCUTB(Vector3 const &position = Vector3::ZERO, Rgba const &color = Rgba::WHITE, Vector2 const &texCoords = Vector2::ZERO, Vector3 const &tangent = Vector3(1.f, 0.f, 0.f), Vector3 const &biTangent = Vector3(0.f, 0.f, 1.f));
};


//-------------------------------------------------------------------------------------------------
struct Vertex_PCU
{
public:
	Vector3 m_position;
	Rgba m_color;
	Vector2 m_texCoord;

public:
	Vertex_PCU(const Vector3& position = Vector3::ZERO, const Rgba& color = Rgba::WHITE, const Vector2& texCoords = Vector2::ZERO);
};


//-------------------------------------------------------------------------------------------------
struct Vertex_PC
{
public:
	Vector3 m_position;
	Rgba m_color;

public:
	Vertex_PC(const Vector3& position = Vector3::ZERO, const Rgba& color = Rgba::WHITE);
};

struct Vertex_SKINED_MESH
{
public:
	Vector3 m_position;
	Rgba m_color;
	Vector2 m_texCoord;
	Vector3 m_tangent;
	Vector3 m_biTangent;

	UIntVector4 m_boneIndices; // unsigned int vec4
	Vector4 m_boneWeights;  // add up to one.   (x + y + z + w == 1.0f)
public:
	Vertex_SKINED_MESH(Vector3 const &position = Vector3::ZERO, Rgba const &color = Rgba::WHITE, Vector2 const &texCoords = Vector2::ZERO,
		Vector3 const &tangent = Vector3(1.f, 0.f, 0.f), Vector3 const &biTangent = Vector3(0.f, 0.f, 1.f),
		Vector4 boneWeights = Vector4(1.f,0.f,0.f,0.f), UIntVector4 boneIndicies = UIntVector4(0,0,0,0));

};

struct Vertex_SPRITE
{
public:
	Vector2 m_position2D;
	Rgba m_color;
	Vector2 m_texCoord;

public:
	Vertex_SPRITE(const Vector2& m_position2D = Vector2::ZERO, const Rgba& color = Rgba::WHITE, const Vector2& texCoords = Vector2::ZERO);
};

struct VertexMaster
{
	Vector3 m_position;
	Vector2 m_position2D;

	Vector3 m_tangent;
	Vector3 m_bitangent;
	Vector3 m_normal;

	Rgba m_color;

	Vector2 m_uv0;
	Vector2 m_uv1;

	Vector4 m_boneWeights;  // add up to one.   (x + y + z + w == 1.0f)
	UIntVector4 m_boneIndicies; // unsigned int vec4

	VertexMaster(Vector3 const &position = Vector3::ZERO, Rgba const &color = Rgba::WHITE,
		Vector3 const &tangent = Vector3(1.f, 0.f, 0.f), Vector3 const &biTangent = Vector3(0.f, 1.f, 0.f), const Vector3 &normal = Vector3(0.f, 1.f, 0.f),
		Vector2 const uv0 = Vector2::ZERO, Vector2 const uv1 = Vector2::ZERO);
	friend bool operator==(const VertexMaster& lhs, const VertexMaster& rhs);

};
