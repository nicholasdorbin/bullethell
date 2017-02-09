
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <gl/gl.h>
#include <gl/GLU.h>
#include <direct.h>
#include <sstream>
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Camera3D.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Renderer/Shaders/Attribute.hpp"
#include "Engine/Renderer/Shaders/Light.hpp"
#include "Engine/Renderer/Shaders/Uniform.hpp"
#include "Engine/Math/Quaternion.hpp"
#include "Engine/Renderer/Shaders/GPUProgram.hpp"


#pragma warning( disable : 4201 )  // nonstandard extension used: nameless struct/union
#pragma warning( disable : 4127 )  // conditional expression is constant

#pragma comment( lib, "opengl32" ) // Link in the OpenGL32.lib static library
#pragma comment( lib, "glu32" ) // Link in the OpenGL32.lib static library

Renderer* g_theRenderer = nullptr;
std::vector<RenderCommand*> g_Commands;
const std::string DEFAULT_IMAGE_FILE = "Data/Images/TestImageWhite.png";

const int OFFSET_FROM_WINDOWS_DESKTOP = 50;
const int WINDOW_PHYSICAL_WIDTH = 1600;
const int WINDOW_PHYSICAL_HEIGHT = 900;
const double VIEW_LEFT = 0.0;
const double VIEW_RIGHT = 1600.0;
const double VIEW_BOTTOM = 0.0;
const double VIEW_TOP = VIEW_RIGHT * static_cast<double>(WINDOW_PHYSICAL_HEIGHT) / static_cast<double>(WINDOW_PHYSICAL_WIDTH);

// static GLuint gShaderProgram;
// static GLuint gVAO = NULL;
// static GLuint gVBO = NULL;
// static GLuint gvIBO = NULL;
//static GLuint gShaderTimedProgram;

static GLuint gSamplerId = NULL;
static GLuint gDiffuseTex = NULL;
static GLuint gNormalTex = NULL;
static GLuint gSpecularTex = NULL;
static GLuint gEmissiveTex = NULL;
static GLuint gDissolveTex = NULL;
static GLuint gMaskTex = NULL;
static GLuint gNoiseTex = NULL;


