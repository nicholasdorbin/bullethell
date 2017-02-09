#version 410 core

uniform mat4 gView;
uniform mat4 gProj;

in vec2 inPosition;
in vec4 inColor;
in vec2 inUV0;

out vec4 passColor;
out vec2 passUV0;

void main (void)
{

	vec4 pos = vec4(inPosition, 0 , 1.f);
	passColor = inColor;
	passUV0 = inUV0;

	
	//pos = pos * gModel * gView * gProj;//collumn major
	pos = gProj * gView * pos; //Row Major
	gl_Position = pos;
}