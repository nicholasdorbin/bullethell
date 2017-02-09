#include "Engine/Renderer/Shaders/MeshBuilder.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/MathUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/Console.hpp"



//-------------------------------------------------------------------------------------------------
PlaneData::PlaneData()
{
	//Dirty Values
}


//-------------------------------------------------------------------------------------------------
Vector3 PlaneFunc(void const *userArgs, float x, float y)
{
	PlaneData const *plane = (PlaneData const*)userArgs;
	Vector3 position = plane->initialPosition
		+ x * plane->rightVector
		+ y * plane->upVector;

	return position;
}


//-------------------------------------------------------------------------------------------------
Vector3 WavesFunc(void const *userArgs, float x, float y)
{
	//float z = sin( x * y );
	//float z = x * y * y * y - y * x * x * x;
	//float z = cos( abs( x ) + abs( y ) );
	//float z = cos( abs( x ) + abs( y ) )*( abs( x ) + abs( y ) );
	float z = sin(sqrt(x*x + y*y));
	PlaneData const *plane = (PlaneData const*)userArgs;
	Vector3 normal = CrossProduct(plane->rightVector, plane->upVector);
	Vector3 position = plane->initialPosition
		+ x * plane->rightVector
		+ y * plane->upVector
		+ z * normal;

	return position;
}


//-------------------------------------------------------------------------------------------------
MeshBuilder::MeshBuilder(bool useIBO /*= true */)
	: m_useIBO(useIBO)
	, m_startIndex(0)
	, m_vertexStamp()
	, m_instructionStamp(useIBO)
	, m_bitMask(0)
	, m_isSkinned(false)
{
	m_vertexes.reserve(1024);
	m_indicies.reserve(1024);
	m_drawInstructions.reserve(4);
}


MeshBuilder::MeshBuilder(std::vector<MeshBuilder*> meshes)
	: m_useIBO(false)
	, m_startIndex(0)
	, m_vertexStamp()
	, m_instructionStamp(0)
	, m_bitMask(0)
	, m_isSkinned(false)
{
	int currentIBOIndex = 0;
	std::vector<DrawInstruction> shiftedDrawInstructions;


	//a.insert(a.end(), b.begin(), b.end());
	for each (MeshBuilder* mesh in meshes)
	{
		shiftedDrawInstructions.clear();
		m_bitMask = mesh->m_bitMask;
		if (this->m_vertexes.size() > 0)
		{
			currentIBOIndex = this->m_vertexes.size() - 1;
		}

		//Draw Instructions
		shiftedDrawInstructions.insert(shiftedDrawInstructions.end(), mesh->m_drawInstructions.begin(), mesh->m_drawInstructions.end());

		for (auto instructionIter = shiftedDrawInstructions.begin(); instructionIter != shiftedDrawInstructions.end(); ++instructionIter)
		{
			int vertCount = this->m_vertexes.size();
			instructionIter->m_startIndex = instructionIter->m_startIndex + vertCount;
		}

		this->m_drawInstructions.insert(this->m_drawInstructions.end(), shiftedDrawInstructions.begin(), shiftedDrawInstructions.end());

		
		//Vertexes
		this->m_vertexes.insert(this->m_vertexes.end(), mesh->m_vertexes.begin(), mesh->m_vertexes.end());


		//IBOs
		if (mesh->DoesUseIBO())
		{
			this->m_useIBO = true;

			for each (short index in mesh->m_indicies)
			{
				short newIndex = index + (short)currentIBOIndex;
				this->m_indicies.push_back(newIndex);
			}
		}
	}
	
}

//-------------------------------------------------------------------------------------------------
MeshBuilder::~MeshBuilder()
{

}


//-------------------------------------------------------------------------------------------------
void MeshBuilder::Begin()
{


	if (m_useIBO)
	{
		m_startIndex = m_indicies.size();
	}
	else
	{
		m_startIndex = m_vertexes.size();
	}
}


//-------------------------------------------------------------------------------------------------
void MeshBuilder::End()
{
	int vertexCount = 0;
	if (m_useIBO)
	{
		vertexCount = m_indicies.size();
	}
	else
	{
		vertexCount = m_vertexes.size();
	}
	m_instructionStamp.m_startIndex = m_startIndex;
	m_instructionStamp.m_count = vertexCount - m_startIndex;

	m_drawInstructions.push_back(m_instructionStamp);
	m_startIndex = vertexCount;
}