Renderer::Renderer() : g_DefaultImage(DEFAULT_IMAGE_FILE, 1, 1), m_active_fbo(NULL), m_currentRenderState(RenderState::BASIC_3D)
,m_currentLineWidth(1.f)
, m_currentPointSize(1.f)
{
	InitializeEngineCommon();
	

	int a = 5;
	m_versionString = (const char*) glGetString(GL_VERSION);

	a = a * a;
	DebuggerPrintf("OpenGL version: %s\n", m_versionString);
	m_glslString = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
	DebuggerPrintf("GLSL version: %s\n", m_glslString);

	glGenBuffers				= (PFNGLGENBUFFERSPROC) wglGetProcAddress("glGenBuffers");
	glBindBuffer				= (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
	glBufferData				= (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
	glDeleteBuffers				= (PFNGLDELETEBUFFERSPROC)wglGetProcAddress("glDeleteBuffers");

	glCreateShader				= (PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader");
	glShaderSource				= (PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource");
	glCompileShader				= (PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader");
	glGetShaderiv				= (PFNGLGETSHADERIVPROC)wglGetProcAddress("glGetShaderiv");
	glDeleteShader				= (PFNGLDELETESHADERPROC)wglGetProcAddress("glDeleteShader");
	glGetShaderInfoLog			= (PFNGLGETSHADERINFOLOGPROC)wglGetProcAddress("glGetShaderInfoLog");

	glCreateProgram				= (PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram");
	glAttachShader				= (PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader");
	glLinkProgram				= (PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram");
	glGetProgramiv				= (PFNGLGETPROGRAMIVPROC)wglGetProcAddress("glGetProgramiv");
	glDetachShader				= (PFNGLDETACHSHADERPROC)wglGetProcAddress("glDetachShader");
	glDeleteProgram				= (PFNGLDELETEPROGRAMPROC)wglGetProcAddress("glDeleteProgram");
	glGetProgramInfoLog			= (PFNGLGETPROGRAMINFOLOGPROC)wglGetProcAddress("wglGetProcAddress");

	glGenVertexArrays			= (PFNGLGENVERTEXARRAYSPROC)wglGetProcAddress("glGenVertexArrays");
	glDeleteVertexArrays		= (PFNGLDELETEVERTEXARRAYSPROC)wglGetProcAddress("glDeleteVertexArrays");
	glBindVertexArray			= (PFNGLBINDVERTEXARRAYPROC)wglGetProcAddress("glBindVertexArray");

	glGetActiveAttrib			= (PFNGLGETACTIVEATTRIBPROC)wglGetProcAddress("glGetActiveAttrib");
	glGetAttribLocation			= (PFNGLGETATTRIBLOCATIONPROC)wglGetProcAddress("glGetAttribLocation");
	glEnableVertexAttribArray	= (PFNGLENABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glEnableVertexAttribArray");
	glVertexAttribPointer		= (PFNGLVERTEXATTRIBPOINTERPROC)wglGetProcAddress("glVertexAttribPointer");
	glVertexAttribIPointer		= (PFNGLVERTEXATTRIBIPOINTERPROC)wglGetProcAddress("glVertexAttribIPointer");
	glUseProgram				= (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");


	glGetActiveUniform			= (PFNGLGETACTIVEUNIFORMPROC)wglGetProcAddress("glGetActiveUniform");
	glGetUniformLocation		= (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation");
	glUniform1fv				= (PFNGLUNIFORM1FVPROC)wglGetProcAddress("glUniform1fv");
	glUniform2fv				= (PFNGLUNIFORM1FVPROC)wglGetProcAddress("glUniform2fv");
	glUniform3fv				= (PFNGLUNIFORM3FVPROC)wglGetProcAddress("glUniform3fv");
	glUniform4fv				= (PFNGLUNIFORM4FVPROC)wglGetProcAddress("glUniform4fv");
	glUniform1iv				= (PFNGLUNIFORM1IVPROC)wglGetProcAddress("glUniform1iv");
	glUniformMatrix4fv			= (PFNGLUNIFORMMATRIX4FVPROC)wglGetProcAddress("glUniformMatrix4fv");

	glActiveTexture				= (PFNGLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture");
	glBindSampler				= (PFNGLBINDSAMPLERPROC)wglGetProcAddress("glBindSampler");
	glGenSamplers				= (PFNGLGENSAMPLERSPROC)wglGetProcAddress("glGenSamplers");
	glSamplerParameteri			= (PFNGLSAMPLERPARAMETERIPROC)wglGetProcAddress("glSamplerParameteri");

	glGenFramebuffers			= (PFNGLGENFRAMEBUFFERSPROC)wglGetProcAddress("glGenFramebuffers");
	glBindFramebuffer			= (PFNGLBINDFRAMEBUFFERPROC)wglGetProcAddress("glBindFramebuffer");
	glFramebufferTexture		= (PFNGLFRAMEBUFFERTEXTUREPROC)wglGetProcAddress("glFramebufferTexture");
	glCheckFramebufferStatus	= (PFNGLCHECKFRAMEBUFFERSTATUSPROC)wglGetProcAddress("glCheckFramebufferStatus");
	glDeleteFramebuffers		= (PFNGLDELETEFRAMEBUFFERSPROC)wglGetProcAddress("glDeleteFramebuffers");
	glDrawBuffers				= (PFNGLDRAWBUFFERSPROC)wglGetProcAddress("glDrawBuffers");
	glBlitFramebuffer			= (PFNGLBLITFRAMEBUFFERPROC)wglGetProcAddress("glBlitFramebuffer");	

	Mesh::InitializeDefaultMeshes();
	
	/*
	m_rendererMesh = Mesh::GetMeshShape(MeshShape_QUAD_XY);
	GPUProgram* program = new GPUProgram("Data/Shaders/sprite.vert", "Data/Shaders/sprite.frag");
	m_rendererMaterial = new Material(program);
	m_rendererMaterial->SetUniform("gDiffuseTex", g_DefaultImage.m_spriteSheetTexture);
	*/
}

Renderer::~Renderer()
{
	Mesh::DestroyDefaultMeshes();

	DestroyEngineCommon();
}

void Renderer::CreateScreen()
{
	glOrtho(VIEW_LEFT, VIEW_RIGHT, VIEW_BOTTOM, VIEW_TOP, 0.f, 1.f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glLineWidth(2.f);
	glEnable(GL_LINE_SMOOTH);
}

void Renderer::ClearScreen(float red, float green, float blue)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(red, green, blue, 1.f);
}

void Renderer::ClearScreen(const Rgba& clearColor)
{
	glClearColor(clearColor.GetRFloat(), clearColor.GetGFloat(), clearColor.GetBFloat(), clearColor.GetAFloat());
	glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void Renderer::SetOrtho(const Vector2& leftRight, const Vector2& bottomTop)
{
	glLoadIdentity();
	glOrtho(leftRight.x, leftRight.y, bottomTop.x, bottomTop.y, 0.f, 1.f);
}

void Renderer::DrawLine(const Vector2& start, const Vector2& end, float red, float green, float blue, float alpha, float lineThickness)
{
	BindTexture(*g_DefaultImage.m_spriteSheetTexture);
	glDisable(GL_TEXTURE_2D);
	SetColor(red, green, blue, alpha);
	glLineWidth(lineThickness);

	unsigned char uRed = FloatToUChar(red);
	unsigned char uGreen = FloatToUChar(green);
	unsigned char uBlue = FloatToUChar(blue);
	unsigned char uAlpha = FloatToUChar(alpha);

	Rgba color = Rgba(uRed, uGreen, uBlue, uAlpha);

	const int numVerts = 2;
	Vertex3D_PCT verts[numVerts];
	Vector2 texCoords = Vector2(0.f, 1.f);
	verts[0].m_position = Vector3(start.x, start.y, 0.f);
	verts[0].m_color = color;
	verts[0].m_texCoords = texCoords;

	verts[1].m_position = Vector3(end.x, end.y, 0.f);
	verts[1].m_color = color;
	verts[1].m_texCoords = texCoords;

	DrawVertexArray_PCT(verts, numVerts, PRIMITIVE_LINES);
	/*
	glBegin(GL_LINES);
	{
		glVertex2f(start.x, start.y);
		glVertex2f(end.x, end.y);
	}
	glEnd();
	*/
}


void Renderer::DrawLine(const Vector3& start, const Vector3& end, float red, float green, float blue, float alpha, float lineThickness)
{
	BindTexture(*g_DefaultImage.m_spriteSheetTexture);
	glEnable(GL_TEXTURE_2D);
	SetColor(red, green, blue, alpha);
	glLineWidth(lineThickness);

	unsigned char uRed = FloatToUChar(red);
	unsigned char uGreen = FloatToUChar(green);
	unsigned char uBlue = FloatToUChar(blue);
	unsigned char uAlpha = FloatToUChar(alpha);

	Rgba color = Rgba(uRed, uGreen, uBlue, uAlpha);

	Vector2 texCoords = Vector2(0.f, 1.f);
	const int numVerts = 2;
	Vertex3D_PCT verts[numVerts];
	verts[0].m_position = Vector3(start.x, start.y, start.z);
	verts[0].m_color = color;
	verts[0].m_texCoords = Vector2(0.f, 0.f);

	verts[1].m_position = Vector3(end.x, end.y, end.z);
	verts[1].m_color = color;
	verts[1].m_texCoords = Vector2(1.f, 1.f);

	DrawVertexArray_PCT(verts, numVerts, PRIMITIVE_LINES);
	/*
	glBegin(GL_LINES);
	{
		glVertex3f(start.x, start.y, start.z);
		glVertex3f(end.x, end.y, end.z);
	}
	glEnd();
	*/
}


void Renderer::DrawLine(const Vector3& start, const Vector3& end, Rgba color, float lineThickness)
{
	BindTexture(*g_DefaultImage.m_spriteSheetTexture);
	SetColor(color.m_red, color.m_green, color.m_blue, color.m_alpha);
	glLineWidth(lineThickness);
	glEnable(GL_TEXTURE_2D);
	Vector2 texCoords = Vector2(1.f, 1.f);
	const int numVerts = 2;
	Vertex3D_PCT verts[numVerts];
	verts[0].m_position = Vector3(start.x, start.y, start.z);
	verts[0].m_color = color;
	verts[0].m_texCoords = Vector2(0.f, 0.f);

	verts[1].m_position = Vector3(end.x, end.y, end.z);
	verts[1].m_color = color;
	verts[1].m_texCoords = Vector2(1.f, 1.f);

	DrawVertexArray_PCT(verts, numVerts, PRIMITIVE_LINES);
	/*
	glBegin(GL_LINES);
	{
		glVertex3f(start.x, start.y, start.z);
		glVertex3f(end.x, end.y, end.z);
	}
	glEnd();
	*/
}

void Renderer::DrawLineLoop(const std::vector< Vertex3D_PCT >& vertexes, float lineThickness)
{
	for (unsigned int i = 1; i < vertexes.size(); i++)
	{
		DrawLine(vertexes[i - 1].m_position, vertexes[i].m_position, vertexes[i].m_color, lineThickness);
	}
	DrawLine(vertexes[vertexes.size() - 1].m_position, vertexes[0].m_position, vertexes[0].m_color, lineThickness);
}

void Renderer::RenderPolygon(float centerX, float centerY, float radius, float numSides, float degreesOffset)
{
	BindTexture(*g_DefaultImage.m_spriteSheetTexture);
	//const float pi = 3.141592653589793f;
	const float radiansTotal = 2.f * pi;
	const float radiansPerSide = radiansTotal / numSides;

	Vector2 texCoords = Vector2(0.f, 1.f);
	std::vector<Vertex3D_PCT> verts;
	int index = 0;

	for (float radians = 0.f; radians < radiansTotal; radians += radiansPerSide)
	{
		Vertex3D_PCT vert;
		float x = centerX + (radius * cos(radians + degreesOffset));
		float y = centerY + (radius * sin(radians + degreesOffset));
		vert.m_position = Vector3(x, y, 0.f);
		vert.m_color = Rgba::WHITE;
		vert.m_texCoords = texCoords;
		/*
		verts[index].m_position = Vector3(x, y,0.f);
		verts[index].m_color = Rgba::WHITE;
		verts[index].m_texCoords = texCoords;
		*/
		verts.push_back(vert);
		index++;
	}
	DrawVertexArray_PCT(&verts[0], index, PRIMITIVE_LINES_LOOP);
	/*
	glBegin(GL_LINE_LOOP);
	{
		for (float radians = 0.f; radians < radiansTotal; radians += radiansPerSide)
		{
			float x = centerX + (radius * cos(radians + degreesOffset));
			float y = centerY + (radius * sin(radians + degreesOffset));
			glVertex2f(x, y);
		}
	}
	glEnd();
	*/
}

void Renderer::SetColor(float red, float green, float blue, float alpha)
{
	glColor4f(red, green, blue, alpha);
}

void Renderer::SetColor(Rgba color)
{
	glColor4f(color.m_red, color.m_green, color.m_blue, color.m_alpha);
}

void Renderer::DrawLoop(Vector2* vecArray[], int vertCount, float red, float green, float blue, float alpha, float lineThickness)
{
	BindTexture(*g_DefaultImage.m_spriteSheetTexture);
	SetColor(red, green, blue, alpha);
	glLineWidth(lineThickness);
	Vector2 texCoords = Vector2(0.f, 1.f);
	Vertex3D_PCT* verts = new Vertex3D_PCT;

	unsigned char uRed = FloatToUChar(red);
	unsigned char uGreen = FloatToUChar(green);
	unsigned char uBlue = FloatToUChar(blue);
	unsigned char uAlpha = FloatToUChar(alpha);

	Vector2* thisVertex;
	for (int i = 0; i < vertCount; i++)
	{
		thisVertex = vecArray[i];
		verts[i].m_position = Vector3(thisVertex->x, thisVertex->y, 0.f);
		verts[i].m_color = Rgba(uRed, uGreen, uBlue, uAlpha);
		verts[i].m_texCoords = texCoords;
	}


	DrawVertexArray_PCT(verts, vertCount, PRIMITIVE_LINES_LOOP);
	/*
	glBegin(GL_LINE_LOOP);
	{
		Vector2* thisVertex;
		for (int i = 0; i < vertCount; i++)
		{
			thisVertex = vecArray[i];
			glVertex2f(thisVertex->x, thisVertex->y);
		}
	}
	glEnd();
	*/
}

void Renderer::DrawLoop(AABB2 bounds, Rgba color, float lineThickness)
{
	BindTexture(*g_DefaultImage.m_spriteSheetTexture);
	SetColor(color.m_red, color.m_alpha, color.m_blue, color.m_alpha);
	glLineWidth(lineThickness);
	Vector2 texCoords = Vector2(0.f, 1.f);
	const int numVerts = 4;
	Vertex3D_PCT verts[numVerts];

	//TODO: No color here, may influence other games
	for (int i = 0; i < numVerts; i++)
	{
		verts[i].m_texCoords = texCoords;
		verts[i].m_color = color;
	}


	verts[0].m_position = Vector3(bounds.m_mins.x, bounds.m_mins.y, 0.f);

	verts[1].m_position = Vector3(bounds.m_maxs.x, bounds.m_mins.y, 0.f);

	verts[2].m_position = Vector3(bounds.m_maxs.x, bounds.m_maxs.y, 0.f);

	verts[3].m_position = Vector3(bounds.m_mins.x, bounds.m_maxs.y, 0.f);

	DrawVertexArray_PCT(verts, numVerts, PRIMITIVE_LINES_LOOP);
	/*
	glBegin(GL_LINE_LOOP);
	{
		glVertex2f(bounds.m_mins.x, bounds.m_mins.y);
		glVertex2f(bounds.m_mins.x, bounds.m_maxs.y);
		glVertex2f(bounds.m_maxs.x, bounds.m_maxs.y);
		glVertex2f(bounds.m_maxs.x, bounds.m_mins.y);

	}
	glEnd();
	*/
}

//Draws a flat shaded (solid, non-textured) quad
void Renderer::DrawAABB(const AABB2& bounds, const Rgba& color)
{
	glEnable(GL_TEXTURE_2D);
	BindTexture(*g_DefaultImage.m_spriteSheetTexture);
	SetColor(color.m_red, color.m_green, color.m_blue, color.m_alpha);
	Vector2 texCoords = Vector2(0.f, 1.f);
	const int numVerts = 4;
	Vertex3D_PCT verts[numVerts];

	//TODO: No color here, may influence other games
	for (int i = 0; i < numVerts; i++)
	{
		verts[i].m_texCoords = texCoords;
		verts[i].m_color = color;
	}

	
	verts[0].m_position = Vector3(bounds.m_mins.x, bounds.m_mins.y, 0.f);
	
	verts[1].m_position = Vector3(bounds.m_maxs.x, bounds.m_mins.y, 0.f);

	verts[2].m_position = Vector3(bounds.m_maxs.x, bounds.m_maxs.y, 0.f);

	verts[3].m_position = Vector3(bounds.m_mins.x, bounds.m_maxs.y, 0.f);

	DrawVertexArray_PCT(verts, numVerts, PRIMITIVE_QUADS);
	/*
	glBegin(GL_QUADS);
	{
		glVertex2f(bounds.m_mins.x, bounds.m_mins.y);
		glVertex2f(bounds.m_maxs.x, bounds.m_mins.y);
		glVertex2f(bounds.m_maxs.x, bounds.m_maxs.y);
		glVertex2f(bounds.m_mins.x, bounds.m_maxs.y);
	}
	glEnd();
	*/
	glDisable(GL_TEXTURE_2D);
}

//Draws a textured quad
void Renderer::DrawTexturedAABB(const AABB2& bounds, const Texture& texture, const Vector2& texCoordMins, const Vector2& texCoordMaxs)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture.m_openglTextureID);

	const int numVerts = 4;
	Vertex3D_PCT verts[numVerts];

	//TODO: No color here, may influence other games
	for (int i = 0; i < numVerts; i++)
	{
		verts[i].m_color = Rgba::WHITE;
	}

	verts[0].m_texCoords = Vector2(texCoordMins.x, texCoordMaxs.y);
	verts[0].m_position = Vector3(bounds.m_mins.x, bounds.m_mins.y, 0.f);

	verts[1].m_texCoords = Vector2(texCoordMaxs.x, texCoordMaxs.y);
	verts[1].m_position = Vector3(bounds.m_maxs.x, bounds.m_mins.y, 0.f);

	verts[2].m_texCoords = Vector2(texCoordMaxs.x, texCoordMins.y);
	verts[2].m_position = Vector3(bounds.m_maxs.x, bounds.m_maxs.y, 0.f);

	verts[3].m_texCoords = Vector2(texCoordMins.x, texCoordMins.y);
	verts[3].m_position = Vector3(bounds.m_mins.x, bounds.m_maxs.y, 0.f);

	DrawVertexArray_PCT(verts, numVerts, PRIMITIVE_QUADS);
	/*
	glBegin(GL_QUADS);
	{
		//Tex Coords start at the top left, vertex coords start at the bottom left

		glTexCoord2f(texCoordMins.x, texCoordMaxs.y);
		glVertex2f(bounds.m_mins.x, bounds.m_mins.y);

		glTexCoord2f(texCoordMaxs.x, texCoordMaxs.y);
		glVertex2f(bounds.m_maxs.x, bounds.m_mins.y);

		glTexCoord2f(texCoordMaxs.x, texCoordMins.y);
		glVertex2f(bounds.m_maxs.x, bounds.m_maxs.y);

		glTexCoord2f(texCoordMins.x, texCoordMins.y);
		glVertex2f(bounds.m_mins.x, bounds.m_maxs.y);
	}
	glEnd();
	*/
	glDisable(GL_TEXTURE_2D);
}

//Draws a textured (and tinted) quad
void Renderer::DrawTexturedAABB(const AABB2& bounds, const Texture& texture, const Vector2& texCoordMins, const Vector2& texCoordMaxs, const Rgba& tint)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture.m_openglTextureID);
	glColor4f(tint.m_red, tint.m_green, tint.m_blue, tint.m_alpha);


	const int numVerts = 4;
	Vertex3D_PCT verts[numVerts];

	//TODO: No color here, may influence other games
	for (int i = 0; i < numVerts; i++)
	{
		verts[i].m_color = tint;
	}

	verts[0].m_texCoords = Vector2(texCoordMins.x, texCoordMaxs.y);
	verts[0].m_position = Vector3(bounds.m_mins.x, bounds.m_mins.y, 0.f);

	verts[1].m_texCoords = Vector2(texCoordMaxs.x, texCoordMaxs.y);
	verts[1].m_position = Vector3(bounds.m_maxs.x, bounds.m_mins.y, 0.f);

	verts[2].m_texCoords = Vector2(texCoordMaxs.x, texCoordMins.y);
	verts[2].m_position = Vector3(bounds.m_maxs.x, bounds.m_maxs.y, 0.f);

	verts[3].m_texCoords = Vector2(texCoordMins.x, texCoordMins.y);
	verts[3].m_position = Vector3(bounds.m_mins.x, bounds.m_maxs.y, 0.f);

	DrawVertexArray_PCT(verts, numVerts, PRIMITIVE_QUADS);
	/*
	glBegin(GL_QUADS);
	{
		//Tex Coords start at the top left, vertex coords start at the bottom left

		glTexCoord2f(texCoordMins.x, texCoordMaxs.y); 
		glVertex2f(bounds.m_mins.x, bounds.m_mins.y);

		glTexCoord2f(texCoordMaxs.x, texCoordMaxs.y);
		glVertex2f(bounds.m_maxs.x, bounds.m_mins.y);

		glTexCoord2f(texCoordMaxs.x, texCoordMins.y);
		glVertex2f(bounds.m_maxs.x, bounds.m_maxs.y);

		glTexCoord2f(texCoordMins.x, texCoordMins.y);
		glVertex2f(bounds.m_mins.x, bounds.m_maxs.y);
	}
	glEnd();
	*/
	glDisable(GL_TEXTURE_2D);
}

void Renderer::DrawTexturedAABB(const AABB2& bounds, const int& textureID, const Vector2& texCoordMins, const Vector2& texCoordMaxs, const Rgba& tint, bool flipVerts)
{
	flipVerts;
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureID);
	//BindTexture(*g_DefaultImage.m_spriteSheetTexture);
	Vector2 texCoords = Vector2(0.f, 1.f);
	const int numVerts = 4;
	Vertex3D_PCT verts[numVerts];

	//TODO: No color here, may influence other games
	for (int i = 0; i < numVerts; i++)
	{
		verts[i].m_color = tint;
	}

	verts[0].m_texCoords = Vector2(texCoordMins.x, texCoordMaxs.y);
	verts[1].m_texCoords = Vector2(texCoordMaxs.x, texCoordMaxs.y);
	verts[2].m_texCoords = Vector2(texCoordMaxs.x, texCoordMins.y);
	verts[3].m_texCoords = Vector2(texCoordMins.x, texCoordMins.y);

	AABB2 thisBounds = bounds;
	/*
	if (g_spriteRenderSystem != nullptr)
	{
		float importSize = g_spriteRenderSystem->GetImportSize();
		float virtualSize = g_spriteRenderSystem->GetVirtualSize();
		
		thisBounds.m_mins = g_spriteRenderSystem->GetActualCoordsFromVirtual(thisBounds.m_mins);
		
		thisBounds.m_maxs = g_spriteRenderSystem->GetActualCoordsFromVirtual(thisBounds.m_maxs);

	}
	*/
	verts[0].m_position = Vector3(thisBounds.m_mins.x, thisBounds.m_mins.y, 0.f);
	verts[1].m_position = Vector3(thisBounds.m_maxs.x, thisBounds.m_mins.y, 0.f);
	verts[2].m_position = Vector3(thisBounds.m_maxs.x, thisBounds.m_maxs.y, 0.f);
	verts[3].m_position = Vector3(thisBounds.m_mins.x, thisBounds.m_maxs.y, 0.f);
	

	DrawVertexArray_PCT(verts, numVerts, PRIMITIVE_QUADS);

	//glColor4f(tint.m_red, tint.m_green, tint.m_blue, tint.m_alpha);
	/*
	glBegin(GL_QUADS);
	{
		//Tex Coords start at the top left, vertex coords start at the bottom left

		glTexCoord2f(texCoordMins.x, texCoordMaxs.y);
		glVertex2f(bounds.m_mins.x, bounds.m_mins.y);

		glTexCoord2f(texCoordMaxs.x, texCoordMaxs.y);
		glVertex2f(bounds.m_maxs.x, bounds.m_mins.y);

		glTexCoord2f(texCoordMaxs.x, texCoordMins.y);
		glVertex2f(bounds.m_maxs.x, bounds.m_maxs.y);

		glTexCoord2f(texCoordMins.x, texCoordMins.y);
		glVertex2f(bounds.m_mins.x, bounds.m_maxs.y);
	}
	glEnd();
	*/
	glDisable(GL_TEXTURE_2D);
}



void Renderer::RotateView(const float& rotation)
{
	glRotatef(rotation, 0.f, 0.f, 1.f);
}

void Renderer::RotateView(float degrees, float x, float y, float z)
{
	glRotatef(degrees, x, y, z);
}

void Renderer::RotateView(float degrees, Vector3 axis)
{
	glRotatef(degrees, axis.x, axis.y, axis.z);
}

/*
void Renderer::SetCullFace(bool enable)
{
	if (enable)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);
}
*/
/*
void Renderer::SetDepthTest(bool enable)
{
	if (enable)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
}*/

void Renderer::TranslateView(const Vector2& position)
{
	glTranslatef(position.x, position.y, 0.f);
}

void Renderer::DrawPoint(const Vector3& pos, const Rgba& tint, float scale)
{
	Vertex3D_PCT verts[6];
	verts[0].m_color = tint;
	verts[0].m_position = Vector3(pos.x - 0.5f * scale, pos.y,pos.z) ;
	verts[0].m_texCoords = Vector2(0.f, 1.f);

	verts[1].m_color = tint;
	verts[1].m_position = Vector3(pos.x + 0.5f * scale, pos.y, pos.z);
	verts[1].m_texCoords = Vector2(0.f, 1.f);

	verts[2].m_color = tint;
	verts[2].m_position = Vector3(pos.x, pos.y - 0.5f * scale, pos.z);
	verts[2].m_texCoords = Vector2(0.f, 1.f);

	verts[3].m_color = tint;
	verts[3].m_position = Vector3(pos.x, pos.y + 0.5f * scale, pos.z);
	verts[3].m_texCoords = Vector2(0.f, 1.f);

	verts[4].m_color = tint;
	verts[4].m_position = Vector3(pos.x, pos.y, pos.z - 0.5f * scale);
	verts[4].m_texCoords = Vector2(0.f, 1.f);

	verts[5].m_color = tint;
	verts[5].m_position = Vector3(pos.x, pos.y, pos.z + 0.5f * scale);
	verts[5].m_texCoords = Vector2(0.f, 1.f);

	DrawVertexArray_PCT(verts, 6, PRIMITIVE_LINES);
}

void Renderer::DrawAxes(float scale)
{
	BindTexture(*g_DefaultImage.m_spriteSheetTexture);
	Vector2 texCoords = Vector2(0.f, 1.f);
	const int numVerts = 6;
	Vertex3D_PCT verts[numVerts];
	//glDisable(GL_TEXTURE_2D);

	glDisable(GL_DEPTH_TEST);
	glLineWidth(2.f);

	verts[0].m_position = Vector3(0.f, 0.f, 0.f);
	verts[0].m_color = Rgba::RED;
	verts[0].m_texCoords = texCoords;

	verts[1].m_position = Vector3(1.f, 0.f, 0.f) * scale;
	verts[1].m_color = Rgba::RED;
	verts[1].m_texCoords = texCoords;

	verts[2].m_position = Vector3(0.f, 0.f, 0.f);
	verts[2].m_color = Rgba::GREEN;
	verts[2].m_texCoords = texCoords;

	verts[3].m_position = Vector3(0.f, 1.f, 0.f) * scale;
	verts[3].m_color = Rgba::GREEN;
	verts[3].m_texCoords = texCoords;

	verts[4].m_position = Vector3(0.f, 0.f, 0.f);
	verts[4].m_color = Rgba::BLUE;
	verts[4].m_texCoords = texCoords;

	verts[5].m_position = Vector3(0.f, 0.f, 1.f) * scale;
	verts[5].m_color = Rgba::BLUE;
	verts[5].m_texCoords = texCoords;

	DrawVertexArray_PCT(verts, numVerts, PRIMITIVE_LINES);
	/*
	glBegin(GL_LINES);
	{
		SetColor(1.f, 0.f, 0.f, 1.f);
		glVertex3f(0.f, 0.f, 0.f);
		glVertex3f(1.f, 0.f, 0.f);

		SetColor(0.f, 1.f, 0.f, 1.f);
		glVertex3f(0.f, 0.f, 0.f);
		glVertex3f(0.f, 1.f, 0.f);

		SetColor(0.f, 0.f, 1.f, 1.f);
		glVertex3f(0.f, 0.f, 0.f);
		glVertex3f(0.f, 0.f, 1.f);
	}
	glEnd();
	*/
	glEnable(GL_DEPTH_TEST);
	glLineWidth(3.f);

	verts[0].m_position = Vector3(0.f, 0.f, 0.f);
	verts[0].m_color = Rgba::RED;
	verts[0].m_texCoords = texCoords;

	verts[1].m_position = Vector3(1.f, 0.f, 0.f) * scale;
	verts[1].m_color = Rgba::RED;
	verts[1].m_texCoords = texCoords;

	verts[2].m_position = Vector3(0.f, 0.f, 0.f);
	verts[2].m_color = Rgba::GREEN;
	verts[2].m_texCoords = texCoords;

	verts[3].m_position = Vector3(0.f, 1.f, 0.f) * scale;
	verts[3].m_color = Rgba::GREEN;
	verts[3].m_texCoords = texCoords;

	verts[4].m_position = Vector3(0.f, 0.f, 0.f);
	verts[4].m_color = Rgba::BLUE;
	verts[4].m_texCoords = texCoords;

	verts[5].m_position = Vector3(0.f, 0.f, 1.f) * scale;
	verts[5].m_color = Rgba::BLUE;
	verts[5].m_texCoords = texCoords;

	DrawVertexArray_PCT(verts, numVerts, PRIMITIVE_LINES);
	/*
	glBegin(GL_LINES);
	{
		SetColor(1.f, 0.f, 0.f, 1.f);
		glVertex3f(0.f, 0.f, 0.f);
		glVertex3f(3.f, 0.f, 0.f);

		SetColor(0.f, 1.f, 0.f, 1.f);
		glVertex3f(0.f, 0.f, 0.f);
		glVertex3f(0.f, 3.f, 0.f);

		SetColor(0.f, 0.f, 1.f, 1.f);
		glVertex3f(0.f, 0.f, 0.f);
		glVertex3f(0.f, 0.f, 3.f);
	}
	glEnd();
	*/
}



void Renderer::DrawAxesRotation(const Vector3& pos, Quaternion rotation, float scale)
{
	BindTexture(*g_DefaultImage.m_spriteSheetTexture);
	Vector2 texCoords = Vector2(0.f, 1.f);
	const int numVerts = 6;
	Vertex3D_PCT verts[numVerts];
	//glDisable(GL_TEXTURE_2D);

	glDisable(GL_DEPTH_TEST);
	glLineWidth(2.f);


	verts[0].m_position = Vector3(0.f, 0.f, 0.f) + pos;
	verts[0].m_color = Rgba::RED;
	verts[0].m_texCoords = texCoords;

	verts[1].m_position = Vector3(1.f, 0.f, 0.f);
	verts[1].m_color = Rgba::RED;
	verts[1].m_texCoords = texCoords;

	verts[2].m_position = Vector3(0.f, 0.f, 0.f) + pos;
	verts[2].m_color = Rgba::GREEN;
	verts[2].m_texCoords = texCoords;

	verts[3].m_position = Vector3(0.f, 1.f, 0.f);
	verts[3].m_color = Rgba::GREEN;
	verts[3].m_texCoords = texCoords;

	verts[4].m_position = Vector3(0.f, 0.f, 0.f) + pos;
	verts[4].m_color = Rgba::BLUE;
	verts[4].m_texCoords = texCoords;

	verts[5].m_position = Vector3(0.f, 0.f, 1.f);
	verts[5].m_color = Rgba::BLUE;
	verts[5].m_texCoords = texCoords;

	Matrix4 rotationMatrix = Matrix4::IDENTITY;


// 	rotation.ToRotationMatrix(rotationMatrix);

	EulerAngles angles;

	rotation.ToEulerAngles(angles);
	angles.NegateYaw();
	rotationMatrix.MatrixMakeRotationEuler(-angles.m_yawDegreesAboutZ, angles.m_pitchDegreesAboutX, angles.m_rollDegreesAboutY);
 	rotationMatrix.TransposeRotation();


	Vector3 V0Rot = (verts[1].m_position) * rotationMatrix * scale;

	Vector3 V1Rot = (verts[3].m_position) * rotationMatrix * scale;


	Vector3 V2Rot = (verts[5].m_position) * rotationMatrix * scale;



	verts[1].m_position = V0Rot + pos;


	verts[3].m_position = V1Rot + pos;

	verts[5].m_position = V2Rot + pos;

	DrawVertexArray_PCT(verts, numVerts, PRIMITIVE_LINES);
}

void Renderer::DrawText2D(const Vector2& startBottomLeft, const std::string& asciiText, float cellHeight,
	const Rgba& tint, const BitmapFont* font)
{
	//glDisable(GL_CULL_FACE);
	//glDisable(GL_DEPTH_TEST);

	Vector2 curMins = startBottomLeft;
	Vector2 curMaxs = Vector2(curMins.x + 16.f, curMins.y + cellHeight);
	AABB2 curPos; 
	Texture* tex = font->GetTexture();
	for (std::string::const_iterator it = asciiText.begin(); it != asciiText.end(); ++it) {

		curMaxs = Vector2(curMins.x + 16.f, curMins.y + cellHeight);
		curPos = AABB2(curMins, curMaxs);
		AABB2 texCoords = font->GetTexCoordsForGlyph(*it);
		DrawTexturedAABB(curPos, *tex, texCoords.m_mins, texCoords.m_maxs, tint);
		curMins.x += 16.f;
	}
}

void Renderer::DrawTextMeta2D(const Vector3& position, float scale, const std::string& asciiText, const Rgba& color, 
	BitmapFontMeta* font /*= nullptr*/, const Vector3& up /*= Vector3(0.f,1.f,0.f)*/, const Vector3& right /*= Vector3(1.f, 0.f, 0.f)*/)
{

	
	if (asciiText == "")
	{
		
		return;
	}
	Vector3 cursor = position;
	float tex_width = (float)font->m_textureWidth;
	float tex_height = (float)font->m_textureHeight;

	Glyph* prev_glyph = nullptr;
	const int vertLength = asciiText.length() * 4;
	std::vector<Vertex3D_PCT> verts;

	
	for (std::string::const_iterator it = asciiText.begin(); it != asciiText.end(); ++it)
	{
		Glyph* thisGlyph = font->m_glyphRegistry.at(*it);

		if (thisGlyph == nullptr)
			continue;
		int kerningVal = 0;
		if (prev_glyph != nullptr)
		{
			font->GetKerning(prev_glyph->m_charId, thisGlyph->m_charId, kerningVal);
		}
		
		
		cursor += kerningVal * scale * right;

		Vector3 topLeft = cursor + thisGlyph->m_xOffset * scale * right
			- thisGlyph->m_yOffset * scale * up;
// 		topLeft.x += thisGlyph->m_xOffset * scale;
// 		topLeft.y -= thisGlyph->m_yOffset * scale;

		Vector3 topRight = topLeft + scale * (float)thisGlyph->m_width * right;
		//topRight.x += scale * thisGlyph->m_width;

		Vector3 bottomLeft = topLeft - up * scale * (float)thisGlyph->m_height;
		//bottomLeft.y -= scale * thisGlyph->m_height;

		Vector3 bottomRight = bottomLeft + scale * (float)thisGlyph->m_width * right;
		//bottomRight.x += scale * thisGlyph->m_width;

		Vector2 uv_topLeft = Vector2((float) thisGlyph->m_xPos / tex_width, (float)thisGlyph->m_yPos / tex_height);

		Vector2 uv_topRight = uv_topLeft;
		uv_topRight.x += ((float)thisGlyph->m_width / tex_width);

		Vector2 uv_bottomLeft = uv_topLeft;
		uv_bottomLeft.y += ((float)thisGlyph->m_height / tex_height);

		Vector2 uv_bottomRight = uv_bottomLeft;
		uv_bottomRight.x += ((float)thisGlyph->m_width / tex_width);

		Vertex3D_PCT vert;
		vert.m_position = bottomLeft;
		vert.m_texCoords = uv_bottomLeft;
		vert.m_color = color;
		verts.push_back(vert);

		vert.m_position = bottomRight;
		vert.m_texCoords = uv_bottomRight;
		vert.m_color = color;
		verts.push_back(vert);

		vert.m_position = topRight;
		vert.m_texCoords = uv_topRight;
		vert.m_color = color;
		verts.push_back(vert);

		vert.m_position = topLeft;
		vert.m_texCoords = uv_topLeft;
		vert.m_color = color;
		verts.push_back(vert);

		cursor += thisGlyph->m_xAdvance * scale * right;
		prev_glyph = thisGlyph;
		

	}
	
	BindTexture(*font->GetTexture());
	DrawVertexArray_PCT(&verts[0], vertLength, PRIMITIVE_QUADS);
	
}

void Renderer::PushMatrix()
{
	glPushMatrix();
}

void Renderer::PopMatrix()
{
	glPopMatrix();
}

void Renderer::DrawGrid(int xCells, int yCells)
{
	Vector2 v1 = Vector2(0, 0);
	Vector2 v2 = Vector2(0, 0);
	for (int i = 0; i < xCells; i++)
	{
		v1.x = (v1.x + 1);
		v2.x = v1.x;
		v2.y = WINDOW_PHYSICAL_HEIGHT;
		DrawLine(v1,v2,1.f,1.f,1.f,1.f,2.f);
	}

	v1 = Vector2(0, 0);
	v2 = Vector2(0, 0);

	for (int j = 0; j < yCells; j++)
	{
		v1.y = (v1.y + 1);
		v2.y = v1.y;
		v2.x = WINDOW_PHYSICAL_WIDTH;
		DrawLine(v1, v2, 1.f, 1.f, 1.f, 1.f, 2.f);
	}
}

void Renderer::ScaleView(const Vector2& scale)
{
	glScalef(scale.x, scale.y, 0.f);
}

void Renderer::SetInverseDestBlend()
{
	glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
}
void Renderer::SetAlphaBlend()
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Renderer::SetAdditiveBlend()
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
}

void Renderer::SetPointSize(float size)
{
	glPointSize(size);
}

void Renderer::SetPerspective(float fovDegreesY, float aspect, float nearDist, float farDist)
{
	glLoadIdentity();
	gluPerspective(fovDegreesY, aspect, nearDist, farDist);
}

void Renderer::ClearScreenDepth(float red, float green, float blue)
{
	glClearColor(red, green, blue, 1.f);
	glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::TranslateView(float x, float y, float z)
{
	glTranslatef(x, y, z);
}

void Renderer::TranslateView(const Vector3& translation)
{
	glTranslatef(translation.x, translation.y, translation.z);
}

// void Renderer::DrawCubeFace(CubeDirection face, const AABB2& bounds, const Texture& texture, const Vector2& texCoordMins, const Vector2& texCoordMaxs, const Rgba& tint)
// {
// 	glEnable(GL_CULL_FACE);
// 	glEnable(GL_DEPTH_TEST);
// 	glEnable(GL_TEXTURE_2D);
// 	glColor4f(1.f, 1.f, 1.f, 1.f);
// 	//glColor4f(tint.m_red, tint.m_green, tint.m_blue, tint.m_alpha);
// 	glBindTexture(GL_TEXTURE_2D, texture.m_openglTextureID);
// 	/*
// 	// 		11
// 	// 		10
// 	// 		00
// 	// 		01
// 	// 		*/
// 	glBegin(GL_QUADS);
// 	{
// 		switch (face)
// 		{
// 		case FRONT:
// 		{
// 			glTexCoord2f(texCoordMaxs.x, texCoordMaxs.y);
// 			glVertex3f(1.f, 0.f, 0.f);
// 			glTexCoord2f(texCoordMaxs.x, texCoordMins.y);
// 			glVertex3f(1.f, 0.f, 1.f);
// 			glTexCoord2f(texCoordMins.x, texCoordMins.y);
// 			glVertex3f(0.f, 0.f, 1.f);
// 			glTexCoord2f(texCoordMins.x, texCoordMaxs.y);
// 			glVertex3f(0.f, 0.f, 0.f);
// 			break;
// 		}
// 		case BACK:
// 		{
// 			glTexCoord2f(texCoordMaxs.x, texCoordMaxs.y);
// 			glVertex3f(0.f, 1.f, 0.f);
// 			glTexCoord2f(texCoordMaxs.x, texCoordMins.y);
// 			glVertex3f(0.f, 1.f, 1.f);
// 			glTexCoord2f(texCoordMins.x, texCoordMins.y);
// 			glVertex3f(1.f, 1.f, 1.f);
// 			glTexCoord2f(texCoordMins.x, texCoordMaxs.y);
// 			glVertex3f(1.f, 1.f, 0.f);
// 			break;
// 		}
// 		case LEFT:
// 		{
// 			glTexCoord2f(texCoordMaxs.x, texCoordMaxs.y);
// 			glVertex3f(0.f, 0.f, 0.f);
// 			glTexCoord2f(texCoordMaxs.x, texCoordMins.y);
// 			glVertex3f(0.f, 0.f, 1.f);
// 			glTexCoord2f(texCoordMins.x, texCoordMins.y);
// 			glVertex3f(0.f, 1.f, 1.f);
// 			glTexCoord2f(texCoordMins.x, texCoordMaxs.y);
// 			glVertex3f(0.f, 1.f, 0.f);
// 			break;
// 		}
// 		case RIGHT:
// 		{
// 			glTexCoord2f(texCoordMaxs.x, texCoordMaxs.y);
// 			glVertex3f(1.f, 1.f, 0.f);
// 			glTexCoord2f(texCoordMaxs.x, texCoordMins.y);
// 			glVertex3f(1.f, 1.f, 1.f);
// 			glTexCoord2f(texCoordMins.x, texCoordMins.y);
// 			glVertex3f(1.f, 0.f, 1.f);
// 			glTexCoord2f(texCoordMins.x, texCoordMaxs.y);
// 			glVertex3f(1.f, 0.f, 0.f);
// 			break;
// 		}
// 		case TOP:
// 		{
// 			glTexCoord2f(texCoordMaxs.x, texCoordMaxs.y);
// 			glVertex3f(1.f, 0.f, 1.f);
// 			glTexCoord2f(texCoordMaxs.x, texCoordMins.y);
// 			glVertex3f(1.f, 1.f, 1.f);
// 			glTexCoord2f(texCoordMins.x, texCoordMins.y);
// 			glVertex3f(0.f, 1.f, 1.f);
// 			glTexCoord2f(texCoordMins.x, texCoordMaxs.y);
// 			glVertex3f(0.f, 0.f, 1.f);
// 			break;
// 		}
// 		case BOTTOM:
// 		{
// 			glTexCoord2f(texCoordMaxs.x, texCoordMaxs.y);
// 			glVertex3f(0.f, 0.f, 0.f);
// 			glTexCoord2f(texCoordMaxs.x, texCoordMins.y);
// 			glVertex3f(0.f, 1.f, 0.f);
// 			glTexCoord2f(texCoordMins.x, texCoordMins.y);
// 			glVertex3f(1.f, 1.f, 0.f);
// 			glTexCoord2f(texCoordMins.x, texCoordMaxs.y);
// 			glVertex3f(1.f, 0.f, 0.f);
// 			break;
// 		}
// 		default:
// 		{
// 			glTexCoord2f(texCoordMaxs.x, texCoordMaxs.y);
// 			glVertex3f(1.f, 0.f, 0.f);
// 			glTexCoord2f(texCoordMaxs.x, texCoordMins.y);
// 			glVertex3f(1.f, 0.f, 1.f);
// 			glTexCoord2f(texCoordMins.x, texCoordMins.y);
// 			glVertex3f(0.f, 0.f, 1.f);
// 			glTexCoord2f(texCoordMins.x, texCoordMaxs.y);
// 			glVertex3f(0.f, 0.f, 0.f);
// 			break;
// 		};
// 		}
// 	}
// 	glEnd();
// 	glDisable(GL_TEXTURE_2D);
// // 	glDisable(GL_CULL_FACE);
// // 	glDisable(GL_DEPTH_TEST);
// }

void Renderer::DrawTexturedQuad3D(const Texture& tex, const Vector3& vertex1, const Vector3& vertex2, const Vector3& vertex3, const Vector3& vertex4, 
	const AABB2& texCoords, const bool mirrored, const Rgba& tint /*= Rgba::WHITE*/ )
{
	const int numVerts = 4;
	Vertex3D_PCT verts[numVerts];
	Vector2 texCoordMaxs = Vector2(texCoords.m_maxs);
	Vector2 texCoordMins = Vector2(texCoords.m_mins);
	glColor4f(tint.m_red, tint.m_green, tint.m_blue, tint.m_alpha);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex.m_openglTextureID);
	if (mirrored)
	{
		verts[0].m_position = Vector3(vertex1.x, vertex1.y, vertex1.z);
		verts[0].m_color = tint;
		verts[0].m_texCoords = Vector2(texCoordMins.x, texCoordMins.y);

		verts[1].m_position = Vector3(vertex2.x, vertex2.y, vertex2.z);
		verts[1].m_color = tint;
		verts[1].m_texCoords = Vector2(texCoordMaxs.x, texCoordMins.y);

		verts[2].m_position = Vector3(vertex3.x, vertex3.y, vertex3.z);
		verts[2].m_color = tint;
		verts[2].m_texCoords = Vector2(texCoordMaxs.x, texCoordMaxs.y);

		verts[3].m_position = Vector3(vertex4.x, vertex4.y, vertex4.z);
		verts[3].m_color = tint;
		verts[3].m_texCoords = Vector2(texCoordMins.x, texCoordMaxs.y);
		/*
		glBegin(GL_QUADS);
		{
			glTexCoord2f(texCoordMins.x, texCoordMins.y);
			glVertex3f(vertex1.x, vertex1.y, vertex1.z);
			glTexCoord2f(texCoordMaxs.x, texCoordMins.y);
			glVertex3f(vertex2.x, vertex2.y, vertex2.z);
			glTexCoord2f(texCoordMaxs.x, texCoordMaxs.y);
			glVertex3f(vertex3.x, vertex3.y, vertex3.z);
			glTexCoord2f(texCoordMins.x, texCoordMaxs.y);
			glVertex3f(vertex4.x, vertex4.y, vertex4.z);

		}
		glEnd();
		*/
	}
	else
	{

		verts[0].m_position = Vector3(vertex1.x, vertex1.y, vertex1.z);
		verts[0].m_color = tint;
		verts[0].m_texCoords = Vector2(texCoordMins.x, texCoordMaxs.y);

		verts[1].m_position = Vector3(vertex2.x, vertex2.y, vertex2.z);
		verts[1].m_color = tint;
		verts[1].m_texCoords = Vector2(texCoordMaxs.x, texCoordMaxs.y);

		verts[2].m_position = Vector3(vertex3.x, vertex3.y, vertex3.z);
		verts[2].m_color = tint;
		verts[2].m_texCoords = Vector2(texCoordMaxs.x, texCoordMins.y);

		verts[3].m_position = Vector3(vertex4.x, vertex4.y, vertex4.z);
		verts[3].m_color = tint;
		verts[3].m_texCoords = Vector2(texCoordMins.x, texCoordMins.y);
		/*
		glBegin(GL_QUADS);
		{
			glTexCoord2f(texCoordMins.x, texCoordMaxs.y);
			glVertex3f(vertex1.x, vertex1.y, vertex1.z);
			glTexCoord2f(texCoordMaxs.x, texCoordMaxs.y);
			glVertex3f(vertex2.x, vertex2.y, vertex2.z);
			glTexCoord2f(texCoordMaxs.x, texCoordMins.y);
			glVertex3f(vertex3.x, vertex3.y, vertex3.z);
			glTexCoord2f(texCoordMins.x, texCoordMins.y);
			glVertex3f(vertex4.x, vertex4.y, vertex4.z);

		}
		glEnd();
		*/
	}
	
	DrawVertexArray_PCT(verts,numVerts,PRIMITIVE_QUADS);
	glDisable(GL_TEXTURE_2D);
}


void Renderer::DrawSphere(const Vector3& pos, const float radius, const Rgba& color /*= Rgba::WHITE*/)
{
	BindTexture(*g_DefaultImage.m_spriteSheetTexture);

	int numSides = 25; // # of triangles used to draw polygon

	float twicePi = 2.0f * pi;

	Vertex3D_PCT verts[25];

	for (int i = 0; i < numSides; i++)
	{
		verts[i].m_position = Vector3(pos.x + (radius * cos(i *  twicePi / numSides)), pos.y + (radius* sin(i * twicePi / numSides)), pos.z);
		verts[i].m_color = color;
		verts[i].m_texCoords = Vector2(0.0f, 0.0f);
	}

	DrawVertexArray_PCT(verts, 25, PRIMITIVE_LINES_LOOP);

	for (int i = 0; i < numSides; i++)
	{
		verts[i].m_position = Vector3(pos.x + (radius * cos(i *  twicePi / numSides)), pos.y, pos.z + (radius* sin(i * twicePi / numSides)));
		verts[i].m_color = color;
		verts[i].m_texCoords = Vector2(0.0f, 0.0f);
	}

	DrawVertexArray_PCT(verts, 25, PRIMITIVE_LINES_LOOP);

	for (int i = 0; i < numSides; i++)
	{
		verts[i].m_position = Vector3(pos.x, pos.y + (radius * cos(i *  twicePi / numSides)), pos.z + (radius* sin(i * twicePi / numSides)));
		verts[i].m_color = color;
		verts[i].m_texCoords = Vector2(0.0f, 0.0f);
	}

	DrawVertexArray_PCT(verts, 25, PRIMITIVE_LINES_LOOP);
}

void Renderer::DrawArrowRotation(Vector3 startPos, Vector3 endPos, Rgba color, Quaternion rotation)
{
	DrawLine(startPos, endPos, color, 3.f);
	DrawAxesRotation(endPos, rotation , 0.5f);
}

void Renderer::BindTexture(const Texture& texture)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture.m_openglTextureID);
}


void Renderer::DrawVertexArray(const std::vector< Vertex3D_PCT >& vertexes)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex3D_PCT), &vertexes[0].m_position);
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex3D_PCT), &vertexes[0].m_color); // float Rgbas
	glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex3D_PCT), &vertexes[0].m_texCoords);

	glDrawArrays(GL_QUADS, 0, vertexes.size());

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void Renderer::DrawVertexArrayPoints(const std::vector< Vertex3D_PC >& vertexes)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex3D_PC), &vertexes[0].m_position);
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex3D_PC), &vertexes[0].m_color); // float Rgbas


	glDrawArrays(GL_POINT, 0, vertexes.size());

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

}

