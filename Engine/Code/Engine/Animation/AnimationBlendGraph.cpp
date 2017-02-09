#include "Engine/Animation/AnimationBlendGraph.hpp"
#include "Engine/Tools/Parsers/xmlParser.h"
#include "Engine/Tools/Parsers/XMLUtilities.hpp"

//-----------------------------------------------------------------------------------------------

AnimationBlendGraph::AnimationBlendGraph(Skeleton* skeleton)
{
	skeleton;
	LoadWalkJumpHurtMotions();
	LoadNodesFromXML();
}

//-----------------------------------------------------------------------------------------------
void AnimationBlendGraph::LoadWalkJumpHurtMotions()
{
	// Walk motion
	Motion* walkMotion = new Motion();
	std::string filename = "Data/FBXs/unitychan_walk.anim";
	walkMotion->LoadFromFile(filename);
	m_availableMotions.insert(std::pair< std::string, Motion* >("walk", walkMotion));

	// Jump motion
	Motion* jumpMotion = new Motion();
	filename = "Data/FBXs/unitychan_jump.anim";
	jumpMotion->LoadFromFile(filename);
	m_availableMotions.insert(std::pair< std::string, Motion* >("jump", jumpMotion));

	// Hurt motion
	Motion* hurtMotion = new Motion();
	filename = "Data/FBXs/unitychan_hurt.anim";
	hurtMotion->LoadFromFile(filename);
	m_availableMotions.insert(std::pair< std::string, Motion* >("hurt", hurtMotion));
}


//-----------------------------------------------------------------------------------------------
void AnimationBlendGraph::LoadNodesFromXML()
{
	const std::string fileName = "Data/Graphs/AnimationBlendGraph.xml";
	XMLNode node = XMLNode::openFileHelper(fileName.c_str());
	XMLNode rootNode = node.getChildNode(0);//MotionGraph
	


	if (!rootNode.IsContentEmpty())
	{
		int nodeCount = rootNode.nChildNode(); //# of Node nodes

		for (int i = 0; i < nodeCount; i++)
		{
			XMLNode thisNode = rootNode.getChildNode(i);
			std::string name = thisNode.getName();
			if (name != "Node")
			{
				//Not valid node, skip
				continue;
			}
			MakeNodeFromXMLNode(thisNode);

		}
	}


}




void AnimationBlendGraph::MakeNodeFromXMLNode(const XMLNode& xmlNode)
{
	/* AnimNode
	std::string nodeName;
	Motion* thisNodeMotion;
	std::map< std::string, AnimationTransition* > nodeTransitions;
	*/
	/*
	struct AnimationTransition
{
	AnimationNode* toNode;
	float timeOfTransition;
	std::string trigger;
};
	*/

	//Get all the child nodes of this AnimationNode

	if (!xmlNode.IsContentEmpty())
	{
		int nodeCount = xmlNode.nChildNode(); //# of Transition nodes

		for (int i = 0; i < nodeCount; i++)
		{
			XMLNode thisNode = xmlNode.getChildNode(i);
			std::string name = thisNode.getName();

			//Is it called Transition?
			if (name != "Transition")
			{
				//Not valid node, skip
				continue;
			}

			//If so, get the following:
			//To (string)[convert this to a corresponding node via CreateOrGet]
			//At (float)
			//When (string) [trigger = [blah]]

		}
	}


	
	





}

AnimationNode* AnimationBlendGraph::CreateOrGet(std::string name)
{
	return nullptr;
}

//-----------------------------------------------------------------------------------------------
Motion* AnimationBlendGraph::Update(float deltaSeconds)
{
	deltaSeconds;
	if (m_isTransitioning)
	{
		// Return interpolated motion

	}
	else
	{
		// Return current node's default motion
		//m_currentNode.thisNodeMotion->Update(m_currentSkeleton, deltaSeconds);
	}
	return nullptr;
}
