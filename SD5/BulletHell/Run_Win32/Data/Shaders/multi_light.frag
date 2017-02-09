#version 410 core

uniform vec4 gColor;
uniform sampler2D gDiffuseTex;
uniform sampler2D gNormalTex;
uniform sampler2D gSpecularTex;
uniform sampler2D gEmissiveTex;
uniform sampler2D gDissolveTex;

uniform vec4 gAmbientLight;

uniform vec3 gLightPosition[ 16 ];
uniform vec4 gLightColor[ 16 ];
uniform float gLightIntensity[ 16 ];

uniform float gSpecularPower;
uniform float gAge;

uniform vec3 g_cameraPosition;

uniform float g_minDistToCamera;
uniform float g_maxDistToCamera;
uniform vec4 g_fogColor;


in vec4 passColor;
in vec2 passUV0;
in vec3 passPosition;
in vec3 passTangent;
in vec3 passBitangent;

out vec4 outColor;

bool CanDissolve()
{
	vec4 dissolve = texture(gDissolveTex, passUV0);

	if(dissolve.r < gAge)
		return true;
	return false;
}

vec3 CalculateSurfaceLight( vec4 color, vec3 position, vec3 normal, float intensity, vec3 emissive )
{
	vec3 surfaceLight = gAmbientLight.rgb * gAmbientLight.a;

	vec3 vecToLight = position - passPosition;
	float invLen = dot( vecToLight, vecToLight );
	invLen = inversesqrt( invLen );

	vecToLight = vecToLight * invLen;
	float power = intensity * ( invLen * invLen );

	float nDotL = clamp( dot( normal, vecToLight ), 0.0f, 1.0f );
	surfaceLight += nDotL  * color.rgb * power;

	surfaceLight += emissive;
	surfaceLight = clamp( surfaceLight, vec3( 0.0f ), vec3( 1.0f) );

	return surfaceLight;
}

vec3 CalculateSpecularFactor( vec4 color, vec3 position, vec3 normal, float intensity )
{
	float specularIntensity = texture( gSpecularTex, passUV0 ).r;

	vec3 vecToLight = normalize( position - passPosition );
	vec3 vecToEye = normalize( g_cameraPosition - passPosition );
	vec3 halfVector = vecToLight + vecToEye;
	halfVector = normalize( halfVector );

	float halfDotNormal = clamp( dot( halfVector, normal ), 0.0f, 1.0f );
	float specIntensity = pow( halfDotNormal, gSpecularPower ) * specularIntensity;

	float power = dot( vecToLight, vecToLight );
	power = intensity / power;

	vec4 specColor = specIntensity * color * power;
	return specColor.rgb;
}


vec4 CalculateFogFactor (vec4 color)
{
	vec3 vecToObject = passPosition - g_cameraPosition;
	float vec_len = dot(vecToObject, vecToObject);
	vec_len = sqrt(vec_len);
	
	

	if (vec_len < g_minDistToCamera)
	{
		return color;
	}
	else if (vec_len < g_maxDistToCamera)
	{
		vec_len -= g_minDistToCamera;
		float diff = g_maxDistToCamera - g_minDistToCamera;
		float percentDist = vec_len / diff;

		return vec4(g_fogColor.rgba * (percentDist)) + vec4(color * (1-percentDist));
	}
	else
	{
		return g_fogColor;
	}
}

vec4 ColorFromVector(vec3 v)
{
	//Convert unit vector to color space
	return vec4((v + vec3(1.0)) * 0.5f, 1.f);
}

void debugmain (void)
{
	vec3 diff = passPosition - g_cameraPosition;
	float l = abs(diff.y);
	l /= 5.f;
	outColor = vec4(l,l,l,1.f);
}

void main ( void )
{
	//outColor = ColorFromVector(passBitangent);

	//return;
	vec4 dissolve = texture(gDissolveTex, passUV0);

	if(dissolve.r < gAge)
	{
		discard;
	}
	else if ((dissolve.r - 0.01f) < (gAge ))
	{
		outColor = vec4(1.f,0.5f,0.f,1.f);
		return;
	}
	
		
	

	vec3 surface_tangent = normalize(passTangent);
	vec3 surface_bitangent = normalize(passBitangent);
	vec3 surface_normal = cross(surface_tangent, surface_bitangent);
	surface_bitangent = cross(surface_normal, surface_tangent);

	mat3 tbn = mat3(surface_tangent, surface_bitangent, surface_normal);
	tbn = transpose(tbn); // make need for TBN



	vec4 color = texture(gDiffuseTex, passUV0);
	vec3 normal= texture(gNormalTex, passUV0).rgb;
	vec3 emissive = texture(gEmissiveTex, passUV0).rgb;
	//outColor = vec4(emissive,1.f);
	//return;
/*
	if (passUV0.x > 0.5f)
	{
		outColor = color;
	}
	else
	{
		outColor = normal;
	}
*/
	vec3 light_intensity = gAmbientLight.rgb * gAmbientLight.a;

	//normal = (normal + vec3(2.f,1.f,0.f)) * vec3(0.5f,0.5f,0.5f);
	normal = normal * vec3(2.f,2.0f,1.f) - vec3(1.f,1.f,0.f);
	normal = normalize(normal);
	normal = normal * tbn;
	//vec3 normal = vec3(0.f,-1.f,0.f);//negative your forward
	//normal = normal * vec3(2.f,2.0f,1.f) - vec3(1.f,1.f,0.f);

	//vec3 final_normal = vec3(normal.x, -normal.z, normal.y);
	//final_normal = normalize(final_normal);


	// Calculation of surfaceLight
	vec3 surfaceLight = CalculateSurfaceLight( gLightColor[ 0 ], gLightPosition[ 0 ], normal, gLightIntensity[ 0 ], emissive );

	// Calculation of specular factor
	vec3 specularFactor = CalculateSpecularFactor( gLightColor[ 0 ], gLightPosition[ 0 ], normal, gLightIntensity[ 0 ] );


	outColor = color * vec4( surfaceLight, 1.0f ) + vec4( specularFactor, 1.0f );
	outColor = CalculateFogFactor(outColor);
	outColor = clamp(outColor, vec4(0.f), vec4(1.f));
	//outColor = texture(gNormalTex, passUV0);
	//outColor = vec4(passUV0, 0.f, 1.f);
	//outColor = gColor;
}

