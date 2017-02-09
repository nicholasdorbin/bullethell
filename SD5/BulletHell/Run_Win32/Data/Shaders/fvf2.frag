#version 410 core

uniform vec4 gColor;
uniform sampler2D gDiffuseTex;

uniform int gEffectState;
uniform float gEffectTime;

in vec4 passColor;
in vec2 passUV0;
out vec4 outColor;

void main ( void )
{
	//vec2 rb = gl_FragCoord.xy;

	//float len = length( vec2( 800, 600 ) - rb );

	//outColor = vec4 (sin(len / 20.0f), 0.0f, 0.0f, 1.0f);

	vec4 diffuse = texture(gDiffuseTex, passUV0);

  if ( gEffectState == 1 )
  {
	   outColor = vec4 ( sin( passColor.x * gEffectTime), cos( passColor.y * gEffectTime), sin( passColor.z * gEffectTime) * cos( passColor.z * gEffectTime), 
	   	cos(passColor.w * gEffectTime)) * gColor * diffuse;
  }
  else
  {
    outColor = passColor * gColor * diffuse;
  }
  //outColor = vec4(0.f,1.f,0.f,0.f);
	//outColor = vec4(passUV0, 0.f, 1.f);
	//outColor = gColor;
}
