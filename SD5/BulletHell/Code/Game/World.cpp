#include "Game/World.hpp"
#include "Game/TheApp.hpp"
//#include "Game/GameCommon.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Game/TheGame.hpp"
#include <string>
#include "Engine/Core/Time.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Game/Entity.hpp"
#include "Game/EnemyShip.hpp"
#include "Game/Meteor.hpp"
#include "Game/Star.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/SpriteRenderer/Sprite.hpp"
#include "Engine/Renderer/SpriteRenderer/AnimatedSprite.hpp"
#include "Game/Map.hpp"

const int WORLD_X = 4;
const int WORLD_Y = 4;
const int STAR_FRAME_FREQUENCY = 2;
const float MAP_ROW_SPAWN_RATE = 0.1f;
const float WAVE_TITLE_SHOW_TIME = 2.f;


World::World()
	: m_startTrace(0.f, 0.f, 0.f)
	, m_endTrace(0.f, 0.f, 0.f)
	, m_player (Player(Vector2(0.f, -2.f)))
	, m_starFrameCount(0)
	, m_worldTimer(0)
	, m_mapTick(0)
	, m_mapIndex(0)
	, m_rowIndex(0)
	, m_waveSpriteTick(0)
	, m_currentWaveSprite(nullptr)
{

	//m_player = Player(Vector3(2.3f, 2.3f, 69.f + (1.85f / 2.f)));
	
	//m_camera.m_position.SetXYZ(2.f, 2.f, 70.f);
	//m_camera.m_position = m_player.m_pos;
	m_moveMod = 1.f;
	for (int i = 0; i < 1; i++)
	{
		float randX = GetRandomFloatBetween(g_spriteRenderSystem->GetVirtualBounds().m_mins.x, g_spriteRenderSystem->GetVirtualBounds().m_maxs.x);
		//m_entities.push_back(new EnemyShip(Vector2(randX, g_spriteRenderSystem->GetVirtualBounds().m_maxs.y)));

	}

	m_mapList.push_back(new Map("Data/Maps/Map1.png"));
	m_mapList.push_back(new Map("Data/Maps/Map2.png"));

// 	m_spriteAttractScreen = Sprite::Create("attract", eGame_SpriteLayers::LAYER_ATTRACT);
// 	m_spriteAttractScreen->m_scale = Vector2(0.2f, 0.2f);

	m_spriteGameOverScreen = Sprite::Create("gameover", eGame_SpriteLayers::LAYER_UI);
	m_spriteGameOverScreen->m_scale = Vector2(0.5f, 0.5f);
	m_spriteGameOverScreen->SetEnabled(false);

	m_spriteVictoryScreen = Sprite::Create("victory", eGame_SpriteLayers::LAYER_UI);
	m_spriteVictoryScreen->m_scale = Vector2(0.5f, 0.5f);
	m_spriteVictoryScreen->SetEnabled(false);


	m_spriteStrikerRound = Sprite::Create("strikerRound", eGame_SpriteLayers::LAYER_UI);
	m_spriteStrikerRound->m_scale = Vector2(0.2f, 0.2f);
	m_spriteStrikerRound->m_position.y += 1.f;
	m_spriteStrikerRound->SetEnabled(false);



	m_spriteSpeedRound = Sprite::Create("speedRound", eGame_SpriteLayers::LAYER_UI);
	m_spriteSpeedRound->m_scale = Vector2(0.2f, 0.2f);
	m_spriteSpeedRound->m_position.y += 1.f;
	m_spriteSpeedRound->SetEnabled(false);

	m_spriteSlayerRound = Sprite::Create("slayerRound", eGame_SpriteLayers::LAYER_UI);
	m_spriteSlayerRound->m_scale = Vector2(0.2f, 0.2f);
	m_spriteSlayerRound->m_position.y += 1.f;
	m_spriteSlayerRound->SetEnabled(false);





	//attractAnim
	m_attractAnimScreen = AnimatedSprite::Create("attractAnim", eGame_SpriteLayers::LAYER_ATTRACT);
	m_attractAnimScreen->m_scale = Vector2(0.2f, 0.2f);
	m_attractAnimScreen->m_position.y += 1.7f;
	m_attractAnimScreen->m_isPlaying = true;


	m_attractReadyAnimScreen = AnimatedSprite::Create("startAnim", eGame_SpriteLayers::LAYER_ATTRACT);
	m_attractReadyAnimScreen->m_scale = Vector2(0.1f, 0.1f);
	m_attractReadyAnimScreen->m_position.y -= 3.2f;
	m_attractReadyAnimScreen->m_isPlaying = false;
}

