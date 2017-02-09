#pragma once

#include <string>
#include <vector>

#include "Engine/Math/Matrix44.hpp"
#include "Engine/Math/Vector3.hpp"
class IBinaryWriter;
class IBinaryReader;

//-----------------------------------------------------------------------------------------------
struct Joint
{
	Joint() {}
	Joint(const std::string& jointName, int jointParentIndex, const mat44_fl& initialBoneSpace)
		: m_jointName(jointName)
		, m_jointParentIndex(jointParentIndex)
		, m_initialBoneSpace(initialBoneSpace)
	{
	}
	std::string m_jointName;
	int m_jointParentIndex;
	mat44_fl m_initialBoneSpace;

};


//-----------------------------------------------------------------------------------------------
class Skeleton
{
public:
	void AddJoint(const std::string& jointName, int jointParentIndex, const mat44_fl& initialBoneSpace);
	int GetLastAddedIndex() const { return m_jointNames.size() - 1; }
	int GetJointCount() const { return m_jointNames.size(); }
	Vector3 GetJointPosition(int jointIndex) const;
	Joint* GetJointByIndex(int jointIndex);
	int GetParentJoint(int jointIndex) const;
	void SetJointWorldTransform(int jointIndex, mat44_fl modelTransform);

	void RenderBones() const;

	//File I/O
	void WriteToFile(std::string fileName);
	void LoadFromFile(std::string fileName);
	void WriteToStream(IBinaryWriter &writer);
	void ReadFromStream(IBinaryReader &reader);

	std::vector< mat44_fl> const GetBoneMatrices();

	int GetJointIndexForJointName(std::string name);
public:
	std::vector< std::string > m_jointNames;
	std::vector< int > m_jointParentIndicies;
	std::vector< mat44_fl > m_modelToBoneSpace;	// For initial model space layout
	std::vector< mat44_fl > m_boneToModelSpace;	// For current bone's model space
	std::vector<Joint*> m_joints;
};