void MeshBuilder::SetPrimitiveType(unsigned int primitiveType)
{
	m_instructionStamp.m_primitiveType = primitiveType;
}

void MeshBuilder::SetColor(Rgba const &Rgba)
{
	m_vertexStamp.m_color = Rgba;
	m_bitMask &= ~(1 << eMeshData_Color);
	m_bitMask |= (1 << eMeshData_Color);
}

void MeshBuilder::SetTangent(Vector3 const &tangent)
{
	m_vertexStamp.m_tangent = tangent;
	m_bitMask &= ~(1 << eMeshData_Tangent);
	m_bitMask |= (1 << eMeshData_Tangent);
}

void MeshBuilder::SetBitangent(Vector3 const &bitangent)
{
	m_vertexStamp.m_bitangent = bitangent;
	m_bitMask &= ~(1 << eMeshData_Bitangent);
	m_bitMask |= (1 << eMeshData_Bitangent);
}

void MeshBuilder::SetNormal(Vector3 const &normal)
{
	m_vertexStamp.m_normal = normal;
	m_bitMask &= ~(1 << eMeshData_Normal);
	m_bitMask |= (1 << eMeshData_Normal);
}

//-------------------------------------------------------------------------------------------------
void MeshBuilder::SetTBN(Vector3 const &tangent, Vector3 const &bitangent, Vector3 const &normal)
{
	SetTangent(tangent);
	SetBitangent(bitangent);
	SetNormal(normal);
}


void MeshBuilder::SetUV0(Vector2 const &uv)
{
	m_vertexStamp.m_uv0 = uv;
	m_bitMask &= ~(1 << eMeshData_UV0);
	m_bitMask |= (1 << eMeshData_UV0);
}

void MeshBuilder::SetUV0(float u, float v)
{
	m_vertexStamp.m_uv0 = Vector2(u, v);
	m_bitMask &= ~(1 << eMeshData_UV0);
	m_bitMask |= (1 << eMeshData_UV0);
}

void MeshBuilder::SetUV1(Vector2 const &uv)
{
	m_vertexStamp.m_uv1 = uv;
	m_bitMask &= ~(1 << eMeshData_UV1);
	m_bitMask |= (1 << eMeshData_UV1);
}

void MeshBuilder::SetUV1(float u, float v)
{
	m_vertexStamp.m_uv1 = Vector2(u, v);
	m_bitMask &= ~(1 << eMeshData_UV1);
	m_bitMask |= (1 << eMeshData_UV1);
}

//-------------------------------------------------------------------------------------------------
void MeshBuilder::AddVertex(Vector3 const &position)
{
	m_vertexStamp.m_position = position;
	if (m_isSkinned)
	{
		m_vertexStamp.m_boneIndicies = m_skinWeightStamp.indices;
		m_vertexStamp.m_boneWeights = m_skinWeightStamp.weights;
	}
	
	m_bitMask &= ~(1 << eMeshData_Position);
	m_bitMask |= (1 << eMeshData_Position);

	m_vertexes.push_back(m_vertexStamp);
}


void MeshBuilder::AddVertex2D(Vector2 const &position)
{
	m_vertexStamp.m_position2D = position;
	if (m_isSkinned)
	{
		m_vertexStamp.m_boneIndicies = m_skinWeightStamp.indices;
		m_vertexStamp.m_boneWeights = m_skinWeightStamp.weights;
	}

	m_bitMask &= ~(1 << eMeshData_Position2D);
	m_bitMask |= (1 << eMeshData_Position2D);

	m_vertexes.push_back(m_vertexStamp);
}

//-------------------------------------------------------------------------------------------------
void MeshBuilder::AddQuadIndicies(short tl, short tr, short bl, short br)
{
	m_indicies.push_back(bl);
	m_indicies.push_back(br);
	m_indicies.push_back(tr);

	m_indicies.push_back(bl);
	m_indicies.push_back(tr);
	m_indicies.push_back(tl);
}


