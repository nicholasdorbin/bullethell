#pragma once

#include <vector>
#include <string>

#include "Engine/CinematicCamera/SceneKeyframe.hpp"

struct XMLNode;
enum eCurveType
{
	CURVE_LINEAR = 0,
	CURVE_HERMITE,
	CURVE_BEZIER
};

class CutsceneTrack
{
public:
	std::vector<SceneKeyframe> m_keyFrames;
	std::string m_name;
	float m_totalSceneTime;
	size_t m_id;
	eCurveType m_curveType;
	bool m_generateTangents;
public:
	CutsceneTrack();
	CutsceneTrack(std::string name);
	CutsceneTrack(XMLNode node);
	void GenerateTangents();

};