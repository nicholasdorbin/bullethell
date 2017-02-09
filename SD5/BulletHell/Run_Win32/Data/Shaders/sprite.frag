#version 410 core

// Camera position - needed for specular
uniform vec3 gCameraPosition;

// Surface Information : Textures
uniform sampler2D gDiffuseTex;


// Surface Information : Specular
uniform float gSpecularPower;
uniform float gSpecularIntensity;

// Global Light Values
uniform vec4 gAmbientLight;

// Surface Informat

// Global Fog Values
uniform float gFogNear = 10.0f;
uniform float gFogFar = 100.0f;
uniform float gFogNearFactor = 0.0f;
uniform float gFogFarFactor = 1.0f;
uniform vec3 gFogColor;


//Time
uniform float gAge = 1.f;

//State
uniform int gRenderState = 0;

// Surface Information
in vec4 passColor;
in vec2 passUV0;

// Output Channels (Just the default one)
out vec4 outColor;

vec4 outColor2 = vec4(0.f);


//------------------------------------------------------------------------
// Used for passing information back from my lighting equation
struct surface_light_t
{
   vec3 surface;
   vec3 specular;
};

//------------------------------------------------------------------------
vec4 UVAsColor( vec2 uv )
{
	return vec4( uv, 0.f, 1.f );
}

//------------------------------------------------------------------------
vec4 VectorAsColor( vec3 vec ) 
{
	return vec4( (vec + vec3(1.0f)) * vec3(.5f), 1.0f );
}

//------------------------------------------------------------------------


//------------------------------------------------------------------------
void main()
{

   vec4 c = texture(gDiffuseTex, passUV0);

   outColor = c * passColor;
   //outColor = texture( uTexDiffuse, passUV0 ) * passColor;
   //outColor = final_color + outColor2;
}
