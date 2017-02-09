#include "Engine/Renderer/Shaders/MeshRenderer.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Renderer/Shaders/Material.hpp"
#include "Engine/Renderer/Shaders/Mesh.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Shaders/Uniform.hpp"
#include "Engine/Renderer/Shaders/Attribute.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Shaders/Light.hpp"


//-------------------------------------------------------------------------------------------------
MeshRenderer::MeshRenderer(MeshShape const &meshShape, Transform const &transform /*= Transform( )*/, RenderState const &renderState /*= RenderState::BASIC_3D*/)
	: m_transform(transform)
	, m_renderState(renderState)
	, m_vaoID(NULL)
	, m_mesh(nullptr)
	, m_material(nullptr)
{
	//m_mesh = Mesh::GetMeshShape(meshShape);
	//m_material = Material::GetDefault();
	meshShape;
	//Create VAO
	glGenVertexArrays(1, &m_vaoID);
	ASSERT_OR_DIE(m_vaoID != NULL, "VAO didn't generate.");

	g_theRenderer->BindMeshToVAO(m_vaoID, m_mesh, m_material);
}


//-------------------------------------------------------------------------------------------------
MeshRenderer::MeshRenderer(Mesh *mesh, Transform const &transform /*= Transform( )*/, RenderState const &renderState /*= RenderState::BASIC_3D */)
	: m_transform(transform)
	, m_renderState(renderState)
	, m_vaoID(NULL)
	, m_mesh(mesh)
	, m_material(nullptr)
{
	//m_material = Material::GetDefault();

	//Create VAO
	glGenVertexArrays(1, &m_vaoID);
	ASSERT_OR_DIE(m_vaoID != NULL, "VAO didn't generate.");

	g_theRenderer->BindMeshToVAO(m_vaoID, m_mesh, m_material);
}


//-------------------------------------------------------------------------------------------------
MeshRenderer::MeshRenderer(Mesh *mesh, Material *material, Transform const &transform /*= Transform() */, RenderState const &renderState /*= RenderState::BASIC_3D*/)
	: m_transform(transform)
	, m_renderState(renderState)
	, m_vaoID(NULL)
	, m_mesh(mesh)
	, m_material(material)
{
	//Create VAO
	glGenVertexArrays(1, &m_vaoID);
	ASSERT_OR_DIE(m_vaoID != NULL, "VAO didn't generate.");

	g_theRenderer->BindMeshToVAO(m_vaoID, m_mesh, m_material);

	//Clear Matricies
	SetUniform("gModel", GetModelMatrix());
	SetUniform("gView", Matrix4::IDENTITY);
	SetUniform("gProj", Matrix4::IDENTITY);
}


MeshRenderer::MeshRenderer()
	: m_transform(Transform())
	, m_renderState(RenderState::BASIC_2D)
	, m_vaoID(NULL)
	, m_mesh(nullptr)
	, m_material(nullptr)

{

}

//-------------------------------------------------------------------------------------------------
MeshRenderer::~MeshRenderer()
{
	glDeleteVertexArrays(1, &m_vaoID);
	auto iter = m_uniforms.begin();
	while (iter != m_uniforms.end())
	{
		if (iter->second != nullptr)
		{
			delete iter->second;
			iter->second = nullptr;
		}
		++iter;
	}
}


//-------------------------------------------------------------------------------------------------
void MeshRenderer::Update()
{
	g_theRenderer->SetMatrixUniforms(this);
}


void MeshRenderer::BindMesh()
{
	glGenVertexArrays(1, &m_vaoID);
	ASSERT_OR_DIE(m_vaoID != NULL, "VAO didn't generate.");

	g_theRenderer->BindMeshToVAO(m_vaoID, m_mesh, m_material);

	//Clear Matricies
	SetUniform("gModel", GetModelMatrix());
	SetUniform("gView", Matrix4::IDENTITY);
	SetUniform("gProj", Matrix4::IDENTITY);
}

