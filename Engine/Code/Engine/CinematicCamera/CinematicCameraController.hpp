#pragma once
#include <map>
#include "Engine/CinematicCamera/CutsceneTrack.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Rgba.hpp"


class CinematicCameraController;



extern CinematicCameraController* g_cinematicCameraController;


struct InterpData
{
	Vector3 startPosition;
	Vector3 endPosition;
	Rgba lineColor;
};

class CinematicCameraController
{
private:
	std::map<std::string, CutsceneTrack*> m_CutsceneTrackDatabase;

	std::vector<InterpData> GetInterpDataBetweenFrames(SceneKeyframe& startFrame, SceneKeyframe& endFrame, float sceneEndTime, int segmentCount = 10) const;
public:
	CutsceneTrack* m_cutScene;
	bool m_isScenePlaying;
	bool m_isDrawDebugScene;
	float m_curSceneTime;
	float m_nextSceneStartAtTime;
	float m_startFOV;
	float m_currentFOV;
	int m_curSceneIndex;
	bool m_isLookingAtTarget;
	Vector3 m_lookAtPosition;

public:
	CinematicCameraController();
	~CinematicCameraController();
	void LoadAllCutsceneTracks();
	void PlayCutscene();
	void EndCutscene();
	bool LoadCutscene(std::string sceneName);
	bool IsScenePlaying();
	void Update(const float deltaSeconds);
	void RenderDebugCutscene() const;
};