#include "Engine/Renderer/SpriteRenderer/SpriteGameRenderer.hpp"



#include "Engine/Renderer/Shaders/GPUProgram.hpp"
#include "Engine/Renderer/SpriteRenderer/Sprite.hpp"
#include "Engine/Renderer/SpriteRenderer/SpriteResource.hpp"
#include "Engine/Renderer/SpriteRenderer/SpriteLayer.hpp"
#include "Engine/Math/HashUtils.hpp"
#include "Engine/Math/Rgba.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Tools/Parsers/XMLUtilities.hpp"
#include "Engine/Renderer/Shaders/Framebuffer.hpp"
#include "Engine/Renderer/SpriteRenderer/AnimatedSprite.hpp"
#include "Engine/Renderer/Shaders/MeshBuilder.hpp"
#include "Engine/Renderer/ParticleRenderer/ParticleSystem.hpp"
#include "Engine/Renderer/ParticleRenderer/ParticleEmitter.hpp"
#include "Engine/Renderer/ParticleRenderer/ParticleEmitterDefinition.hpp"
#include "Engine/Renderer/ParticleRenderer/ParticleSystemDefinition.hpp"

const std::string DEFAULT_SPRITE_VERT_SHADER = "Data/Shaders/sprite.vert";
const std::string DEFAULT_SPRITE_FRAG_SHADER = "Data/Shaders/sprite.frag";

SpriteGameRenderer * g_spriteRenderSystem = nullptr;



SpriteGameRenderer::SpriteGameRenderer()
    : m_clearColor(Rgba::BLACK)
    ,m_virtualSize(10.f)
    ,m_importSize(240.f)
    ,m_aspectRatio(16.f/9.f)
{
    m_spriteMesh = nullptr;
	m_meshBuilder = new MeshBuilder(true);
    m_spriteMeshRenderer = new MeshRenderer();
    m_screenEffect = new MeshRenderer();
    
    m_screenEffect->SetMesh(Mesh::GetMeshShape(MeshShape_QUAD_FULLSCREEN));
    m_defaultScreenMaterial = new Material("Data/Shaders/post.vert", "Data/Shaders/post_passthrough.frag");
    m_screenEffect->SetRenderState(RenderState::BASIC_2D);
    m_screenEffect->SetMaterial(m_defaultScreenMaterial);
    m_screenEffect->BindMesh();


    std::vector<eTextureFormat> textureFormats; //#TODO: Why exactly is this an array again?
    textureFormats.push_back(eTextureFormat_RGBA8);


    //Create Nothing Shader PostProcessFX
    m_fboCurrent = new Framebuffer(1600, 900, textureFormats, eTextureFormat_D24S8);
    m_fboEffect = new Framebuffer(1600, 900, textureFormats, eTextureFormat_D24S8);
    //Matrix4 view = Matrix4::IDENTITY;


    ResetVirtualScreen();
}

SpriteGameRenderer::~SpriteGameRenderer()
{

	//delete m_testSprite;
	//delete m_testSprite2;
	//delete m_testSprite3;

	delete m_screenEffect;
	//delete m_screenMesh;
	//delete m_screenMaterial;

	//Material * m_defaultScreenMaterial;

	delete m_defaultScreenMaterial;
	delete m_spriteMesh;
	delete m_meshBuilder;
	delete m_spriteMeshRenderer;
	delete m_fboEffect;
	delete m_fboCurrent;


	
	
	

	std::map< std::string, AnimationSequence* >::iterator itAnim = m_animationSequenceDatabase.begin();
	while (itAnim != m_animationSequenceDatabase.end()) {
		delete(itAnim->second);
		itAnim = m_animationSequenceDatabase.erase(itAnim);
	}

	std::map< std::string, SpriteResource* >::iterator itResource = m_spriteResourceDatabase.begin();
	while (itResource != m_spriteResourceDatabase.end()) {
		delete(itResource->second);
		itResource = m_spriteResourceDatabase.erase(itResource);
	}

	
	
	std::map< int, SpriteLayer* >::iterator itLayer = m_spriteLayers.begin();
	while (itLayer != m_spriteLayers.end()) {
		delete(itLayer->second);
		itLayer = m_spriteLayers.erase(itLayer);
	}
}

