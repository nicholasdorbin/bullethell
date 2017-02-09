#pragma once

#pragma once
#ifndef __TOOLS_FBX__
#define __TOOLS_FBX__
#include <string>
#include <vector>
#include <map>
class MeshBuilder;
class Skeleton;
struct mat44_fl;
class Motion;


void FbxListScene(std::string filename);

class SceneImport
{
public:
	~SceneImport()
	{
		//destroy all meshes
	}
	std::vector<MeshBuilder*> meshes;
	std::vector<Skeleton*> skeletons;
	std::vector< Motion* > motions;
};
SceneImport* FbxLoadSceneFromFile(std::string filename,
	mat44_fl engine_basis,
	bool is_engine_basis_right_handed,
	mat44_fl transform);
#endif