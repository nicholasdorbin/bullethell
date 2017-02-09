#pragma once

#include <vector>
#include "Engine/Renderer/Vertex.hpp"
#include "Engine/Renderer/VertexDefinition.hpp"
#include "Engine/Renderer/Shaders/MeshBuilder.hpp"
#include "Engine/Core/Memory/UntrackerAllocator.hpp"

//-------------------------------------------------------------------------------------------------
class BitmapFont;


//-------------------------------------------------------------------------------------------------
enum MeshShape
{
	MeshShape_QUAD,
	MeshShape_QUAD_XY,
	MeshShape_QUAD_FULLSCREEN,
	MeshShape_CUBE,
	MeshShape_SPHERE,
	MeshShape_AXIS,
	MeshShape_COUNT,
};




//-------------------------------------------------------------------------------------------------
class Mesh
{
public:
	std::vector<VertexDefinition> m_layout;
	std::vector<DrawInstruction> m_drawInstructions;
	unsigned int m_vboID;
	unsigned int m_iboID;
	unsigned int drawMode;
	int m_indexBufferVertCount;
private:


	static std::vector<Mesh*, TUntrackedAllocator<Mesh*>> s_defaultMeshes;
	

private:
	Mesh(MeshShape const &shape);

public:
	Mesh();
	Mesh(std::string const &textString, float scale = 12.f, BitmapFont const *font = nullptr);
	Mesh(MeshBuilder const *meshBuilder, VertexType const &vertexType);
	~Mesh();

	unsigned int GetVBOID() const { return m_vboID; }
	unsigned int GetIBOID() const { return m_iboID; }
	unsigned int GetDrawMode() const { return drawMode; }
	int GetIBOorVertCount() const;
	std::vector< VertexDefinition > GetLayout() const;
	std::vector< DrawInstruction > const & GetDrawInstructions() const;
	int GetVertexSize() const;
	void SetVertexLayout(VertexType vertexType);
	void UpdateText(std::string newText, float scale = 12.f, BitmapFont const *font = nullptr);
	void Update(MeshBuilder const *meshBuilder, VertexType const &vertexType);

	static Mesh* GetMeshShape(MeshShape const &meshShape);

	static void InitializeDefaultMeshes();
	static void DestroyDefaultMeshes();

};