void World::Update(float deltaSeconds)
{
	if (g_theGame->m_state.GetState() == STATE_READY)
	{
		m_attractAnimScreen->m_animClock->m_scale = 3.f;
		m_attractReadyAnimScreen->m_isPlaying = true;
		m_worldTimer += deltaSeconds;
		
	}
	else
	{
		m_attractAnimScreen->m_animClock->m_scale = 0.8f;
	}

	m_attractAnimScreen->Update();
	m_attractReadyAnimScreen->Update();

	MakeStars();
	UpdateBackGroundEntities(deltaSeconds);

	if (g_theGame->m_state.GetState() == STATE_PLAYING)
	{ 
		LoadMapEnts(deltaSeconds);

		if (g_theGame->m_state.GetState() != STATE_PLAYING)
		{
			return;
		}
		UpdateCameraAndPlayer(deltaSeconds);
		UpdateEntities(deltaSeconds);


		if (m_player.m_isDead)
		{
			g_theGame->m_state.SetState(STATE_GAMEOVER);
			g_spriteRenderSystem->DisableAllButThisLayer(eGame_SpriteLayers::LAYER_UI);
			g_spriteRenderSystem->EnableLayer(eGame_SpriteLayers::LAYER_UI);
			g_spriteRenderSystem->EnableLayer(eGame_SpriteLayers::LAYER_BACKGROUND);
			m_spriteGameOverScreen->SetEnabled(true);
		}
	}
	
}

void World::Render()
{
	g_theRenderer->SetAlphaTest(true, 0.5f);
	//g_theRenderer->DrawAABB3Wireframe(m_player.m_bounds, Rgba::WHITE);
	g_theRenderer->SetAlphaTest(false);
}