void Renderer::DrawVertexArray_PCT(const Vertex3D_PCT* verts, int numVerts, PrimitiveType primitiveType = PRIMITIVE_POINTS)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex3D_PCT), &verts[0].m_position);
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex3D_PCT), &verts[0].m_color); // float Rgbas
	glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex3D_PCT), &verts[0].m_texCoords);

	switch (primitiveType)
	{
	case PRIMITIVE_POINTS:
	{
		glDrawArrays(GL_POINT, 0, numVerts);
		break;
	}
	case PRIMITIVE_LINES:
	{
		glDrawArrays(GL_LINES, 0, numVerts);
		break;
	}
	case PRIMITIVE_LINES_LOOP:
	{
		glDrawArrays(GL_LINE_LOOP, 0, numVerts);
		break;
	}
	case PRIMITIVE_TRIANGLES:
	{
		glDrawArrays(GL_TRIANGLES, 0, numVerts);
		break;
	}
	case PRIMITIVE_QUADS:
	{
		glDrawArrays(GL_QUADS, 0, numVerts);
		break;
	}
	default:
	{
		glDrawArrays(GL_POINT, 0, numVerts);
		break;
	}
	}
	//glDrawArrays(GL_POINT, 0, numVerts);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void Renderer::DrawAABB3Wireframe(AABB3 bounds, Rgba color)
{
	glDisable(GL_TEXTURE_2D);

	glEnable(GL_DEPTH_TEST);
	glLineWidth(2.f);
	SetColor(color);
	BindTexture(*g_DefaultImage.m_spriteSheetTexture);

	const int numVerts = 24;

	Vertex3D_PCT verts[numVerts];
	Vector2 texCoords = Vector2(0.f, 1.f);

	for (int i = 0; i < numVerts; i++)
	{
		verts[i].m_color = color;
		verts[i].m_texCoords = texCoords;
	}

	//Bottom Face, Counter Clockwise
	verts[0].m_position = Vector3(bounds.m_mins.x, bounds.m_mins.y, bounds.m_mins.z);
	verts[1].m_position = Vector3(bounds.m_mins.x, bounds.m_maxs.y, bounds.m_mins.z);


	verts[2].m_position = Vector3(bounds.m_mins.x, bounds.m_maxs.y, bounds.m_mins.z);
	verts[3].m_position = Vector3(bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_mins.z);


	verts[4].m_position = Vector3(bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_mins.z);
	verts[5].m_position = Vector3(bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z);


	verts[6].m_position = Vector3(bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z);
	verts[7].m_position = Vector3(bounds.m_mins.x, bounds.m_mins.y, bounds.m_mins.z);

	//Z-Aligned Lines
	verts[8].m_position = Vector3(bounds.m_mins.x, bounds.m_mins.y, bounds.m_mins.z);
	verts[9].m_position = Vector3(bounds.m_mins.x, bounds.m_mins.y, bounds.m_maxs.z);

	verts[10].m_position = Vector3(bounds.m_mins.x, bounds.m_maxs.y, bounds.m_mins.z);
	verts[11].m_position = Vector3(bounds.m_mins.x, bounds.m_maxs.y, bounds.m_maxs.z);

	verts[12].m_position = Vector3(bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_mins.z);
	verts[13].m_position = Vector3(bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_maxs.z);

	verts[14].m_position = Vector3(bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z);
	verts[15].m_position = Vector3(bounds.m_maxs.x, bounds.m_mins.y, bounds.m_maxs.z);


	//Top Face, Counter Clockwise
	verts[16].m_position = Vector3(bounds.m_mins.x, bounds.m_mins.y, bounds.m_maxs.z);
	verts[17].m_position = Vector3(bounds.m_mins.x, bounds.m_maxs.y, bounds.m_maxs.z);


	verts[18].m_position = Vector3(bounds.m_mins.x, bounds.m_maxs.y, bounds.m_maxs.z);
	verts[19].m_position = Vector3(bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_maxs.z);

	verts[20].m_position = Vector3(bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_maxs.z);
	verts[21].m_position = Vector3(bounds.m_maxs.x, bounds.m_mins.y, bounds.m_maxs.z);

	verts[22].m_position = Vector3(bounds.m_maxs.x, bounds.m_mins.y, bounds.m_maxs.z);
	verts[23].m_position = Vector3(bounds.m_mins.x, bounds.m_mins.y, bounds.m_maxs.z);

	DrawVertexArray_PCT(verts, numVerts, PRIMITIVE_LINES);

	/*
	glBegin(GL_LINES);
	{
		//Bottom Face, Counter Clockwise
		glVertex3f(bounds.m_mins.x, bounds.m_mins.y, bounds.m_mins.z);
		glVertex3f(bounds.m_mins.x, bounds.m_maxs.y, bounds.m_mins.z);

		
		glVertex3f(bounds.m_mins.x, bounds.m_maxs.y, bounds.m_mins.z);
		glVertex3f(bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_mins.z);

		glVertex3f(bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_mins.z);
		glVertex3f(bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z);

		glVertex3f(bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z);
		glVertex3f(bounds.m_mins.x, bounds.m_mins.y, bounds.m_mins.z);

		//Z-Aligned Lines
		glVertex3f(bounds.m_mins.x, bounds.m_mins.y, bounds.m_mins.z);
		glVertex3f(bounds.m_mins.x, bounds.m_mins.y, bounds.m_maxs.z);

		glVertex3f(bounds.m_mins.x, bounds.m_maxs.y, bounds.m_mins.z);
		glVertex3f(bounds.m_mins.x, bounds.m_maxs.y, bounds.m_maxs.z);

		glVertex3f(bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_mins.z);
		glVertex3f(bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_maxs.z);

		glVertex3f(bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z);
		glVertex3f(bounds.m_maxs.x, bounds.m_mins.y, bounds.m_maxs.z);

		//Top Face, Counter Clockwise
		glVertex3f(bounds.m_mins.x, bounds.m_mins.y, bounds.m_maxs.z);
		glVertex3f(bounds.m_mins.x, bounds.m_maxs.y, bounds.m_maxs.z);


		glVertex3f(bounds.m_mins.x, bounds.m_maxs.y, bounds.m_maxs.z);
		glVertex3f(bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_maxs.z);
															  
		glVertex3f(bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_maxs.z);
		glVertex3f(bounds.m_maxs.x, bounds.m_mins.y, bounds.m_maxs.z);
															  
		glVertex3f(bounds.m_maxs.x, bounds.m_mins.y, bounds.m_maxs.z);
		glVertex3f(bounds.m_mins.x, bounds.m_mins.y, bounds.m_maxs.z);
	}
	glEnd();
	*/
}



