#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Rgba.hpp"

// Data structure which holds all the Vertex Attributes for a single vertex
//	which is used to draw the face of one of our voxels (Minecraft cubes).
//	PCT = Position, Color, TexCoords
struct Vertex3D_PCT
{
	Vertex3D_PCT()
	{
		m_position = Vector3::ZERO;
		m_color = Rgba::WHITE;
		m_texCoords = Vector2(0.f,0.f);
	};
	Vertex3D_PCT(Vector3 pos, Rgba color, Vector2 texCoords)
	{
		m_position = pos;
		m_color = color;
		m_texCoords = texCoords;
	};

	Vector3		m_position;
	Rgba		m_color;		// Rgba is 4 bytes (unsigned chars) or 4 floats; bytes are better
	Vector2		m_texCoords;

};