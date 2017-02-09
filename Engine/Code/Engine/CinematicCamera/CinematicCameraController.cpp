#include "Engine/CinematicCamera/CinematicCameraController.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/MathUtils.hpp"
#include "Engine/Core/Console.hpp"
#include "Engine/Math/HashUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Tools/Parsers/XMLUtilities.hpp"
#include "Engine/Core/EngineBuildConfig.hpp"


CinematicCameraController* g_cinematicCameraController = nullptr;
const Rgba START_DEBUG_COLOR = Rgba::YELLOW;
const Rgba END_DEBUG_COLOR = Rgba::PURPLE;
#ifdef CINEMATIC_CAMERA
std::vector<InterpData> CinematicCameraController::GetInterpDataBetweenFrames(SceneKeyframe& startFrame, SceneKeyframe& endFrame, float sceneEndTime, int segmentCount) const
{
	std::vector<InterpData> results;

	float keyframeTime = endFrame.timeAt - startFrame.timeAt;
	float interpInterval = keyframeTime / segmentCount;
	

	float percentCompleteStart = startFrame.timeAt / sceneEndTime;
	float percentCompleteEnd = endFrame.timeAt / sceneEndTime;
	Rgba startColor = Lerp(START_DEBUG_COLOR, END_DEBUG_COLOR, percentCompleteStart);
	Rgba endColor = Lerp(START_DEBUG_COLOR, END_DEBUG_COLOR, percentCompleteEnd);


	Vector3 prevPosition = startFrame.position;
	float curTime = interpInterval;

	for (int i = 0; i < segmentCount; i++)
	{
		InterpData thisSegmentData;

		float scaledTime = RangeMap(curTime, 0.f, keyframeTime, 0.f, 1.f);
		Vector3 currentPosition = HermiteSpline(startFrame.position, endFrame.position, startFrame.exitVelocity, endFrame.entryVelocity, scaledTime);

		Rgba segmentColor = Lerp(startColor, endColor, curTime);


		thisSegmentData.startPosition = prevPosition;
		thisSegmentData.endPosition = currentPosition;
		thisSegmentData.lineColor = segmentColor;

		results.push_back(thisSegmentData);


		prevPosition = currentPosition;

		curTime += interpInterval;
	}
	

	return results;
}

CinematicCameraController::CinematicCameraController()
	: m_isScenePlaying(false)
	, m_isDrawDebugScene(true)
	, m_curSceneIndex(0)
	, m_curSceneTime(0.f)
	, m_nextSceneStartAtTime(0.f)
	, m_isLookingAtTarget(false)
	, m_lookAtPosition(Vector3::ZERO)
{
#ifdef CINEMATIC_CAMERA
	m_cutScene = new CutsceneTrack();

	LoadAllCutsceneTracks();
	//m_cutScene = m_CutsceneTrackDatabase.find("test1")->second;
	if (m_CutsceneTrackDatabase.size() > 0)
	{
// 		m_cutScene = m_CutsceneTrackDatabase.find("XMLTest")->second;
// 		if (m_cutScene->m_generateTangents)
// 		{
// 			m_cutScene->GenerateTangents();
// 		}
	}
	
#endif
}

CinematicCameraController::~CinematicCameraController()
{
#ifdef CINEMATIC_CAMERA
	//#TODO iterate thru map and delete each Cutscene Track
	delete m_cutScene;
	std::map< std::string, CutsceneTrack* >::iterator it = m_CutsceneTrackDatabase.begin();
	while (it != m_CutsceneTrackDatabase.end()) {
		
		delete(it->second);
		it = m_CutsceneTrackDatabase.erase(it);
	}
#endif
}