void Renderer::GenerateBuffer(int numberofBufferObjects, GLuint* vboID)
{
	glGenBuffers(numberofBufferObjects, vboID);
}

void Renderer::BindBuffer(int vboID)
{
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
}

void Renderer::BufferData(GLsizeiptr size, const GLvoid *  data)
{
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

void Renderer::DeleteBuffers(int numberofBufferObjects, const GLuint* vboID)
{
	glDeleteBuffers(numberofBufferObjects, vboID);
}

void Renderer::DrawVBO_PCT(unsigned int vboID, int numVerts, PrimitiveType primitiveType)
{
	glPushMatrix();
	//glTranslatef(m_worldMins.x, m_worldMins.y, m_worldMins.z);
	glBindBuffer(GL_ARRAY_BUFFER,vboID);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(3, GL_FLOAT, sizeof(Vertex3D_PCT), (const GLvoid*)offsetof(Vertex3D_PCT, m_position));
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex3D_PCT), (const GLvoid*)offsetof(Vertex3D_PCT, m_color));
	glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex3D_PCT), (const GLvoid*)offsetof(Vertex3D_PCT, m_texCoords));

	switch (primitiveType)
	{
	case PRIMITIVE_POINTS:
		break;
	case PRIMITIVE_LINES:
		break;
	case PRIMITIVE_LINES_LOOP:
		break;
	case PRIMITIVE_TRIANGLES:
		break;
	case PRIMITIVE_QUADS:
	{
		glDrawArrays(GL_QUADS, 0, numVerts);
		break;
	}
	case NUM_PRIMITIVE_TYPES:
		break;
	default:
	{
		glDrawArrays(GL_QUADS, 0, numVerts);
		break;
	}
	}
	glDrawArrays(GL_QUADS, 0, numVerts);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glPopMatrix();
}