//-------------------------------------------------------------------------------------------------
void MeshBuilder::AddTriangle(Vector3 const topLeft, Vector3 const bottomLeft, Vector3 const bottomRight)
{
	Begin();

	SetColor(Rgba::WHITE);

	// Let's calculate some TBN information
	Vector3 tangent = bottomRight - bottomLeft;
	Vector3 bitangent = topLeft - bottomLeft;
	tangent.SetNormalized();
	bitangent.SetNormalized();
	Vector3 normal = CrossProduct(tangent, bitangent);
	bitangent = CrossProduct(normal, tangent);

	SetTBN(tangent, bitangent, normal);

	SetUV0(0.0f, 0.0f);
	AddVertex(bottomLeft);

	SetUV0(1.0f, 0.0f);
	AddVertex(bottomRight);

	SetUV0(0.0f, 1.0f);
	AddVertex(topLeft);

	AddIndex(0);
	AddIndex(1);
	AddIndex(2);

	End();
}


//-------------------------------------------------------------------------------------------------
void MeshBuilder::AddPlane(Vector3 const &pos, Vector3 const &right, Vector3 const &up, float xStart, float xEnd, int xSections, float yStart, float yEnd, int ySections)
{
	ASSERT_OR_DIE(xSections > 0, "Not enough X Sections.");
	ASSERT_OR_DIE(ySections > 0, "Not enough Y Sections.");
	ASSERT_OR_DIE(m_useIBO, "Needs to use IBOs.");

	PlaneData plane;
	plane.initialPosition = pos;
	plane.rightVector = right;
	plane.upVector = up;

	AddPatch(&PlaneFunc, &plane, xStart, xEnd, xSections, yStart, yEnd, ySections);
}


//-------------------------------------------------------------------------------------------------
void MeshBuilder::AddCustom(Vector3 const &pos, Vector3 const &right, Vector3 const &up, float xStart, float xEnd, int xSections, float yStart, float yEnd, int ySections)
{
	ASSERT_OR_DIE(xSections > 0, "Not enough X Sections.");
	ASSERT_OR_DIE(ySections > 0, "Not enough Y Sections.");
	ASSERT_OR_DIE(m_useIBO, "Needs to use IBOs.");

	PlaneData plane;
	plane.initialPosition = pos;
	plane.rightVector = right;
	plane.upVector = up;

	AddPatch(&WavesFunc, &plane, xStart, xEnd, xSections, yStart, yEnd, ySections);
}


//-------------------------------------------------------------------------------------------------
void MeshBuilder::AddPatch(PatchFunc *patchFunc, void const *userData, float xStart, float xEnd, int xSections, float yStart, float yEnd, int ySections)
{
	ASSERT_OR_DIE(xSections > 0, "Not enough X Sections.");
	ASSERT_OR_DIE(ySections > 0, "Not enough Y Sections.");
	ASSERT_OR_DIE(m_useIBO, "Needs to use IBOs.");

	Begin();

	// So, let's add out our vertices.
	int xVertexCount = xSections + 1;
	int yVertexCount = ySections + 1;

	float const xRange = xEnd - xStart;
	float const yRange = yEnd - yStart;
	float const xStep = xRange / (float)xSections;
	float const yStep = yRange / (float)ySections;

	// Texture goes over the whole patch
	// BONUS - you can provite uv ranges as well.
	float const uStep = 1.0f / (float)xSections;
	float const vStep = 1.0f / (float)ySections;

	int startVertexIndex = GetCurrentIndex();

	// Okay, now, let's add all our vertices
	float x, y;
	float u, v;

	y = yStart;
	v = 0.0f;

	float const delta = .01f; // arbitrarily small value, can go smaller

	for (int iy = 0; iy < yVertexCount; ++iy)
	{
		x = xStart;
		u = 0.0f;

		for (int ix = 0; ix < xVertexCount; ++ix)
		{
			SetUV0(u, v);

			// calculate tangent along u (that is, x)
			Vector3 tangent = patchFunc(userData, x + delta, y)
				- patchFunc(userData, x - delta, y);

			// calculate bitangent along v (taht is, y)
			Vector3 bitangent = patchFunc(userData, x, y + delta)
				- patchFunc(userData, x, y - delta);

			tangent.SetNormalized();
			bitangent.SetNormalized();
			Vector3 normal = CrossProduct(tangent, bitangent);
			bitangent = CrossProduct(normal, tangent);
			SetTBN(tangent, bitangent, normal);

			Vector3 position = patchFunc(userData, x, y);
			AddVertex(position);

			x += xStep;
			u += uStep;
		}

		y += yStep;
		v += vStep;
	}

	// Next, add all the indices for this patch
	for (int iy = 0; iy < ySections; ++iy)
	{
		for (int ix = 0; ix < xSections; ++ix)
		{
			short bottomLeft = (short)(startVertexIndex + (iy * xVertexCount) + ix);
			short bottomRight = (short)(bottomLeft + 1);
			short topLeft = (short)(bottomLeft + xVertexCount);
			short topRight = (short)(topLeft + 1);

			// How many indices is this actually adding
			// under the hood, if we're working with triangles?
			AddQuadIndicies(topLeft, topRight, bottomLeft, bottomRight);
		}
	}

	End();
}


