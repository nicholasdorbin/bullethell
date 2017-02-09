#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <gl/gl.h>
#include "ThirdParty/OpenGL/glext.h"
#include "ThirdParty/OpenGL/wglext.h"

extern PFNGLGENBUFFERSPROC		glGenBuffers;
extern PFNGLBINDBUFFERPROC		glBindBuffer;
extern PFNGLBUFFERDATAPROC		glBufferData;
extern PFNGLDELETEBUFFERSPROC	glDeleteBuffers;

extern PFNGLCREATESHADERPROC   glCreateShader;
extern PFNGLSHADERSOURCEPROC   glShaderSource;
extern PFNGLCOMPILESHADERPROC  glCompileShader;
extern PFNGLGETSHADERIVPROC    glGetShaderiv;
extern PFNGLDELETESHADERPROC   glDeleteShader;
extern PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;

extern PFNGLCREATEPROGRAMPROC  glCreateProgram;
extern PFNGLATTACHSHADERPROC   glAttachShader;
extern PFNGLLINKPROGRAMPROC    glLinkProgram;
extern PFNGLGETPROGRAMIVPROC   glGetProgramiv;
extern PFNGLDETACHSHADERPROC   glDetachShader;
extern PFNGLDELETEPROGRAMPROC  glDeleteProgram;
extern PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;

extern PFNGLGENVERTEXARRAYSPROC    glGenVertexArrays;
extern PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
extern PFNGLBINDVERTEXARRAYPROC glBindVertexArray;

extern PFNGLGETACTIVEATTRIBPROC glGetActiveAttrib;
extern PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
extern PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
extern PFNGLVERTEXATTRIBIPOINTERPROC glVertexAttribIPointer;

extern PFNGLUSEPROGRAMPROC glUseProgram;

extern PFNGLGETACTIVEUNIFORMPROC		glGetActiveUniform;
extern PFNGLGETUNIFORMLOCATIONPROC		glGetUniformLocation;
extern PFNGLUNIFORM1FVPROC				glUniform1fv;
extern PFNGLUNIFORM2FVPROC				glUniform2fv;
extern PFNGLUNIFORM3FVPROC				glUniform3fv;
extern PFNGLUNIFORM4FVPROC				glUniform4fv;
extern PFNGLUNIFORM1IVPROC				glUniform1iv;

extern PFNGLUNIFORMMATRIX4FVPROC		glUniformMatrix4fv;

extern PFNGLACTIVETEXTUREPROC			glActiveTexture;
extern PFNGLBINDSAMPLERPROC				glBindSampler;
extern PFNGLGENSAMPLERSPROC				glGenSamplers;
extern PFNGLSAMPLERPARAMETERIPROC		glSamplerParameteri;

extern PFNGLGENFRAMEBUFFERSPROC			glGenFramebuffers;
extern PFNGLBINDFRAMEBUFFERPROC         glBindFramebuffer;
extern PFNGLFRAMEBUFFERTEXTUREPROC      glFramebufferTexture;
extern PFNGLCHECKFRAMEBUFFERSTATUSPROC  glCheckFramebufferStatus;
extern PFNGLDELETEFRAMEBUFFERSPROC		glDeleteFramebuffers;
extern PFNGLDRAWBUFFERSPROC				glDrawBuffers;
extern PFNGLBLITFRAMEBUFFERPROC			glBlitFramebuffer;