void Renderer::SetAlphaTest(bool alphaState, float threshold)
{
	if (alphaState)
	{
		glAlphaFunc(GL_GREATER, threshold);
		glEnable(GL_ALPHA_TEST);
	}
	else
	{
		glDisable(GL_ALPHA_TEST);
	}
}

void Renderer::HandleRenderCommands()
{
	//TODO draw command based on type

	for (auto commandIter = g_Commands.begin(); commandIter != g_Commands.end(); ++commandIter)
	{
		RenderCommand* command = *commandIter;
		

		RenderCommand::DrawMode mode = command->m_drawMode;

		switch (mode)
		{
		case RenderCommand::DEPTH_TEST_OFF:
		{
			SetDepthTest(false);
			DrawRenderCommands(command, 1.f);
			break;
		}
		case RenderCommand::DEPTH_TEST_ON:
		{
			SetDepthTest(true);
			DrawRenderCommands(command, 1.f);
			break;
		}
		case RenderCommand::XRAY:
		{
			SetDepthTest(false);
			DrawRenderCommands(command, 1.f);
			SetDepthTest(true);
			DrawRenderCommands(command, 3.f);
			break;
		}
		default:
			break;
		}

		
		
	}
}

void Renderer::DrawRenderCommands(const RenderCommand* command, float lineThickness)
{
	RenderCommand::CommandType type = command->m_type;
	switch (type)
	{
		case RenderCommand::POINT:
		{
			DrawPoint(command->m_startPostion, command->m_color);
			break;
		}
		case RenderCommand::LINE:
		{
			DrawLine(command->m_startPostion, command->m_endPosition, command->m_color, lineThickness);
			break;
		}
		case RenderCommand::ARROW:
		{
			DrawLine(command->m_startPostion, command->m_endPosition, command->m_color, lineThickness);
			DrawPoint(command->m_endPosition, command->m_color);
			break;
		}
		case RenderCommand::AABB3:
		{
			AABB3 bounds;
			bounds.m_mins = command->m_mins;
			bounds.m_maxs = command->m_maxs;
			DrawAABB3Wireframe(bounds, command->m_color);
			break;
		}
		case RenderCommand::SPHERE:
		{
			DrawSphere(command->m_startPostion, command->m_radius, command->m_color);
			break;
		}
	}
}

void Renderer::UpdateRenderCommands(const float deltaSeconds)
{
	std::vector<RenderCommand*>::iterator it = g_Commands.begin();

	while (it != g_Commands.end()) {
		if ((*it)->m_seconds < 0.f) //Run forever if negative value
		{
			++it;
		}
		else
		{
			(*it)->m_seconds -= deltaSeconds;
			if ((*it)->m_seconds <= 0.f)
			{
				delete(*it);
				it = g_Commands.erase(it);
			}
			else
			{
				++it;
			}
		}
		
	}
}

void Renderer::AddDebugPoint(Vector3 pos, Rgba color, const float seconds, RenderCommand::DrawMode drawMode)
{
	//TODO Create instance of Point RenderCommand and insert it into the vector
	RenderCommand* command = new RenderCommand();
	command->m_type = RenderCommand::POINT;
	command->m_startPostion = pos;
	command->m_color = color;
	command->m_seconds = seconds;
	command->m_drawMode = drawMode;
	g_Commands.push_back(command);
}


void Renderer::AddDebugLine(Vector3 startPos, Vector3 endPos, Rgba color, const float seconds, RenderCommand::DrawMode drawMode)
{
	RenderCommand* command = new RenderCommand();
	command->m_type = RenderCommand::LINE;
	command->m_startPostion = startPos;
	command->m_endPosition = endPos;
	command->m_color = color;
	command->m_seconds = seconds;
	command->m_drawMode = drawMode;
	g_Commands.push_back(command);
}


void Renderer::AddDebugArrow(Vector3 startPos, Vector3 endPos, Rgba color, const float seconds, RenderCommand::DrawMode drawMode)
{
	RenderCommand* command = new RenderCommand();
	command->m_type = RenderCommand::ARROW;
	command->m_startPostion = startPos;
	command->m_endPosition = endPos;
	command->m_color = color;
	command->m_seconds = seconds;
	command->m_drawMode = drawMode;
	g_Commands.push_back(command);
}

void Renderer::AddDebugAABB3(Vector3 mins, Vector3 maxs, Rgba color, const float seconds, RenderCommand::DrawMode drawMode)
{
	RenderCommand* command = new RenderCommand();
	command->m_type = RenderCommand::AABB3;
	command->m_mins = mins;
	command->m_maxs = maxs;
	command->m_color = color;
	command->m_seconds = seconds;
	command->m_drawMode = drawMode;
	g_Commands.push_back(command);
}

void Renderer::AddDebugSphere(Vector3 pos, float radius, Rgba color, const float seconds, RenderCommand::DrawMode drawMode)
{
	RenderCommand* command = new RenderCommand();
	command->m_type = RenderCommand::SPHERE;
	command->m_startPostion = pos;
	command->m_radius = radius;
	command->m_color = color;
	command->m_seconds = seconds;
	command->m_drawMode = drawMode;
	g_Commands.push_back(command);
}


GLuint Renderer::LoadShader(char const *filename, GLenum shader_type)
{
	std::vector<unsigned char> buffer;
	LoadBufferFromBinaryFile(buffer, filename);
	buffer.push_back('\0');

	GLuint shader_id = glCreateShader(shader_type);
	ASSERT_OR_DIE(shader_id != NULL, "You dun goofed.");

	GLint src_length = buffer.size();
	GLchar* bufferStart = (GLchar*)&buffer[0];
	glShaderSource(shader_id, 1, &bufferStart, &src_length);

	glCompileShader(shader_id);

	GLint status;
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &status);
	if (GL_FALSE == status)
	{
		GLint length;
		glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &length);

		char *errorBuffer = new char[length + 1];
		glGetShaderInfoLog(shader_id, length, &length, errorBuffer);

		errorBuffer[length] = NULL;
		std::string errorBufferStr(errorBuffer);

		std::stringstream errorStream(errorBufferStr);
		std::vector<std::string> errorLines;

		std::string line;
		while (std::getline(errorStream, line))
		{
			errorLines.push_back(line);
		}

		for (auto lineIter = errorLines.begin(); lineIter != errorLines.end(); ++lineIter)
		{
			if (lineIter->find("error") != std::string::npos)
			{
				//Error Found
				std::string prunedStr = "";
				std::string lineNumber = ExtractToken(*lineIter, "(", ")");
				lineNumber = std::to_string(std::stoi(lineNumber) - 1);
				std::string filePathStr(filename);

				std::string errorText = ExtractToken(*lineIter, ":", "");
				errorText = ExtractToken(errorText, ":", "");
				prunedStr += errorText;
				prunedStr += "\n";
				char temp[_MAX_PATH];
				std::string workingDirStr = _getcwd(temp, _MAX_PATH);
				filePathStr = workingDirStr + "\\" + filePathStr;
				DebuggerPrintf("%s(%d): %s", filePathStr.c_str(), std::stoi(lineNumber),
					prunedStr.c_str()); // Use this specific format so Visual Studio users can double-click to jump to file-and-line of error
										//DebuggerPrintf(prunedStr.c_str());
				const char* versionString = (const char*)glGetString(GL_VERSION);
				std::string oglVer = Stringf("OpenGL version: %s\n", versionString);
				const char* glslString = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
				std::string glslVer = Stringf("GLSL version: %s\n", glslString);
				ASSERT_OR_DIE(false, prunedStr + "\n In file: " + filePathStr + "\n\n" + errorBufferStr + "\n" + oglVer + "\n" + glslVer);
			}
			else
			{
				//Warnings
			}

		}

		//__FILE__, prune up to Code, append shader path
		
		delete errorBuffer;

		glDeleteShader(shader_id);
		return 0;
	}
	//ASSERT_OR_DIE(status == GL_TRUE, "You dun goofed.");

	return shader_id;
	
}

int Renderer::GenerateVboID()
{
	GLuint vboID;
	GenerateBuffer(1, &vboID);
	return (int)vboID;
}

GLuint Renderer::RenderBufferCreate(void *data, size_t count, size_t elem_size, GLenum usage)
{
	GLuint  buffer;
	glGenBuffers(1, &buffer);

	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, count * elem_size, data, usage);
	glBindBuffer(GL_ARRAY_BUFFER, NULL);

	return buffer;
}
/*
GLuint Renderer::UpdateVBO(void *data, size_t elem_size, int vboID)
{
	GLuint buffer = (GLuint)vboID;

	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, count * elem_size, data, usage);
	glBindBuffer(GL_ARRAY_BUFFER, NULL);

	return buffer;
}
*/
void Renderer::RenderBufferDestroy(GLuint buffer)
{
	glDeleteBuffers(1, &buffer);
}

GLuint Renderer::CreateShader(std::string const &filename, GLenum shader_type)
{

	std::vector<unsigned char> buffer;
	bool success = LoadBufferFromBinaryFile(buffer, filename);
	ASSERT_OR_DIE(success, "File not found.");
	//buffer.push_back(NULL);

	GLuint shaderID = glCreateShader(shader_type);
	ASSERT_OR_DIE(shaderID != NULL, "Shader failed to load.");

	GLint src_length = buffer.size();
	GLchar* bufferStart = (GLchar*)&buffer[0];
	glShaderSource(shaderID, 1, &bufferStart, &src_length);

	glCompileShader(shaderID);

	//Check for errors
	GLint status;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint length;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &length);

		std::string bufferError;
		bufferError.resize(length + 1);
		glGetShaderInfoLog(shaderID, length, &length, &bufferError[0]);

		OutputDebugStringA("\n===================================================================================================\n");
		OutputDebugStringA("ERROR COMPILING SHADER\n");

		char fullPath[_MAX_PATH];
		_fullpath(fullPath, &filename[0], _MAX_PATH);
		std::string parseBufferError = bufferError;
		bool cont = true;
		while (cont)
		{
			int checkIndex = parseBufferError.substr(1).find("ERROR:");
			std::string errorLine;

			//Last error
			if (checkIndex == -1)
			{
				cont = false;
				errorLine = parseBufferError;
			}
			else
			{
				errorLine = parseBufferError.substr(0, checkIndex);
			}
			int startIndex = parseBufferError.find("0:") + 2; //Make start after "0:"
			parseBufferError = parseBufferError.substr(startIndex);
			int endIndex = parseBufferError.find(":");
			std::string lineNumber = parseBufferError.substr(0, endIndex);

			//Prepare next parseBuffer
			if (cont)
			{
				int nextIndex = parseBufferError.find("ERROR:");
				parseBufferError = parseBufferError.substr(nextIndex);
			}

			std::string debugString = Stringf("%s(%s): %s\n", fullPath, &lineNumber[0], &errorLine[0]);
			OutputDebugStringA(&debugString[0]);
		}
		glDeleteShader(shaderID);

		std::string const openGLVersion = GetOpenGLVersion();
		std::string const openGLSLVersion = GetGLSLVersion();
		ShaderError(fullPath, bufferError, openGLVersion, openGLSLVersion);
		return 0;
	}

	return shaderID;
}

