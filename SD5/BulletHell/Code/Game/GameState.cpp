#include "Game/GameState.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"

StateMachine::StateMachine(GameState state)
{
	m_currentState = state;
}

std::string StateMachine::GetStateAsString() const
{
	if (GetState() == STATE_ATTRACT_SCREEN)
	{
		return Stringf("STATE_ATTRACT_SCREEN");
	}
	else if (GetState() == STATE_READY)
	{
		return Stringf("STATE_READY");
	}
	else if (GetState() == STATE_PLAYING)
	{
		return Stringf("STATE_PLAYING");
	}
	else if (GetState() == STATE_GAMEOVER)
	{
		return Stringf("STATE_GAMEOVER");
	}
	else if (GetState() == STATE_PAUSED)
	{
		return Stringf("STATE_PAUSED");
	}
	else if (GetState() == STATE_VICTORY)
	{
		return Stringf("STATE_VICTORY");
	}
	else if (GetState() == STATE_SHUTDOWN)
	{
		return Stringf("STATE_SHUTDOWN");
	}
}

bool StateMachine::SetState(GameState newState)
{
	bool didStateChange = false;

	if (GetState() != newState)
	{
		m_currentState = newState;
		didStateChange = true;
	}
	else
	{
		ASSERT_OR_DIE(false, "Attempted to set new state to same state!");
	}
}
