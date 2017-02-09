#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Rgba.hpp"


struct Vertex3D_PCUTB
{
	Vertex3D_PCUTB()
	{
		m_position = Vector3::ZERO;
		m_color = Rgba::WHITE;
		m_texCoords = Vector2(0.f, 0.f);
	};
	Vertex3D_PCUTB(Vector3 pos, Rgba color, Vector2 texCoords, Vector3 tangent, Vector3 bitangent)
	{
		m_position = pos;
		m_color = color;
		m_texCoords = texCoords;
		m_tangent = tangent;
		m_bitangent = bitangent;
	};

	Vector3		m_position;
	Rgba		m_color;		// Rgba is 4 bytes (unsigned chars) or 4 floats; bytes are better
	Vector2		m_texCoords;
	Vector3		m_tangent;
	Vector3		m_bitangent;

};