void SpriteGameRenderer::Render()
{

    //Framebuffer * startingFBO = g_theRenderer->m_active_fbo;
    
    //Setup current FBO
/*    g_theRenderer->FramebufferBind(m_fboCurrent);*/
    g_theRenderer->ClearScreen(m_clearColor);
// 	delete m_spriteMesh;
// 	m_spriteMesh = nullptr;


    for (auto layerIter = m_spriteLayers.begin(); layerIter != m_spriteLayers.end(); ++layerIter)
    {
        RenderLayer(layerIter->second);

        ////FBO stuff here
// 		g_theRenderer->FramebufferBind(m_fboEffect);
//         m_screenEffect->SetMaterial(layerIter->second->m_screenEffect);
//         m_screenEffect->SetUniform("gDiffuseTex", m_fboCurrent->GetColorTexture(0));
//         g_theRenderer->MeshRenderPPFX(m_screenEffect);
//         		
//         Framebuffer* temp = m_fboEffect;
//         m_fboEffect = m_fboCurrent;
//         m_fboCurrent = temp;
    }
	

//     g_theRenderer->FramebufferBind(startingFBO);
//     m_screenEffect->SetUniform("gDiffuseTex", m_fboCurrent->GetColorTexture(0));
// 
//     g_theRenderer->MeshRenderPPFX(m_screenEffect);
 }

void SpriteGameRenderer::RenderLayer(SpriteLayer const * layer)
{
    if (!layer->m_isLayerEnabled)
        return;
    for each (Sprite* sprite in layer->m_spritesInLayer)
    {
        RenderSprite(sprite);
    }
}

void SpriteGameRenderer::RenderSprite(Sprite const * sprite)
{

	if (m_spriteMesh == nullptr)
	{
		m_spriteMesh = new Mesh();
		ConstructMesh(m_spriteMesh, sprite);

		m_spriteMeshRenderer->SetMesh(m_spriteMesh);
		m_spriteMeshRenderer->SetMaterial(sprite->GetMaterial());
		m_spriteMeshRenderer->BindMesh();

	}
	else
	{
		ConstructMesh(m_spriteMesh, sprite);


		m_spriteMeshRenderer->SetMesh(m_spriteMesh);
		m_spriteMeshRenderer->SetMaterial(sprite->GetMaterial());

		//m_spriteMeshRenderer->BindMesh();
	}
    



    m_spriteMeshRenderer->SetUniform("gView",Matrix4::IDENTITY);
    m_spriteMeshRenderer->SetUniform("gProj", m_projectionMatrix);


    g_theRenderer->MeshRender(m_spriteMeshRenderer);



}

void SpriteGameRenderer::LoadAllSpriteResources()
{

    std::vector< std::string > spriteFiles = EnumerateFilesInFolder("Data/Sprites",
        "*.Sprites.xml");

    for each (std::string str in spriteFiles)
    {
        //std::string npcFamilyName = ExtractToken(str, "Data/NPCs/", ".");
        XMLNode node = XMLNode::openFileHelper(str.c_str());
        XMLNode rootNode = node.getChildNode(0);//Sprites


        if (!rootNode.IsContentEmpty())
        {
            int spriteCount = rootNode.nChildNode(); //# of Sprite nodes

            for (int i = 0; i < spriteCount; i++)
            {
                XMLNode thisNode = rootNode.getChildNode(i);
                std::string name = thisNode.getName();
                if (name != "Sprite")
                {
                    //Not valid node, skip
                    continue;
                }
                SpriteResource* newResource = new SpriteResource(thisNode);
                if (newResource != nullptr)
                {
                    m_spriteResourceDatabase.insert(std::pair< std::string, SpriteResource* >(newResource->GetID(), newResource));
                }

            }
        }
    }
}

