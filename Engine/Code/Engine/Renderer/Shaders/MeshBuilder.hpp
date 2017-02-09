#pragma once

#include <vector>
#include <string>

#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/UIntVector4.hpp"
#include "Engine/Math/Rgba.hpp"
#include "Engine/Renderer/Vertex.hpp"
#include "Engine/Renderer/Shaders/DrawInstruction.hpp"

class IBinaryWriter;
class IBinaryReader;

enum eMeshDataFlag
{
	eMeshData_Position = 0,
	eMeshData_Position2D,
	eMeshData_Tangent,
	eMeshData_Bitangent,
	eMeshData_Normal,
	eMeshData_Color,
	eMeshData_UV0,
	eMeshData_UV1,
	eMeshData_COUNT
};


//-------------------------------------------------------------------------------------------------
//#TODO: Put these is a mesh builder utils file
struct PlaneData
{
public:
	Vector3 initialPosition;
	Vector3 rightVector;
	Vector3 upVector;

public:
	PlaneData();
};

struct skin_weight_t
{
	UIntVector4 indices;
	Vector4 weights;
};

//-------------------------------------------------------------------------------------------------
typedef Vector3(PatchFunc)(void const*, float x, float y);
Vector3 PlaneFunc(void const *userArgs, float x, float y);
Vector3 WavesFunc(void const *userArgs, float x, float y);


//-------------------------------------------------------------------------------------------------
class MeshBuilder
{
private:
	bool m_useIBO;
	int m_startIndex;
	VertexMaster m_vertexStamp;
	DrawInstruction m_instructionStamp;
	std::vector<VertexMaster> m_vertexes;
	std::vector<short> m_indicies;
	std::vector<DrawInstruction> m_drawInstructions;
	skin_weight_t m_skinWeightStamp;
	bool m_isSkinned;

	unsigned char m_bitMask;
public:
	MeshBuilder(bool useIBO = false);
	MeshBuilder(std::vector<MeshBuilder*> meshes);
	~MeshBuilder();

	void Begin();
	void End(); //Taylor is awesome

	void SetPrimitiveType(unsigned int primitiveType);
	void SetColor(Rgba const &Rgba);
	void SetTangent(Vector3 const &tangent);
	void SetBitangent(Vector3 const &bitangent);
	void SetNormal(Vector3 const &normal);
	void SetTBN(Vector3 const &tangent, Vector3 const &bitangent, Vector3 const &normal);
	void SetUV0(Vector2 const &uv);
	void SetUV0(float u, float v);
	void SetUV1(Vector2 const &uv);
	void SetUV1(float u, float v);

	void AddVertex(Vector3 const &position);
	void AddVertex2D(Vector2 const &position);
	void AddQuadIndicies(short tl, short tr, short bl, short br);
	void AddTriangle(Vector3 const topLeft, Vector3 const bottomLeft, Vector3 const bottomRight);
	void AddPlane(Vector3 const &pos, Vector3 const &right, Vector3 const &up,
		float xStart, float xEnd, int xSections,
		float yStart, float yEnd, int ySections);
	void AddCustom(Vector3 const &pos, Vector3 const &right, Vector3 const &up,
		float xStart, float xEnd, int xSections,
		float yStart, float yEnd, int ySections);
	void AddPatch(PatchFunc *patchFunc, void const *user_data,
		float xStart, float xEnd, int xSections,
		float yStart, float yEnd, int ySections);
	MeshBuilder CombineMeshBuilders(std::vector<MeshBuilder> meshes);

	std::vector<DrawInstruction> const & GetDrawInstructions() const { return m_drawInstructions; }
	std::vector<VertexMaster> const & GetVertexData() const { return m_vertexes; }
	std::vector<short> const & GetIndexBuffer() const { return m_indicies; }
	int GetCurrentIndex() const { return m_startIndex; }
	bool DoesUseIBO() const { return m_useIBO; }
	void AddIndex(short index);


	//File I/O
	void WriteToFile(std::string fileName);
	void LoadFromFile(std::string fileName);
	void WriteToStream(IBinaryWriter &writer);
	void ReadFromStream(IBinaryReader &reader);

	void WriteDataMask(IBinaryWriter &writer, uint32_t mask);
	uint32_t GetDataMask();
	void WriteVertices(IBinaryWriter & writer, std::vector<VertexMaster> vertexes, uint32_t data_mask);
	void WriteIndices(IBinaryWriter & writer, std::vector<short> indicies);
	void WriteDrawInstructions(IBinaryWriter & writer, std::vector<DrawInstruction> drawInstructions);

	void ReadVertices(IBinaryReader & reader, uint32_t count, uint32_t data_mask, std::vector<VertexMaster>* vertexes);
	void ReadIndices(IBinaryReader & reader, uint32_t count, std::vector<short>* indicies);
	void ReadDrawInstructions(IBinaryReader & reader, uint32_t count, std::vector<DrawInstruction>* drawInstructions);
	void ReadDataMask(IBinaryReader &reader, uint32_t* out_mask);

	void MeshReduce();

	void ClearBoneWeights(); // reset to default, and clear flag

	// Set the bone weights AND mark the data flag
	void SetBoneWeights(UIntVector4 bone_indices, Vector4 bone_weights); //IT'S A STAMP HOLY SHIT
	void RenormalizeSkinWeights();

	bool IsSkinned() const;
};