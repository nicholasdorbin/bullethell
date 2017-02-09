#pragma once
#include <string>
#include <set>
enum GameState
{
	STATE_ATTRACT_SCREEN,
	STATE_READY,
	STATE_PLAYING,
	STATE_GAMEOVER,
	STATE_PAUSED,
	STATE_VICTORY,
	STATE_SHUTDOWN,
	NUM_GAME_STATES
};




class State
{
public:
	bool m_isStartState;
	GameState m_name;
	//list of potential states…?

	virtual void Update();
};

class StateTransition
{
	State m_fromState;
	State m_toState;
	unsigned char m_transitionOnKey;
};

class StateMachine
{
private:

	std::set<State*> m_stateSet;
	GameState m_currentState;
public:
	StateMachine(GameState state);
	GameState GetState() const { return m_currentState; }
	std::string GetStateAsString() const;
	bool SetState(GameState newState);
};
