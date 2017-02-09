#include "Engine/Renderer/Shaders/Mesh.hpp"

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
//#include "Engine/Renderer/Glyph.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/MathUtils.hpp"


//-------------------------------------------------------------------------------------------------
std::vector<Mesh*, TUntrackedAllocator<Mesh*>> Mesh::s_defaultMeshes;


//-------------------------------------------------------------------------------------------------
Mesh::Mesh(MeshShape const &shape)
	: m_vboID(NULL)
	, m_iboID(NULL)
	, m_indexBufferVertCount(0)
	, drawMode(GL_TRIANGLES)
{
	//Create object
	switch (shape)
	{//Remember Tex Coords: TL(0,0) BR(1,1)
	case MeshShape_QUAD:
	{
		//Vertex Definition Layout
		SetVertexLayout(VertexType_PCUTB);
		std::vector<Vertex_PCUTB> vertexes;

		//Setup Vertex
		vertexes.push_back(Vertex_PCUTB(Vector3(-0.5f, +0.0f, -0.5f), Rgba::WHITE, Vector2(0.f, 1.f), Vector3(1.f, 0.f, 0.f), Vector3(0.f, 0.f, 1.f)));  //bottomLeft
		vertexes.push_back(Vertex_PCUTB(Vector3(+0.5f, +0.0f, -0.5f), Rgba::WHITE, Vector2(1.f, 1.f), Vector3(1.f, 0.f, 0.f), Vector3(0.f, 0.f, 1.f)));  //bottomRight
		vertexes.push_back(Vertex_PCUTB(Vector3(+0.5f, +0.0f, +0.5f), Rgba::WHITE, Vector2(1.f, 0.f), Vector3(1.f, 0.f, 0.f), Vector3(0.f, 0.f, 1.f)));  //topRight
		vertexes.push_back(Vertex_PCUTB(Vector3(-0.5f, +0.0f, +0.5f), Rgba::WHITE, Vector2(0.f, 0.f), Vector3(1.f, 0.f, 0.f), Vector3(0.f, 0.f, 1.f)));  //topLeft

																																							  //Create VBO
		m_vboID = g_theRenderer->CreateRenderBuffer(&vertexes[0], vertexes.size(), sizeof(Vertex_PCUTB));

		//Create IBO
		uint16_t indicies[] =
		{
			0, 1, 2,
			0, 2, 3
		};
		m_indexBufferVertCount = 2 * 3;
		m_iboID = g_theRenderer->CreateRenderBuffer(&indicies, m_indexBufferVertCount, sizeof(indicies[0]));

		break;
	}
	case MeshShape_QUAD_XY:
	{
		//Vertex Definition Layout
		SetVertexLayout(VertexType_PCUTB);
		std::vector<Vertex_PCUTB> vertexes;

		//Setup Vertex
		vertexes.push_back(Vertex_PCUTB(Vector3(-0.5f, -0.5f ,+0.0f), Rgba::WHITE, Vector2(0.f, 1.f), Vector3(1.f, 0.f, 0.f), Vector3(0.f, 0.f, 1.f)));  //bottomLeft
		vertexes.push_back(Vertex_PCUTB(Vector3(+0.5f, -0.5f ,+0.0f), Rgba::WHITE, Vector2(1.f, 1.f), Vector3(1.f, 0.f, 0.f), Vector3(0.f, 0.f, 1.f)));  //bottomRight
		vertexes.push_back(Vertex_PCUTB(Vector3(+0.5f, +0.5f ,+0.0f), Rgba::WHITE, Vector2(1.f, 0.f), Vector3(1.f, 0.f, 0.f), Vector3(0.f, 0.f, 1.f)));  //topRight
		vertexes.push_back(Vertex_PCUTB(Vector3(-0.5f, +0.5f ,+0.0f), Rgba::WHITE, Vector2(0.f, 0.f), Vector3(1.f, 0.f, 0.f), Vector3(0.f, 0.f, 1.f)));  //topLeft

																																						 //Create VBO
		m_vboID = g_theRenderer->CreateRenderBuffer(&vertexes[0], vertexes.size(), sizeof(Vertex_PCUTB));

		//Create IBO
		uint16_t indicies[] =
		{
			0, 1, 2,
			0, 2, 3
		};
		m_indexBufferVertCount = 2 * 3;
		m_iboID = g_theRenderer->CreateRenderBuffer(&indicies, m_indexBufferVertCount, sizeof(indicies[0]));

		break;
	}
	case MeshShape_QUAD_FULLSCREEN:
	{
		//Vertex Definition Layout
		SetVertexLayout(VertexType_PCUTB);
		std::vector<Vertex_PCUTB> vertexes;

		//Setup Vertex
		vertexes.push_back(Vertex_PCUTB(Vector3(-1.f, -1.f, +0.0f), Rgba::WHITE, Vector2(0.f, 1.f), Vector3(1.f, 0.f, 0.f), Vector3(0.f, 0.f, 1.f)));  //bottomLeft
		vertexes.push_back(Vertex_PCUTB(Vector3(+1.f, -1.f, +0.0f), Rgba::WHITE, Vector2(1.f, 1.f), Vector3(1.f, 0.f, 0.f), Vector3(0.f, 0.f, 1.f)));  //bottomRight
		vertexes.push_back(Vertex_PCUTB(Vector3(+1.f, +1.f, +0.0f), Rgba::WHITE, Vector2(1.f, 0.f), Vector3(1.f, 0.f, 0.f), Vector3(0.f, 0.f, 1.f)));  //topRight
		vertexes.push_back(Vertex_PCUTB(Vector3(-1.f, +1.f, +0.0f), Rgba::WHITE, Vector2(0.f, 0.f), Vector3(1.f, 0.f, 0.f), Vector3(0.f, 0.f, 1.f)));  //topLeft

																																						 //Create VBO
		m_vboID = g_theRenderer->CreateRenderBuffer(&vertexes[0], vertexes.size(), sizeof(Vertex_PCUTB));

		//Create IBO
		uint16_t indicies[] =
		{
			0, 1, 2,
			0, 2, 3
		};
		m_indexBufferVertCount = 2 * 3;
		m_iboID = g_theRenderer->CreateRenderBuffer(&indicies, m_indexBufferVertCount, sizeof(indicies[0]));

		break;
	}
	case MeshShape_CUBE:
	{
		//Vertex Definition Layout
		SetVertexLayout(VertexType_PCUTB);
		std::vector<Vertex_PCUTB> vertexes;

		//#TODO: Use IBOs
		//Top
		vertexes.push_back(Vertex_PCUTB(Vector3(-0.5f, -0.5f, +0.5f), Rgba::WHITE, Vector2(0.f, 1.f), Vector3(1.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f)));  //bottomLeft
		vertexes.push_back(Vertex_PCUTB(Vector3(+0.5f, -0.5f, +0.5f), Rgba::WHITE, Vector2(1.f, 1.f), Vector3(1.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f)));  //bottomRight
		vertexes.push_back(Vertex_PCUTB(Vector3(+0.5f, +0.5f, +0.5f), Rgba::WHITE, Vector2(1.f, 0.f), Vector3(1.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f)));  //topRight
		vertexes.push_back(Vertex_PCUTB(Vector3(-0.5f, -0.5f, +0.5f), Rgba::WHITE, Vector2(0.f, 1.f), Vector3(1.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f)));  //bottomLeft
		vertexes.push_back(Vertex_PCUTB(Vector3(+0.5f, +0.5f, +0.5f), Rgba::WHITE, Vector2(1.f, 0.f), Vector3(1.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f)));  //topRight
		vertexes.push_back(Vertex_PCUTB(Vector3(-0.5f, +0.5f, +0.5f), Rgba::WHITE, Vector2(0.f, 0.f), Vector3(1.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f)));  //topLeft

		//Bottom
		vertexes.push_back(Vertex_PCUTB(Vector3(-0.5f, +0.5f, -0.5f), Rgba::WHITE, Vector2(0.f, 1.f), Vector3(1.f, 0.f, 0.f), Vector3(0.f, -1.f, 0.f)));  //bottomRight
		vertexes.push_back(Vertex_PCUTB(Vector3(+0.5f, +0.5f, -0.5f), Rgba::WHITE, Vector2(1.f, 1.f), Vector3(1.f, 0.f, 0.f), Vector3(0.f, -1.f, 0.f)));  //bottomLeft
		vertexes.push_back(Vertex_PCUTB(Vector3(+0.5f, -0.5f, -0.5f), Rgba::WHITE, Vector2(1.f, 0.f), Vector3(1.f, 0.f, 0.f), Vector3(0.f, -1.f, 0.f)));  //topLeft

		vertexes.push_back(Vertex_PCUTB(Vector3(-0.5f, +0.5f, -0.5f), Rgba::WHITE, Vector2(0.f, 1.f), Vector3(1.f, 0.f, 0.f), Vector3(0.f, -1.f, 0.f)));  //bottomRight
		vertexes.push_back(Vertex_PCUTB(Vector3(+0.5f, -0.5f, -0.5f), Rgba::WHITE, Vector2(1.f, 0.f), Vector3(1.f, 0.f, 0.f), Vector3(0.f, -1.f, 0.f)));  //topLeft
		vertexes.push_back(Vertex_PCUTB(Vector3(-0.5f, -0.5f, -0.5f), Rgba::WHITE, Vector2(0.f, 0.f), Vector3(1.f, 0.f, 0.f), Vector3(0.f, -1.f, 0.f)));  //topRight

		//Left
		vertexes.push_back(Vertex_PCUTB(Vector3(-0.5f, +0.5f, -0.5f), Rgba::WHITE, Vector2(0.f, 1.f), Vector3(0.f, -1.f, 0.f), Vector3(0.f, 0.f, 1.f)));  //bottomLeft
		vertexes.push_back(Vertex_PCUTB(Vector3(-0.5f, -0.5f, -0.5f), Rgba::WHITE, Vector2(1.f, 1.f), Vector3(0.f, -1.f, 0.f), Vector3(0.f, 0.f, 1.f)));  //bottomRight
		vertexes.push_back(Vertex_PCUTB(Vector3(-0.5f, -0.5f, +0.5f), Rgba::WHITE, Vector2(1.f, 0.f), Vector3(0.f, -1.f, 0.f), Vector3(0.f, 0.f, 1.f)));  //topRight
		vertexes.push_back(Vertex_PCUTB(Vector3(-0.5f, +0.5f, -0.5f), Rgba::WHITE, Vector2(0.f, 1.f), Vector3(0.f, -1.f, 0.f), Vector3(0.f, 0.f, 1.f)));  //bottomLeft
		vertexes.push_back(Vertex_PCUTB(Vector3(-0.5f, -0.5f, +0.5f), Rgba::WHITE, Vector2(1.f, 0.f), Vector3(0.f, -1.f, 0.f), Vector3(0.f, 0.f, 1.f)));  //topRight
		vertexes.push_back(Vertex_PCUTB(Vector3(-0.5f, +0.5f, +0.5f), Rgba::WHITE, Vector2(0.f, 0.f), Vector3(0.f, -1.f, 0.f), Vector3(0.f, 0.f, 1.f)));  //topLeft

		//Right - x - labels
		vertexes.push_back(Vertex_PCUTB(Vector3(+0.5f, -0.5f, -0.5f), Rgba::WHITE, Vector2(0.f, 1.f), Vector3(0.f, 1.f, 0.f), Vector3(0.f, 0.f, 1.f)));  //bottomRight
		vertexes.push_back(Vertex_PCUTB(Vector3(+0.5f, +0.5f, -0.5f), Rgba::WHITE, Vector2(1.f, 1.f), Vector3(0.f, 1.f, 0.f), Vector3(0.f, 0.f, 1.f)));  //bottomLeft
		vertexes.push_back(Vertex_PCUTB(Vector3(+0.5f, +0.5f, +0.5f), Rgba::WHITE, Vector2(1.f, 0.f), Vector3(0.f, 1.f, 0.f), Vector3(0.f, 0.f, 1.f)));  //topLeft
		vertexes.push_back(Vertex_PCUTB(Vector3(+0.5f, -0.5f, -0.5f), Rgba::WHITE, Vector2(0.f, 1.f), Vector3(0.f, 1.f, 0.f), Vector3(0.f, 0.f, 1.f)));  //bottomRight
		vertexes.push_back(Vertex_PCUTB(Vector3(+0.5f, +0.5f, +0.5f), Rgba::WHITE, Vector2(1.f, 0.f), Vector3(0.f, 1.f, 0.f), Vector3(0.f, 0.f, 1.f)));  //topLeft
		vertexes.push_back(Vertex_PCUTB(Vector3(+0.5f, -0.5f, +0.5f), Rgba::WHITE, Vector2(0.f, 0.f), Vector3(0.f, 1.f, 0.f), Vector3(0.f, 0.f, 1.f)));  //topRight

		
		//Back
		vertexes.push_back(Vertex_PCUTB(Vector3(+0.5f, +0.5f, -0.5f), Rgba::WHITE, Vector2(0.f, 1.f), Vector3(-1.f, 0.f, 0.f), Vector3(0.f, 0.f, 1.f)));  //bottomRight
		vertexes.push_back(Vertex_PCUTB(Vector3(-0.5f, +0.5f, -0.5f), Rgba::WHITE, Vector2(1.f, 1.f), Vector3(-1.f, 0.f, 0.f), Vector3(0.f, 0.f, 1.f)));  //bottomLeft
		vertexes.push_back(Vertex_PCUTB(Vector3(-0.5f, +0.5f, +0.5f), Rgba::WHITE, Vector2(1.f, 0.f), Vector3(-1.f, 0.f, 0.f), Vector3(0.f, 0.f, 1.f)));  //topLeft
		vertexes.push_back(Vertex_PCUTB(Vector3(+0.5f, +0.5f, -0.5f), Rgba::WHITE, Vector2(0.f, 1.f), Vector3(-1.f, 0.f, 0.f), Vector3(0.f, 0.f, 1.f)));  //bottomRight
		vertexes.push_back(Vertex_PCUTB(Vector3(-0.5f, +0.5f, +0.5f), Rgba::WHITE, Vector2(1.f, 0.f), Vector3(-1.f, 0.f, 0.f), Vector3(0.f, 0.f, 1.f)));  //topLeft
		vertexes.push_back(Vertex_PCUTB(Vector3(+0.5f, +0.5f, +0.5f), Rgba::WHITE, Vector2(0.f, 0.f), Vector3(-1.f, 0.f, 0.f), Vector3(0.f, 0.f, 1.f)));  //topRight

		//Front
		vertexes.push_back(Vertex_PCUTB(Vector3(-0.5f, -0.5f, -0.5f), Rgba::WHITE, Vector2(0.f, 1.f), Vector3(1.f, 0.f, 0.f), Vector3(0.f, 0.f, 1.f)));  //bottomLeft
		vertexes.push_back(Vertex_PCUTB(Vector3(+0.5f, -0.5f, -0.5f), Rgba::WHITE, Vector2(1.f, 1.f), Vector3(1.f, 0.f, 0.f), Vector3(0.f, 0.f, 1.f)));  //bottomRight
		vertexes.push_back(Vertex_PCUTB(Vector3(+0.5f, -0.5f, +0.5f), Rgba::WHITE, Vector2(1.f, 0.f), Vector3(1.f, 0.f, 0.f), Vector3(0.f, 0.f, 1.f)));  //topRight
		vertexes.push_back(Vertex_PCUTB(Vector3(-0.5f, -0.5f, -0.5f), Rgba::WHITE, Vector2(0.f, 1.f), Vector3(1.f, 0.f, 0.f), Vector3(0.f, 0.f, 1.f)));  //bottomLeft
		vertexes.push_back(Vertex_PCUTB(Vector3(+0.5f, -0.5f, +0.5f), Rgba::WHITE, Vector2(1.f, 0.f), Vector3(1.f, 0.f, 0.f), Vector3(0.f, 0.f, 1.f)));  //topRight
		vertexes.push_back(Vertex_PCUTB(Vector3(-0.5f, -0.5f, +0.5f), Rgba::WHITE, Vector2(0.f, 0.f), Vector3(1.f, 0.f, 0.f), Vector3(0.f, 0.f, 1.f)));  //topLeft

		//Create VBO
		m_indexBufferVertCount = vertexes.size();
		m_vboID = g_theRenderer->CreateRenderBuffer(&vertexes[0], m_indexBufferVertCount, sizeof(Vertex_PCUTB));


		//No IBO
		break;
	}
	case MeshShape_SPHERE: //#TODO: Redo with less vertexes
	{
		//Vertex Definition Layout
		SetVertexLayout(VertexType_PCUTB);
		std::vector<Vertex_PCUTB> vertexes;



		std::vector<short> indicies;
		//short vertIndex = 0;

		// Add all vertexes to the mesh for VAO, add all indices to the mesh for IBO
		float radius = 1.0f;
		int numRings = 100;
		int numSectors = 100;
		int currentRow = 0;
		int nextRow = 0;
		int nextSector;

		float const R = 1.0f / (float)(numRings - 1);
		float const S = 1.0f / (float)(numSectors - 1);


		for (int r = 0; r < numRings; ++r)
		{
			for (int s = 0; s < numSectors; ++s)
			{
				float const z = sin(-pi / 2 + pi * r * R);
				float const y = cos(2 * pi * s * S) * sin(pi * r * R);
				float const x = sin(2 * pi * s * S) * sin(pi * r * R);

				//r is constant
				float const tanZ = 0.f;
				float const tanY = sin(2 * pi * s * S);// * -sin(pi * r * R);//-4 * pi * sin(pi * s * S) * cos(pi * s * S) * sin(pi * r * R);//
				float const tanX = cos(2 * pi * s * S);// * sin(pi * r * R);//0.5f * (pi * cos(pi * r * R - 2 * pi * s * S) + pi * cos(pi * r * R + 2 * pi * s * S));//cos(2 * pi * s * S) * sin(pi * r * R);

													   //s is constant
				float const biTanZ = pi * sin(pi * r * R);
				float const biTanY = cos(2 * pi * s * S) * cos(pi * r * R);//0.5f * (pi * cos(pi * r * R - 2 * pi * s * S) + pi * cos(pi * r * R + 2 * pi * s * S));//
				float const biTanX = sin(2 * pi * s * S) * cos(pi * r * R);//0.5f * (pi * sin(pi  * r * R + 2 * pi * s * S) - pi * sin(pi  * r * R - 2 * pi * s * S));//

				Vector3 pos = Vector3(x * radius, y * radius, -z * radius);
				Vector3 posNorm = pos.GetNormalized();



				Vector3 tan = Vector3(-tanX, tanY, tanZ);
				Vector3 biTan = CrossProduct(posNorm, tan);
				//Vector3 biTan = Vector3(biTanX, biTanY, biTanZ);

				vertexes.push_back(Vertex_PCUTB(Vector3(x * radius, y * radius, -z * radius), Rgba::WHITE, Vector2(1 - s * S, r * R),
					tan, biTan));

				currentRow = r * numSectors;
				nextRow = (r + 1) * numSectors;
				nextSector = (s + 1) % numSectors;

				if (r < numRings - 1)
				{
					indicies.push_back((uint16_t)(nextRow + nextSector));
					indicies.push_back((uint16_t)(nextRow + s));
					indicies.push_back((uint16_t)(currentRow + s));

					indicies.push_back((uint16_t)(currentRow + nextSector));
					indicies.push_back((uint16_t)(nextRow + nextSector));
					indicies.push_back((uint16_t)(currentRow + s));
				}
			}
		}

		//Create VBO
		m_vboID = g_theRenderer->CreateRenderBuffer(&vertexes[0], vertexes.size(), sizeof(Vertex_PCUTB));

		//Create IBO
		m_indexBufferVertCount = indicies.size();
		m_iboID = g_theRenderer->CreateRenderBuffer(&indicies[0], m_indexBufferVertCount, sizeof(indicies[0]));

		break;
	}
	case MeshShape_AXIS:
	{
		drawMode = GL_LINES;

		//Vertex Definition Layout
		SetVertexLayout(VertexType_PC);
		std::vector<Vertex_PC> vertexes;

		vertexes.push_back(Vertex_PC(Vector3::ZERO, Rgba::RED));
		vertexes.push_back(Vertex_PC(Vector3(+1.0f, +0.0f, +0.0f), Rgba::RED));
		vertexes.push_back(Vertex_PC(Vector3::ZERO, Rgba::GREEN));
		vertexes.push_back(Vertex_PC(Vector3(+0.0f, +1.0f, +0.0f), Rgba::GREEN));
		vertexes.push_back(Vertex_PC(Vector3::ZERO, Rgba::BLUE));
		vertexes.push_back(Vertex_PC(Vector3(+0.0f, +0.0f, +1.0f), Rgba::BLUE));

		//Create VBO
		m_indexBufferVertCount = vertexes.size();
		m_vboID = g_theRenderer->CreateRenderBuffer(&vertexes[0], m_indexBufferVertCount, sizeof(Vertex_PC));

		//No IBO
		break;
	}
	default:
		ASSERT_OR_DIE(MeshShape_QUAD == shape, "Shape Mesh not implemented.")
			break;
	}
}