void CinematicCameraController::LoadAllCutsceneTracks()
{
	//XML Loading
	

	
	std::vector< std::string > animFiles = EnumerateFilesInFolder("Data/SceneTracks",
		"*.SceneTrack.xml");

	for each (std::string str in animFiles)
	{

		XMLNode node = XMLNode::openFileHelper(str.c_str());
		XMLNode rootNode = node.getChildNode(0);//Scenes

		if (!rootNode.IsContentEmpty())
		{
			int sceneCount = rootNode.nChildNode(); //# of SceneTrack nodes

			for (int i = 0; i < sceneCount; i++)
			{
				XMLNode thisNode = rootNode.getChildNode(i);
				std::string name = thisNode.getName();
				if (name != "SceneTrack")
				{
					//Not valid node, skip
					continue;
				}
				CutsceneTrack* newScene = new CutsceneTrack(thisNode);
				if (newScene != nullptr)
				{
					m_CutsceneTrackDatabase.insert(std::pair< std::string, CutsceneTrack* >(newScene->m_name, newScene));
				}

			}
		}
	}

	
	//#TODO Make this configuration in XML
	/*
	std::vector<SceneKeyframe> keyframes;

	CutsceneTrack* m_testScene1 = new CutsceneTrack("test1");
	CutsceneTrack* m_testScene2 = new CutsceneTrack("test2");

	Quaternion test = Quaternion(45.f, Vector3::FORWARD);
	Vector3 direction = test.GetUnitVector();

	Quaternion test2 = Quaternion(315.f, Vector3::UP);
	Vector3 direction2 = test2.GetUnitVector();

	m_testScene1->m_keyFrames.push_back(SceneKeyframe(Vector3(-2.f, 1.f, 1.f), Quaternion(0.f, Vector3::FORWARD), 0.f ,Vector3::UP, Vector3::UP));
	m_testScene1->m_keyFrames.push_back(SceneKeyframe(Vector3(-1.f, -1.f, 1.f), Quaternion(45.f, Vector3::UP), 2.f, -Vector3::UP, Vector3::RIGHT));
	m_testScene1->m_keyFrames.push_back(SceneKeyframe(Vector3(0.f, -1.f, 0.f), Quaternion(315.f, Vector3::UP), 4.f, Vector3::UP, Vector3::UP));
	m_testScene1->m_keyFrames.push_back(SceneKeyframe(Vector3(1.f, 1.f, 0.f), Quaternion(2.f, Vector3::UP), 6.f, Vector3::UP, -Vector3::UP));
	m_testScene1->m_keyFrames.push_back(SceneKeyframe(Vector3(-2.f, 1.f, 0.f), Quaternion(45.f, Vector3::FORWARD), 8.f, -Vector3::UP, Vector3::UP));
	m_testScene1->m_keyFrames.push_back(SceneKeyframe(Vector3(1.f, 1.f, 1.f), Quaternion(45.f, Vector3::RIGHT), 10.f, Vector3::UP, -Vector3::RIGHT));
	m_testScene1->m_keyFrames.push_back(SceneKeyframe(Vector3(4.f, 1.f, 1.f), Quaternion(0.f, Vector3::FORWARD), 12.f, -Vector3::UP, Vector3::UP));

	m_testScene1->m_totalSceneTime = 12.f;
	m_testScene1->m_curveType = CURVE_HERMITE;
	m_testScene1->m_generateTangents = true;



	m_testScene2->m_keyFrames.push_back(SceneKeyframe(Vector3::ONE, Quaternion(0.f, Vector3::FORWARD), 0.f));
	m_testScene2->m_keyFrames.push_back(SceneKeyframe(Vector3::ONE, Quaternion(-90.f, Vector3::RIGHT), 2.f));
	m_testScene2->m_keyFrames.push_back(SceneKeyframe(Vector3::ONE, Quaternion(45.f, Vector3::RIGHT), 4.f));

	m_testScene2->m_totalSceneTime = 4.f;

	m_CutsceneTrackDatabase.insert(std::pair< std::string, CutsceneTrack* >(m_testScene1->m_name, m_testScene1));
	m_CutsceneTrackDatabase.insert(std::pair< std::string, CutsceneTrack* >(m_testScene2->m_name, m_testScene2));

	*/
}


void CinematicCameraController::PlayCutscene()
{
	g_cinematicCameraController->m_isScenePlaying = true;
}

void CinematicCameraController::EndCutscene()
{
	m_isScenePlaying = false;
	m_curSceneIndex = 0;
	m_curSceneTime = 0.f;
	m_nextSceneStartAtTime = 0.f;
}

bool CinematicCameraController::LoadCutscene(std::string sceneName)
{
	auto foundScene = m_CutsceneTrackDatabase.find(sceneName);
	if (foundScene != m_CutsceneTrackDatabase.end())
	{

		m_cutScene = foundScene->second;
		if (m_cutScene->m_generateTangents)
		{
			m_cutScene->GenerateTangents();
		}
		return true;
	}
	else
	{
		m_cutScene = nullptr;
		return false;
	}
	
}

bool CinematicCameraController::IsScenePlaying()
{
	return m_isScenePlaying;
}

