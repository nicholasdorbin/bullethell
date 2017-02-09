#version 410 core

uniform sampler2D gDiffuseTex;
uniform sampler2D gDepthTex;

in vec2 passUV0;
out vec4 outColor;

void main ( void )
{
	//outColor = vec4(1, 0, 1, 1);
	vec4 diffuse = texture(gDiffuseTex, passUV0);
	outColor = diffuse;
	//outColor = vec4(1,0,0,1);
	return;
}