void World::UpdateCameraAndPlayer(float deltaSeconds)
{
	if (!g_theApp->m_inputSystem.m_isInFocus)
		return;
	//Raycast endpos : dist * ForwardXYZ().Norm + cameraPos
	IntVector2 cursorPos = g_theApp->m_inputSystem.GetMousePos();
	g_theApp->m_inputSystem.ShowMouse(false);
	g_theApp->m_inputSystem.SetMousePos(snapBackPos.x, snapBackPos.y);
	IntVector2 cursorDelta = cursorPos - snapBackPos;


	if (cursorDelta.x != 0 || cursorDelta.y != 0)
	{
		//m_camera.m_orientation.m_yawDegreesAboutZ -= mouseSensitivity  * (float)cursorDelta.x;
		//m_camera.m_orientation.m_pitchDegreesAboutY += mouseSensitivity  * (float)cursorDelta.y;
		g_camera.m_orientation.m_yawDegreesAboutZ += mouseSensitivity  * (float)cursorDelta.x;
		g_camera.m_orientation.m_pitchDegreesAboutX -= mouseSensitivity  * (float)cursorDelta.y;
	}

	//m_camera.FixAndClampAngles();
	g_camera.FixAndClampAngles();

	m_player.ClearMoveStates();



	



	//Controller Input
	Vector2 leftStick = g_theApp->m_controllers[0].GetLeftStickF();
	Vector2 rightStick = g_theApp->m_controllers[0].GetRightStickF();

	if (leftStick.x != 0 || leftStick.y != 0 || rightStick.x != 0 || rightStick.y != 0)
	{
		float right = leftStick.x;
		float forward = leftStick.y;

		//TODO set mod
		m_player.m_moveModUp = forward;
		m_player.m_moveModRight = right;
		//Forward
		if (forward > 0)
		{
			m_player.m_isMovingForward = true;
			m_player.m_isMovingBackwards = false;

		}

		else if (forward < 0)
		{
			m_player.m_isMovingForward = false;
			m_player.m_isMovingBackwards = true;
		}
		else
		{
			m_player.m_isMovingForward = false;
			m_player.m_isMovingBackwards = false;
		}


		if (right > 0)
		{
			m_player.m_isMovingRight = true;
			m_player.m_isMovingLeft = false;
		}
		else if (right < 0)
		{
			m_player.m_isMovingRight = false;
			m_player.m_isMovingLeft = true;
		}
		else
		{
			m_player.m_isMovingRight = false;
			m_player.m_isMovingLeft = false;
		}
		
	}
	//Keyboard input
	else
	{
		m_player.m_moveModUp = 1.f;
		m_player.m_moveModRight = 1.f;
		if (g_theApp->m_inputSystem.IsKeyDown('W') || g_theApp->m_inputSystem.IsKeyDown(VK_UP))
		{
			m_player.m_isMovingForward = true;
			m_player.m_isMovingBackwards = false;
		}
		else if (g_theApp->m_inputSystem.IsKeyDown('S') || g_theApp->m_inputSystem.IsKeyDown(VK_DOWN))
		{
			m_player.m_moveModUp = -1.f;
			m_player.m_isMovingForward = false;
			m_player.m_isMovingBackwards = true;
		}
		if (g_theApp->m_inputSystem.IsKeyDown('A') || g_theApp->m_inputSystem.IsKeyDown(VK_LEFT))
		{
			m_player.m_moveModRight = -1.f;
			m_player.m_isMovingLeft = true;
			m_player.m_isMovingRight = false;
		}
		else if (g_theApp->m_inputSystem.IsKeyDown('D') || g_theApp->m_inputSystem.IsKeyDown(VK_RIGHT))
		{
			m_player.m_isMovingLeft = false;
			m_player.m_isMovingRight = true;
		}
	}



	if (g_theApp->m_inputSystem.IsKeyDown(VK_SPACE) || g_theApp->m_controllers[0].IsButtonDown(XBOX_A))
	{
		m_player.m_isShooting = true;
	}
	else
	{
		m_player.m_isShooting = false;
	}

	

	m_player.Update(deltaSeconds);




	//TODO: MOUSE WHEEL
	int mouseWheelDir = g_theApp->m_inputSystem.GetMouseWheelDirection();

	if (mouseWheelDir != 0)
	{
		/*
		SoundID sound = g_audio->CreateOrGetSound(BLOCK_SELECT_SOUND);
		g_audio->PlaySound(sound, 0.5f);

		if (mouseWheelDir > 0)
		{
			m_selectedBlockIndex++;
			if (m_selectedBlockIndex > 9)
				m_selectedBlockIndex = 0;
		}
		else
		{
			m_selectedBlockIndex--;
			if (m_selectedBlockIndex < 0)
				m_selectedBlockIndex = 9;
		}
		*/
	}




	//Vector3 cameraForwardDirection = m_camera.GetForwardXYZ().GetNormalized();
	Vector3 cameraForwardDirection = g_camera.GetForwardXYZ().GetNormalized();
	//DebuggerPrintf("Camera Pos: X: %f,Y: %f, Z:%f\nCamForwardXYZ: X: %f,Y: %f, Z:%f\n", g_camera.m_position.x, g_camera.m_position.y, g_camera.m_position.z, cameraForwardDirection.x, cameraForwardDirection.y, cameraForwardDirection.z);

	


}

void World::UpdateEntities(float deltaSeconds)
{
	for each (Entity* ent in m_entities)
	{
		ent->Update(deltaSeconds);
	}


	std::vector<Entity*>::iterator addIter = m_entitiesToAdd.begin();
	while (addIter != m_entitiesToAdd.end()) {
		Entity* entToAdd = *addIter;
		m_entities.push_back(entToAdd);
		addIter = m_entitiesToAdd.erase(addIter);
	}


	//Clean dead ents
	std::vector<Entity*>::iterator removeIter = m_entities.begin();
	while (removeIter != m_entities.end()) {
		Entity* b = *removeIter;
		if (b->m_isDead)
		{
			removeIter = m_entities.erase(removeIter);
			delete b;
		}
		else
			++removeIter;
	}
}

void World::UpdateBackGroundEntities(float deltaSeconds)
{
	for each (Entity* ent in m_entitiesBackGround)
	{
		ent->Update(deltaSeconds);
	}


	std::vector<Entity*>::iterator addIter = m_entitiesBackGroundToAdd.begin();
	while (addIter != m_entitiesBackGroundToAdd.end()) {
		Entity* entToAdd = *addIter;
		m_entitiesBackGround.push_back(entToAdd);
		addIter = m_entitiesBackGroundToAdd.erase(addIter);
	}


	//Clean dead ents
	std::vector<Entity*>::iterator removeIter = m_entitiesBackGround.begin();
	while (removeIter != m_entitiesBackGround.end()) {
		Entity* b = *removeIter;
		if (b->m_isDead)
		{
			removeIter = m_entitiesBackGround.erase(removeIter);
			delete b;
		}
		else
			++removeIter;
	}
}