void MeshRenderer::BindMeshNoVAOGen()
{
	ASSERT_OR_DIE(m_vaoID != NULL, "VAO didn't generate.");

	g_theRenderer->BindMeshToVAO(m_vaoID, m_mesh, m_material);

	//Clear Matricies
	SetUniform("gModel", GetModelMatrix());
	SetUniform("gView", Matrix4::IDENTITY);
	SetUniform("gProj", Matrix4::IDENTITY);
}

void MeshRenderer::BindMatrixes()
{
	//Set Render State
	g_theRenderer->ApplyRenderState(GetRenderState());

	//Get Model View Projection Matrixes
	Matrix4 model = GetModelMatrix();
	Matrix4 view = GetViewMatrix();
	Matrix4 proj = GetProjMatrix();

	//Set Model View Projection Matrixes
	unsigned int gpuProgramID = GetGPUProgramID();
	g_theRenderer->ShaderProgramSetMat4(gpuProgramID, "gModel", model);
	g_theRenderer->ShaderProgramSetMat4(gpuProgramID, "gView", view);
	g_theRenderer->ShaderProgramSetMat4(gpuProgramID, "gProj", proj);
}

//-------------------------------------------------------------------------------------------------
RenderState MeshRenderer::GetRenderState() const
{
	return m_renderState;
}


void MeshRenderer::SetRenderState(RenderState state)
{
	m_renderState = state;
}

//-------------------------------------------------------------------------------------------------
void MeshRenderer::SetLineWidth(float lineWidth)
{
	m_renderState.m_lineWidth = lineWidth;
}


//-------------------------------------------------------------------------------------------------
//#TODO: Could pre-calculate for each time the model is transformed
Matrix4 MeshRenderer::GetModelMatrix() const
{
	return m_transform.GetModelMatrix();
}


Matrix4 MeshRenderer::GetViewMatrix() const
{
	return m_viewMatrix;
}

Matrix4 MeshRenderer::GetProjMatrix() const
{
	return m_projMatrix;
}

void MeshRenderer::SetViewMatrix(Matrix4 mat)
{
	m_viewMatrix = mat;
}

void MeshRenderer::SetProjMatrix(Matrix4 mat)
{
	m_projMatrix = mat;
}

//-------------------------------------------------------------------------------------------------
unsigned int MeshRenderer::GetGPUProgramID() const
{
	return m_material->GetGPUProgramID();
}


//-------------------------------------------------------------------------------------------------
unsigned int MeshRenderer::GetSamplerID() const
{
	return m_material->GetSamplerID();
}


//-------------------------------------------------------------------------------------------------
unsigned int MeshRenderer::GetVAOID() const
{
	return m_vaoID;
}


//-------------------------------------------------------------------------------------------------
unsigned int MeshRenderer::GetIBOID() const
{
	return m_mesh->GetIBOID();
}


//-------------------------------------------------------------------------------------------------
unsigned int MeshRenderer::GetDrawMode() const
{
	return m_mesh->GetDrawMode();
}


//-------------------------------------------------------------------------------------------------
int MeshRenderer::GetVertCount() const
{
	return m_mesh->GetIBOorVertCount();
}


//-------------------------------------------------------------------------------------------------
int MeshRenderer::GetIndexBufferCount() const
{
	return m_mesh->GetIBOorVertCount();
}


std::vector<DrawInstruction> const & MeshRenderer::GetDrawInstructions() const
{
	return m_mesh->GetDrawInstructions();
}

//-------------------------------------------------------------------------------------------------
void MeshRenderer::SetPosition(Vector3 const &pos)
{
	m_transform.SetPosition(pos);
}


//-------------------------------------------------------------------------------------------------
void MeshRenderer::SetPosition(float xPos, float yPos, float zPos)
{
	m_transform.SetPosition(Vector3(xPos, yPos, zPos));
}


//-------------------------------------------------------------------------------------------------
void MeshRenderer::SetTransform(Transform const &transform)
{
	m_transform = transform;
}


void MeshRenderer::SetMaterial(Material* material)
{
	m_material = material;
}

void MeshRenderer::SetMesh(Mesh* mesh)
{
	m_mesh = mesh;
}

