#pragma once

#include <map>
#include "Engine/Math/Transform.hpp"
#include "Engine/Renderer/Shaders/Mesh.hpp" //Need MeshShape enum
#include "Engine/Renderer/RenderState.hpp"


//-------------------------------------------------------------------------------------------------
class Material;
class Rgba;
class Matrix4;
struct Uniform;
struct Attribute;
class Light;


//-------------------------------------------------------------------------------------------------
class MeshRenderer
{
private:
	Transform m_transform;
	RenderState m_renderState;
	unsigned int m_vaoID;
	Mesh *m_mesh;
	Material *m_material;
	std::map<size_t, Uniform*> m_uniforms; //Uniforms individual to this MeshRenderer. Overwrites Material
	Matrix4 m_viewMatrix;
	Matrix4 m_projMatrix;

public:
	MeshRenderer();
	MeshRenderer(MeshShape const &meshShape, Transform const &transform = Transform(), RenderState const &renderState = RenderState::BASIC_3D);
	MeshRenderer(Mesh *mesh, Transform const &transform = Transform(), RenderState const &renderState = RenderState::BASIC_3D);
	MeshRenderer(Mesh *mesh, Material *material, Transform const &transform = Transform(), RenderState const &renderState = RenderState::BASIC_3D);
	~MeshRenderer();

	void Update();
	void BindMesh();
	void BindMeshNoVAOGen();
	void BindMatrixes();
	RenderState GetRenderState() const;
	void SetRenderState(RenderState state);
	void SetLineWidth(float lineWidth);
	Matrix4 GetModelMatrix() const;
	Matrix4 GetViewMatrix() const;
	Matrix4 GetProjMatrix() const;
	void SetViewMatrix(Matrix4 mat);
	void SetProjMatrix(Matrix4 mat);
	unsigned int GetGPUProgramID() const;
	unsigned int GetSamplerID() const;
	unsigned int GetVAOID() const;
	unsigned int GetIBOID() const;
	unsigned int GetDrawMode() const;
	int GetVertCount() const;
	int GetIndexBufferCount() const;
	std::vector<DrawInstruction> const & GetDrawInstructions() const;
	void SetPosition(Vector3 const &pos);
	void SetPosition(float xPos, float yPos, float zPos);
	void SetTransform(Transform const &transform);
	void SetMaterial(Material* material);
	void SetMesh(Mesh* mesh);
	
	std::map<size_t, Uniform*>& GetUniformList();
	const std::map<size_t, Uniform*>& GetUniformList() const;

	std::map<size_t, Uniform*>& GetMaterialUniformList();
	const std::map<size_t, Uniform*>& GetMaterialUniformList() const;

	std::map<size_t, Attribute*>& GetMaterialAttributeList();
	const std::map<size_t, Attribute*>& GetMaterialAttributeList() const;

	void SetUniform(std::string const &uniformName, unsigned int uniformValue);
	void SetUniform(std::string const &uniformName, int uniformValue);
	void SetUniform(std::string const &uniformName, float uniformValue);
	void SetUniform(std::string const &uniformName, Vector2 const &uniformValue);
	void SetUniform(std::string const &uniformName, Vector3 const &uniformValue);
	void SetUniform(std::string const &uniformName, Vector4 const &uniformValue);
	void SetUniform(std::string const &uniformName, Matrix4 const &uniformValue);
	void SetUniform(std::string const &uniformName, Rgba const &uniformValue);
	void SetUniform(std::string const &uniformName, std::string const &uniformValue);

	void SetUniform(std::string const &uniformName, int *uniformValue);
	void SetUniform(std::string const &uniformName, float *uniformValue);
	void SetUniform(std::string const &uniformName, Vector2 *uniformValue);
	void SetUniform(std::string const &uniformName, Vector3 *uniformValue);
	void SetUniform(std::string const &uniformName, Vector4 *uniformValue);

	void SetUniform(std::vector<Light> const &uniformLights, int lightCount);
	void SetUniform(std::string const &uniformName, std::vector< Matrix4 > const &arrayOfMatrices, int numOfMatrices);
	
	void SetUniform(std::string const &uniformName, const Matrix4 *uniformValue);
};