void CinematicCameraController::Update(const float deltaSeconds)
{
	if (m_isScenePlaying)
	{
		//#TODO Ensure scene is valid somehow
		m_curSceneTime += deltaSeconds;
		if (m_curSceneTime >= m_cutScene->m_totalSceneTime)
		{
			EndCutscene();
			return;
		}

		

		//Get what frame we're on
		SceneKeyframe thisFrame = m_cutScene->m_keyFrames[m_curSceneIndex];
		SceneKeyframe nextFrame = m_cutScene->m_keyFrames[m_curSceneIndex + 1];

		

		if (nextFrame.timeAt <= m_curSceneTime)
		{
			m_curSceneIndex++;
			thisFrame = m_cutScene->m_keyFrames[m_curSceneIndex];
		}

		//Make sure we're not out of bounds
		if ((size_t)m_curSceneIndex >= m_cutScene->m_keyFrames.size() - 1)
		{
			//End scene, out of keyframes
			EndCutscene();
			return;
		}
		//Set it again incase we shifted the value up above

		nextFrame = m_cutScene->m_keyFrames[m_curSceneIndex + 1];


		//Check if we are using look at
		if (thisFrame.lookAtState == LOOKAT_ENABLE)
		{
			m_isLookingAtTarget = true;
			m_lookAtPosition = thisFrame.lookAtTargetPosition;
		}
		else if (thisFrame.lookAtState == LOOKAT_DISABLE && m_isLookingAtTarget != false)
		{
			m_isLookingAtTarget = false;
			//Overload this frame's orientation to smoothly Slerp from here to the target
			thisFrame.rotation = Quaternion(g_camera.m_orientation);
		}


		//Calculate where the camera should look
		

		float scaledTime = RangeMap(m_curSceneTime, thisFrame.timeAt, nextFrame.timeAt, 0.f, 1.f);

		float currentFOV = Lerp(thisFrame.fov, nextFrame.fov, scaledTime);

		if (m_isLookingAtTarget == true)
		{
			g_camera.m_orientation = GetLookAtAngle(g_camera.m_position, m_lookAtPosition);
		}
		else
		{
			Quaternion intermidiateQuat = SlerpQuats(thisFrame.rotation, nextFrame.rotation, scaledTime);

			intermidiateQuat.ToEulerAngles(g_camera.m_orientation);
		}
		
		g_camera.FixAndClampAngles();
		g_camera.ChangeFov(currentFOV);



		//Calculate where the camera's position should be
		if (m_cutScene->m_curveType != CURVE_LINEAR)
		{
			Vector3 currentPosition = HermiteSpline(thisFrame.position, nextFrame.position, thisFrame.exitVelocity, nextFrame.entryVelocity, scaledTime);

			g_camera.m_position = currentPosition;
		}
	}
}

void CinematicCameraController::RenderDebugCutscene() const
{
	if (!m_isDrawDebugScene || m_cutScene == nullptr)
		return;

	float totalTime = m_cutScene->m_totalSceneTime;
	int currentFrame = 0;
	int totalFrames = m_cutScene->m_keyFrames.size();
	for each (SceneKeyframe frame in m_cutScene->m_keyFrames)
	{
		//Get a color that shows how far this keyframe is in the Cutscene
		float timeOfFrame = frame.timeAt;
		float percentComplete =  timeOfFrame / totalTime;
		//percentComplete = RangeMap(percentComplete, 0.f, 100.f, 0, 1.f);
		Rgba drawColor = Lerp(START_DEBUG_COLOR, END_DEBUG_COLOR, percentComplete);

		//Get a vector in the direction of the angle

		//Draw an arrow in this direction
		Vector3 direction = frame.rotation.GetUnitVector();
		Vector3 endPos = frame.position + direction;
		direction.SetNormalized();

		g_theRenderer->SetLineWidth(5.f);
		//Draw a point at this position
		if (frame.position == m_cutScene->m_keyFrames[0].position)
		{
			g_theRenderer->DrawPoint(frame.position, Rgba::GREEN, 0.2f);
		}
		else if (frame.position == m_cutScene->m_keyFrames[totalFrames - 1].position)
		{
			g_theRenderer->DrawPoint(frame.position, Rgba::RED, 0.2f);
		}
		else
		{
			g_theRenderer->DrawPoint(frame.position, Rgba::WHITE, 0.2f);
		}
		

		g_theRenderer->DrawAxesRotation(frame.position, frame.rotation, 0.5f);

		//Draw a line to the next point if there is a next key frame
		if (currentFrame < totalFrames - 1)
		{
			SceneKeyframe nextFrame = m_cutScene->m_keyFrames[currentFrame + 1];
			std::vector<InterpData> segmentData = GetInterpDataBetweenFrames(frame, nextFrame, totalTime, 10);



			for each (InterpData lineData in segmentData)
			{
				g_theRenderer->DrawLine(lineData.startPosition, lineData.endPosition, lineData.lineColor, 5.f);
			}

		}
		currentFrame++;
	}
	g_theRenderer->SetLineWidth(1.f);
}



CONSOLE_COMMAND(play_scene)
{
	args;
	g_cinematicCameraController->PlayCutscene();
}

CONSOLE_COMMAND(debug_scene_load)
{
	if (args.m_argList.size() < 1)
		return;
	bool foundScene = g_cinematicCameraController->LoadCutscene(args.m_argList[0]);
	if (!foundScene)
	{
		g_theConsole->ConsolePrint("Invalid Cutscene name", Rgba::RED);
	}
}

CONSOLE_COMMAND(debug_scene_off)
{
	args;
	g_cinematicCameraController->m_isDrawDebugScene = false;
}

CONSOLE_COMMAND(debug_scene_on)
{
	args;
	g_cinematicCameraController->m_isDrawDebugScene = true;
}

#else
CinematicCameraController::CinematicCameraController()
	: m_isScenePlaying(false)
	, m_isDrawDebugScene(true)
	, m_curSceneIndex(0)
	, m_curSceneTime(0.f)
	, m_nextSceneStartAtTime(0.f)
{}
void CinematicCameraController::PlayCutscene(){;}

void CinematicCameraController::EndCutscene(){;}
bool CinematicCameraController::LoadCutscene(std::string sceneName) { return false; }
void CinematicCameraController::Update(const float deltaSeconds) { ; }
void CinematicCameraController::RenderDebugCutscene() const { ; }
bool CinematicCameraController::IsScenePlaying() {return false;}
#endif