#include "Engine/CinematicCamera/CutsceneTrack.hpp"
#include "Engine/Math/HashUtils.hpp"
#include "Engine/Tools/Parsers/XMLUtilities.hpp"
#include "Engine/Core/EngineCommon.hpp"

CutsceneTrack::CutsceneTrack()
	:m_name("UNNAMED SCENE")
	, m_totalSceneTime(0.f)
	, m_id(0)
	, m_generateTangents(false)
{
	m_curveType = CURVE_LINEAR;
}

CutsceneTrack::CutsceneTrack(std::string name)
	:m_name(name)
	, m_totalSceneTime(0.f)
	, m_id(0)
	, m_generateTangents(false)
{
	m_id = HashString(name);
	m_curveType = CURVE_LINEAR;
}

CutsceneTrack::CutsceneTrack(XMLNode node)
	: m_generateTangents(false)
	, m_totalSceneTime(0.f)
{
	float lastFOV = g_camera.GetFOV();
	if (!node.IsContentEmpty())
	{
		m_name = ReadXMLAttribute(node, "name", "NONAME");
		m_generateTangents = ReadXMLAttribute(node, "generateTangents", m_generateTangents);
		m_totalSceneTime = ReadXMLAttribute(node, "totalTime", m_totalSceneTime);

		//#TODO Load other types of curves.
		m_curveType = CURVE_HERMITE;

		int frameCount = node.nChildNode(); //# of Frame nodes
		for (int i = 0; i < frameCount; i++)
		{
			XMLNode frameNode = node.getChildNode(i);
			std::string name = frameNode.getName();
			if (name != "Frame")
			{
				//Not valid node, skip
				continue;
			}

			SceneKeyframe thisFrame;

			thisFrame.timeAt = ReadXMLAttribute(frameNode, "timeAt", 0.f);

			if (frameNode.getAttribute("position"))
			{
				std::string positionStr = frameNode.getAttribute("position");

				size_t pos = positionStr.find(",");
				std::string xStr = positionStr.substr(0, pos);
				positionStr.erase(0, pos + 1);

				pos = positionStr.find(",");
				std::string yStr = positionStr.substr(0, pos);
				positionStr.erase(0, pos + 1);

				std::string zStr = positionStr;

				thisFrame.position = Vector3(stof(xStr), stof(yStr), stof(zStr));
			}

			if (frameNode.getAttribute("axis") && frameNode.getAttribute("angle"))
			{

				float angle = ReadXMLAttribute(frameNode, "angle", 0.f);

				std::string axisStr = frameNode.getAttribute("axis");

				size_t pos = axisStr.find(",");
				std::string xStr = axisStr.substr(0, pos);
				axisStr.erase(0, pos + 1);

				pos = axisStr.find(",");
				std::string yStr = axisStr.substr(0, pos);
				axisStr.erase(0, pos + 1);

				std::string zStr = axisStr;

				Vector3 axis = Vector3(stof(xStr), stof(yStr), stof(zStr));

				Quaternion frameQuat = Quaternion(angle, axis);
				thisFrame.rotation = frameQuat;
	
			}

			if (frameNode.getAttribute("fov"))
			{
				lastFOV = ReadXMLAttribute(frameNode, "fov", lastFOV);
				thisFrame.fov = lastFOV;
			}

			if (frameNode.getAttribute("lookAtEnable"))
			{

				std::string lookAtState = ReadXMLAttribute(frameNode, "lookAtEnable", "");

				if (lookAtState == "true" && frameNode.getAttribute("lookAtPosition"))
				{
					//Enable Lookat and store the lookat Position

					std::string lookAtPosStr = frameNode.getAttribute("lookAtPosition");

					size_t pos = lookAtPosStr.find(",");
					std::string xStr = lookAtPosStr.substr(0, pos);
					lookAtPosStr.erase(0, pos + 1);

					pos = lookAtPosStr.find(",");
					std::string yStr = lookAtPosStr.substr(0, pos);
					lookAtPosStr.erase(0, pos + 1);

					std::string zStr = lookAtPosStr;

					Vector3 lookAtPos = Vector3(stof(xStr), stof(yStr), stof(zStr));

					thisFrame.lookAtState = LOOKAT_ENABLE;
					thisFrame.lookAtTargetPosition = lookAtPos;
				}
				else if (lookAtState == "false")
				{
					thisFrame.lookAtState = LOOKAT_DISABLE;
				}
				else
				{
					thisFrame.lookAtState = LOOKAT_NOCHANGE;
				}
			}

			m_keyFrames.push_back(thisFrame);
		}
	}

}

void CutsceneTrack::GenerateTangents()
{
	for (size_t i = 1; i < m_keyFrames.size() - 1; i++)
	{
		Vector3 thisPos = m_keyFrames[i].position;
		Vector3 prevPos = m_keyFrames[i-1].position;
		Vector3 nextPos = m_keyFrames[i+1].position;


		Vector3 tangent = GetCatmullRomVelocity(thisPos, prevPos, nextPos);
		m_keyFrames[i].SetBothVelocities(tangent);
	}

	m_keyFrames[0].SetBothVelocities(Vector3::ZERO);
	m_keyFrames[m_keyFrames.size() - 1].SetBothVelocities(Vector3::ZERO);
}
