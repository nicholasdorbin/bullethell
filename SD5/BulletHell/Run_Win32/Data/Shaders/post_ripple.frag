#version 410 core

uniform float gTime;
uniform sampler2D gDiffuseTex;
uniform sampler2D gDepthTex;

in vec4 passColor;
in vec2 passUV0;
out vec4 outColor;

void main ( void )
{
	vec2 offset = passUV0 - vec2(0.5f);
	float d = length(offset);
	vec2 u = offset / d;

	float offsetAmount = 0.05f * -cos((gTime * 4.0f) + d * 100.f);
	vec2 uv = passUV0 + u * offsetAmount;

	float blue = texture( gDiffuseTex, passUV0 ).r;
 	outColor = vec4( 0, 0, blue, 1 );
	outColor = outColor + texture(gDiffuseTex, uv);
	outColor = clamp(outColor, vec4(0), vec4(1));
}