//-------------------------------------------------------------------------------------------------
const std::map<size_t, Uniform*>& MeshRenderer::GetUniformList() const
{
	return m_uniforms;
}


std::map<size_t, Uniform*>& MeshRenderer::GetUniformList()
{
	return m_uniforms;
}


//-------------------------------------------------------------------------------------------------
const std::map<size_t, Uniform*>& MeshRenderer::GetMaterialUniformList() const
{
	return m_material->GetUniformList();
}


std::map<size_t, Uniform*>& MeshRenderer::GetMaterialUniformList()
{
	return m_material->GetUniformList();
}



//-------------------------------------------------------------------------------------------------
const std::map<size_t, Attribute*>& MeshRenderer::GetMaterialAttributeList() const
{
	return m_material->GetAttributeList();
}


std::map<size_t, Attribute*>& MeshRenderer::GetMaterialAttributeList()
{
	return m_material->GetAttributeList();
}

//-------------------------------------------------------------------------------------------------
void MeshRenderer::SetUniform(std::string const &uniformName, unsigned int uniformValue)
{
	std::map<size_t, Uniform*>& uniformList = m_material->GetUniformList();
	//First time being set
	size_t nameHash = std::hash<std::string>{}(uniformName);
	if (uniformList[nameHash] != nullptr && m_uniforms[nameHash] == nullptr)
	{
		m_uniforms[nameHash] = new Uniform
			(
				((Uniform*)uniformList[nameHash])->m_bindPoint,
				((Uniform*)uniformList[nameHash])->m_length,
				((Uniform*)uniformList[nameHash])->m_size,
				((Uniform*)uniformList[nameHash])->m_type,
				((Uniform*)uniformList[nameHash])->m_name,
				new int(uniformValue) //Set new value
				);
	}
	//Updating value
	else if (m_uniforms[nameHash] != nullptr)
	{
		*((int*)m_uniforms[nameHash]->m_data) = uniformValue;
	}
	else
	{
		//ASSERT_OR_DIE(false, "Uniform doesn't exist on Material");
	}
}


//-------------------------------------------------------------------------------------------------
void MeshRenderer::SetUniform(std::string const &uniformName, int uniformValue)
{
	std::map<size_t, Uniform*>& uniformList = m_material->GetUniformList();
	//First time being set
	size_t nameHash = std::hash<std::string>{}(uniformName);
	if (uniformList[nameHash] != nullptr && m_uniforms[nameHash] == nullptr)
	{
		m_uniforms[nameHash] = new Uniform
			(
				((Uniform*)uniformList[nameHash])->m_bindPoint,
				((Uniform*)uniformList[nameHash])->m_length,
				((Uniform*)uniformList[nameHash])->m_size,
				((Uniform*)uniformList[nameHash])->m_type,
				((Uniform*)uniformList[nameHash])->m_name,
				new int(uniformValue) //Set new value
				);
	}
	//Updating value
	else if (m_uniforms[nameHash] != nullptr)
	{
		*((int*)m_uniforms[nameHash]->m_data) = uniformValue;
	}
	else
	{
		//ASSERT_OR_DIE(false, "Uniform doesn't exist on Material");
	}
}


//-------------------------------------------------------------------------------------------------
void MeshRenderer::SetUniform(std::string const &uniformName, float uniformValue)
{
	std::map<size_t, Uniform*>& uniformList = m_material->GetUniformList();
	size_t nameHash = std::hash<std::string>{}(uniformName);
	if (uniformList[nameHash] != nullptr && m_uniforms[nameHash] == nullptr)
	{
		m_uniforms[nameHash] = new Uniform
			(
				((Uniform*)uniformList[nameHash])->m_bindPoint,
				((Uniform*)uniformList[nameHash])->m_length,
				((Uniform*)uniformList[nameHash])->m_size,
				((Uniform*)uniformList[nameHash])->m_type,
				((Uniform*)uniformList[nameHash])->m_name,
				new float(uniformValue) //Set new value
				);
	}
	else if (m_uniforms[nameHash] != nullptr)
	{
		*((float*)m_uniforms[nameHash]->m_data) = uniformValue;
	}
	else
	{
		//ASSERT_OR_DIE(false, "Uniform doesn't exist on Material");
	}
}


