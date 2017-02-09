#pragma once
#include <vector>
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/OpenGLExtensions.hpp"

class Framebuffer
{
public:
	GLuint m_id;

	std::vector<Texture*> m_color_targets;
	Texture* m_depth_stencil_target;

	uint32_t m_pixel_width;
	uint32_t m_pixel_height;

	Framebuffer(unsigned int width, unsigned int height, std::vector<eTextureFormat> const &colorFormats, eTextureFormat const &depthStencilFormat);
	~Framebuffer();
	unsigned int GetColorTexture(int colorIndex);
	unsigned int GetDepthTexture();


};



void FramebufferDelete(Framebuffer *fbo);

//Framebuffer* FramebufferCreate(uint32_t width, uint32_t height, std::vector<eTextureFormat> color_formats, eTextureFormat depth_stencil_format);
Framebuffer* FramebufferCreate(size_t color_count, Texture **color_targets, Texture* depth_stencil_target);