Mesh::Mesh()
	: m_vboID(NULL)
	, m_iboID(NULL)
	, m_indexBufferVertCount(0)
	, drawMode(GL_TRIANGLES)
{

}

//-------------------------------------------------------------------------------------------------
/*
Mesh::Mesh(std::string const &textString, float scale /*= 12.f , BitmapFont const *font /*= nullptr)
	: m_vboID(NULL)
	, m_iboID(NULL)
	, m_indexBufferVertCount(0)
	, drawMode(GL_TRIANGLES)
{
	ASSERT_OR_DIE(textString.size() > 0, "Can only create mesh for strings at least the size of 1.")

		//Vertex Definition Layout
		SetVertexLayout(VertexType_PCU);
	std::vector<Vertex_PCU> vertexes;
	std::vector<short> indicies;
	short vertIndex = 0;

	Vector3 origin(Vector3::ZERO);
	Vector3 rightVector(Vector3(1.f, 0.f, 0.f));
	Vector3 upVector(Vector3(0.f, 1.f, 0.f));

	if (font == nullptr)
	{
		return;
		//font = g_theRenderer->GetDefaultFont();

	}

	scale /= (float)font->GetFontSize();
	scale /= (float) 200.f;

	Glyph const *previousGlyph = nullptr;
	Glyph const *undefinedGlyph = font->GetGlyph('?');

	for (char const &glyphToDraw : textString)
	{
		Glyph const *glyph = font->GetGlyph(glyphToDraw);

		//Unknown characters
		if (glyph == nullptr)
		{
			glyph = undefinedGlyph;
		}

		//Adjust for kerning
		Kerning const kerning = font->GetKerning(previousGlyph, glyph);
		origin += rightVector * (kerning.m_amount * scale);

		Vector2i const offset = glyph->GetOffset();
		Vector2i const size = glyph->GetSize();

		//Coords for Quad
		Vector3 const topLeft = origin
			+ rightVector * (float)offset.x * scale
			- upVector * (float)offset.y * scale;

		Vector3 const bottomLeft = topLeft
			- upVector * (float)size.y * scale;

		Vector3 const topRight = topLeft
			+ rightVector * (float)size.x * scale;

		Vector3 const bottomRight = bottomLeft
			+ rightVector * (float)size.x * scale;

		//AABB2 for uvs
		AABB2 const uvTexCoords = glyph->GetTexCoords();

		vertexes.push_back(Vertex_PCU(bottomLeft, Color::WHITE, Vector2(uvTexCoords.mins.x, uvTexCoords.maxs.y)));
		vertexes.push_back(Vertex_PCU(bottomRight, Color::WHITE, uvTexCoords.maxs));
		vertexes.push_back(Vertex_PCU(topRight, Color::WHITE, Vector2(uvTexCoords.maxs.x, uvTexCoords.mins.y)));
		vertexes.push_back(Vertex_PCU(topLeft, Color::WHITE, uvTexCoords.mins));

		indicies.push_back(vertIndex + 0);
		indicies.push_back(vertIndex + 1);
		indicies.push_back(vertIndex + 2);
		indicies.push_back(vertIndex + 0);
		indicies.push_back(vertIndex + 2);
		indicies.push_back(vertIndex + 3);
		vertIndex += 4;

		//Move origin for next glyph
		origin += rightVector * (glyph->m_xadvance * scale);

		previousGlyph = glyph;
	}

	//Create VBO
	m_vboID = g_theRenderer->CreateRenderBuffer(&vertexes[0], vertexes.size(), sizeof(Vertex_PCU));

	//Create IBO
	m_indexBufferVertCount = indicies.size();
	m_iboID = g_theRenderer->CreateRenderBuffer(&indicies[0], m_indexBufferVertCount, sizeof(indicies[0]));
}
*/