//-------------------------------------------------------------------------------------------------
void MeshRenderer::SetUniform(std::string const &uniformName, Vector2 const &uniformValue)
{
	std::map<size_t, Uniform*>& uniformList = m_material->GetUniformList();
	size_t nameHash = std::hash<std::string>{}(uniformName);
	if (uniformList[nameHash] != nullptr && m_uniforms[nameHash] == nullptr)
	{
		m_uniforms[nameHash] = new Uniform
			(
				((Uniform*)uniformList[nameHash])->m_bindPoint,
				((Uniform*)uniformList[nameHash])->m_length,
				((Uniform*)uniformList[nameHash])->m_size,
				((Uniform*)uniformList[nameHash])->m_type,
				((Uniform*)uniformList[nameHash])->m_name,
				new Vector2(uniformValue) //Set new value
				);
	}
	else if (m_uniforms[nameHash] != nullptr)
	{
		*((Vector2*)m_uniforms[nameHash]->m_data) = uniformValue;
	}
	else
	{
		//ASSERT_OR_DIE(false, "Uniform doesn't exist on Material");
	}
}


//-------------------------------------------------------------------------------------------------
void MeshRenderer::SetUniform(std::string const &uniformName, Vector3 const &uniformValue)
{
	std::map<size_t, Uniform*>& uniformList = m_material->GetUniformList();
	size_t nameHash = std::hash<std::string>{}(uniformName);
	if (uniformList[nameHash] != nullptr && m_uniforms[nameHash] == nullptr)
	{
		m_uniforms[nameHash] = new Uniform
			(
				((Uniform*)uniformList[nameHash])->m_bindPoint,
				((Uniform*)uniformList[nameHash])->m_length,
				((Uniform*)uniformList[nameHash])->m_size,
				((Uniform*)uniformList[nameHash])->m_type,
				((Uniform*)uniformList[nameHash])->m_name,
				new Vector3(uniformValue) //Set new value
				);
	}
	else if (m_uniforms[nameHash] != nullptr)
	{
		*((Vector3*)m_uniforms[nameHash]->m_data) = uniformValue;
	}
	else
	{
		//ASSERT_OR_DIE(false, "Uniform doesn't exist on Material");
	}
}


//-------------------------------------------------------------------------------------------------
void MeshRenderer::SetUniform(std::string const &uniformName, Vector4 const &uniformValue)
{
	std::map<size_t, Uniform*>& uniformList = m_material->GetUniformList();
	size_t nameHash = std::hash<std::string>{}(uniformName);
	if (uniformList[nameHash] != nullptr && m_uniforms[nameHash] == nullptr)
	{
		m_uniforms[nameHash] = new Uniform
			(
				((Uniform*)uniformList[nameHash])->m_bindPoint,
				((Uniform*)uniformList[nameHash])->m_length,
				((Uniform*)uniformList[nameHash])->m_size,
				((Uniform*)uniformList[nameHash])->m_type,
				((Uniform*)uniformList[nameHash])->m_name,
				new Vector4(uniformValue) //Set new value
				);
	}
	else if (m_uniforms[nameHash] != nullptr)
	{
		*((Vector4*)m_uniforms[nameHash]->m_data) = uniformValue;
	}
	else
	{
		//ASSERT_OR_DIE(false, "Uniform doesn't exist on Material");
	}
}