GLuint Renderer::CreateAndLinkProgram(GLuint vs, GLuint fs, std::string const &debugFilepath)
{
	//debugFilepath is only used if there is a link error
	//#TODO: Ask Forseth if he recommends a better way to get the file's path?

	GLuint programID = glCreateProgram();
	ASSERT_OR_DIE(programID != NULL, "No Program!");

	glAttachShader(programID, vs);
	glAttachShader(programID, fs);

	glLinkProgram(programID);

	//GetProgrami - will get integer
	//GetProgramiv - will get (multiple) integers
	GLint status;
	char fullPath[_MAX_PATH];
	_fullpath(fullPath, &debugFilepath[0], _MAX_PATH);
	glGetProgramiv(programID, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) //#TODO: Make this a function
	{
		//#TODO: Maybe make the logging of an error its own function
		GLint logLength;
		glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &logLength);

		std::string bufferError;
		bufferError.resize(logLength + 1);
		glGetProgramInfoLog(programID, logLength, &logLength, &bufferError[0]);

		std::string debugString = Stringf("%s: \n%s\n", fullPath, &bufferError[0]);
		OutputDebugStringA("\n===================================================================================================\n");
		OutputDebugStringA("ERROR LINKING PROGRAM\n");
		OutputDebugStringA(&debugString[0]);

		glDeleteProgram(programID);

		std::string const openGLVersion = GetOpenGLVersion();
		std::string const openGLSLVersion = GetGLSLVersion();
		ShaderError(fullPath, bufferError, openGLVersion, openGLSLVersion);
		return 0;
	}
	else
	{
		//Frees up shaders id, 
		glDetachShader(programID, vs);
		glDetachShader(programID, fs);
	}

	return programID;
}
GLuint Renderer::CreateRenderBuffer(void const *data, size_t count, size_t elem_size, GLenum const &usage /*= GL_STATIC_DRAW*/)
{
	
	GLuint buffer;
	glGenBuffers(1, &buffer); //Create

	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, count * elem_size, data, usage); //Fill

	glBindBuffer(GL_ARRAY_BUFFER, NULL); //Unbind

	return buffer; //Return generate ID
}
void Renderer::UpdateRenderBuffer( GLuint bufferID, void const *data, size_t count, size_t elem_size, GLenum const &usage /*= GL_STATIC_DRAW*/)
{
	glBindBuffer(GL_ARRAY_BUFFER, bufferID);
	glBufferData(GL_ARRAY_BUFFER, count * elem_size, data, usage); //Fill

	glBindBuffer(GL_ARRAY_BUFFER, NULL); //Unbind
}

void Renderer::BindMeshToVAO(GLuint vao, GLuint vbo, GLuint ibo, GLuint program)
{
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	ShaderProgramBindProperty(program, "inPosition",
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex3D_PCT), offsetof(Vertex3D_PCT, m_position));

	ShaderProgramBindProperty(program, "inColor",
		4,
		GL_UNSIGNED_BYTE,
		GL_TRUE,
		sizeof(Vertex3D_PCT), offsetof(Vertex3D_PCT, m_color));
	ShaderProgramBindProperty(program, "inUV0",
		2,
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex3D_PCT), offsetof(Vertex3D_PCT, m_texCoords));

	glBindBuffer(GL_ARRAY_BUFFER, NULL);

	if (NULL != ibo) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	}

	glBindVertexArray(NULL);
}

//-------------------------------------------------------------------------------------------------
void Renderer::BindMeshToVAO(GLuint vaoID, Mesh const *mesh, Material const *material)
{
	unsigned int vboID = mesh->GetVBOID();
	unsigned int iboID = mesh->GetIBOID();
	std::vector< VertexDefinition > layout = mesh->GetLayout();
	const std::map< size_t, Attribute* >& attributes = material->GetAttributeList();

	glBindVertexArray(vaoID);
	glBindBuffer(GL_ARRAY_BUFFER, vboID);

	//Bind Properties
	for (VertexDefinition vertexLayout : layout)
	{
		//Map Types
		GLenum type = GL_FLOAT;
		if (vertexLayout.m_type == VertexDataType_FLOAT)
		{
			type = GL_FLOAT;
		}
		else if (vertexLayout.m_type == VertexDataType_UBYTE)
		{
			type = GL_UNSIGNED_BYTE;
		}
		else if (vertexLayout.m_type == VertexDataType_INTEGER)
		{
			type = GL_UNSIGNED_INT;
		}

		//Map Boolean
		GLboolean normalize = vertexLayout.m_normalized ? GL_TRUE : GL_FALSE;
		size_t nameHash = std::hash<std::string>{}(vertexLayout.m_name);
		std::map< size_t, Attribute* >::const_iterator found = attributes.find(nameHash);
		if (found != attributes.end())
		{
			int bindPoint = found->second->GetBindPoint();
			if (type == GL_UNSIGNED_INT)
			{
				ShaderProgramBindPropertyInt(bindPoint, vertexLayout.m_count, type, vertexLayout.m_stride, vertexLayout.m_offset);
			}
			else
			{
				ShaderProgramBindProperty(bindPoint, vertexLayout.m_count, type, normalize, vertexLayout.m_stride, vertexLayout.m_offset);
			}
		}
		else
		{
			continue;
			//#TODO: Make sure I don't want to handle this
			//ASSERT_OR_DIE( found != attributes.end( ), Stringf( "Attribute %s doesn't exist.", &vertexLayout.m_name[0] ) );
		}
	}

	glBindBuffer(GL_ARRAY_BUFFER, NULL);

	if (iboID != NULL)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID);
	}

	//Unbinds the vertex array object
	glBindVertexArray(NULL);
}
//-------------------------------------------------------------------------------------------------
void Renderer::SetMatrixUniforms(MeshRenderer* setMeshRenderer)
{
	//Get Model View Projection Matrixes
	Matrix4 model = setMeshRenderer->GetModelMatrix();
	Matrix4 cameraView = g_camera.GetViewMatrix();
	Matrix4 cameraProj = g_camera.GetProjectionMatrix();

	//Set Model View Projection Matrixes
	setMeshRenderer->SetUniform("gModel", model);
	setMeshRenderer->SetUniform("gView", cameraView);
	setMeshRenderer->SetUniform("gProj", cameraProj);
}