MeshBuilder MeshBuilder::CombineMeshBuilders(std::vector<MeshBuilder> meshes)
{
	MeshBuilder output = MeshBuilder();
	int currentIBOIndex = 0;
	for each (MeshBuilder mesh in meshes)
	{
		currentIBOIndex = output.m_vertexes.size() - 1;

		//a.insert(a.end(), b.begin(), b.end());
		output.m_vertexes.insert(output.m_vertexes.end(), mesh.m_vertexes.begin(), mesh.m_vertexes.end());
		output.m_drawInstructions.insert(output.m_drawInstructions.end(), mesh.m_drawInstructions.begin(), mesh.m_drawInstructions.end());

		if (mesh.DoesUseIBO())
		{
			output.m_useIBO = true;

			for each (short index in mesh.m_indicies)
			{
				short newIndex = index + (short)currentIBOIndex;
				output.m_indicies.push_back(newIndex);
			}
		}
	}
	return output;
}

//-------------------------------------------------------------------------------------------------
void MeshBuilder::AddIndex(short index)
{
	m_indicies.push_back(index);
}

void MeshBuilder::WriteToFile(std::string fileName)
{
	FileBinaryWriter writer;
	if (writer.Open(fileName))
	{
		// Data I want to Write
		WriteToStream(writer);
		writer.Close();
	}
	
	
	
}

void MeshBuilder::LoadFromFile(std::string fileName)
{
	FileBinaryReader reader;
	if (reader.Open(fileName))
	{
		ReadFromStream(reader);
		reader.Close();
	}
	// Data I want to Read

	
}

void MeshBuilder::WriteToStream(IBinaryWriter &writer)
{
	// FILE VERSION
	// material id
	// vertex data mask (ie: position, tangent, normal, etc...)
	// vertices
	// indices
	// draw_instructions

	//writer.Write<uint32_t>(FILE_VERSION);

	//writer.WriteString(get_material_id());

	uint32_t vertex_count = m_vertexes.size();
	uint32_t indices_count = m_indicies.size();
	uint32_t draw_instruction_count = m_drawInstructions.size();

	writer.Write<uint32_t>(vertex_count);
	writer.Write<uint32_t>(indices_count);
	writer.Write<uint32_t>(draw_instruction_count);

	uint32_t data_mask = GetDataMask();
	WriteDataMask(writer, data_mask);

	WriteVertices(writer, m_vertexes, data_mask);
	WriteIndices(writer, m_indicies);
	WriteDrawInstructions(writer, m_drawInstructions);
}