void SpriteGameRenderer::LoadAllAnimationSequences()
{
	std::vector< std::string > animFiles = EnumerateFilesInFolder("Data/Sprites",
		"*.Anims.xml");

	for each (std::string str in animFiles)
	{
		
		XMLNode node = XMLNode::openFileHelper(str.c_str());
		XMLNode rootNode = node.getChildNode(0);//Animation


		if (!rootNode.IsContentEmpty())
		{
			int animCount = rootNode.nChildNode(); //# of Sprite nodes

			for (int i = 0; i < animCount; i++)
			{
				XMLNode thisNode = rootNode.getChildNode(i);
				std::string name = thisNode.getName();
				if (name != "Animation")
				{
					//Not valid node, skip
					continue;
				}
				AnimationSequence* newAnim = new AnimationSequence(thisNode);
				if (newAnim != nullptr)
				{
					m_animationSequenceDatabase.insert(std::pair< std::string, AnimationSequence* >(newAnim->m_name, newAnim));
				}

			}
		}
	}
}

void SpriteGameRenderer::LoadAllEmitterDefinitions()
{
	std::vector< std::string > animFiles = EnumerateFilesInFolder("Data/Sprites",
		"*.ParticleEmitterDefs.xml");

	for each (std::string str in animFiles)
	{

		XMLNode node = XMLNode::openFileHelper(str.c_str());
		XMLNode rootNode = node.getChildNode(0);//Emitters


		if (!rootNode.IsContentEmpty())
		{
			int animCount = rootNode.nChildNode(); //# of Emitter nodes

			for (int i = 0; i < animCount; i++)
			{
				XMLNode thisNode = rootNode.getChildNode(i);
				std::string name = thisNode.getName();
				if (name != "Emitter")
				{
					//Not valid node, skip
					continue;
				}
				ParticleEmitterDefinition* newEmitter = new ParticleEmitterDefinition(thisNode);
				if (newEmitter != nullptr)
				{
					//m_particleEmitterDefinitionsDatabase.insert(std::pair< std::string, ParticleEmitterDefinition* >(newEmitter->m_emitterDefTag, newEmitter));
				}

			}
		}
	}
}

void SpriteGameRenderer::LoadAllParticleSystems()
{

}

void SpriteGameRenderer::AddSpriteResource(std::string const & id, std::string const & filename)
{
    SpriteResource* resource = new SpriteResource(id, filename);
    m_spriteResourceDatabase.insert(std::pair<std::string, SpriteResource*>(resource->GetID(), resource));
}

SpriteLayer * SpriteGameRenderer::CreateOrGetLayer(int layer)
{

    auto foundLayer = m_spriteLayers.find(layer);
    if (foundLayer != m_spriteLayers.end())
    {
        return foundLayer->second;
    }
    else
    {
        SpriteLayer* newLayer = new SpriteLayer(layer);
        m_spriteLayers.insert(std::pair<int, SpriteLayer*>(layer, newLayer));
        return newLayer;
    }
}

void SpriteGameRenderer::RemoveSpriteFromLayer(Sprite* sprite)
{
    auto foundLayer = m_spriteLayers.find(sprite->GetLayerID());
    if (foundLayer != m_spriteLayers.end())
    {
        SpriteLayer* layer = foundLayer->second;

        layer->RemoveSprite(sprite);
    }
}

void SpriteGameRenderer::AddEffectToLayer(Material* effect, int layer)
{
    auto foundLayer = m_spriteLayers.find(layer);
    if (foundLayer != m_spriteLayers.end())
    {
        SpriteLayer* layer = foundLayer->second;

        layer->SetScreenEffect(effect);
    }
}

void SpriteGameRenderer::RemoveEffectFromLayer(int layer)
{
    auto foundLayer = m_spriteLayers.find(layer);
    if (foundLayer != m_spriteLayers.end())
    {
        SpriteLayer* layer = foundLayer->second;

        layer->RemoveScreenEffect();
    }
}

