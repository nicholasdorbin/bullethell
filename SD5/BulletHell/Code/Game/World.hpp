#pragma once
#include "Game/Player.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/IntVector2.hpp"
#include <map>
#include <deque>
#include <vector>

class Sprite;
class AnimatedSprite;
class Entity;
class Map;
struct MapRow;

const IntVector2 snapBackPos = IntVector2(800, 450);
const float mouseSensitivity = 0.025f;
const float DIST_TO_RAYCAST = 8.f;
const int ACTIVE_CHUNK_RADIUS = 14;
const int FLUSH_CHUNK_RADIUS = 16;
const float MIN_FLUSH_DIST = 16.f * FLUSH_CHUNK_RADIUS;
const float MIN_FLUSH_DIST_SQUARED = MIN_FLUSH_DIST * MIN_FLUSH_DIST;
const float MIN_ACTIVE_DIST = 16.f * ACTIVE_CHUNK_RADIUS;
const float MIN_ACTIVE_DIST_SQUARED = MIN_ACTIVE_DIST * MIN_ACTIVE_DIST;
const float DIG_TIME_SOUND_THRESHOLD = 0.19f;
const float FACE_SELECTION_DIFF = 0.01f;




class World
{
public:
	World();
	Player m_player;
	Camera3D m_camera;
	float m_moveMod;
	float m_worldTimer;
	float m_mapTick;
	float m_waveSpriteTick;
	Vector3 m_startTrace;
	Vector3 m_endTrace;
	std::vector<Entity*> m_entities;
	std::vector<Entity*> m_entitiesBackGround;
	std::vector<Entity*> m_entitiesToAdd;
	std::vector<Entity*> m_entitiesBackGroundToAdd;
	std::vector<Map*> m_mapList;

	int m_starFrameCount;
	int m_mapIndex;
	int m_rowIndex;

	Sprite* m_spriteAttractScreen;
	Sprite* m_spriteGameOverScreen;
	Sprite* m_spriteVictoryScreen;
	Sprite* m_spriteStrikerRound;
	Sprite* m_spriteSpeedRound;
	Sprite* m_spriteSlayerRound;

	Sprite* m_currentWaveSprite;

	AnimatedSprite* m_attractAnimScreen;
	AnimatedSprite* m_attractReadyAnimScreen;

	void Update(float deltaSeconds);
	void UpdateCameraAndPlayer(float deltaSeconds);
	void UpdateEntities(float deltaSeconds);
	void UpdateBackGroundEntities(float deltaSeconds);
	void LoadMapEnts(float deltaSeconds);
	void EnableWaveSpriteForCurrentMap();
	void SpawnRow(MapRow row);
	void MakeStars();
	void Render();
};