//-------------------------------------------------------------------------------------------------
void MeshRenderer::SetUniform(std::string const &uniformName, Matrix4 const &uniformValue)
{
	g_theProfiler->StartProfilerSample("Matrix SetUniform");
	g_theProfiler->StartProfilerSample("Matrix GetAndHash");
	g_theProfiler->StartProfilerSample("Get");
	std::map<size_t, Uniform*>& uniformList = m_material->GetUniformList();
	g_theProfiler->EndProfilerSample();
	g_theProfiler->StartProfilerSample("Hash");
	size_t nameHash = std::hash<std::string>{}(uniformName);
	g_theProfiler->EndProfilerSample();
	g_theProfiler->EndProfilerSample();
	if (uniformList[nameHash] != nullptr && m_uniforms[nameHash] == nullptr)
	{
		g_theProfiler->StartProfilerSample("Matrix SetUniform NewValue");
		m_uniforms[nameHash] = new Uniform
			(
				((Uniform*)uniformList[nameHash])->m_bindPoint,
				((Uniform*)uniformList[nameHash])->m_length,
				((Uniform*)uniformList[nameHash])->m_size,
				((Uniform*)uniformList[nameHash])->m_type,
				((Uniform*)uniformList[nameHash])->m_name,
				new Matrix4(uniformValue) //Set new value
				);
		g_theProfiler->EndProfilerSample();
	}
	else if (m_uniforms[nameHash] != nullptr)
	{
		g_theProfiler->StartProfilerSample("Matrix SetUniform Replace");
		*((Matrix4*)m_uniforms[nameHash]->m_data) = uniformValue;
		g_theProfiler->EndProfilerSample();
	}
	else
	{
		//ASSERT_OR_DIE(false, "Uniform doesn't exist on Material");
	}
	g_theProfiler->EndProfilerSample();
}


//-------------------------------------------------------------------------------------------------
void MeshRenderer::SetUniform(std::string const &uniformName, Rgba const &uniformValue)
{
	std::map<size_t, Uniform*>& uniformList = m_material->GetUniformList();
	size_t nameHash = std::hash<std::string>{}(uniformName);
	if (uniformList[nameHash] != nullptr && m_uniforms[nameHash] == nullptr)
	{
		m_uniforms[nameHash] = new Uniform
			(
				((Uniform*)uniformList[nameHash])->m_bindPoint,
				((Uniform*)uniformList[nameHash])->m_length,
				((Uniform*)uniformList[nameHash])->m_size,
				((Uniform*)uniformList[nameHash])->m_type,
				((Uniform*)uniformList[nameHash])->m_name,
				new Vector4(uniformValue.FloatRepresentation()) //Set new value
				);
	}
	else if (m_uniforms[nameHash] != nullptr)
	{
		*((Vector4*)m_uniforms[nameHash]->m_data) = uniformValue.FloatRepresentation();
	}
	else
	{
		//ASSERT_OR_DIE(false, "Uniform doesn't exist on Material");
	}
}


//-------------------------------------------------------------------------------------------------
void MeshRenderer::SetUniform(std::string const &uniformName, std::string const &uniformValue)
{
	std::map<size_t, Uniform*>& uniformList = m_material->GetUniformList();
	Texture const *generatedTexture = Texture::CreateOrGetTexture(uniformValue);//Texture::CreateOrLoadTexture(uniformValue);
	size_t nameHash = std::hash<std::string>{}(uniformName);
	if (uniformList[nameHash] != nullptr && m_uniforms[nameHash] == nullptr)
	{
		m_uniforms[nameHash] = new Uniform
			(
				((Uniform*)uniformList[nameHash])->m_bindPoint,
				((Uniform*)uniformList[nameHash])->m_length,
				((Uniform*)uniformList[nameHash])->m_size,
				((Uniform*)uniformList[nameHash])->m_type,
				((Uniform*)uniformList[nameHash])->m_name,
				new unsigned int(generatedTexture->m_openglTextureID) //Set new value
				);
	}
	else if (m_uniforms[nameHash] != nullptr)
	{
		*((unsigned int*)m_uniforms[nameHash]->m_data) = generatedTexture->m_openglTextureID;
	}
	else
	{
		//ASSERT_OR_DIE(false, "Uniform doesn't exist on Material");
	}
}