void Renderer::RenderStep(const float deltaSeconds)
{
	deltaSeconds;
	/*
	static float m_age = 0.f;
	m_age += deltaSeconds;

	g_theRenderer->SetCullFace(true);
	g_theRenderer->SetDepthTest(true);

	//FBO STUFF
	

	//Vector3 lightPos = Vector3(cos(m_age) *2.f, sin(m_age), 0.f);
	//Vector3 lightPos = Vector3(cos(m_age), -1.f, cos(m_age) * sin(m_age));
	//Vector3 lightPos = Vector3(cos(m_age) * 1.5f, sin(m_age)  * 1.5f, cos(m_age) * sin(m_age) * 1.5f);
	Vector3 mainLightPos = g_camera.m_position;
	//Vector3 lightPos = Vector3(0.f,-1.f,0.f);
	glActiveTexture(GL_TEXTURE0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	

	mat44_fl model;
	mat44_fl view;
	mat44_fl proj;

	MatrixMakeIdentity(&model);
	MatrixMakeIdentity(&view);
	MatrixMakeIdentity(&proj);


	float aspect = 16.f/9.f;

	//PROJECTION MATRIX
	//MatrixMakeProjOrthogonal(&proj, 50.0f * aspect, 50.0f, 0.0f, 100.0f);
	MatrixMakePerspective(&proj, 70.f, aspect, 0.1f, 100.f);
	
	
	//MODEL MATRIX
	Vector3 trans = Vector3(0.f, 0.f, 0.f);
	Vector3 trans2 = Vector3(5.f, 0.f, 0.f);
	MatrixTranslate(&model, trans);
	//MatrixMakeRotationEuler(&model, 0.f,0.f,cos(m_age) * 50.f, trans);
	Vector3 transLight = mainLightPos + trans;
	//g_theRenderer->DrawSphere(transLight, 0.2f);

	int lightCount = 16;
	std::vector<Light> cubeLights;
	Vector4 ambientLight(1.0f, 1.0f, 1.0f, 0.1f);
	Vector4 lightColor(1.0f, 1.0f, 1.0f, 1.0f);
	Vector3 lightPosition = Vector3::ZERO;
	float lightIntensity = 2.f;

	if (g_lightState == 0)// Global Point Lights
	{
		Vector3 globalLightPos = Vector3(-1.f, -1.f, 5.f);
		lightColor = Vector4(1.f, 1.f, 0.f, 1.f);
		Light globalPointLight = Light(lightColor, globalLightPos, Vector3::ZERO, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, -1.f, 1.f, 1.f);
		g_theRenderer->DrawSphere(globalLightPos, 0.2f, Rgba::YELLOW);
		cubeLights.push_back(globalPointLight);
		lightCount = 1;
	}
	else if (g_lightState == 1) // Local Point Lights
	{
		for (int i = 0; i < lightCount; i++)
		{
			lightPosition = Vector3(cos(m_age + (2.0f * pi / 16.0f * i)), sin(m_age + (2.0f * pi / 16.0f * i)) - 0.1f, 0.0f);
			if (i == 0)
			{
				lightColor = Vector4(1.f, 0.f, 0.f, 1.f);
				g_theRenderer->DrawSphere(lightPosition, 0.2f, Rgba::RED);

			}
			else if (i == 4)
			{
				lightColor = Vector4(0.f, 1.f, 0.f, 1.f);
				g_theRenderer->DrawSphere(lightPosition, 0.2f, Rgba::GREEN);
			}
			else if (i == 8)
			{
				lightColor = Vector4(0.f, 0.f, 1.f, 1.f);
				g_theRenderer->DrawSphere(lightPosition, 0.2f, Rgba::BLUE);
			}
			else if (i == 12)
			{
				lightColor = Vector4(1.f, 0.f, 0.f, 1.f);
				g_theRenderer->DrawSphere(lightPosition, 0.2f, Rgba::YELLOW);
			}
			else
			{
				lightColor = Vector4(0.f, 0.f, 0.f, 1.f);
				g_theRenderer->DrawSphere(lightPosition, 0.2f, Rgba::BLACK);
			}
			cubeLights.push_back(Light(lightColor, lightPosition, 1.f));

		}
	}
	else if (g_lightState == 2) //Spot Light on Camera
	{
		lightColor = Vector4(1.f, 1.f, 1.f, 1.f);
		Vector3 camDir = g_camera.GetForwardXYZ().GetNormalized();
		Light globalPointLight = Light(lightColor, mainLightPos, camDir,
			1.f, 6.f, 1.f, 0.f, 
			0.f, 
			CosDegrees(5.f), CosDegrees(15.f), 1.f, 0.f);
		
		cubeLights.push_back(globalPointLight);
		lightCount = 1;
	}
	else if (g_lightState == 3) //Global Spot Light
	{
		lightColor = Vector4(1.f, 0.f, 1.f, 1.f);
		Vector3 globalSpotLightPos = Vector3(5.f, 0.f, 6.f);
		Vector3 globalSpotLightPosDir = Vector3(0.f,0.f,-1.f);
		Light globalPointLight = Light(lightColor, globalSpotLightPos, globalSpotLightPosDir,
			1.f, 6.f, 1.f, 1.f,
			0.f,
			CosDegrees(5.f), CosDegrees(15.f), 1.f, 0.f);

		cubeLights.push_back(globalPointLight);
		g_theRenderer->DrawSphere(globalSpotLightPos, 0.2f, Rgba::PINK);
		lightCount = 1;
	}
	else if (g_lightState == 4) //Global Direct Light
	{
		lightColor = Vector4(1.f, 1.f, 0.f, 1.f);
		Vector3 globalSpotLightPos = Vector3(5.f, 0.f, 6.f);
		Vector3 globalSpotLightPosDir = Vector3(-1.f, 0.f, -1.f);
		Light globalPointLight = Light(lightColor, Vector3::ZERO, globalSpotLightPosDir,
			1.f, 6.f, 1.f, 1.f,
			1.f,
			1.f, -1.f, 1.f, 1.f);

		cubeLights.push_back(globalPointLight);
		
		lightCount = 1;
	}

	//lightPosition = Vector3(cos(m_age), sin(m_age) - 0.1f, 0.0f);

	
	
	

	//VIEW MATRIX
	//MatrixMakeBasis(&view, Vector3(1.f,0.f,0.f), Vector3(0.f,0.f,1.f), Vector3(0.f,1.f,0.f));
	MatrixMakeRotationEuler(&view, g_camera.m_orientation.m_yawDegreesAboutZ, g_camera.m_orientation.m_pitchDegreesAboutX, 
		g_camera.m_orientation.m_rollDegreesAboutY, g_camera.m_position);
	MatrixForsethCOBNick(&view);

	MatrixInvertOrthonormal(&view);
	



	m_testMaterial->SetUniform("gModel", &model);
	m_testMaterial->SetUniform("gView", &view);
	m_testMaterial->SetUniform("gProj", &proj);

// 	m_cubeMapMaterial->SetUniform("gModel", &model);
// 	m_cubeMapMaterial->SetUniform("gView", &view);
// 	m_cubeMapMaterial->SetUniform("gProj", &proj);

	




	m_testMaterial->BindTexture("gDiffuseTex", &gDiffuseTex, 0);
	m_testMaterial->BindTexture("gNormalTex", &gNormalTex, 1);
	m_testMaterial->BindTexture("gSpecularTex", &gSpecularTex, 2);
	m_testMaterial->BindTexture("gEmissiveTex", &gEmissiveTex, 3);
	m_testMaterial->BindTexture("gDissolveTex", &gDissolveTex, 4);

	int textureObj = m_cubeMap->GetTextureObj();
	//m_cubeMapMaterial->BindTexture("gCubeMap", &textureObj, 0);

	
	//m_testMaterial->BindTextures();




	float specPower = 100.f;
	float minDistToCamera = 5.f;
	float maxDistToCamera = 10.f;
	//vec4_fl ambientLight = vec4_fl(1.f, 1.f, 1.f, 0.1f);
	//vec4_fl lightColor = vec4_fl(1.f, 1.f, 1.f, 1.f);
	vec4_fl fogColor = vec4_fl(0.5f, 0.5f, 0.5f, 1.f);
	//float age = ClampFloat(cos(m_age),0.f,1.f);
	float age = abs(cos(m_age));
	


// 	m_testMaterial->SetUniform("gLightPosition", &transLight);
// 	m_testMaterial->SetUniform("gLightIntensity", &lightIntensity);
// 	m_testMaterial->SetUniform("gLightColor", &lightColor);
	
	m_testMaterial->SetLights(cubeLights);
	m_testMaterial->SetUniform("gLightCount", &lightCount);

	m_testMaterial->SetUniform("gAmbientLight", &ambientLight);
	m_testMaterial->SetUniform("gSpecularPower", &specPower);
	m_testMaterial->SetUniform("gSpecularIntensity", &lightIntensity);
	m_testMaterial->SetUniform("gCameraPosition", &g_camera.m_position);
	m_testMaterial->SetUniform("g_minDistToCamera", &minDistToCamera);
	m_testMaterial->SetUniform("g_maxDistToCamera", &maxDistToCamera);
	m_testMaterial->SetUniform("gFogColor", &fogColor);
	m_testMaterial->SetUniform("gAge", &age);



	//ShaderProgramSetInt(gShaderProgram, "gDiffuseTex",tex_index);
// 	ShaderProgramSetInt(gShaderTimedProgram, "gEffectState", g_effectState);
// 	ShaderProgramSetInt(gShaderTimedProgram, "gDiffuseTex", tex_index);
// 	ShaderProgramSetFloat(gShaderTimedProgram, "gEffectTime", m_age);

// 	glBindVertexArray(gVAO);
// 	glUseProgram(gShaderProgram);


// 	m_fboToDrawWith->SetUniform("gModel", &model);
// 	m_fboToDrawWith->SetUniform("gView", &view);
// 	m_fboToDrawWith->SetUniform("gProj", &proj);
//	m_screenQuad->DrawWithMaterial(m_fboToDrawWith);
	m_cube->DrawWithMaterial(m_testMaterial);
	MatrixTranslate(&model, trans2);
	m_testMaterial->SetUniform("gModel", &model);
	m_sphere->DrawWithMaterial(m_testMaterial);

	//m_cubeMapMesh->DrawWithMaterial(m_cubeMapMaterial);

	/*
	glBindVertexArray(m_cubeVAO);
	glUseProgram(gShaderProgram);
	glDrawElements(GL_TRIANGLES, m_cube->m_indices.size(), GL_UNSIGNED_SHORT, (GLvoid*)0);
	*/

	glUseProgram(NULL);
	glBindVertexArray(NULL);

// 	glBindVertexArray(m_sphereVAO);
// 	glUseProgram(gShaderTimedProgram);
// 	glDrawElements(GL_TRIANGLES, m_sphere->m_indices.size(), GL_UNSIGNED_SHORT, (GLvoid*)0);

	glUseProgram(NULL);
	glBindVertexArray(NULL);

	//glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (GLvoid*)0);
	//glDrawArrays(GL_TRIANGLES, 0, 3);

	glUseProgram(NULL);
	glBindVertexArray(NULL);
	glActiveTexture(GL_TEXTURE0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//FBO STUFF
	


	//g_theRenderer->DrawSphere(lightPos, 0.2f);
	//g_theRenderer->DrawSphere(lightPos, 0.2f);
}

void Renderer::RenderCleanup()
{
// 	glDeleteProgram(gShaderProgram);
// 	glDeleteVertexArrays(1, &gVAO);
// 
// 	glDeleteBuffers(1, &gVBO);
	//DeleteVertexBuffer(gVBO);
}

void Renderer::ShaderProgramBindProperty(GLuint program, const char *name, GLint count, GLenum type, GLboolean normalize, GLsizei stride, GLsizei offset)
{
	GLint pos_bind = glGetAttribLocation(program, name);
	if (pos_bind >= 0)
	{
		glEnableVertexAttribArray(pos_bind);
		glVertexAttribPointer(pos_bind, count, type, normalize, stride, (GLvoid*) offset);
	}
}

//-------------------------------------------------------------------------------------------------
void Renderer::ShaderProgramBindProperty(int bindPoint, GLint count, GLenum type, GLboolean normalize, GLsizei stride, GLsizei offset)
{
	//Position to bind
	if (bindPoint >= 0)
	{
		glEnableVertexAttribArray(bindPoint);
		glVertexAttribPointer(
			bindPoint,      //Bind point to Shader
			count,        //Number of data elements passed
			type,        //type of data
			normalize,      //if we're passing in normals (it'll now normalize for us)
			stride,        //Size of the stride between elements
			(GLvoid*)offset    //Pointer to data's location in class
			);
	}
	else
	{
		//#TODO: Make Assert and continue
		//ASSERT_OR_DIE( posBind >= 0, "No bind point." );
	}
}

//-------------------------------------------------------------------------------------------------
void Renderer::ShaderProgramBindPropertyInt(int bindPoint, GLint count, GLenum type, GLsizei stride, GLsizei offset)
{
	//Position to bind

	if (bindPoint >= 0)
	{
		glEnableVertexAttribArray(bindPoint);
		glVertexAttribIPointer(
			bindPoint,      //Bind point to Shader
			count,        //Number of data elements passed
			type,        //type of data
			stride,        //Size of the stride between elements
			(GLvoid*)offset    //Pointer to data's location in class
			);
	}
	else
	{
		//#TODO: Make Assert and continue
		//ASSERT_OR_DIE( posBind >= 0, "No bind point." );
	}
}
bool Renderer::ShaderProgramSetVec4(GLuint shader, const char *name, const vec4_fl &v)
{
	glUseProgram(shader);
	GLint loc = glGetUniformLocation(shader, name);
	if (loc >= 0)
	{
		glUniform4fv(loc, 1, (GLfloat*)&v);
		return true;
	}
	return false;
}

void Renderer::MatrixMakeIdentity(mat44_fl *mat)
{
	float data[] = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	memcpy(mat->data, data, sizeof(data));
}

bool Renderer::ShaderProgramSetMat4(GLuint shader, const char *name, const Matrix4 &v)
{
	glUseProgram(shader);
	GLint loc = glGetUniformLocation(shader, name);
	if (loc >= 0)
	{
		glUniformMatrix4fv(loc, 1, GL_TRUE, (GLfloat*)&v);
		//glUniformMatrix4fv(loc, 1, GL_FALSE, (GLfloat*)&v);
		return true;
	}

	return false;
}

//------------------------------------------------------------------------
//
void Renderer::MatrixMakeProjOrthogonal(mat44_fl *mat, float nx, float fx, float ny, float fy, float nz, float fz)
{
	float sx = 1.0f / (fx - nx);
	float sy = 1.0f / (fy - ny);
	float sz = 1.0f / (fz - nz);

	float const values[] = {
		2.0f * sx,  0.0f,       0.0f,       -(fx + nx) * sx,
		0.0f,       2.0f * sy,  0.0f,       -(fy + ny) * sy,
		0.0f,       0.0f,       2.0f * sz,  0.0f,
		0.0f,       0.0f,      -(fz + nz) * sz,       1.0f,
	};

	memcpy(mat->data, values, sizeof(values));
}

void Renderer::MatrixMakeProjOrthogonal(mat44_fl *mat, float width, float height, float nz, float fz)
{
	float sz = 1.0f / (fz - nz);

	float const values[] = {
		2.0f / width,  0.0f,       0.0f,        0,
		0.0f,       2.0f / height,  0.0f,        0,
		0.0f,       0.0f,       2.0f * sz,  0.0f,
		0.0f,       0.0f,       -(fz + nz) * sz,       1.0f,
	};

	memcpy(mat->data, values, sizeof(values));
}

bool Renderer::ShaderProgramSetInt(GLuint shader, const char *name, const int &v)
{
	glUseProgram(shader);
	GLint loc = glGetUniformLocation(shader, name);
	if (loc >= 0)
	{
		glUniform1iv(loc, 1, (GLint*)&v);
		return true;
	}

	return false;
}

GLuint Renderer::CreateSampler(GLenum min_filter, GLenum mag_filter, GLenum u_wrap, GLenum v_wrap)
{
	GLuint id;
	glGenSamplers(1, &id);

	glSamplerParameteri(id, GL_TEXTURE_MIN_FILTER, min_filter);
	glSamplerParameteri(id, GL_TEXTURE_MAG_FILTER, mag_filter);
	glSamplerParameteri(id, GL_TEXTURE_WRAP_S, u_wrap);
	glSamplerParameteri(id, GL_TEXTURE_WRAP_T, v_wrap);

	return id;
}

/*
void Renderer::MatrixMakePerspective(mat44_fl* mat, float fov_degrees, float aspect, float nz, float fz)
{
	float rads = DegToRad(fov_degrees);
	//float size = atan(rads / 2.f);
	float size = 1.0f / tan(rads / 2.f);

	float w = size;
	float h = size;
	if (aspect > 1.f)
	{
		w /= aspect;
	}
	else {
		h *= aspect;
	}

	float q = 1.f / (fz - nz);

	//ROW MAJOR
	float const values[] = {
		w, 0.f, 0.0f, 0.0f,
		0.0f, h, 0.0f, 0.0f,
		0.0f, 0.0f, (fz + nz) * q, 1.f,
		0.0f, 0.0f, -2.f * nz * fz * q, 0.0f,
	};


	memcpy(mat->data, values, sizeof(values));
}
*/

void Renderer::MatrixTranslate(mat44_fl* mat, Vector3 translation)
{
	mat->data[12] = translation.x;
	mat->data[13] = translation.y;
	mat->data[14] = translation.z;

	//memcpy(mat->data, values, sizeof(values));
}

void Renderer::MatrixInvertOrthonormal(mat44_fl *mat)
{
	//implement
	mat44_fl matRotation = *mat;

	//Clearing the sides
	matRotation.data[3] = 0.f;
	matRotation.data[7] = 0.f;
	matRotation.data[11] = 0.f;
	
	matRotation.data[12] = 0.f;
	matRotation.data[13] = 0.f;
	matRotation.data[14] = 0.f;
	matRotation.data[15] = 1.f;

	//Invert Rotation

	MatrixTransposeRotation(&matRotation);

	//Get Translation and Invert
	Vector3 translation = MatrixGetOffset(mat);
	translation = -translation;
	mat44_fl matTrans = mat44_fl::identity;
	MatrixSetOffset(&matTrans, translation);

	//(R * T)^-1 = T^-1 * R^1
	MatrixMultiply(mat, &matTrans, &matRotation);
	//*mat = matTrans * matRotation;

// 	std::swap(mat->data[1], mat->data[4]);
// 	std::swap(mat->data[2], mat->data[8]);
// 	std::swap(mat->data[6], mat->data[9]);
// 
// 	
// 	mat->data[12] = -mat->data[12];
// 	mat->data[13] = -mat->data[13];
// 	mat->data[14] = -mat->data[14];
}

void Renderer::MatrixMakeRotationEuler(mat44_fl *mat, float const yaw, float const pitch, float const roll, Vector3 const &o)
{
	// yaw = y
	// pitch = x
	// roll = z

	float sx = sin(DegToRad(pitch));
	float cx = cos(DegToRad(pitch));

	float sy = sin(DegToRad(yaw));
	float cy = cos(DegToRad(yaw));

	float sz = sin(DegToRad(roll));
	float cz = cos(DegToRad(roll));

	/*
	float const values[] = {
		cy*cz + sx*sy*sz,	cy*sz - cz*sx*sy, cx*sy, o.x,
		-cx*sz,				cx*cz,			  sx,	 o.y,
		-cz*sy + cy*sx*sz, -sy*sz - cy*cz*sx, cx*cy, o.z,
		0.f,0.f, 0.f, 1.f
	};*/

	float const values[] = {
		cy*cz + sx*sy*sz,	-cx*sz,	-cz*sy + cy*sx*sz , 0.f,
		cy*sz - cz*sx*sy,	cx*cz,	-sy*sz - cy*cz*sx,	0.f,
		cx*sy,				sx ,	cx*cy,				0.f,
		o.x,				o.z,	o.y,				1.f
	};

	/*
	float const values[] = {
	cy*cz,	cz * sx * sy - cx * sz ,cx * cz * sy + sx * sz , 0.f,
	cy * sz ,	cx * cz + sx * sy * sz,	-cz * sx + cx * sy * sz,	0.f,
	-sy,				cy * sx ,	cx * cy,				0.f,
	o.x,				o.z,	o.y,				1.f
	};
	*/
	memcpy(mat->data, values, sizeof(values));
}

void Renderer::MatrixMakeBasis(mat44_fl* mat, const Vector3& right, const Vector3& up, const Vector3& forward)
{
	mat->data[0] = right.x;
	mat->data[1] = right.y;
	mat->data[2] = right.z;

	mat->data[4] = up.x;
	mat->data[5] = up.y;
	mat->data[6] = up.z;

	mat->data[8] = forward.x;
	mat->data[9] = forward.y;
	mat->data[10] = forward.z;
}

void Renderer::MatrixMakeLookAt(mat44_fl* mat, const Vector3& pos, const Vector3& up, const Vector3& target)
{
	Vector3 dir = target - pos;
	dir.SetNormalized();
	Vector3 normUp = up.GetNormalized();
	Vector3 right = CrossProduct(dir, normUp);
	right.SetNormalized();
	Vector3 lup = CrossProduct(right, dir);//local up


	mat->data[0] = right.x;
	mat->data[1] = right.y;
	mat->data[2] = right.z;

	mat->data[4] = lup.x;
	mat->data[5] = lup.y;
	mat->data[6] = lup.z;

	mat->data[8]  = -dir.x;
	mat->data[9]  = -dir.y;
	mat->data[10] = -dir.z;


// 	mat->data[4] = right.x;
// 	mat->data[5] = right.y;

// 	mat->data[12] = pos.x;
// 	mat->data[13] = pos.y;
// 	mat->data[14] = pos.z;
}

void Renderer::MatrixForsethCOBNick(mat44_fl* mat)
{
	//temp = y
	float yCol0 = mat->data[1];
	float yCol1 = mat->data[5];
	float yCol2 = mat->data[9];
	float yCol3 = mat->data[13];

	//y = z
	mat->data[1] = mat->data[2];
	mat->data[5] = mat->data[6];
	mat->data[9] = mat->data[10];
	mat->data[13] = mat->data[14];

	//z = temp
	mat->data[2] = yCol0;
	mat->data[6] = yCol1;
	mat->data[10] = yCol2;
	mat->data[14] = yCol3;
}
/*
void Renderer::MatrixTransposeRotation(mat44_fl* mat)
{
	//temp = y
	float item0 = mat->data[1];
	float item1 = mat->data[2];
	float item2 = mat->data[6];

	//y = z
	mat->data[1] = mat->data[4];
	mat->data[2] = mat->data[8];
	mat->data[6] = mat->data[9];

	//z = temp
	mat->data[4] = item0;
	mat->data[8] = item1;
	mat->data[9] = item2;
}
*/
bool Renderer::ShaderProgramSetFloat(GLuint shader, const char* name, const float &v)
{
	glUseProgram(shader);
	GLint loc = glGetUniformLocation(shader, name);
	if (loc >= 0)
	{
		glUniform1fv(loc, 1, (GLfloat*)&v);
		return false;
	}

	return false;
}


bool Renderer::ShaderProgramSetVec3(GLuint shader, const char* name, Vector3 &v)
{
	glUseProgram(shader);
	GLint loc = glGetUniformLocation(shader, name);
	if (loc >= 0)
	{
		glUniform3fv(loc, 1, (GLfloat*)&v);
		return true;
	}
	return false;
}

void Renderer::FramebufferBind(Framebuffer *fbo)
{
	
	if (m_active_fbo == fbo)
	{
		return;
	}
	

	m_active_fbo = fbo;
	if (fbo == nullptr)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, NULL);
		//TODO Reset view resolution
		glViewport(0, 0, 1600, 900);
		//glViewport(0, 0, Renderer::GetScreenWidth, Render::GetScreenHeight);
	}
	else
	{
		glBindFramebuffer(GL_FRAMEBUFFER, fbo->m_id);
		glViewport(0, 0, fbo->m_pixel_width, fbo->m_pixel_height);

		GLenum render_targets[32];
		for (uint32_t i = 0; i < fbo->m_color_targets.size(); ++i)
		{
			render_targets[i] = GL_COLOR_ATTACHMENT0 + i;
		}
		glDrawBuffers(fbo->m_color_targets.size(), render_targets);
	}
}