void SpriteGameRenderer::ConstructMesh(Mesh * out_updateMesh, Sprite const * sprite)
{
	g_theProfiler->StartProfilerSample("ConstructMesh");
    if(!sprite->IsEnabled()) return;

	g_theProfiler->StartProfilerSample("Mesh CTOR");
	MeshBuilder spriteBuilder = MeshBuilder(true);
	spriteBuilder.Begin();

	
    out_updateMesh->SetVertexLayout(VertexType_SPRITE);
	g_theProfiler->EndProfilerSample();
	g_theProfiler->StartProfilerSample("Mesh Math");
    Vertex_SPRITE vertexes [4];
	

    Rgba spriteColor = sprite->m_color;

    /*
    v0 = -pivot (-.5, -.5)
    v1 = (w - pivot.x, -pivot.y)
    v2 = (w - pivot.x, h - pivot.y);
    v3 = (-pivot.x, h - pivot.y);

    */
    float w = sprite->GetWidth();
    float h = sprite->GetHeight();

    //Position
    Vector2 pivot = Vector2(sprite->m_pivot.x * w, sprite->m_pivot.y * h);
    Vector2 v0 = pivot * -1.f;
    Vector2 v1 = Vector2(w - pivot.x, -pivot.y);
    Vector2 v2 = Vector2(w - pivot.x, h - pivot.y);
    Vector2 v3 = Vector2(-pivot.x, h - pivot.y);


    

    //Rotation
    /*
    [Cos(theta), Sin(theta)
    -sin(theta), cos(theta)]
    */
    float sinRotation = SinDegrees(sprite->m_rotationDegrees);
    float cosRotation = CosDegrees(sprite->m_rotationDegrees);

    Vector2 V0Rot = v0;
    V0Rot.x = v0.x * cosRotation + v0.y * -sinRotation;
    V0Rot.y = v0.x * sinRotation + v0.y * cosRotation;



    Vector2 V1Rot = v1;
    V1Rot.x = v1.x * cosRotation + v1.y * -sinRotation;
    V1Rot.y = v1.x * sinRotation + v1.y * cosRotation;

    Vector2 V2Rot = v2;
    V2Rot.x = v2.x * cosRotation + v2.y * -sinRotation;
    V2Rot.y = v2.x * sinRotation + v2.y * cosRotation;

    Vector2 V3Rot = v3;
    V3Rot.x = v3.x * cosRotation + v3.y * -sinRotation;
    V3Rot.y = v3.x * sinRotation + v3.y * cosRotation;


    //Scale
    Vector2 scale = sprite->m_scale;
    V0Rot = V0Rot * scale;
    V1Rot = V1Rot * scale;
    V2Rot = V2Rot * scale;
    V3Rot = V3Rot * scale;
	g_theProfiler->EndProfilerSample();

	g_theProfiler->StartProfilerSample("Insert Vert");
	g_theProfiler->StartProfilerSample("PushBack");
	if (sprite->IsUsingCamera())
	{
		Vector2 actualPosition = sprite->m_position - m_camera;
		vertexes[0] = (Vertex_SPRITE(actualPosition + V0Rot, spriteColor, Vector2(0.f, 1.f)));  //bottomLeft
		vertexes[1] = (Vertex_SPRITE(actualPosition + V1Rot, spriteColor, Vector2(1.f, 1.f)));  //bottomRight
		vertexes[2] = (Vertex_SPRITE(actualPosition + V2Rot, spriteColor, Vector2(1.f, 0.f)));  //topRight
		vertexes[3] = (Vertex_SPRITE(actualPosition + V3Rot, spriteColor, Vector2(0.f, 0.f)));  //topLeft
	}
	else
	{
		vertexes[0] = (Vertex_SPRITE(sprite->m_position + V0Rot, spriteColor, Vector2(0.f, 1.f)));  //bottomLeft
		vertexes[1] = (Vertex_SPRITE(sprite->m_position + V1Rot, spriteColor, Vector2(1.f, 1.f)));  //bottomRight
		vertexes[2] = (Vertex_SPRITE(sprite->m_position + V2Rot, spriteColor, Vector2(1.f, 0.f)));  //topRight
		vertexes[3] = (Vertex_SPRITE(sprite->m_position + V3Rot, spriteColor, Vector2(0.f, 0.f)));  //topLeft
	}
   
	g_theProfiler->EndProfilerSample();

	g_theProfiler->StartProfilerSample("ForLoop");
	for each (Vertex_SPRITE& vert in vertexes)
	{
		spriteBuilder.SetUV0(vert.m_texCoord);
		spriteBuilder.SetColor(vert.m_color);
		spriteBuilder.AddVertex2D(vert.m_position2D);
	}
	g_theProfiler->EndProfilerSample();
    /*
    vertexes.push_back(Vertex_SPRITE(Vector2(-0.5f, -0.5f), spriteColor, Vector2(0.f, 1.f)));  //bottomLeft
    vertexes.push_back(Vertex_SPRITE(Vector2(+0.5f, -0.5f), spriteColor, Vector2(1.f, 1.f)));  //bottomRight
    vertexes.push_back(Vertex_SPRITE(Vector2(+0.5f, +0.5f), spriteColor, Vector2(1.f, 0.f)));  //topRight
    vertexes.push_back(Vertex_SPRITE(Vector2(-0.5f, +0.5f), spriteColor, Vector2(0.f, 0.f)));  //topLeft
    */
    //Create VBO
    
	g_theProfiler->StartProfilerSample("AddEnd");
	spriteBuilder.AddQuadIndicies(3, 2, 0, 1);
	spriteBuilder.End();

	g_theProfiler->EndProfilerSample();
	g_theProfiler->EndProfilerSample();//Insert
	
	g_theProfiler->StartProfilerSample("Mesh Set VBO");
    //Set up vbo and IBO
	if (out_updateMesh->m_vboID == NULL)
	{
		g_theProfiler->StartProfilerSample("VBO Create");
		out_updateMesh->m_vboID = g_theRenderer->CreateRenderBuffer(&vertexes[0], 4, sizeof(Vertex_SPRITE));

		//Create IBO
		uint16_t indicies[] =
		{
			0, 1, 2,
			0, 2, 3
		};
		out_updateMesh->m_indexBufferVertCount = 2 * 3;
		out_updateMesh->m_iboID = g_theRenderer->CreateRenderBuffer(&indicies, out_updateMesh->m_indexBufferVertCount, sizeof(indicies[0]));
		g_theProfiler->EndProfilerSample();
	}
	else
	{
		g_theProfiler->StartProfilerSample("VBO Update");
		out_updateMesh->Update(&spriteBuilder, VertexType_SPRITE);
		g_theProfiler->EndProfilerSample();
	}
	g_theProfiler->EndProfilerSample();
	

	g_theProfiler->EndProfilerSample();//ConstructMesh
}