//-------------------------------------------------------------------------------------------------
void MeshRenderer::SetUniform(std::string const &uniformName, int *uniformValue)
{
	std::map<size_t, Uniform*>& uniformList = m_material->GetUniformList();

	//Doesn't exist yet, lets make space for it and assign it to the data
	size_t nameHash = std::hash<std::string>{}(uniformName);
	if (uniformList[nameHash] != nullptr && m_uniforms[nameHash] == nullptr)
	{
		int uniformSize = ((Uniform*)uniformList[nameHash])->m_size;
		size_t dataSize = sizeof(int) * uniformSize;
		int *data = (int*)malloc(dataSize);
		memcpy(data, uniformValue, dataSize);

		m_uniforms[nameHash] = new Uniform
			(
				((Uniform*)uniformList[nameHash])->m_bindPoint,
				((Uniform*)uniformList[nameHash])->m_length,
				uniformSize,
				((Uniform*)uniformList[nameHash])->m_type,
				((Uniform*)uniformList[nameHash])->m_name,
				data //Setting new value
				);
	}

	//update the data
	else if (m_uniforms[nameHash] != nullptr)
	{
		int uniformSize = ((Uniform*)uniformList[nameHash])->m_size;
		size_t dataSize = sizeof(int) * uniformSize;
		memcpy(m_uniforms[nameHash]->m_data, uniformValue, dataSize);
	}

	//That's not real
	else
	{
		//ASSERT_OR_DIE(false, "Uniform doesn't exist on Material");
	}
}


//-------------------------------------------------------------------------------------------------
void MeshRenderer::SetUniform(std::string const &uniformName, float *uniformValue)
{
	std::map<size_t, Uniform*>& uniformList = m_material->GetUniformList();

	//Doesn't exist yet, lets make space for it and assign it to the data
	size_t nameHash = std::hash<std::string>{}(uniformName);
	if (uniformList[nameHash] != nullptr && m_uniforms[nameHash] == nullptr)
	{
		int uniformSize = ((Uniform*)uniformList[nameHash])->m_size;
		size_t dataSize = sizeof(float) * uniformSize;
		float *data = (float*)malloc(dataSize);
		memcpy(data, uniformValue, dataSize);

		m_uniforms[nameHash] = new Uniform
			(
				((Uniform*)uniformList[nameHash])->m_bindPoint,
				((Uniform*)uniformList[nameHash])->m_length,
				uniformSize,
				((Uniform*)uniformList[nameHash])->m_type,
				((Uniform*)uniformList[nameHash])->m_name,
				data
				);
	}

	//update the data
	else if (m_uniforms[nameHash] != nullptr)
	{
		int uniformSize = ((Uniform*)uniformList[nameHash])->m_size;
		size_t dataSize = sizeof(float) * uniformSize;
		memcpy(m_uniforms[nameHash]->m_data, uniformValue, dataSize);
	}

	//That's not real
	else
	{
		//ASSERT_OR_DIE(false, "Uniform doesn't exist on Material");
	}
}


//-------------------------------------------------------------------------------------------------
void MeshRenderer::SetUniform(std::string const &uniformName, Vector2 *uniformValue)
{
	std::map<size_t, Uniform*>& uniformList = m_material->GetUniformList();

	//Doesn't exist yet, lets make space for it and assign it to the data
	size_t nameHash = std::hash<std::string>{}(uniformName);
	if (uniformList[nameHash] != nullptr && m_uniforms[nameHash] == nullptr)
	{
		int uniformSize = ((Uniform*)uniformList[nameHash])->m_size;
		size_t dataSize = sizeof(Vector2) * uniformSize;
		Vector2 *data = (Vector2*)malloc(dataSize);
		memcpy(data, uniformValue, dataSize);

		m_uniforms[nameHash] = new Uniform
			(
				((Uniform*)uniformList[nameHash])->m_bindPoint,
				((Uniform*)uniformList[nameHash])->m_length,
				uniformSize,
				((Uniform*)uniformList[nameHash])->m_type,
				((Uniform*)uniformList[nameHash])->m_name,
				data
				);
	}

	//update the data
	else if (m_uniforms[nameHash] != nullptr)
	{
		int uniformSize = ((Uniform*)uniformList[nameHash])->m_size;
		size_t dataSize = sizeof(Vector2) * uniformSize;
		memcpy(m_uniforms[nameHash]->m_data, uniformValue, dataSize);
	}

	//That's not real
	else
	{
		//ASSERT_OR_DIE(false, "Uniform doesn't exist on Material");
	}
}