void MeshBuilder::ReadFromStream(IBinaryReader &reader)
{
	// FILE VERSION
	// material id
	// vertex data mask (ie: position, tangent, normal, etc...)
	// vertices
	// indices
	// draw_instructions

	//reader.Read<uint32_t>(FILE_VERSION);

	//reader.Read(get_material_id());

	uint32_t vertex_count = 0;
	uint32_t indices_count = 0;
	uint32_t draw_instruction_count = 0;

	reader.Read(&vertex_count);
	reader.Read(&indices_count);
	reader.Read(&draw_instruction_count);

	uint32_t data_mask = 0;
	ReadDataMask(reader, &data_mask);


	std::vector<VertexMaster> vertexes;
	vertexes.reserve(vertex_count);
	std::vector<short> indicies;
	indicies.reserve(indices_count);
	std::vector<DrawInstruction> drawInstructions;
	drawInstructions.reserve(draw_instruction_count);

	ReadVertices(reader, vertex_count, data_mask, &vertexes);
	ReadIndices(reader, indices_count, &indicies);
	ReadDrawInstructions(reader, draw_instruction_count, &drawInstructions);

	m_vertexes = vertexes;
	m_indicies = indicies;
	m_drawInstructions = drawInstructions;

	if (m_indicies.size() > 0 )
	{
		m_useIBO = true;
	}

}

void MeshBuilder::WriteDataMask(IBinaryWriter &writer, uint32_t mask)
{
	if ((mask & (1 << eMeshData_Position)) != 0) {
		writer.WriteString("position");
	}
	if ((mask & (1 << eMeshData_Tangent)) != 0) {
		writer.WriteString("tangent");
	}
	if ((mask & (1 << eMeshData_Bitangent)) != 0) {
		writer.WriteString("bitangent");
	}
	if ((mask & (1 << eMeshData_Normal)) != 0) {
		writer.WriteString("normal");
	}
	if ((mask & (1 << eMeshData_Color)) != 0) {
		writer.WriteString("color");
	}
	if ((mask & (1 << eMeshData_UV0)) != 0) {
		writer.WriteString("uv0");
	}
	if ((mask & (1 << eMeshData_UV1)) != 0) {
		writer.WriteString("uv1");
	}

	writer.WriteString("END");
}

uint32_t MeshBuilder::GetDataMask()
{
	return m_bitMask;
}

void MeshBuilder::WriteVertices(IBinaryWriter & writer, std::vector<VertexMaster> vertexes, uint32_t data_mask)
{
	for (VertexMaster vertex : vertexes)
	{
		if ((data_mask & (1 << eMeshData_Position)) != 0) {
			writer.Write<Vector3>(vertex.m_position);
		}
		if ((data_mask & (1 << eMeshData_Tangent)) != 0) {
			writer.Write<Vector3>(vertex.m_tangent);
		}
		if ((data_mask & (1 << eMeshData_Bitangent)) != 0) {
			writer.Write<Vector3>(vertex.m_bitangent);
		}
		if ((data_mask & (1 << eMeshData_Normal)) != 0) {
			writer.Write<Vector3>(vertex.m_normal);
		}
		if ((data_mask & (1 << eMeshData_Color)) != 0) {
			writer.Write<uint32_t>(vertex.m_color.m_red);
			writer.Write<uint32_t>(vertex.m_color.m_green);
			writer.Write<uint32_t>(vertex.m_color.m_blue);
			writer.Write<uint32_t>(vertex.m_color.m_alpha);
		}
		if ((data_mask & (1 << eMeshData_UV0)) != 0) {
			writer.Write<Vector2>(vertex.m_uv0);
		}
		if ((data_mask & (1 << eMeshData_UV1)) != 0) {
			writer.Write<Vector2>(vertex.m_uv1);
		}
	}
}

void MeshBuilder::WriteIndices(IBinaryWriter & writer, std::vector<short> indicies)
{
	std::vector< int > intIndicies;
	for (short index : indicies)
	{
		//intIndicies.push_back((short)index);
		writer.Write<int>( (int)index);
	}
	//writer.WriteInts(intIndicies);
}

void MeshBuilder::WriteDrawInstructions(IBinaryWriter & writer, std::vector<DrawInstruction> drawInstructions)
{
	for (DrawInstruction drawInstruction : drawInstructions)
	{
		writer.Write< bool >(drawInstruction.m_useIndexBuffer);
		writer.Write< uint32_t >(drawInstruction.m_primitiveType);
		writer.Write< uint32_t >(drawInstruction.m_startIndex);
		writer.Write< uint32_t >(drawInstruction.m_count);
	}
}