void Renderer::FramebufferCopyToBack(Framebuffer *fbo)
{
	if (fbo == nullptr)
	{
		return;
	}

	GLuint fboh = fbo->m_id;
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fboh);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, NULL);

	uint32_t read_width = fbo->m_pixel_width;
	uint32_t read_height = fbo->m_pixel_height;

	uint32_t draw_width = 1600;
	uint32_t draw_height = 900;

	glBlitFramebuffer(0, 0,
		read_width, read_height,
		0, 0,
		draw_width, draw_height,
		GL_COLOR_BUFFER_BIT,
		GL_NEAREST);
}

void Renderer::RenderPostProcess(Framebuffer *fbo)
{
	fbo;
	//Quad
	//(-1,-1,0.0)  -> (1,1,0)
	mat44_fl model;
	mat44_fl view;
	mat44_fl proj;

	MatrixMakeIdentity(&model);
	MatrixMakeIdentity(&view);
	MatrixMakeIdentity(&proj);
	/*
	m_fboToDrawWith->SetUniform("gModel", &model);
	m_fboToDrawWith->SetUniform("gView", &view);
	m_fboToDrawWith->SetUniform("gProj", &proj);

	m_fboToDrawWith->BindTexture("gDiffuseTex", fbo->color_targets[0], 0);
	m_fboToDrawWith->BindTexture("gDepthTex", fbo->depth_stencil_target, 1);
	if (m_fboToDrawWith == m_fboNightVisionMaterial)
	{
		g_theRenderer->m_fboNightVisionMaterial->BindTexture("gMaskTex", &gMaskTex, 2);
		g_theRenderer->m_fboNightVisionMaterial->BindTexture("gNoiseTex", &gNoiseTex, 3);
	}


	m_screenQuad->DrawWithMaterial(m_fboToDrawWith);

	for (int i = 0; i < 16; i++)
	{
		//m_fboToDrawWith->BindTexture("gDiffuseTex", fbo->color_targets[0], i);
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, NULL);
	}


	glUseProgram(NULL);
	glBindVertexArray(NULL);
	glActiveTexture(GL_TEXTURE0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	*/
	
}

//-------------------------------------------------------------------------------------------------
std::string Renderer::GetOpenGLVersion() const
{
	char* openGLVersion = (char*)glGetString(GL_VERSION);
	return std::string(openGLVersion);
}


//-------------------------------------------------------------------------------------------------
std::string Renderer::GetGLSLVersion() const
{
	char* openGLSLVersion = (char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
	return std::string(openGLSLVersion);
}

void Renderer::MeshRender(MeshRenderer const *meshRenderer, Camera3D const *camera)
{
	//Set Render State
	ApplyRenderState(meshRenderer->GetRenderState());

	//Get Model View Projection Matrixes
	Matrix4 model = meshRenderer->GetModelMatrix();
	Matrix4 cameraView = camera->GetViewMatrix();
	Matrix4 cameraProj = camera->GetProjectionMatrix();

	//Set Model View Projection Matrixes
	unsigned int gpuProgramID = meshRenderer->GetGPUProgramID();
	ShaderProgramSetMat4(gpuProgramID, "gModel", model);
	ShaderProgramSetMat4(gpuProgramID, "gView", cameraView);
	ShaderProgramSetMat4(gpuProgramID, "gProj", cameraProj);

	MeshRender(meshRenderer);
}

void Renderer::MeshRender(MeshRenderer const *meshRenderer)
{

	g_theProfiler->StartProfilerSample("Renderer: MeshRender");
	unsigned int gpuProgramID = meshRenderer->GetGPUProgramID();
	//Set Uniforms
	GLuint samplerID = meshRenderer->GetSamplerID();
	//Set shader program to bind everything to
	glUseProgram(gpuProgramID);

	//----------------------------------------------------------------------------------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------------------------------------------------------------------------------
	// MAKE THIS MAP RECIEVE BY CONST & | OR YOU ARE FUCKED
	//----------------------------------------------------------------------------------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------------------------------------------------------------------------------

	const std::map<size_t, Uniform*>& materialUniforms = meshRenderer->GetMaterialUniformList();
	ShaderProgramSetUniforms(gpuProgramID, samplerID, materialUniforms);
	const std::map<size_t, Uniform*>& meshRendererUniforms = meshRenderer->GetUniformList();
	ShaderProgramSetUniforms(gpuProgramID, samplerID, meshRendererUniforms);

	//Reset active Texture to 0 ("unbinding")
	glActiveTexture(GL_TEXTURE0 + 0); //#TODO: Remove when everything uses shaders to draw?

									  //Attach Vertex Array Object and Shader Program
	unsigned vaoID = meshRenderer->GetVAOID();
	glBindVertexArray(vaoID);
	glUseProgram(gpuProgramID);

	//Draw Vertexes
	unsigned int iboID = meshRenderer->GetIBOID();
	if (meshRenderer->GetDrawInstructions().size() > 0)
	{
		for (DrawInstruction instruction : meshRenderer->GetDrawInstructions())
		{
			if (!instruction.m_useIndexBuffer)
			{
				glDrawArrays(instruction.m_primitiveType, instruction.m_startIndex, instruction.m_count);
			}
			else
			{
				glDrawElements(instruction.m_primitiveType, instruction.m_count, GL_UNSIGNED_SHORT, (GLvoid*)0);
			}
		}
	}
	else
	{
		if (iboID == 0)
		{
			int numOfVerts = meshRenderer->GetVertCount();
			glDrawArrays(GL_TRIANGLES, 0, numOfVerts);
		}
		else
		{
			int indexBufferCount = meshRenderer->GetIndexBufferCount();
			glDrawElements(GL_TRIANGLES, indexBufferCount, GL_UNSIGNED_SHORT, (GLvoid*)0);
		}
	}



	//Remove and Clear
	glUseProgram(NULL);
	glBindVertexArray(NULL);
	g_theProfiler->EndProfilerSample();
}

void Renderer::ApplyRenderState(RenderState const &renderState)
{
	SetCullFace(renderState.m_backfaceCullingEnabled);
	SetDepthWrite(renderState.m_depthWritingEnabled);
	SetDepthTest(renderState.m_depthTestingEnabled);
	SetBlending(renderState.m_blendingMode);
	SetDrawMode(renderState.m_drawMode);
	SetWindingOrder(renderState.m_windingClockwise);
}

void Renderer::MeshRenderPPFX(MeshRenderer * meshRenderer)
{
	//Set Render State
	ApplyRenderState(meshRenderer->GetRenderState());

	//Get Model View Projection Matrixes
	Matrix4 model = meshRenderer->GetModelMatrix();
	Matrix4 cameraView = Matrix4::IDENTITY;
	Matrix4 cameraProj = Matrix4::IDENTITY;

	//Set Model View Projection Matrixes
	unsigned int gpuProgramID = meshRenderer->GetGPUProgramID();
	ShaderProgramSetMat4(gpuProgramID, "gModel", model);
	ShaderProgramSetMat4(gpuProgramID, "gView", cameraView);
	ShaderProgramSetMat4(gpuProgramID, "gProj", cameraProj);

	//Set Uniforms
	GLuint samplerID = meshRenderer->GetSamplerID();
	//Set shader program to bind everything to
	glUseProgram(gpuProgramID);
	const std::map<size_t, Uniform*>& materialUniforms = meshRenderer->GetMaterialUniformList();
	ShaderProgramSetUniforms(gpuProgramID, samplerID, materialUniforms);
	const std::map<size_t, Uniform*>& meshRendererUniforms = meshRenderer->GetUniformList();
	ShaderProgramSetUniforms(gpuProgramID, samplerID, meshRendererUniforms);

	//Reset active Texture to 0 ("unbinding")
	glActiveTexture(GL_TEXTURE0 + 0); //#TODO: Remove when everything uses shaders to draw?

									  //Attach Vertex Array Object and Shader Program
	unsigned vaoID = meshRenderer->GetVAOID();
	glBindVertexArray(vaoID);
	glUseProgram(gpuProgramID);

	//Draw Vertexes
	unsigned int iboID = meshRenderer->GetIBOID();
	if (iboID == 0)
	{
		int numOfVerts = meshRenderer->GetVertCount();
		glDrawArrays(GL_TRIANGLES, 0, numOfVerts);
	}
	else
	{
		int indexBufferCount = meshRenderer->GetIndexBufferCount();
		glDrawElements(GL_TRIANGLES, indexBufferCount, GL_UNSIGNED_SHORT, (GLvoid*)0);
	}


	//Remove and Clear
	glUseProgram(NULL);
	glBindVertexArray(NULL);
}

void Renderer::SetCullFace(bool enable)
{
	//No redundant actions
// 	if (m_currentRenderState.m_backfaceCullingEnabled == enable)
// 		return;
	m_currentRenderState.m_backfaceCullingEnabled = enable;

	if (enable)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);
}


//-------------------------------------------------------------------------------------------------
void Renderer::SetDepthTest(bool enable)
{
	//No redundant actions
// 	if (m_currentRenderState.m_depthTestingEnabled == enable)D
// 		return;
	m_currentRenderState.m_depthTestingEnabled = enable;

	if (enable)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
}


//-------------------------------------------------------------------------------------------------
void Renderer::SetDepthWrite(bool enable)
{
	//No redundant actions
// 	if (m_currentRenderState.m_depthWritingEnabled == enable)
// 		return;
	m_currentRenderState.m_depthWritingEnabled = enable;

	if (enable)
	{
		glDepthMask(GL_TRUE);
	}
	else
	{
		glDepthMask(GL_FALSE);
	}
}


//-------------------------------------------------------------------------------------------------
void Renderer::EnableAlphaTesting(float threshold /*= 0.5f */)
{
	glAlphaFunc(GL_GREATER, threshold);
	glEnable(GL_ALPHA_TEST);
}


//-------------------------------------------------------------------------------------------------
void Renderer::DisableAlphaTesting()
{
	glDisable(GL_ALPHA_TEST);
}


//-------------------------------------------------------------------------------------------------
void Renderer::SetBlending(const Blending& setBlendingMode)
{
	//No redundant actions
// 	if (m_currentRenderState.m_blendingMode == setBlendingMode)
// 		return;
	m_currentRenderState.m_blendingMode = setBlendingMode;

	if (setBlendingMode == Blending_NORMAL)
	{
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else if (setBlendingMode == Blending_ADDITIVE)
	{
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	}
	else if (setBlendingMode == Blending_SUBTRACTIVE)
	{
		glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);
	}
	else if (setBlendingMode == Blending_INVERTED)
	{
		glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
	}
}


//-------------------------------------------------------------------------------------------------
void Renderer::SetDrawMode(DrawMode const &drawMode)
{
	//No redundant actions
// 	if (m_currentRenderState.m_drawMode == drawMode)
// 		return;
	m_currentRenderState.m_drawMode = drawMode;

	if (drawMode == DrawMode_FULL)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	else if (drawMode == DrawMode_LINE)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else if (drawMode == DrawMode_POINT)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}
}


//-------------------------------------------------------------------------------------------------
void Renderer::SetWindingOrder(bool clockwise)
{
	//No redundant actions
// 	if (m_currentRenderState.m_windingClockwise == clockwise)
// 		return;
	m_currentRenderState.m_windingClockwise = clockwise;

	if (clockwise)
	{
		glCullFace(GL_FRONT);
	}
	else
	{
		glCullFace(GL_BACK);
	}
}




//-------------------------------------------------------------------------------------------------
void Renderer::SetLineWidth(float size)
{
	//No redundant changes
	if (m_currentLineWidth == size)
		return;

	m_currentLineWidth = size;
	glLineWidth(m_currentLineWidth);
}

//-------------------------------------------------------------------------------------------------
void Renderer::ShaderProgramSetUniforms(unsigned int gpuProgramID, GLuint samplerID, std::map<size_t, Uniform*> const &uniformList)
{
	GLuint textureID = NULL;
	unsigned int textureIndexPort = 0;
	for (auto uniformIter = uniformList.begin(); uniformIter != uniformList.end(); ++uniformIter)
	{
		Uniform* currentUniform = uniformIter->second;
		if (currentUniform == nullptr)
			continue;
		switch (currentUniform->m_type)
		{
		case GL_INT:
			glUniform1iv(currentUniform->m_bindPoint, currentUniform->m_size, (GLint*)currentUniform->m_data);
			break;
		case GL_FLOAT:
			glUniform1fv(currentUniform->m_bindPoint, currentUniform->m_size, (GLfloat*)currentUniform->m_data);
			break;
		case GL_FLOAT_VEC2:
			glUniform2fv(currentUniform->m_bindPoint, currentUniform->m_size, (GLfloat*)currentUniform->m_data);
			break;
		case GL_FLOAT_VEC3:
			glUniform3fv(currentUniform->m_bindPoint, currentUniform->m_size, (GLfloat*)currentUniform->m_data);
			break;
		case GL_FLOAT_VEC4:
			glUniform4fv(currentUniform->m_bindPoint, currentUniform->m_size, (GLfloat*)currentUniform->m_data);
			break;
		case GL_SAMPLER_2D:
			textureID = *((unsigned int*)currentUniform->m_data);
			if (textureID != NULL)
			{
				std::string uniformName = currentUniform->m_name;
				BindTextureSampler(gpuProgramID, samplerID, textureID, uniformName, textureIndexPort);
			}
			++textureIndexPort; //Increment even if NULL, is this correct?
			break;
		case GL_FLOAT_MAT4:
		{
			glUniformMatrix4fv(currentUniform->m_bindPoint, currentUniform->m_size, GL_FALSE,(GLfloat*)currentUniform->m_data);
			break;
		}
		default:
			ASSERT_OR_DIE(false, "Uniform type case not handled");
			break;
		}
	}
}

//-------------------------------------------------------------------------------------------------
void Renderer::BindTextureSampler(unsigned int gpuProgramID, GLuint samplerID, GLuint textureID, std::string const &uniformName, unsigned int textureIndexPort)
{
	if (textureID != NULL)
	{
		glActiveTexture(GL_TEXTURE0 + textureIndexPort);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glBindSampler(textureIndexPort, samplerID);
		ShaderProgramSetInt(gpuProgramID, uniformName.c_str(), textureIndexPort);
	}
}