void World::LoadMapEnts(float deltaSeconds)
{
	m_mapTick += deltaSeconds;

	if (m_currentWaveSprite != nullptr)
	{
		m_waveSpriteTick += deltaSeconds;
		if (m_waveSpriteTick > WAVE_TITLE_SHOW_TIME)
		{
			m_currentWaveSprite->SetEnabled(false);
			m_currentWaveSprite = nullptr;
		}
	}

	if (m_mapTick > MAP_ROW_SPAWN_RATE)
	{
		m_mapTick = 0;
		int rowCount = m_mapList[m_mapIndex]->m_rowData.size() - 1;
		
		if (m_rowIndex > rowCount)
		{
			//Map is done. Load the next one.
			m_mapIndex++;
			m_rowIndex = 0;
			//Any more maps? if not, player wins.
			int mapCount = m_mapList.size() - 1;
			if (m_mapIndex > mapCount)
			{
				g_theGame->m_state.SetState(STATE_VICTORY);
				m_spriteVictoryScreen->SetEnabled(true);
				g_spriteRenderSystem->DisableAllButThisLayer(LAYER_UI);
				g_spriteRenderSystem->EnableLayer(LAYER_BACKGROUND);
				return;
			}
			else
			{
				EnableWaveSpriteForCurrentMap();
				SpawnRow(m_mapList[m_mapIndex]->m_rowData[m_rowIndex]);
				m_rowIndex++;
			}
		}
		else
		{
			SpawnRow(m_mapList[m_mapIndex]->m_rowData[m_rowIndex]);
			m_rowIndex++;
		}

	}
}

void World::EnableWaveSpriteForCurrentMap()
{
	Map* currentMap = m_mapList[m_mapIndex];

	switch (currentMap->m_mapType)
	{
	case MAPTYPE_SPEED:
	{
		m_currentWaveSprite = m_spriteSpeedRound;
		m_currentWaveSprite->SetEnabled(true);
		break;
	}
	case MAPTYPE_STRIKER:
	{
		m_currentWaveSprite = m_spriteStrikerRound;
		m_currentWaveSprite->SetEnabled(true);
		break;
	}
	case MAPTYPE_SLAYER:
	{
		m_currentWaveSprite = m_spriteSlayerRound;
		m_currentWaveSprite->SetEnabled(true);
		break;
	}
	default:
	{
		m_currentWaveSprite = nullptr;
		break;
	}
	}
	m_waveSpriteTick = 0;
}

void World::SpawnRow(MapRow row)
{
	for each (Entity* ent in row.m_rowEntities)
	{
		ent->m_sprite->SetEnabled(true);
		ent->m_pos.y +=  g_spriteRenderSystem->GetVirtualBounds().m_maxs.y - ent->m_bounds.m_mins.y;
		ent->m_sprite->m_position = ent->m_pos;
		ent->m_bounds = ent->GetBounds();
		m_entitiesToAdd.push_back(ent);
	}
}

void World::MakeStars()
{
	m_starFrameCount++;
	if (m_starFrameCount >= STAR_FRAME_FREQUENCY)
	{
		m_starFrameCount = 0;
		//Make a Star

		Star* newStar = new Star();
		Vector2 xBounds = Vector2(newStar->m_bounds.m_mins.x, newStar->m_bounds.m_maxs.x);
		Vector2 yBounds = Vector2(newStar->m_bounds.m_mins.y, newStar->m_bounds.m_maxs.y);
		float width = xBounds.y - xBounds.x;
		float height = yBounds.y - yBounds.x;

		float minX = g_spriteRenderSystem->GetVirtualBounds().m_mins.x + width;
		float maxX = g_spriteRenderSystem->GetVirtualBounds().m_maxs.x - width;
		float xSpawn = GetRandomFloatBetween(minX, maxX);
		float ySpawn = g_spriteRenderSystem->GetVirtualBounds().m_maxs.y + height;

		newStar->m_pos = Vector2(xSpawn, ySpawn);
		newStar->m_sprite->m_position = newStar->m_pos;
		newStar->m_bounds = newStar->GetBounds();

		m_entitiesBackGroundToAdd.push_back(newStar);
	}
}