void SpriteGameRenderer::EnableLayer(int layer)
{
    auto foundLayer = m_spriteLayers.find(layer);
    if (foundLayer != m_spriteLayers.end())
    {
        foundLayer->second->m_isLayerEnabled = true;
    }
}

void SpriteGameRenderer::DisableLayer(int layer)
{
    auto foundLayer = m_spriteLayers.find(layer);
    if (foundLayer != m_spriteLayers.end())
    {
        foundLayer->second->m_isLayerEnabled = false;
    }
}

void SpriteGameRenderer::DisableAllButThisLayer(int layer)
{
	for (auto layerIter = m_spriteLayers.begin(); layerIter != m_spriteLayers.end(); ++layerIter)
	{
		int otherLayer = layerIter->second->m_layerID;
		if (layer != otherLayer)
			layerIter->second->m_isLayerEnabled = false;
		else
		{
			layerIter->second->m_isLayerEnabled = true;
		}
	}
}

void SpriteGameRenderer::EnableAllButThisLayer(int layer)
{
	for (auto layerIter = m_spriteLayers.begin(); layerIter != m_spriteLayers.end(); ++layerIter)
	{
		int otherLayer = layerIter->second->m_layerID;
		if (layer != otherLayer)
			layerIter->second->m_isLayerEnabled = true;
		else
		{
			layerIter->second->m_isLayerEnabled = false;
		}
	}
}