void MeshBuilder::ReadVertices(IBinaryReader & reader, uint32_t count, uint32_t data_mask, std::vector<VertexMaster>* vertexes)
{
	for (unsigned int i = 0; i < count; ++i)
	{
		VertexMaster thisVertex;

		if ((data_mask & (1 << eMeshData_Position)) != 0) {
			reader.Read<Vector3>(&(thisVertex.m_position));
// 			std::vector< float > positionData;
// 			reader.ReadFloats(&positionData);
// 			thisVertex.m_position.x = positionData[0];
// 			thisVertex.m_position.y = positionData[1];
// 			thisVertex.m_position.z = positionData[2];
		}
		if ((data_mask & (1 << eMeshData_Tangent)) != 0) {
			reader.Read<Vector3>(&(thisVertex.m_tangent));
// 			std::vector< float > tangentData;
// 			reader.ReadFloats(&tangentData);
// 			thisVertex.m_tangent.x = tangentData[0];
// 			thisVertex.m_tangent.y = tangentData[1];
// 			thisVertex.m_tangent.z = tangentData[2];
		}
		if ((data_mask & (1 << eMeshData_Bitangent)) != 0) {
			reader.Read<Vector3>(&(thisVertex.m_bitangent));
// 			std::vector< float > bitangentData;
// 			reader.ReadFloats(&bitangentData);
// 			thisVertex.m_bitangent.x = bitangentData[0];
// 			thisVertex.m_bitangent.y = bitangentData[1];
// 			thisVertex.m_bitangent.z = bitangentData[2];
		}
		if ((data_mask & (1 << eMeshData_Normal)) != 0) {
			reader.Read<Vector3>(&(thisVertex.m_normal));
// 			std::vector< float > normalData;
// 			reader.ReadFloats(&normalData);
// 			thisVertex.m_normal.x = normalData[0];
// 			thisVertex.m_normal.y = normalData[1];
// 			thisVertex.m_normal.z = normalData[2];
		}
		if ((data_mask & (1 << eMeshData_Color)) != 0) {
			uint32_t red = 0;
			uint32_t green = 0;
			uint32_t blue = 0;
			uint32_t alpha = 0;

			reader.Read(&red);
			reader.Read(&green);
			reader.Read(&blue);
			reader.Read(&alpha);

			thisVertex.m_color.m_red = (unsigned char) red;
			thisVertex.m_color.m_green = (unsigned char) green;
			thisVertex.m_color.m_blue = (unsigned char) blue;
			thisVertex.m_color.m_alpha = (unsigned char) alpha;
		}
		if ((data_mask & (1 << eMeshData_UV0)) != 0) {
			reader.Read<Vector2>(&(thisVertex.m_uv0));
// 			std::vector< float > uv0Data;
// 			reader.ReadFloats(&uv0Data);
// 			thisVertex.m_uv0.x = uv0Data[0];
// 			thisVertex.m_uv0.y = uv0Data[1];
		}
		if ((data_mask & (1 << eMeshData_UV1)) != 0) {
			reader.Read<Vector2>(&(thisVertex.m_uv1));
// 			std::vector< float > uv1Data;
// 			reader.ReadFloats(&uv1Data);
// 			thisVertex.m_uv1.x = uv1Data[0];
// 			thisVertex.m_uv1.y = uv1Data[1];
		}

		vertexes->push_back(thisVertex);
	}
}

void MeshBuilder::ReadIndices(IBinaryReader & reader, uint32_t count, std::vector<short>* indicies)
{
	
	for (unsigned int index = 0; index < count; ++index)
	{
		int iboIndex = 0;
		reader.Read<int>(&(iboIndex));
		indicies->push_back((short)iboIndex);
	}
}


void MeshBuilder::ReadDrawInstructions(IBinaryReader & reader, uint32_t count, std::vector<DrawInstruction>* drawInstructions)
{
	/*
	for (DrawInstruction drawInstruction : drawInstructions)
	{
		writer.Write< bool >(drawInstruction.m_useIndexBuffer);
		writer.Write< uint32_t >(drawInstruction.m_primitiveType);
		writer.Write< uint32_t >(drawInstruction.m_startIndex);
		writer.Write< uint32_t >(drawInstruction.m_count);
	}
	*/

	for (unsigned int i = 0; i < count; ++i)
	{
		DrawInstruction thisInstruction;
		
		bool useIndexBuffer;
		uint32_t primitiveType;
		uint32_t startIndex;
		uint32_t drawCount;

		reader.Read(&useIndexBuffer);
		reader.Read(&primitiveType);
		reader.Read(&startIndex);
		reader.Read(&drawCount);

		thisInstruction.m_useIndexBuffer = useIndexBuffer;
		thisInstruction.m_primitiveType = primitiveType;
		thisInstruction.m_startIndex = startIndex;
		thisInstruction.m_count = drawCount;

		drawInstructions->push_back(thisInstruction);
	}
}