//-------------------------------------------------------------------------------------------------
Mesh::~Mesh()
{
	glDeleteBuffers(1, &m_vboID);
	if (m_iboID != NULL)
	{
		glDeleteBuffers(1, &m_iboID);
	}
	m_layout.clear();
}


//-------------------------------------------------------------------------------------------------
int Mesh::GetIBOorVertCount() const
{
	//Returns num of verts when there is no IBO
	return m_indexBufferVertCount;
}


//-------------------------------------------------------------------------------------------------
std::vector< VertexDefinition > Mesh::GetLayout() const
{
	return m_layout;
}
//-------------------------------------------------------------------------------------------------
std::vector< DrawInstruction > const & Mesh::GetDrawInstructions() const
{
	return m_drawInstructions;
}


//-------------------------------------------------------------------------------------------------
int Mesh::GetVertexSize() const
{
	return m_layout[0].m_stride;
}

//-------------------------------------------------------------------------------------------------
void Mesh::SetVertexLayout(VertexType vertexType)
{
	m_layout.clear();
	m_layout.reserve(7);
	switch (vertexType)
	{
	case VertexType_PC:
		m_layout.push_back(VertexDefinition("inPosition", 0, 3, VertexDataType_FLOAT, false, sizeof(Vertex_PC), offsetof(Vertex_PC, m_position)));
		m_layout.push_back(VertexDefinition("inColor", 1, 4, VertexDataType_UBYTE, true, sizeof(Vertex_PC), offsetof(Vertex_PC, m_color)));
		break;
	case VertexType_PCU:
		m_layout.push_back(VertexDefinition("inPosition", 0, 3, VertexDataType_FLOAT, false, sizeof(Vertex_PCU), offsetof(Vertex_PCU, m_position)));
		m_layout.push_back(VertexDefinition("inColor", 1, 4, VertexDataType_UBYTE, true, sizeof(Vertex_PCU), offsetof(Vertex_PCU, m_color)));
		m_layout.push_back(VertexDefinition("inUV0", 2, 2, VertexDataType_FLOAT, false, sizeof(Vertex_PCU), offsetof(Vertex_PCU, m_texCoord)));
		break;
	case VertexType_PCUTB:
		m_layout.push_back(VertexDefinition("inPosition", 0, 3, VertexDataType_FLOAT, false, sizeof(Vertex_PCUTB), offsetof(Vertex_PCUTB, m_position)));
		m_layout.push_back(VertexDefinition("inColor", 1, 4, VertexDataType_UBYTE, true, sizeof(Vertex_PCUTB), offsetof(Vertex_PCUTB, m_color)));
		m_layout.push_back(VertexDefinition("inUV0", 2, 2, VertexDataType_FLOAT, false, sizeof(Vertex_PCUTB), offsetof(Vertex_PCUTB, m_texCoord)));
		m_layout.push_back(VertexDefinition("inTangent", 3, 3, VertexDataType_FLOAT, false, sizeof(Vertex_PCUTB), offsetof(Vertex_PCUTB, m_tangent)));
		m_layout.push_back(VertexDefinition("inBitangent", 4, 3, VertexDataType_FLOAT, false, sizeof(Vertex_PCUTB), offsetof(Vertex_PCUTB, m_biTangent)));
		break;
	case VertexType_PCUTBN:
		m_layout.push_back(VertexDefinition("inPosition", 0, 3, VertexDataType_FLOAT, false, sizeof(Vertex_PCUTBN), offsetof(Vertex_PCUTBN, m_position)));
		m_layout.push_back(VertexDefinition("inColor", 1, 4, VertexDataType_UBYTE, true, sizeof(Vertex_PCUTBN), offsetof(Vertex_PCUTBN, m_color)));
		m_layout.push_back(VertexDefinition("inUV0", 2, 2, VertexDataType_FLOAT, false, sizeof(Vertex_PCUTBN), offsetof(Vertex_PCUTBN, m_texCoord)));
		m_layout.push_back(VertexDefinition("inTangent", 3, 3, VertexDataType_FLOAT, false, sizeof(Vertex_PCUTBN), offsetof(Vertex_PCUTBN, m_tangent)));
		m_layout.push_back(VertexDefinition("inBitangent", 4, 3, VertexDataType_FLOAT, false, sizeof(Vertex_PCUTBN), offsetof(Vertex_PCUTBN, m_biTangent)));
		m_layout.push_back(VertexDefinition("inNormal", 5, 3, VertexDataType_FLOAT, false, sizeof(Vertex_PCUTBN), offsetof(Vertex_PCUTBN, m_normal)));
		break;
	case VertexType_SKINED_MESH:
		m_layout.push_back(VertexDefinition("inPosition", 0, 3, VertexDataType_FLOAT, false, sizeof(Vertex_SKINED_MESH), offsetof(Vertex_SKINED_MESH, m_position)));
		m_layout.push_back(VertexDefinition("inColor", 1, 4, VertexDataType_UBYTE, true, sizeof(Vertex_SKINED_MESH), offsetof(Vertex_SKINED_MESH, m_color)));
		m_layout.push_back(VertexDefinition("inUV0", 2, 2, VertexDataType_FLOAT, false, sizeof(Vertex_SKINED_MESH), offsetof(Vertex_SKINED_MESH, m_texCoord)));
		m_layout.push_back(VertexDefinition("inTangent", 3, 3, VertexDataType_FLOAT, false, sizeof(Vertex_SKINED_MESH), offsetof(Vertex_SKINED_MESH, m_tangent)));
		m_layout.push_back(VertexDefinition("inBitangent", 4, 3, VertexDataType_FLOAT, false, sizeof(Vertex_SKINED_MESH), offsetof(Vertex_SKINED_MESH, m_biTangent)));
		m_layout.push_back(VertexDefinition("inBoneIndices", 5, 4, VertexDataType_INTEGER, false, sizeof(Vertex_SKINED_MESH), offsetof(Vertex_SKINED_MESH, m_boneIndices)));
		m_layout.push_back(VertexDefinition("inBoneWeights", 6, 4, VertexDataType_FLOAT, false, sizeof(Vertex_SKINED_MESH), offsetof(Vertex_SKINED_MESH, m_boneWeights)));
		break;
	case VertexType_SPRITE:
		m_layout.push_back(VertexDefinition("inPosition", 0, 2, VertexDataType_FLOAT, false, sizeof(Vertex_SPRITE), offsetof(Vertex_SPRITE, m_position2D)));
		m_layout.push_back(VertexDefinition("inColor", 1, 4, VertexDataType_UBYTE, true, sizeof(Vertex_SPRITE), offsetof(Vertex_SPRITE, m_color)));
		m_layout.push_back(VertexDefinition("inUV0", 2, 2, VertexDataType_FLOAT, false, sizeof(Vertex_SPRITE), offsetof(Vertex_SPRITE, m_texCoord)));
		break;
	}
	
}

