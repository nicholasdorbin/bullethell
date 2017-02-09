#pragma once
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Rgba.hpp"

// Data structure which holds all the Vertex Attributes for a single vertex
//	which is used to draw the face of one of our voxels (Minecraft cubes).
//	PCT = Position, Color, TexCoords
struct Vertex3D_PC
{
	Vector3		m_position;
	Rgba		m_color;		// Rgba is 4 bytes (unsigned chars) or 4 floats; bytes are better
};