void SpriteGameRenderer::EnableAllLayers()
{
	for (auto layerIter = m_spriteLayers.begin(); layerIter != m_spriteLayers.end(); ++layerIter)
	{
		layerIter->second->m_isLayerEnabled = true;
	}
}

void SpriteGameRenderer::DisableAllLayers()
{
	for (auto layerIter = m_spriteLayers.begin(); layerIter != m_spriteLayers.end(); ++layerIter)
	{
		layerIter->second->m_isLayerEnabled = false;
	}
}

SpriteResource const * SpriteGameRenderer::GetSpriteResource(std::string const & spriteID) const
{
//    size_t hash = HashString(spriteID);
    auto foundRes = m_spriteResourceDatabase.find(spriteID);
    if (foundRes != m_spriteResourceDatabase.end())
    {
        return foundRes->second;
    }
    else
    {
        return nullptr;
    }
}


AnimationSequence* SpriteGameRenderer::GetAnimationSequence(std::string const & animID) const
{
	auto foundRes = m_animationSequenceDatabase.find(animID);
	if (foundRes != m_animationSequenceDatabase.end())
	{
		return foundRes->second;
	}
	else
	{
		return nullptr;
	}
}

float SpriteGameRenderer::GetVirtualSize() const
{
    return m_virtualSize;
}

float SpriteGameRenderer::GetImportSize() const
{
    return m_importSize;
}

float SpriteGameRenderer::GetAspectRatio() const
{
    return m_aspectRatio;
}

void SpriteGameRenderer::ResetVirtualScreen()
{
    if (m_aspectRatio < 1.f)
    {
        m_virtualScreen.y = m_virtualSize / m_aspectRatio;
        m_virtualScreen.x = m_virtualSize;
    }
    else
    {
        m_virtualScreen.x = m_virtualSize * m_aspectRatio;
        m_virtualScreen.y = m_virtualSize;
    }

    m_projectionMatrix.ChangeToProjectionMatrix(m_virtualScreen.x, m_virtualScreen.y, -1.f, 1.f);


	float minX = -m_virtualScreen.x / 2.f;
	float maxX = m_virtualScreen.x / 2.f;

	float minY = -m_virtualScreen.y / 2.f;
	float maxY = m_virtualScreen.y / 2.f;

	m_virtualBounds.m_mins = Vector2(minX, minY);
	m_virtualBounds.m_maxs = Vector2(maxX, maxY);

}

void SpriteGameRenderer::SetVirtualSize(float size)
{
    m_virtualSize = size;
    ResetVirtualScreen();
}

void SpriteGameRenderer::SetImportSize(float size)
{
    m_importSize = size;
    ResetVirtualScreen();
}

void SpriteGameRenderer::SetAspectRatio(float width, float height)
{
    m_aspectRatio = (width / height);
    ResetVirtualScreen();
}

AABB2 SpriteGameRenderer::GetVirtualBounds()
{
	return m_virtualBounds;
}

Vector2 SpriteGameRenderer::GetActualCoordsFromVirtual(const Vector2 inVec)
{
	Vector2 result;
	
	float minX = -m_virtualScreen.x / 2.f;
	float maxX = m_virtualScreen.x / 2.f;

	float minY = -m_virtualScreen.y / 2.f;
	float maxY = m_virtualScreen.y / 2.f;

	result.x = RangeMap(inVec.x, minX, maxX, 0.f, 900.f );
	result.y = RangeMap(inVec.y,  minY, maxY, 0.f, 1600.f);

	/*
	Vector2 result;
	Vector4 matrixVec = Vector4(inVec.x, inVec.y, 0.f, 0.f);
	
	matrixVec =  m_projectionMatrix.MultiplyRight(matrixVec);
	result.x = matrixVec.x;
	result.y = matrixVec.y;
	*/
	return result;
}

void SpriteGameRenderer::SetCameraPosition(const Vector2& newCameraPos)
{
	m_camera = newCameraPos;
}