//-------------------------------------------------------------------------------------------------
Mesh::Mesh(MeshBuilder const *meshBuilder, VertexType const &vertexType)
	: m_vboID(NULL)
	, m_iboID(NULL)
{
	SetVertexLayout(vertexType);

	m_drawInstructions.clear();
	m_drawInstructions = meshBuilder->GetDrawInstructions();

	std::vector<VertexMaster> vertexData = meshBuilder->GetVertexData();
	int vertexCount = vertexData.size();

	int vertexSize = GetVertexSize();
	int vertexBufferSize = vertexCount * vertexSize;
	unsigned char *vertexBuffer = new unsigned char[vertexBufferSize];
	unsigned char *dest = vertexBuffer;

	switch (vertexType)
	{
	case VertexType_PC:
		for (VertexMaster vert : vertexData)
		{
			Vertex_PC parsed(vert.m_position, vert.m_color);
			memcpy(dest, &parsed, vertexSize);
			dest += vertexSize;
		}
		break;
	case VertexType_PCU:
		for (VertexMaster vert : vertexData)
		{
			Vertex_PCU parsed(vert.m_position, vert.m_color, vert.m_uv0);
			memcpy(dest, &parsed, vertexSize);
			dest += vertexSize;
		}
		break;
	case VertexType_PCUTB:
		for (VertexMaster vert : vertexData)
		{
			Vertex_PCUTB parsed(vert.m_position, vert.m_color, vert.m_uv0, vert.m_tangent, vert.m_bitangent);
			memcpy(dest, &parsed, vertexSize);
			dest += vertexSize;
		}
		break;
	case VertexType_SKINED_MESH:
		for (VertexMaster vert : vertexData)
		{
			Vertex_SKINED_MESH parsed(vert.m_position, vert.m_color, vert.m_uv0, vert.m_tangent, vert.m_bitangent, vert.m_boneWeights, vert.m_boneIndicies);
			memcpy(dest, &parsed, vertexSize);
			dest += vertexSize;
		}
		break;
	default:
		ASSERT_OR_DIE(vertexType == VertexType_PC, "VertexType not implemented");
		break;
	}

	//Create VBO
	m_vboID = g_theRenderer->CreateRenderBuffer(vertexBuffer, vertexCount, vertexSize);
	m_indexBufferVertCount = vertexCount;

	if (meshBuilder->DoesUseIBO())
	{
		std::vector<short> const & indexBuffer = meshBuilder->GetIndexBuffer();
		m_indexBufferVertCount = indexBuffer.size();
		m_iboID = g_theRenderer->CreateRenderBuffer(&indexBuffer[0], indexBuffer.size(), sizeof(indexBuffer[0]));
	}
}

