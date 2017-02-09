#version 410 core

uniform mat4 gModel;
uniform mat4 gView;
uniform mat4 gProj;

in vec3 inPosition;
in vec2 inUV0;
in vec4 inColor;

out vec2 passUV0;
out vec4 passColor;

void main (void)
{
	passUV0 = inUV0;
	passColor = inColor;
	passUV0.y = 1.0 - passUV0.y;

	vec4 pos = vec4 (inPosition, 1.0f);
	pos = gProj * gView * gModel * pos; //Row Major
	gl_Position = pos;
}