//-------------------------------------------------------------------------------------------------
void MeshRenderer::SetUniform(std::string const &uniformName, Vector3 *uniformValue)
{
	std::map<size_t, Uniform*>& uniformList = m_material->GetUniformList();

	//Doesn't exist yet, lets make space for it and assign it to the data
	size_t nameHash = std::hash<std::string>{}(uniformName);
	if (uniformList[nameHash] != nullptr && m_uniforms[nameHash] == nullptr)
	{
		int uniformSize = ((Uniform*)uniformList[nameHash])->m_size;
		size_t dataSize = sizeof(Vector3) * uniformSize;
		Vector3 *data = (Vector3*)malloc(dataSize);
		memcpy(data, uniformValue, dataSize);

		m_uniforms[nameHash] = new Uniform
			(
				((Uniform*)uniformList[nameHash])->m_bindPoint,
				((Uniform*)uniformList[nameHash])->m_length,
				uniformSize,
				((Uniform*)uniformList[nameHash])->m_type,
				((Uniform*)uniformList[nameHash])->m_name,
				data
				);
	}

	//update the data
	else if (m_uniforms[nameHash] != nullptr)
	{
		int uniformSize = ((Uniform*)uniformList[nameHash])->m_size;
		size_t dataSize = sizeof(Vector3) * uniformSize;
		memcpy(m_uniforms[nameHash]->m_data, uniformValue, dataSize);
	}

	//That's not real
	else
	{
		//ASSERT_OR_DIE(false, "Uniform doesn't exist on Material");
	}
}


//-------------------------------------------------------------------------------------------------
void MeshRenderer::SetUniform(std::string const &uniformName, Vector4 *uniformValue)
{
	std::map<size_t, Uniform*>& uniformList = m_material->GetUniformList();

	//Doesn't exist yet, lets make space for it and assign it to the data
	size_t nameHash = std::hash<std::string>{}(uniformName);
	if (uniformList[nameHash] != nullptr && m_uniforms[nameHash] == nullptr)
	{
		int uniformSize = ((Uniform*)uniformList[nameHash])->m_size;
		size_t dataSize = sizeof(Vector4) * uniformSize;
		Vector4 *data = (Vector4*)malloc(dataSize);
		memcpy(data, uniformValue, dataSize);

		m_uniforms[nameHash] = new Uniform
			(
				((Uniform*)uniformList[nameHash])->m_bindPoint,
				((Uniform*)uniformList[nameHash])->m_length,
				uniformSize,
				((Uniform*)uniformList[nameHash])->m_type,
				((Uniform*)uniformList[nameHash])->m_name,
				data
				);
	}

	//update the data
	else if (m_uniforms[nameHash] != nullptr)
	{
		int uniformSize = ((Uniform*)uniformList[nameHash])->m_size;
		size_t dataSize = sizeof(Vector4) * uniformSize;
		memcpy(m_uniforms[nameHash]->m_data, uniformValue, dataSize);
	}

	//That's not real
	else
	{
		//ASSERT_OR_DIE(false, "Uniform doesn't exist on Material");
	}
}

