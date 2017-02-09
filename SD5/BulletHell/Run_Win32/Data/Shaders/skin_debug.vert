#version 410 core

uniform mat4 gModel;

// Also good for a block
uniform mat4 gView;
uniform mat4 gProj;

// Uniform Blocks are SUPER USEFUL for this!
uniform mat4 gBoneMatrices[200]; // max supported bones (inverse_initial * current)

in vec3 inPosition;
in vec4 inColor;
in vec2 inUV0;
in vec3 inTangent;
in vec3 inBitangent;
//in vec3 inNormal;
// in ...  //everything else as normal

// NEW!
// When you pass this glVertexAttribIPointer 
in uvec4 inBoneIndices; // uint[4]  
in vec4 inBoneWeights;  // float[4]  // already normalized to add to one.

out vec4 passColor;
out vec2 passUV0;
out vec3 passPosition;
out vec3 passTangent;
out vec3 passBitangent;
//out vec3 passNormal;
// out ... // everything else you usually pass


void main( void ) 
{
	//NICK NOTE: Do we change the mults for bone_transform to be on the other side
	mat4 bone0 = gBoneMatrices[inBoneIndices.x];//inBoneIndices.x];
	mat4 bone1 = gBoneMatrices[inBoneIndices.y];//inBoneIndices.y];
	mat4 bone2 = gBoneMatrices[inBoneIndices.z];//inBoneIndices.z];
	mat4 bone3 = gBoneMatrices[inBoneIndices.w];//inBoneIndices.w];

	mat4 bone_transform = inBoneWeights.x * bone0
		+ inBoneWeights.y * bone1
		+ inBoneWeights.z * bone2
		+ inBoneWeights.w * bone3;

	// for debug purposes, you can also do
	// mat4 model_to_world = gModel;   
	// That will remove the skinning calculation. 
	mat4 model_to_world = bone_transform;

	vec4 col0 = normalize(vec4(.5, 0, -.82, 0));
	vec4 col1 = normalize(vec4(0, 1, 0, 0));
	vec4 col2 = normalize(vec4(.82, 0, .5, 0));
	vec4 col3 = normalize(vec4(0, 0, -1, 1));
	mat4 testmat = mat4( col0, col1, col2, col3);
	model_to_world = gModel * bone_transform;

	passPosition = ( model_to_world * vec4(inPosition, 1.0f)).xyz;
	//passNormal = ( model_to_world * vec4(inNormal, 0.0f) ).xyz;
	// tangent, bitangent... 
	passUV0 = inUV0;
	passTangent = (vec4(inTangent, 0.f) * model_to_world).xyz;
	passBitangent = (vec4(inBitangent, 0.f) * model_to_world).xyz;

	float weight = inBoneWeights.x + inBoneWeights.y + inBoneWeights.z + inBoneWeights.w;

	//passColor = vec4( vec3(weight), 1.0f );
	passColor = vec4(inBoneWeights.xyz,1.f);

	// Pass position over
	vec4 pos = vec4(inPosition, 1.f);
	pos = gProj * gView  * model_to_world * pos; //collumn Major
	//gl_Postion = vec4(inPosition, 1.0f) * model_to_world * gView * gProj;
	gl_Position = pos;
}


