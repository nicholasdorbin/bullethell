#include "Engine/Renderer/Shaders/Framebuffer.hpp"

void FramebufferDelete(Framebuffer *fbo)
{
	
		glDeleteFramebuffers(1, &(fbo->m_id));
		delete fbo;
	
}
/*
Framebuffer* FramebufferCreate(uint32_t width, uint32_t height,
	std::vector<eTextureFormat> color_formats,
	eTextureFormat depth_stencil_format)
{
	//1. Create the textures,
	//2. Do steps below
	//3. Have FramebufferDelete also destroy the textures created
	return nullptr;
}
*/
/*
Framebuffer* FramebufferCreate(size_t color_count, Texture **color_targets, Texture* depth_stencil_target)
{
	//ASSERT (Color_count > 0);
	Texture* color0 = color_targets[0];
	uint32_t width = color0->m_texelSize.x;
	uint32_t height = color0->m_texelSize.y;

	for (uint32_t i = 0; i < color_count; i++)
	{
		//Texture* color = color_targets[i];
		//ASSERT width == width && height == height
	}

	if (depth_stencil_target != nullptr)
	{
		//ASSERT(depth_stencil_target.width = width && depth_stencil_target.height = height);
	}

	GLuint fbo_handle;
	glGenFramebuffers(1, &fbo_handle);
	//ASSERT_RETURN_VALUE(fbo_handle != NULL, nullptr);
	Framebuffer *fbo = new Framebuffer();
	fbo->id = fbo_handle;
	fbo->pixel_width = width;
	fbo->pixel_height = height;

	//Set Textures
	for (uint32_t i = 0; i < color_count; ++i)
	{
		fbo->m_color_targets.push_back((color_targets[i]));
	}
	fbo->m_depth_stencil_target = depth_stencil_target;

	//OpenGL Init stuff
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_handle);

	for (uint32_t i = 0; i < color_count; ++i)
	{
		Texture* tex = color_targets[i];
		glFramebufferTexture(GL_FRAMEBUFFER, //What we're attaching
			GL_COLOR_ATTACHMENT0 + i, //Where we're attaching
			tex->m_openglTextureID, //OGL ID
			0);//Level
	}

	if (depth_stencil_target != nullptr)
	{
		glFramebufferTexture(GL_FRAMEBUFFER,
			GL_DEPTH_STENCIL_ATTACHMENT,
			depth_stencil_target->m_openglTextureID,
			0);
	}

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		FramebufferDelete(fbo);
		//ErrorMSG;
		return nullptr;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, NULL);

	return fbo;
}
*/
Framebuffer::Framebuffer(unsigned int width, unsigned int height, std::vector<eTextureFormat> const &colorFormats, eTextureFormat const &depthStencilFormat)
{
	std::vector<Texture*> colorTargets;
	for (unsigned int colorIndex = 0; colorIndex < colorFormats.size(); ++colorIndex)
	{
		eTextureFormat format = colorFormats[colorIndex];
		colorTargets.push_back(new Texture(width, height, format));
	}

	Texture *depthStencilTarget = new Texture(width, height, depthStencilFormat);

	//Create the Framebuffer Object
	unsigned int fboHandle;
	glGenFramebuffers(1, &fboHandle);

	ASSERT_OR_DIE(fboHandle != NULL, "No handle created.")


	// Set known values
	
	m_id = fboHandle;
	m_pixel_width = width;
	m_pixel_height = height;
	unsigned int colorCount = colorFormats.size();

	//Set textures
	for (unsigned int currentColor = 0; currentColor < colorCount; ++currentColor)
	{
		m_color_targets.push_back(colorTargets[currentColor]);
	}
	m_depth_stencil_target = depthStencilTarget;

	//OpenGL Initialization
	//If you bound a framebuffer to your Renderer - 
	//be careful you didn't unbind just now...
	glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);

	// Bind our color targets to our FBO
	for (unsigned int i = 0; i < colorCount; ++i)
	{
		Texture *tex = colorTargets[i];
		glFramebufferTexture
			(
				GL_FRAMEBUFFER,
				GL_COLOR_ATTACHMENT0 + i,
				tex->m_openglTextureID, //texture ID
				0
				);
	}

	// Bind depth stencil if it exists
	if (nullptr != depthStencilTarget)
	{
		glFramebufferTexture
			(
				GL_FRAMEBUFFER,
				GL_DEPTH_STENCIL_ATTACHMENT,
				depthStencilTarget->m_openglTextureID,
				0
				);
	}

	//Make sure everything was bound correctly
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	ASSERT_OR_DIE(status == GL_FRAMEBUFFER_COMPLETE, "Buffer creation failed.");
	

	//Revert to old state
	glBindFramebuffer(GL_FRAMEBUFFER, NULL);

	//1. Create the textures
	//2. Then do steps below (FramebufferCreate)
	//3. Have FramebufferDelete also destroy the textures created
}

//-------------------------------------------------------------------------------------------------
Framebuffer::~Framebuffer()
{
	for (Texture *currentTex : m_color_targets)
	{
		delete currentTex;
		currentTex = nullptr;
	}

	delete m_depth_stencil_target;
	m_depth_stencil_target = nullptr;
}


//-------------------------------------------------------------------------------------------------
unsigned int Framebuffer::GetColorTexture(int colorIndex)
{
	return m_color_targets[colorIndex]->m_openglTextureID;
}


//-------------------------------------------------------------------------------------------------
unsigned int Framebuffer::GetDepthTexture()
{
	return m_depth_stencil_target->m_openglTextureID;
}