void MeshBuilder::ReadDataMask(IBinaryReader &reader, uint32_t* out_mask)
{
	std::string out_str = "";
	uint32_t output = 0;

	reader.ReadString(&out_str);
	while (strcmp(out_str.c_str(), "END") != 0)
	{
		if ( strcmp(out_str.c_str(), "position") == 0 )
		{
			uint32_t shift = (1 << eMeshData_Position);
			output |= shift;
		}
		else if (strcmp(out_str.c_str(), "tangent") == 0)
		{
			output |= (1 << eMeshData_Tangent);
		}
		else if (strcmp(out_str.c_str(), "bitangent") == 0)
		{
			output |= (1 << eMeshData_Bitangent);
		}
		else if (strcmp(out_str.c_str(), "normal") == 0)
		{
			output |= (1 << eMeshData_Normal);
		}
		else if (strcmp(out_str.c_str(), "color") == 0)
		{
			output |= (1 << eMeshData_Color);
		}
		else if ( strcmp(out_str.c_str(), "uv0") == 0 )
		{
			output |= (1 << eMeshData_UV0);
		}
		else if (strcmp(out_str.c_str(), "uv1") == 0)
		{
			output |= (1 << eMeshData_UV1);
		}
		reader.ReadString(&out_str);
	}
	*out_mask = output;
}

void MeshBuilder::MeshReduce()
{
	std::vector<VertexMaster> trimmedVerts;
	std::vector<short> indicies;


	for (VertexMaster vertex : m_vertexes)
	{
		bool dupeFound = false;
		for (unsigned int i = 0; i < trimmedVerts.size(); i++)
		{
			if (trimmedVerts[i] == vertex)
			{
				//Dupe Vert
				dupeFound = true;
				indicies.push_back((short)i);
				break;
			}
			
		}
		if (!dupeFound)
		{
			//Unique Vert
			trimmedVerts.push_back(vertex);
			indicies.push_back((short) trimmedVerts.size() - 1);
		}
	}

	m_drawInstructions.clear();
	DrawInstruction newDrawInstruction;
	newDrawInstruction.m_useIndexBuffer = true;
	newDrawInstruction.m_count = indicies.size();
	newDrawInstruction.m_primitiveType = 0x0004; //GL_TRIANGLES
	newDrawInstruction.m_startIndex = 0;

	m_drawInstructions.push_back(newDrawInstruction);
	m_vertexes = trimmedVerts;
	m_indicies = indicies;
	m_useIBO = true;
}

void MeshBuilder::ClearBoneWeights()
{
	m_skinWeightStamp.weights = Vector4(1.0f, 0.0f, 0.0f, 0.0f);
	m_skinWeightStamp.indices = UIntVector4(0, 0, 0, 0);
	m_isSkinned = false;
}

void MeshBuilder::SetBoneWeights(UIntVector4 bone_indices, Vector4 bone_weights)
{
	m_skinWeightStamp.weights = bone_weights;
	m_skinWeightStamp.indices = bone_indices;

	float total_weight = m_skinWeightStamp.weights.x + m_skinWeightStamp.weights.y + m_skinWeightStamp.weights.z + m_skinWeightStamp.weights.w;
	if (total_weight > 0.0f) {
		m_skinWeightStamp.weights = m_skinWeightStamp.weights / total_weight;
		
	}
	else 
	{ 
		m_skinWeightStamp.weights = Vector4( 1.0f, 0.0f, 0.0f, 0.0f );
	};

	m_isSkinned = true;
}

void MeshBuilder::RenormalizeSkinWeights()
{
	//m_skinWeightStamp.weights.SetNormalized();
}

bool MeshBuilder::IsSkinned() const
{
	return m_isSkinned;
}

