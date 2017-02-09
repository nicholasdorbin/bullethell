#pragma once

//#include "Engine/Net/udp/UDPSocket.hpp"
#include "Engine/Net/Session/PacketChannel.hpp"
#include "Engine/Net/Session/NetMessage.hpp"
#include "Engine/Net/Session/NetPacket.hpp"
#include "Engine/Core/Events/EventSystem.hpp"

#include <stdint.h>
#include <map>
#include <vector>

#define GAME_PORT 4334
#define MAX_CONNECTIONS 255
#define Hz30 (1.f / 30.f)
#define Hz50 (1.f / 50.f)
#define BAD_CONNECTION_THRESHOLD 5.f
#define DEAD_CONNECTION_THRESHOLD 15.f





enum eMessageID : uint8_t
{
	eMessageID_PING = 0,
	eMessageID_PONG,
	eMessageID_HEARTBEAT,
	eMessageID_ACK,
	eMessageID_JOIN_REQUEST,
	eMessageID_JOIN_DENY,
	eMessageID_JOIN_ACCEPT,
	eMessageID_CONNECTION_LEFT,
	eMessageID_KICK,
	eMessageID_QUIT,
	eMessageID_INVALID,
	eMessageID_MAX_CORE_MESSAGE

};

enum eNetErrorCode : uint8_t
{
	//Errors
	eNetErrorCode_NO_ERROR = 0,
	eNetErrorCode_START_ERROR_FAILED_TO_CREATE_SOCKET,
	eNetErrorCode_JOIN_ERROR_HOST_TIMEDOUT,
	eNetErrorCode_ERROR_HOST_DISCONNECTED,
	//Denies
	eNetDenyCode_JOIN_DENIED_NOT_ACCEPTING_NEW_CONNECTIONS, //Not listening, game is in progress
	eNetDenyCode_JOIN_DENIED_NOT_HOST,
	eNetDenyCode_JOIN_DENIED_FULL, //Game is full
	eNetDenyCode_JOIN_DENIED_GUID_IN_USE
};


enum eControlFlagID : uint32_t
{
	eControlFlagID_CONNECTIONLESS = (1 << 0)
};

enum eOptionFlagID : uint32_t
{
	eOptionFlagID_RELIABLE = (1 << 0),
	eOptionFlagID_INORDER = (1 << 1)
};


enum eNetSessionState
{
	eNetSessionState_UNINITIALIZED = 0,
	eNetSessionState_DISCONNECTED,
	eNetSessionState_HOSTING,
	eNetSessionState_JOINING,
	eNetSessionState_CONNECTED,
	eNetSessionState_MAX_SESSION_STATES
};

class NetSession;
class NetConnection;


struct SessionEvent : Event
{
	NetConnection* connection;
	uint8_t index;
	float tickRate; //#TODO Implement
	NetMessage* msg;
};

struct NetSender
{
	NetSession* session; //it's a shared session. Not "mine" or "yours"
	sockaddr_in address;
	NetConnection* connection; // Not used right now, using NetSession's UDPSocket
};

typedef void(NetMessageCallback)(const NetSender&, NetMessage&); //typedef returnType(nameOfTypeDef)(args1&, args2&);
// typedef  std::pair<uint8_t, NetConnection*> ConnectionPair;
// typedef  std::map<uint8_t, NetConnection*>  ConnectionMap;
// typedef  std::map<uint8_t, NetConnection*>::iterator  ConnectionMapIter;

struct NetMessageDefinition
{
	uint8_t messageIndex; //PING, PONG, etc.
	const char* debugName;
	NetMessageCallback* cb; 
	//uint8_t flags;
	uint32_t option_flags; //Reliable. how.
	uint32_t control_flags; //Connectionless. The "what", content flags.

	NetMessageDefinition()
		: messageIndex(eMessageID_INVALID),
		debugName("INVALID")
		, cb(nullptr)
	{};
};






class NetSession
{


public:
	PacketChannel m_sessionSocket; //you will send and recv on my socket
	NetMessageDefinition m_messageDefs[256];
	NetConnection* m_connections[MAX_CONNECTIONS];
	//NetConnection* m_connections[MAX_CONNECTIONS];
	NetConnection* m_myConnection;
	float m_timeSinceLastNetTickUpdate;
	float m_joinResponseTime;
	bool m_isHost;
	
	float m_tickRate;

	eNetErrorCode m_lastErrorCode;

	NetSession();
	~NetSession();


	void Start(uint16_t port);
	void Step(const float deltaTime);
	bool Stop();
	bool Host(char const* username);
	bool Join(char const* username, sockaddr_in& hostAddr);
	void Leave();

	bool IsRunning();
	bool IsListening();
	void StartListening();
	void StopListening();
	sockaddr_in GetSessionAddress();

	void SendMessageDirect(sockaddr_in addr, NetMessage& msg);
	void SendMessageToAllConnections(NetMessage& msg);
	void SendMessageToAllConnectionsExcluding(NetMessage& msg, uint8_t indexToExclude);

	void ProcessIncomingPackets();

	bool ReadNextPacketFromSocket(NetPacket* outPacket, sockaddr_in* outAddr);
	bool ReadNextMessageFromPacket(NetMessage* outMessage, NetPacket* packet, NetConnection* connection);

	void RegisterMessage(uint8_t id, char const *debug_name, NetMessageCallback* cb, uint32_t control_flag = 0, uint32_t option_flag = 0);

	void AddDefinition(uint8_t messageIndex, NetMessageDefinition& defn);
	const NetMessageDefinition* FindDefinition(short messageType);

	NetConnection* CreateConnection(uint8_t index, char const* guid, sockaddr_in connAddr); //creates a connection if the slot is free;
	bool DestroyConnection(uint8_t index);
	NetConnection* GetConnection(uint8_t index);
	NetConnection* GetConnectionByAddr(sockaddr_in* connAddr);
	uint8_t GetNextAvailableConnIndex();

	bool CanProcessMessage(NetSender& from, NetMessage const &msg);
	bool IsConnectionBad(NetConnection* connection);

	bool IsConnectionDead(NetConnection* connection);
	void SetLastRecvTimeStamp(double timeStamp);
	void SetLastSentTimeStamp(double timeStamp);

	double GetLastRecvTimeStamp();
	double GetLastSentTimeStamp();

	//States
	void SetState(eNetSessionState state);
	void EnterState(eNetSessionState state);
	void LeaveState(eNetSessionState state);
	void UpdateState(const float deltaTime);


	//Joining
	void OnEnterJoin();
	void UpdateJoining(const float deltaTime);

	//Disconnect
	void OnEnterDisconnect();

	std::string GetSessionStateName();
	bool IsGUIDFree(const char* guid);

	//void OnPingReceived(const NetSender& sender, NetMessage& msg);
	//void OnPongReceived(const NetSender& sender, NetMessage& msg);

private:
	double m_lastSentTimeStampOnSession;
	double m_lastRecvTimeStampOnSession;
	bool m_isListening;
	
	eNetSessionState m_sessionState;
};