/*
//-------------------------------------------------------------------------------------------------
void Mesh::UpdateText(std::string newText, float scale /*= 12.f , BitmapFont const *font /*=nullptr)
{
	//Vertex Definition Layout
	SetVertexLayout(VertexType_PCU);
	std::vector<Vertex_PCU> vertexes;
	std::vector<short> indicies;
	short vertIndex = 0;

	Vector3 origin(Vector3::ZERO);
	Vector3 rightVector(Vector3(1.f, 0.f, 0.f));
	Vector3 upVector(Vector3(0.f, 1.f, 0.f));

	if (font == nullptr)
		font = g_theRenderer->GetDefaultFont();

	scale /= (float)font->GetFontSize();
	scale /= (float) 200.f;

	Glyph const *previousGlyph = nullptr;
	Glyph const *undefinedGlyph = font->GetGlyph('?');

	for (char const &glyphToDraw : newText)
	{
		Glyph const *glyph = font->GetGlyph(glyphToDraw);

		//Unknown characters
		if (glyph == nullptr)
		{
			glyph = undefinedGlyph;
		}

		//Adjust for kerning
		Kerning const kerning = font->GetKerning(previousGlyph, glyph);
		origin += rightVector * (kerning.m_amount * scale);

		Vector2i const offset = glyph->GetOffset();
		Vector2i const size = glyph->GetSize();

		//Coords for Quad
		Vector3 const topLeft = origin
			+ rightVector * (float)offset.x * scale
			- upVector * (float)offset.y * scale;

		Vector3 const bottomLeft = topLeft
			- upVector * (float)size.y * scale;

		Vector3 const topRight = topLeft
			+ rightVector * (float)size.x * scale;

		Vector3 const bottomRight = bottomLeft
			+ rightVector * (float)size.x * scale;

		//AABB2 for uvs
		AABB2 const uvTexCoords = glyph->GetTexCoords();

		vertexes.push_back(Vertex_PCU(bottomLeft, Color::WHITE, Vector2(uvTexCoords.mins.x, uvTexCoords.maxs.y)));
		vertexes.push_back(Vertex_PCU(bottomRight, Color::WHITE, uvTexCoords.maxs));
		vertexes.push_back(Vertex_PCU(topRight, Color::WHITE, Vector2(uvTexCoords.maxs.x, uvTexCoords.mins.y)));
		vertexes.push_back(Vertex_PCU(topLeft, Color::WHITE, uvTexCoords.mins));

		indicies.push_back(vertIndex + 0);
		indicies.push_back(vertIndex + 1);
		indicies.push_back(vertIndex + 2);
		indicies.push_back(vertIndex + 0);
		indicies.push_back(vertIndex + 2);
		indicies.push_back(vertIndex + 3);
		vertIndex += 4;

		//Move origin for next glyph
		origin += rightVector * (glyph->m_xadvance * scale);

		previousGlyph = glyph;
	}

	//Update VBO
	g_theRenderer->UpdateRenderBuffer(m_vboID, &vertexes[0], vertexes.size(), sizeof(Vertex_PCU));

	//Create IBO
	m_indexBufferVertCount = indicies.size();
	g_theRenderer->UpdateRenderBuffer(m_iboID, &indicies[0], m_indexBufferVertCount, sizeof(indicies[0]));
}
*/