//-------------------------------------------------------------------------------------------------
void MeshRenderer::SetUniform(std::string const &uniformName, const Matrix4 *uniformValue)
{
	std::map<size_t, Uniform*>& uniformList = m_material->GetUniformList();

	//Doesn't exist yet, lets make space for it and assign it to the data
	size_t nameHash = std::hash<std::string>{}(uniformName);
	if (uniformList[nameHash] != nullptr && m_uniforms[nameHash] == nullptr)
	{
		int uniformSize = ((Uniform*)uniformList[nameHash])->m_size;
		size_t dataSize = sizeof(Matrix4) * uniformSize;
		Matrix4 *data = (Matrix4*)malloc(dataSize);
		memcpy(data, uniformValue, dataSize);

		m_uniforms[nameHash] = new Uniform
			(
				((Uniform*)uniformList[nameHash])->m_bindPoint,
				((Uniform*)uniformList[nameHash])->m_length,
				uniformSize,
				((Uniform*)uniformList[nameHash])->m_type,
				((Uniform*)uniformList[nameHash])->m_name,
				data
				);
	}

	//update the data
	else if (m_uniforms[nameHash] != nullptr)
	{
		int uniformSize = ((Uniform*)uniformList[nameHash])->m_size;
		size_t dataSize = sizeof(Matrix4) * uniformSize;
		memcpy(m_uniforms[nameHash]->m_data, uniformValue, dataSize);
	}

	//That's not real
	else
	{
		//ASSERT_OR_DIE(false, "Uniform doesn't exist on Material");
	}
}


//-------------------------------------------------------------------------------------------------
void MeshRenderer::SetUniform(std::vector<Light> const &uniformLights, int lightCount)
{
	SetUniform("gLightCount", lightCount);
	Vector4 lightColor[16];
	Vector3 lightPosition[16];
	Vector3 lightDirection[16];
	float lightIsDirectional[16];
	float lightDistanceMin[16];
	float lightDistanceMax[16];
	float lightStrengthAtMin[16];
	float lightStrengthAtMax[16];
	float lightInnerAngle[16];
	float lightOuterAngle[16];
	float lightStrengthInside[16];
	float lightStrengthOutside[16];
	for (unsigned int lightIndex = 0; lightIndex < uniformLights.size(); ++lightIndex)
	{
		//lightColor[lightIndex] = uniformLights[lightIndex].m_lightColor.FloatRepresentation();
		lightColor[lightIndex] = uniformLights[lightIndex].m_color;
		lightPosition[lightIndex] = uniformLights[lightIndex].m_position;
		lightDirection[lightIndex] = uniformLights[lightIndex].m_direction;
		//lightIsDirectional[lightIndex] = uniformLights[lightIndex].m_isLightDirectional;
		lightIsDirectional[lightIndex] = uniformLights[lightIndex].m_directionalInterp;
		lightDistanceMin[lightIndex] = uniformLights[lightIndex].m_minLightDistance;
		lightDistanceMax[lightIndex] = uniformLights[lightIndex].m_maxLightDistance;
		lightStrengthAtMin[lightIndex] = uniformLights[lightIndex].m_powerAtMin;
		lightStrengthAtMax[lightIndex] = uniformLights[lightIndex].m_powerAtMax;
		lightInnerAngle[lightIndex] = uniformLights[lightIndex].m_thetaInner;
		lightOuterAngle[lightIndex] = uniformLights[lightIndex].m_thetaOuter;
		lightStrengthInside[lightIndex] = uniformLights[lightIndex].m_thetaInnerPower;
		lightStrengthOutside[lightIndex] = uniformLights[lightIndex].m_thetaOuterPower;
	}

	SetUniform("gLightPosition", lightPosition);
	SetUniform("gLightDirection", lightDirection);
	SetUniform("gLightColor", lightColor);
	SetUniform("gLightDirectionFactor", lightIsDirectional);

	SetUniform("gNearDistance", lightDistanceMin);
	SetUniform("gFarDistance", lightDistanceMax);
	SetUniform("gNearFactor", lightStrengthAtMin);
	SetUniform("gFarFactor", lightStrengthAtMax);

	SetUniform("gInnerAngle", lightInnerAngle);
	SetUniform("gOuterAngle", lightOuterAngle);
	SetUniform("gInnerFactor", lightStrengthInside);
	SetUniform("gOuterFactor", lightStrengthOutside);
}

void MeshRenderer::SetUniform(std::string const &uniformName, std::vector< Matrix4 > const &arrayOfMatrices, int numOfMatrices)
{
	numOfMatrices;
	SetUniform(uniformName, &arrayOfMatrices[0]);
}

