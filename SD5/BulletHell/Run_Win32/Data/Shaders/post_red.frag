#version 410 core

uniform sampler2D gDiffuseTex;
uniform sampler2D gDepthTex;

in vec2 passUV0;
in vec4 gl_FragCoord;
out vec4 outColor;

void main ( void )
{

	vec4 diffuse = texture(gDiffuseTex, passUV0);
	diffuse *= 3;
	diffuse = floor(diffuse);
	diffuse /= 3;

	float depth_here = texture(gDepthTex, passUV0).r;
	float scale = 1.0f - (depth_here * depth_here);
	float depth_right = texture(gDepthTex, passUV0 + scale * vec2(0.05f,0.0f)).r;
	float depth_left = texture(gDepthTex, passUV0 -  scale *vec2(0.05f,0.0f)).r;

	float diff = abs(depth_right - depth_left);
	if (diff > 0.01f)
	{
		outColor = vec4(0,0,0,1);
	}
	else
	{
		outColor = vec4(1);
	}
	if (gl_FragCoord.x < 800)
	{
		float red = texture( gDiffuseTex, passUV0 ).r;
 		outColor = vec4( red, 0, 0, 1 );
	}
	else
	{
		outColor = vec4(vec3(depth_here), 1);
		//outColor = outColor * (mix(vec4(1.f),diffuse, 0.3f));
	}
	
	//depth = depth*depth*depth;
	
	//outColor = vec4(passUV0,0,1);
}