void Mesh::Update(MeshBuilder const *meshBuilder, VertexType const &vertexType)
{
	SetVertexLayout(vertexType);

	m_drawInstructions.clear();
	m_drawInstructions.reserve(7);
	m_drawInstructions = meshBuilder->GetDrawInstructions();

	const std::vector<VertexMaster>& vertexData = meshBuilder->GetVertexData();
	int vertexCount = vertexData.size();

	int vertexSize = GetVertexSize();
	int vertexBufferSize = vertexCount * vertexSize;
	unsigned char *vertexBuffer = new unsigned char[vertexBufferSize];
	unsigned char *dest = vertexBuffer;

	switch (vertexType)
	{
	case VertexType_PC:
		for (const VertexMaster& vert : vertexData)
		{
			Vertex_PC parsed(vert.m_position, vert.m_color);
			memcpy(dest, &parsed, vertexSize);
			dest += vertexSize;
		}
		break;
	case VertexType_PCU:
		for (const VertexMaster& vert : vertexData)
		{
			Vertex_PCU parsed(vert.m_position, vert.m_color, vert.m_uv0);
			memcpy(dest, &parsed, vertexSize);
			dest += vertexSize;
		}
		break;
	case VertexType_PCUTB:
		for (const VertexMaster& vert : vertexData)
		{
			Vertex_PCUTB parsed(vert.m_position, vert.m_color, vert.m_uv0, vert.m_tangent, vert.m_bitangent);
			memcpy(dest, &parsed, vertexSize);
			dest += vertexSize;
		}
		break;
	case VertexType_SPRITE:
	{
		for (const VertexMaster& vert : vertexData)
		{
			Vertex_SPRITE parsed(vert.m_position2D, vert.m_color, vert.m_uv0);
			memcpy(dest, &parsed, vertexSize);
			dest += vertexSize;
		}
		break;
	}
	default:
		ASSERT_OR_DIE(vertexType == VertexType_PC, "VertexType not implemented");
		break;
	}

	//Update VBO
	g_theRenderer->UpdateRenderBuffer(m_vboID, vertexBuffer, vertexCount, vertexSize);

	if (meshBuilder->DoesUseIBO())
	{
		std::vector<short> const & indexBuffer = meshBuilder->GetIndexBuffer();
		g_theRenderer->UpdateRenderBuffer(m_iboID, &indexBuffer[0], indexBuffer.size(), sizeof(indexBuffer[0]));
	}
	delete[] vertexBuffer;
}

//-------------------------------------------------------------------------------------------------
Mesh  * Mesh::GetMeshShape(MeshShape const &meshShape)
{
	return s_defaultMeshes[meshShape];
}


//-------------------------------------------------------------------------------------------------
STATIC void Mesh::InitializeDefaultMeshes()
{
	for (int meshIndex = 0; meshIndex < MeshShape_COUNT; ++meshIndex)
	{
		s_defaultMeshes.push_back(new Mesh((MeshShape)meshIndex));
	}
}


//-------------------------------------------------------------------------------------------------
STATIC void Mesh::DestroyDefaultMeshes()
{
	for (int meshIndex = 0; meshIndex < MeshShape_COUNT; ++meshIndex)
	{
		delete s_defaultMeshes[meshIndex];
		s_defaultMeshes[meshIndex] = nullptr;
	}
	s_defaultMeshes.resize(0);
}

