#pragma once

#include <vector>
#include <map>

#include "Engine/Animation/Motion.hpp"
#include "Engine/Renderer/Skeleton.hpp"


//-----------------------------------------------------------------------------------------------
struct AnimationNode;
struct XMLNode;


//-----------------------------------------------------------------------------------------------
struct AnimationTransition
{
	AnimationNode* toNode;
	float timeOfTransition;
	std::string trigger;
};


//-----------------------------------------------------------------------------------------------
struct AnimationNode
{
	std::string nodeName;
	Motion* thisNodeMotion;
	std::map< std::string, AnimationTransition* > nodeTransitions;
};


//-----------------------------------------------------------------------------------------------
class AnimationBlendGraph
{
	AnimationBlendGraph(Skeleton* skeleton);

	void LoadWalkJumpHurtMotions();
	void LoadNodesFromXML();

	void LoadInitialMotion();
	void MakeNodeFromXMLNode(const XMLNode& xmlNode);
	AnimationNode* CreateOrGet(std::string name);
	Motion* Update(float deltaSeconds);

	AnimationNode* m_currentNode;
	bool m_isTransitioning;
	std::map< std::string, Motion* > m_availableMotions;
	std::map< std::string, AnimationNode* > m_animationNodes;
	Motion* m_currentMotion;
	Skeleton* m_currentSkeleton;
};
