#include "Engine/Net/Session/NetSession.hpp"
#include "Engine/Net/Session/NetConnection.hpp"

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Net/NetworkSystem.hpp"
#include "Engine/Utils/ByteUtils.hpp"

#include "Engine/Core/Console.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Time.hpp"


NetSession::NetSession()
	:m_lastErrorCode(eNetErrorCode_NO_ERROR),
	m_myConnection(nullptr),
	m_timeSinceLastNetTickUpdate(0.f),
	m_tickRate(Hz50),
	m_lastSentTimeStampOnSession(0),
	m_lastRecvTimeStampOnSession(0),
	m_sessionState(eNetSessionState_UNINITIALIZED),
	m_isListening(true),
	m_isHost(false)
{
	for (uint8_t i = 0u; i < MAX_CONNECTIONS; i++)
	{
		m_connections[i] = nullptr;
	}
	

}

NetSession::~NetSession()
{
	for each (NetConnection* conn in m_connections)
	{
		if (conn == m_myConnection)
		{
			m_myConnection = nullptr;
		}
		delete conn;
	}
}

void NetSession::Start(uint16_t port)
{
	//Make the socket
	for (uint16_t i = 0; i < 8; i++)
	{
		uint16_t outPort = port + i;
		const char* portAsString = g_networkSystem->GetPortAsString(outPort);
		m_sessionSocket = PacketChannel(g_networkSystem->GetLocalHostName(), portAsString);
		if (m_sessionSocket.IsBound())
		{
			SetState(eNetSessionState_DISCONNECTED);
			m_sessionState = eNetSessionState_DISCONNECTED;
			return;
		}
		else
		{
			m_lastErrorCode = eNetErrorCode_START_ERROR_FAILED_TO_CREATE_SOCKET;
		}
	}
	
	
}

void NetSession::Step(const float deltaTime)
{
	ProcessIncomingPackets();

	m_timeSinceLastNetTickUpdate += deltaTime;
	UpdateState(deltaTime);
	if (m_timeSinceLastNetTickUpdate >= m_tickRate)
	{
		for (uint8_t i = 0u; i < MAX_CONNECTIONS; i++)
		{
			NetConnection* thisConnection = GetConnection(i);
			if (thisConnection == nullptr)
				continue;
			if (IsConnectionBad(thisConnection))
			{
				thisConnection->m_connectionState = eNetConnectionState_BAD;
				if (IsConnectionDead(thisConnection))
				{
					DestroyConnection(thisConnection->m_index); //#TODO Works, but re-enable before commiting. Disabled to debug easier
				}
			}
			else
			{
				SessionEvent eventData;
				eventData.connection = thisConnection;
				eventData.index = i;
				EventSystem::GetInstance()->TriggerEvent("onNetTick", &eventData);
				if (thisConnection->HasMessagesToSend())
				{
					thisConnection->SendPacket();
				}
				else 
				{

				}
			}
		}
		m_timeSinceLastNetTickUpdate = 0.f;
	}

}

bool NetSession::Stop()
{
	if (m_sessionState != eNetSessionState_DISCONNECTED)
	{
		return false;
	}
	m_sessionSocket.CloseSocket();
	SetState(eNetSessionState_UNINITIALIZED);
	return true;
}

bool NetSession::Host(char const* username)
{
	m_isHost = true;
	if (m_sessionState != eNetSessionState_DISCONNECTED)
	{
		return false;
	}
	sockaddr_in myAddr = GetSessionAddress();

	std::string addrStr = g_networkSystem->SockAddrToString((sockaddr*)&myAddr);

	CreateConnection(0, username, myAddr);

	SetState(eNetSessionState_CONNECTED);

	return true;
}

bool NetSession::Join(char const* username, sockaddr_in& hostAddr)
{
	if (m_sessionState != eNetSessionState_DISCONNECTED)
	{
		ERROR_RECOVERABLE("Trying to join in a non Disconnected state");
		return false;
	}

	//Make a host with no name, suppliedAddr, and conn index of 0
	NetConnection* host = CreateConnection(0, "host", hostAddr);
	//Make a connection for yourself with supplied name, local addr, and invalid conn index (this is updated on JOIN_ACCEPT)
	sockaddr_in myAddr = GetSessionAddress();
	NetConnection* me = CreateConnection(INVALID_CONNECTION_INDEX, username, myAddr);
	//Send host a JOIN_REQUEST
	
	NetMessage* reliableMessage = new NetMessage(eMessageID_JOIN_REQUEST);
	reliableMessage->m_definition = g_netSession->FindDefinition(reliableMessage->m_messageType);
	
	reliableMessage->WriteStringToMessage(me->m_guid);
	reliableMessage->FinalizeMessage();

	host->AddMessage(reliableMessage);
	host->SendPacket();

	SetState(eNetSessionState_JOINING);
	return true;
}

void NetSession::Leave()
{
	if (m_sessionState == eNetSessionState_CONNECTED)
	{
		if (!m_isHost)
		{
			uint8_t hostIndex = 0u;
			NetConnection* hostConnection = GetConnection(hostIndex);
			if (hostConnection != nullptr)
			{
				uint8_t leaverIndex = m_myConnection->m_index;
				NetMessage* leaveMessage = new NetMessage(eMessageID_CONNECTION_LEFT);
				leaveMessage->m_definition = g_netSession->FindDefinition(leaveMessage->m_messageType);
				
				leaveMessage->WriteToMessage<uint8_t>(&leaverIndex);
				leaveMessage->FinalizeMessage();
				hostConnection->AddMessage(leaveMessage);
				hostConnection->SendPacket();
			}
		}
		SetState(eNetSessionState_DISCONNECTED);
		
	}
	else
	{
		ERROR_RECOVERABLE("Can't call Leave from a non-connected state");
	}
}

bool NetSession::IsRunning()
{
	return (m_sessionState != eNetSessionState_UNINITIALIZED);
}

bool NetSession::IsListening()
{
	return m_isListening;
}

void NetSession::StartListening()
{
	m_isListening = true;
}

void NetSession::StopListening()
{
	m_isListening = false;
}

sockaddr_in NetSession::GetSessionAddress()
{
	return m_sessionSocket.m_socket.m_addrIn;
}

void NetSession::SendMessageDirect(sockaddr_in addr, NetMessage& msg)
{
	if (!IsRunning())
	{
		return;
	}

	NetPacket outPacket;
	uint8_t fromConnectionIndex = INVALID_BYTE;
	uint8_t msgCount = 1;
	//packet header
	const NetMessageDefinition* def = FindDefinition(msg.m_messageType);

	if (def == nullptr)
	{
		return;
	}

	if (m_myConnection != nullptr)
	{
		fromConnectionIndex = m_myConnection->m_index;
	}
	outPacket.Write<uint8_t>(&fromConnectionIndex);


	uint16_t nextSentAckID = INVALID_PACKET_ACK;
	uint16_t highestReceivedAckID = INVALID_PACKET_ACK;
	uint16_t prevHighestReceivedAcksBitfield = INVALID_PACKET_ACK;

	outPacket.Write<uint16_t>(&nextSentAckID);
	outPacket.Write<uint16_t>(&highestReceivedAckID);
	outPacket.Write<uint16_t>(&prevHighestReceivedAcksBitfield);



	outPacket.Write<uint8_t>(&msgCount); 



	size_t my_size = msg.GetHeaderSize() + msg.GetPayloadSize();
	//size_t total = my_size + sizeof(uint8_t);
	if(outPacket.GetWriteableBytes() >= my_size)
	{
		size_t msgSize = msg.GetTypePayloadSize();
		outPacket.Write<uint16_t>(&msgSize); //msg.size
		outPacket.Write<uint8_t>(&msg.m_messageType); //msg header

		if (msg.IsReliable())
		{
			outPacket.Write<uint16_t>(&msg.m_reliableID);
		}


		void* buffer = (byte_t*)msg.m_buffer + msg.GetHeaderSize();
		outPacket.WriteForward(buffer, msg.GetPayloadSize()); // msg payload
	}
	m_sessionSocket.SendTo(addr, outPacket.m_buffer, outPacket.GetTotalReadableBytes());
	

}

void NetSession::SendMessageToAllConnections(NetMessage& msg)
{
	//for each
	for (uint8_t i = 0u; i < MAX_CONNECTIONS; i++)
	{
		if (m_connections[i] == nullptr)
		{ 
			continue;
		}

		NetConnection* thisConnection = m_connections[i];
		//make a copy, Add copy to connection
		NetMessage* copyMessage = new NetMessage(msg);
		thisConnection->AddMessage(copyMessage);
		//We don't send right away, that will be handled on the next tick
		//thisConnection->SendPacket();
	}

}

void NetSession::SendMessageToAllConnectionsExcluding(NetMessage& msg, uint8_t indexToExclude)
{
	for (uint8_t i = 0u; i < MAX_CONNECTIONS; i++)
	{
		if (i == indexToExclude)
		{
			continue;
		}

		if (m_connections[i] == nullptr)
		{
			continue;
		}

		if (m_connections[i] == m_myConnection)
		{
			continue;
		}

		
		NetConnection* thisConnection = m_connections[i];
		//make a copy, Add copy to connection
		NetMessage* copyMessage = new NetMessage(msg);
		thisConnection->AddMessage(copyMessage);
		//We don't send right away, that will be handled on the next tick
		//thisConnection->SendPacket();
	}
}

void NetSession::ProcessIncomingPackets()
{
	NetPacket packet;
	NetSender from;
	from.session = this;

	while (ReadNextPacketFromSocket(&packet, &from.address) )
	{

		uint8_t fromConnIndex;
		uint16_t nextSentAckID;
		uint16_t highestReceivedAckID;
		uint16_t prevHighestReceivedAcksBitfield;

		packet.Read<byte_t>(&fromConnIndex);

		packet.Read<uint16_t>(&nextSentAckID);
		packet.Read<uint16_t>(&highestReceivedAckID);
		packet.Read<uint16_t>(&prevHighestReceivedAcksBitfield);

		packet.m_header.connectionIndex = fromConnIndex;
		packet.m_header.ackID = nextSentAckID;
		packet.m_header.highestReceivedAckID = highestReceivedAckID;
		packet.m_header.prevHighestReceivedAcksBitfield = prevHighestReceivedAcksBitfield;



		/*
		totalSize += Read<uint16_t>(&nextSentAckID);
	totalSize += Read<uint16_t>(&highestReceivedAckID);
	totalSize += Read<uint16_t>(&prevHighestReceivedAcksBitfield);
		
		*/
		from.connection = GetConnection(fromConnIndex);
		if (from.connection == nullptr)
		{
			from.connection = GetConnectionByAddr(&from.address);
		}

		uint8_t numMessages;
		packet.Read<byte_t>(&numMessages);

		for (uint8_t i = 0; i < numMessages; i++)
		{
			NetMessage msg = NetMessage();
			if (ReadNextMessageFromPacket(&msg, &packet, from.connection))
			{
				if (msg.m_messageType == 11)
				{
					//int breakhere = 0;
				}
				if (CanProcessMessage(from, msg))
				{
					DebuggerPrintf("Received '%s' message.\n", msg.m_definition->debugName);
					if (from.connection != nullptr) {
						from.connection->ProcessMessage(from, msg);
					}
					else {
						//msg.process(from);
						msg.m_definition->cb(from, msg);//#TODO Maybe send this guy off on a Job? So we don't stall here
						if (from.connection == nullptr)
						{
							//Find the connection via address and if it exists, set the connection
							from.connection = GetConnectionByAddr(&from.address);

						}
					}


					if (from.connection != nullptr) {
						from.connection->MarkMessageReceived(msg);
					}
					
				}
				if (from.connection != nullptr) 
				{
					from.connection->MarkPacketReceived(&packet);
				}


			}
		}
	}

	
}

bool NetSession::ReadNextPacketFromSocket(NetPacket* outPacket, sockaddr_in* outAddr)
{
	size_t read = m_sessionSocket.PacketReceiveFrom(outAddr, outPacket, PACKET_MTU);
	if (read == 0)
	{
		return false;
	}
	outPacket->SetContentSizeFromBuffer();
	return true;
}

bool NetSession::ReadNextMessageFromPacket(NetMessage* outMessage, NetPacket* packet, NetConnection* connection)
{
	connection;
	//MessageLen (ID + Payload)
	//MessageID
	//Payload

	short msgLen;
	packet->Read<short>(&msgLen);
	outMessage->Write<short>(&msgLen);

	byte_t msgID;
	packet->Read<byte_t>(&msgID);
	outMessage->Write<byte_t>(&msgID);
	outMessage->m_messageType = msgID;

	outMessage->m_definition = FindDefinition(msgID);

	if (outMessage->IsReliable() || outMessage->IsInOrder())
	{
		int lenToRemove = 1;
		if (outMessage->IsReliable())
		{
			uint16_t reliableID;
			packet->Read<uint16_t>(&reliableID);
			outMessage->Write<uint16_t>(&reliableID);
			outMessage->m_reliableID = reliableID;
			lenToRemove += 2;
		}

		if (outMessage->IsInOrder())
		{
			uint16_t sequenceID;
			packet->Read<uint16_t>(&sequenceID);
			outMessage->Write<uint16_t>(&sequenceID);
			outMessage->m_sequenceID = sequenceID;
			lenToRemove += 2;
		}


		byte_t* packetData[MESSAGE_MTU];
		packet->ReadForward(&packetData, msgLen - lenToRemove);
		outMessage->WriteForward(&packetData, msgLen - lenToRemove);

	}
	else
	{
		byte_t* packetData[MESSAGE_MTU];
		packet->ReadForward(&packetData, msgLen - 1);

		outMessage->WriteForward(&packetData, msgLen - 1);
	}

	
// 	const char* str;
// 	str = packet->ReadString();
// 	outMessage->WriteString(str);

	return true;
}

void NetSession::RegisterMessage(uint8_t id, char const *debug_name, NetMessageCallback* cb, uint32_t control_flag, uint32_t option_flag)
{
	if (IsRunning())
		return;
	NetMessageDefinition defn;
	defn.messageIndex = id;
	defn.debugName = debug_name;
	defn.cb = cb;
	defn.control_flags = control_flag;
	defn.option_flags = option_flag;

	if (FindDefinition(id) == nullptr) {
		AddDefinition(id, defn); //#TODO Assert before we add
	}

}

void NetSession::AddDefinition(uint8_t messageIndex, NetMessageDefinition& defn)
{
	m_messageDefs[messageIndex] = defn;
}

const NetMessageDefinition* NetSession::FindDefinition(short messageType)
{
	NetMessageDefinition* defn = &m_messageDefs[messageType];
	if (defn->messageIndex == eMessageID_INVALID)
		return nullptr;
	else
	{
		return defn;
	}
}




NetConnection*  NetSession::CreateConnection(uint8_t index, char const* guid, sockaddr_in connAddr) //creates a connection if the slot is free
{

	NetConnection* foundConnection = GetConnection(index);

	if (foundConnection != nullptr)
	{
		return nullptr;
	}

	NetConnection* newConnection = new NetConnection(index, connAddr, guid, this);
	m_connections[index] = newConnection;

	if ((connAddr.sin_addr.S_un.S_addr == m_sessionSocket.m_socket.m_addrIn.sin_addr.S_un.S_addr) && (connAddr.sin_port == m_sessionSocket.m_socket.m_addrIn.sin_port))
	{
		m_myConnection = newConnection;
		m_myConnection->m_connectionState = eNetConnectionState_LOCAL;
	}
	else
	{
		newConnection->m_connectionState = eNetConnectionState_UNCONFIRMED;
	}




	SessionEvent eventData;
	eventData.connection = newConnection;
	EventSystem::GetInstance()->TriggerEvent("onConnectionJoin", &eventData);

	return newConnection;
}




bool NetSession::DestroyConnection(uint8_t index)
{
	if (index > MAX_CONNECTIONS)
		return nullptr;

	NetConnection* foundConnection = GetConnection(index);

	

	if (foundConnection == nullptr)
	{
		return false;
	}

	if (m_myConnection != nullptr && m_myConnection->m_index == foundConnection->m_index)
	{
		m_myConnection = nullptr;
	}

	m_connections[index] = nullptr;

	SessionEvent eventData;
	eventData.index = foundConnection->m_index;
	eventData.connection = foundConnection; //Game can't really use it if we're deleting it, but we'll send it anyways.
	EventSystem::GetInstance()->TriggerEvent("onConnectionLeave", &eventData);

	delete foundConnection;
	return true;
}

NetConnection* NetSession::GetConnection(uint8_t index)
{

	if (index > MAX_CONNECTIONS || index < 0)
		return nullptr;

	return m_connections[index];
}



NetConnection* NetSession::GetConnectionByAddr(sockaddr_in* connAddr)
{
	for (uint8_t i = 0u; i < MAX_CONNECTIONS; i++)
	{
		NetConnection* thisConnection = GetConnection(i);
		if (thisConnection == nullptr)
			continue;

		sockaddr_in* thisConnAddr = &thisConnection->m_address;

		if (thisConnAddr->sin_addr.S_un.S_addr == connAddr->sin_addr.S_un.S_addr &&
			thisConnAddr->sin_port == connAddr->sin_port)
		{
			return thisConnection;
		}
	}
	return nullptr;
}

uint8_t NetSession::GetNextAvailableConnIndex()
{
	for (uint8_t i = 0u; i < MAX_CONNECTIONS; i++)
	{
		NetConnection* thisConnection = GetConnection(i);
		if (thisConnection == nullptr)
			return i;
	}
	return MAX_CONNECTIONS;
}

bool NetSession::CanProcessMessage(NetSender& from, NetMessage const &msg)
{
	//check definition
	//if no connection, make sure it's connectionless
	//else if connection, do connection's can process

	if (msg.m_definition != nullptr) 
	{
		if (from.connection != nullptr) 
		{
			return from.connection->CanProcessMessage(from, msg);
		}
		else
		{
			if (msg.m_definition->control_flags == eControlFlagID_CONNECTIONLESS)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}
	else
	{
		return false;
	}

}

bool NetSession::IsConnectionBad(NetConnection* connection)
{
	if (connection->m_connectionState == eNetConnectionState_LOCAL)
	{
		return false;
	}

	float diff = (float)GetCurrentTimeSeconds() - (float)connection->m_lastReceivedTimestamp;

	if (diff >= BAD_CONNECTION_THRESHOLD)
	{
		return true;
	}
	return false;
}

bool NetSession::IsConnectionDead(NetConnection* connection)
{
	if (connection->m_connectionState == eNetConnectionState_LOCAL)
	{
		return false;
	}

	float diff = (float)GetCurrentTimeSeconds() - (float)connection->m_lastReceivedTimestamp;

	if (diff >= DEAD_CONNECTION_THRESHOLD && connection->m_connectionState == eNetConnectionState_BAD)
	{
		return true;
	}
	return false;
}


void NetSession::SetLastRecvTimeStamp(double timeStamp)
{
	if (timeStamp > m_lastRecvTimeStampOnSession)
		m_lastRecvTimeStampOnSession = timeStamp;
}

void NetSession::SetLastSentTimeStamp(double timeStamp)
{
	if (timeStamp > m_lastSentTimeStampOnSession)
		m_lastSentTimeStampOnSession = timeStamp;
}

double NetSession::GetLastRecvTimeStamp()
{
	return m_lastRecvTimeStampOnSession;
}

double NetSession::GetLastSentTimeStamp()
{
	return m_lastSentTimeStampOnSession;
}

void NetSession::SetState(eNetSessionState state)
{
	if (state == m_sessionState)
	{
		ERROR_AND_DIE("Trying to set state to current State.");
	}

	LeaveState(m_sessionState);
	EnterState(state);
	m_sessionState = state;
}



void NetSession::EnterState(eNetSessionState state)
{
	switch (state)
	{
	case eNetSessionState_UNINITIALIZED:
	{
		break;
	}
	case eNetSessionState_DISCONNECTED:
	{
		OnEnterDisconnect();
		break;
	}
	case eNetSessionState_CONNECTED:
	{
		break;
	}
	case eNetSessionState_HOSTING:
	{
		break;
	}
	case eNetSessionState_JOINING:
	{
		OnEnterJoin();
		break;
	}

	default:
	{
		break;
	}
	}
}

void NetSession::LeaveState(eNetSessionState state)
{
	switch (state)
	{
	case eNetSessionState_UNINITIALIZED:
	{
		break;
	}
	case eNetSessionState_DISCONNECTED:
	{
		break;
	}
	case eNetSessionState_CONNECTED:
	{
		break;
	}
	case eNetSessionState_HOSTING:
	{
		break;
	}
	case eNetSessionState_JOINING:
	{
		break;
	}

	default:
	{
		break;
	}
	}
}

void NetSession::UpdateState(const float deltaTime)
{
	switch (m_sessionState)
	{
	case eNetSessionState_UNINITIALIZED:
	{
		break;
	}
	case eNetSessionState_DISCONNECTED:
	{
		break;
	}
	case eNetSessionState_CONNECTED:
	{
		break;
	}
	case eNetSessionState_HOSTING:
	{
		break;
	}
	case eNetSessionState_JOINING:
	{
		UpdateJoining(deltaTime);
		break;
	}

	default:
	{
		break;
	}
	}
}

void NetSession::OnEnterDisconnect()
{
	for (uint8_t i = 0u; i < MAX_CONNECTIONS; i++)
	{
		DestroyConnection(i);
	}
	m_isHost = false;
	
}

void NetSession::OnEnterJoin()
{
	m_joinResponseTime = 0.f;
}

void NetSession::UpdateJoining(const float deltaTime)
{
	m_joinResponseTime += deltaTime;

	if (m_joinResponseTime >= DEAD_CONNECTION_THRESHOLD)
	{
		m_lastErrorCode = eNetErrorCode_JOIN_ERROR_HOST_TIMEDOUT;
		g_theConsole->ConsolePrint("ERROR: Host timed out.", Rgba::RED);
		SetState(eNetSessionState_DISCONNECTED);
	}
}

std::string NetSession::GetSessionStateName()
{
	switch (m_sessionState)
	{
	case eNetSessionState_UNINITIALIZED:
	{
		return "UNINITIALIZED";
	}
	case eNetSessionState_DISCONNECTED:
	{
		return "UNCONNECTED";
	}
	case eNetSessionState_CONNECTED:
	{
		return "CONNECTED";
	}
	case eNetSessionState_HOSTING:
	{
		return "HOSTING";
	}
	case eNetSessionState_JOINING:
	{
		return "JOINING";
	}

	default:
	{
		return "?";
	}
	}

	
	
}

bool NetSession::IsGUIDFree(const char* guid)
{

	for (uint8_t i = 0u; i < MAX_CONNECTIONS; i++)
	{
		NetConnection* thisConnection = GetConnection(i);
		if (thisConnection == nullptr)
			continue;
		if (strcmp(guid, thisConnection->m_guid) == 0)
		{
			return false;
		}
	}

	return true;
}

//Events


void OnPingReceived(const NetSender& sender, NetMessage& msg)
{
	msg;
	//char buffer[256];
	msg.ResetOffset();
	msg.m_offset = msg.GetHeaderSize();
	const char *str = msg.ReadString();
	msg.ResetOffset();

	std::string endStr = (str != nullptr ? str : "null");

	std::string addrStr = g_networkSystem->SockAddrToString((sockaddr*)(&sender.address));

	g_theConsole->ConsolePrint("Ping received from: " + addrStr + "[" + endStr + "]");

// 	LogPrintf("Ping Receieved from %s. [%s]",
// 		SockAddrToString(sender.address),
// 		((nullptr != str) ? str : "null"));

	NetMessage pong(eMessageID_PONG);
	pong.AssembleMessage();
	g_netSession->SendMessageDirect(sender.address, pong);
	
}



void OnPongReceived(const NetSender& sender, NetMessage& msg)
{
	msg;
	std::string addrStr = g_networkSystem->SockAddrToString((sockaddr*)(&sender.address));
	g_theConsole->ConsolePrint("Pong received from " + addrStr);
// 	LogPrintf("Pong received from %s.",
// 		SockAddrToString(sender.address));

}


void OnHeartbeatReceived(const NetSender& sender, NetMessage& msg)
{
	sender;
	msg;
	//Shouldn't have to do anything, this updates the last recv time

}

void OnJoinRequestReceived(const NetSender& sender, NetMessage& msg)
{
	if (g_netSession->m_myConnection == nullptr)
		return;

	//Get GUID name
	msg.ResetOffset();
	msg.m_offset = msg.GetHeaderSize();
	const char *theirGuid = msg.ReadString();

	msg.ResetOffset();


	if (!g_netSession->IsListening())
	{
		NetMessage deny(eMessageID_JOIN_DENY);
		deny.m_definition = g_netSession->FindDefinition(deny.m_messageType);
		uint8_t denyValue = eNetDenyCode_JOIN_DENIED_NOT_ACCEPTING_NEW_CONNECTIONS;
		deny.WriteToMessage<uint8_t>(&denyValue);
		deny.FinalizeMessage();
		g_netSession->SendMessageDirect(sender.address, deny);
	}


	//Check to see if requested GUID name is free
	if (g_netSession->IsGUIDFree(theirGuid))
	{
		//if it's free
		//Make a new Connection from this addr
		uint8_t newIndex = g_netSession->GetNextAvailableConnIndex();
		NetConnection* newConnection = g_netSession->CreateConnection(newIndex, theirGuid, sender.address);


		//Send the following: Host GUID, their connection index
		NetMessage* reliableMessage = new NetMessage(eMessageID_JOIN_ACCEPT);
		reliableMessage->m_definition = g_netSession->FindDefinition(reliableMessage->m_messageType);
		
		reliableMessage->WriteStringToMessage(g_netSession->m_myConnection->m_guid);
		reliableMessage->WriteToMessage<uint8_t>(&newIndex);

		//Let game add what they need
		SessionEvent eventData;
		eventData.index = newIndex;
		eventData.msg = reliableMessage;
		EventSystem::GetInstance()->TriggerEvent("onConnectionJoinRequestAccept", &eventData);


		reliableMessage->FinalizeMessage();
		newConnection->AddMessage(reliableMessage);
		newConnection->SendPacket();
	}
	else
	{
		//else Deny
		//eNetDenyCode_JOIN_DENIED_GUID_IN_USE
		NetMessage deny(eMessageID_JOIN_DENY);
		deny.m_definition = g_netSession->FindDefinition(deny.m_messageType);
		uint8_t denyValue = eNetDenyCode_JOIN_DENIED_GUID_IN_USE;
		deny.WriteToMessage<uint8_t>(&denyValue);
		deny.FinalizeMessage();
		g_netSession->SendMessageDirect(sender.address, deny);
	}

}

void OnJoinAcceptReceived(const NetSender&, NetMessage& msg)
{
	//Update host with correct info
	msg.ResetOffset();
	msg.m_offset = msg.GetHeaderSize();
	const char *theirGuid = msg.ReadString();
	
	uint8_t myNewIndex;
	msg.Read<uint8_t>(&myNewIndex);



	uint8_t hostIndex = 0u;
	NetConnection* hostConnection = g_netSession->GetConnection(hostIndex);
	if (hostConnection == nullptr)
	{
		return;
	}
	memcpy((unsigned char*)hostConnection->m_guid, theirGuid, MAX_GUID_LENGTH);
	

	//Update self with final info
	g_netSession->m_myConnection->m_index = myNewIndex;
	//Mark self as connected, call appropriate callbacks
	g_netSession->SetState(eNetSessionState_CONNECTED);

	SessionEvent eventData;
	eventData.index = myNewIndex;
	eventData.connection = g_netSession->m_myConnection;
	eventData.msg =  &msg;
	EventSystem::GetInstance()->TriggerEvent("onConnectionJoinAccept", &eventData);
}

void OnJoinDenyReceived(const NetSender&, NetMessage& msg)
{
	//set error with deny reason, move to DISCONNECTED
	msg.ResetOffset();
	msg.m_offset = msg.GetHeaderSize();

	uint8_t denyReason;
	msg.Read<uint8_t>(&denyReason);

	g_netSession->m_lastErrorCode = (eNetErrorCode)denyReason;

	g_netSession->SetState(eNetSessionState_DISCONNECTED);

	
}

void OnConnectionLeftReceived(const NetSender& sender, NetMessage& msg)
{
	sender;
	msg.ResetOffset();
	msg.m_offset = msg.GetHeaderSize();
	uint8_t leaverIndex;
	msg.Read<uint8_t>(&leaverIndex);

	g_netSession->DestroyConnection(leaverIndex);

	if (g_netSession->m_isHost)
	{
		//SendToAll
		NetMessage leaveMessage = NetMessage(eMessageID_CONNECTION_LEFT);
		leaveMessage.m_definition = g_netSession->FindDefinition(leaveMessage.m_messageType);
		leaveMessage.WriteToMessage<uint8_t>(&leaverIndex);
		leaveMessage.FinalizeMessage();

		g_netSession->SendMessageToAllConnections(leaveMessage);

	}
	SessionEvent eventData;
	eventData.index = leaverIndex;
	EventSystem::GetInstance()->TriggerEvent("onConnectionLeave", &eventData);
}






//Console Commands


//-----------------------------------------------------------------------------------------------
CONSOLE_COMMAND(net_session_start)
{
	args;
	if (g_netSession != nullptr)
	{

		if (g_netSession->IsRunning())
		{
			g_theConsole->ConsolePrint("ERROR: Already running a session", Rgba::RED);
			return;
		}
		g_netSession->RegisterMessage(eMessageID_PING, "ping", OnPingReceived, eControlFlagID_CONNECTIONLESS);
		g_netSession->RegisterMessage(eMessageID_PONG, "pong", OnPongReceived, eControlFlagID_CONNECTIONLESS);
		g_netSession->RegisterMessage(eMessageID_HEARTBEAT, "heartbeat", OnHeartbeatReceived, eControlFlagID_CONNECTIONLESS);
		g_netSession->RegisterMessage(eMessageID_JOIN_REQUEST, "joinRequest", OnJoinRequestReceived, eControlFlagID_CONNECTIONLESS, eOptionFlagID_RELIABLE);
		g_netSession->RegisterMessage(eMessageID_JOIN_ACCEPT, "joinAccept", OnJoinAcceptReceived, 0u,  eOptionFlagID_RELIABLE);
		g_netSession->RegisterMessage(eMessageID_JOIN_DENY, "joinDeny", OnJoinDenyReceived);
		g_netSession->RegisterMessage(eMessageID_CONNECTION_LEFT, "connectionLeft", OnConnectionLeftReceived);

		g_netSession->Start(GAME_PORT);

		if (!g_netSession->IsRunning())
		{
			g_theConsole->ConsolePrint("ERROR: Could not start Session", Rgba::RED);
		}
		else
		{
			std::string addr = g_networkSystem->SockAddrToString((sockaddr*)(&g_netSession->m_sessionSocket.m_socket.m_addrIn));
			g_theConsole->ConsolePrint("Session connected at " + addr, Rgba::GREEN);
		}
	}
}


//-----------------------------------------------------------------------------------------------
CONSOLE_COMMAND(net_session_stop)
{
	args;
	if (g_netSession != nullptr)
	{
		if (g_netSession->IsRunning())
		{
			if (g_netSession->Stop())
			{
				g_theConsole->ConsolePrint("Session Stopped", Rgba::GREEN);
			}
			else
			{
				g_theConsole->ConsolePrint("Error: Could not stop session", Rgba::RED); //#TODO More details
			}
		}
		else
		{
			g_theConsole->ConsolePrint("No UDP session running", Rgba::RED);
		}
	}

}


CONSOLE_COMMAND(net_ping)
{
	args;
	if (g_netSession == nullptr)
		return;

	if (args.m_argList.size() == 0)
	{
		uint16_t port = (uint16_t)atoi("4334");
		const char* ip = "192.168.1.12";//g_networkSystem->GetLocalHostName();
		sockaddr_in to = g_networkSystem->StringToSockAddrIPv4(ip, port);

		NetMessage msg(eMessageID_PING);
		char const *optional_message = "Hello";
		msg.AssembleMessage(optional_message);
		g_netSession->SendMessageDirect(to, msg);
	}


	if (g_netSession != nullptr && args.m_argList.size() >= 2)
	{

		// so it's expecting a full address string,
		// ex: 192.168.52.127:4334
		uint16_t port = (uint16_t)atoi(args.m_argList[1].c_str());
		sockaddr_in to = g_networkSystem->StringToSockAddrIPv4(args.m_argList[0].c_str(), port);
// 		if ( !SockAddrFromString( &to, args.get_next_string() ) ) 
// 		{
// 		
// 		  // Must provide a valid address error
// 		  return;
// 		}
		
		NetMessage msg(eMessageID_PING);
		
		if (args.m_argList.size() > 2)
		{
			char const *optional_message = args.m_argList[2].c_str();
			msg.AssembleMessage(optional_message);
		}
		else
		{
			msg.AssembleMessage();
		}
		
		
		// When I test your program, I will be sending multiple
		// messages in a single packet - so be sure you can handle
		// that as well.
		g_netSession->SendMessageDirect(to, msg);
	}
}

CONSOLE_COMMAND(net_session_create_connection)//<idx> <guid> <address> <port>
{
	args;
	if (g_netSession == nullptr)
		return;

	if (args.m_argList.size() == 0) //Default
	{

		//Player1
		uint8_t index = (uint8_t)atoi("0");

		const char* guid = "nick";

		uint16_t port = (uint16_t)atoi("4334");
		sockaddr_in to = g_networkSystem->StringToSockAddrIPv4("192.168.1.10", port);


		NetConnection* newConnection = g_netSession->CreateConnection(index, guid, to);

		if (newConnection != nullptr)
		{
			std::string addr = g_networkSystem->SockAddrToString((sockaddr*)(&newConnection->m_address));
			g_theConsole->ConsolePrint("Connection created at " + addr, Rgba::GREEN);
		}


		//Player2
		uint8_t index2 = (uint8_t)atoi("1");

		const char* guid2 = "bob";

		uint16_t port2 = (uint16_t)atoi("4335");
		sockaddr_in to2 = g_networkSystem->StringToSockAddrIPv4("192.168.1.10", port2);


		NetConnection* newConnection2 = g_netSession->CreateConnection(index2, guid2, to2);

		if (newConnection2 != nullptr)
		{
			std::string addr = g_networkSystem->SockAddrToString((sockaddr*)(&newConnection2->m_address));
			g_theConsole->ConsolePrint("Connection created at " + addr, Rgba::GREEN);
		}


		return;
	}

	if (args.m_argList.size() < 4)
	{
		g_theConsole->ConsolePrint("Invalid args. Args are: <idx> <guid> <address> <port>", Rgba::RED);
		return;
	}

	uint8_t index = (uint8_t)atoi(args.m_argList[0].c_str());

	const char* guid = args.m_argList[1].c_str();

	uint16_t port = (uint16_t)atoi(args.m_argList[3].c_str());
	sockaddr_in to = g_networkSystem->StringToSockAddrIPv4(args.m_argList[2].c_str(), port);


	NetConnection* newConnection = g_netSession->CreateConnection(index, guid, to);

	if (newConnection != nullptr)
	{
		std::string addr = g_networkSystem->SockAddrToString((sockaddr*)(&newConnection->m_address));
		g_theConsole->ConsolePrint("Connection created at " + addr, Rgba::GREEN);
	}

}


CONSOLE_COMMAND(net_session_destroy_connection)//<idx> 
{
	args;
	
	if (g_netSession == nullptr)
		return;
	if (args.m_argList.size() < 1)
	{
		g_theConsole->ConsolePrint("Error: Needs a Connection index.");
		return;
	}

	uint8_t index = (uint8_t)atoi(args.m_argList[0].c_str());

	bool didDestroy = g_netSession->DestroyConnection(index);

	if (didDestroy)
	{
		g_theConsole->ConsolePrint("Connection destroyed.", Rgba::GREEN);
	}
	else
	{
		g_theConsole->ConsolePrint("Invalid Index.", Rgba::YELLOW);
	}
}

CONSOLE_COMMAND(net_sim_lag) //<min_additional_lag_sec> [max_additional_lag_sec]
{
	args;
	if (g_netSession == nullptr)
		return;
	if (args.m_argList.size() < 1)
	{
		g_theConsole->ConsolePrint("Error: Needs a 1 or 2 values for lag seconds.");
		return;
	}

	if (args.m_argList.size() == 1)
	{
		float min = (float)atof(args.m_argList[0].c_str());

		g_netSession->m_sessionSocket.SetSimLagRate(min, min);

		std::string lagStr = Stringf("%f seconds", min);

		g_theConsole->ConsolePrint("Set Receiving lag rate for " + lagStr);

	}
	else
	{
		float min = (float)atof(args.m_argList[0].c_str());
		float max = (float)atof(args.m_argList[1].c_str());

		g_netSession->m_sessionSocket.SetSimLagRate(min, max);
		std::string lagStr = Stringf("%.2f and %.2f seconds", min, min);

		g_theConsole->ConsolePrint("Set Receiving lag rate between " + lagStr);
	}
}

CONSOLE_COMMAND(net_sim_loss)// <loss_percentage>
{
	args;
	if (g_netSession == nullptr)
		return;

	if (args.m_argList.size() < 1)
	{
		g_theConsole->ConsolePrint("Error: Needs a loss percentage.");
		return;
	}

	if (args.m_argList.size() == 1)
	{
		float lossRate = (float)atof(args.m_argList[0].c_str());
		g_netSession->m_sessionSocket.SetSimDropRate(lossRate);
		std::string lossStr = Stringf("%.2f percent", lossRate);

		g_theConsole->ConsolePrint("Set drop rate for " + lossStr);
		return;
	}
}

CONSOLE_COMMAND(net_go)
{
	if (args.m_argList.size() == 0)
	{
		g_theConsole->ExecuteCommand("net_session_start");
		g_theConsole->ExecuteCommand("net_session_create_connection 0 nick 192.168.1.15 4334");
		g_theConsole->ExecuteCommand("net_session_create_connection 1 bob 192.168.1.15 4335");
	}
	else
	{
		g_theConsole->ExecuteCommand("net_session_start");
		g_theConsole->ExecuteCommand("net_session_create_connection 1 bob 192.168.1.15 4335");
		g_theConsole->ExecuteCommand("net_session_create_connection 0 nick 192.168.1.15 4334");
	}
}
CONSOLE_COMMAND(net_host) //<username> 
{
	args;
	if (g_netSession == nullptr)
		return;
	if (args.m_argList.size() == 1)
	{
		g_netSession->Host(args.m_argList[0].c_str());
	}
	else
	{
		g_theConsole->ConsolePrint("ERROR: Args are: <Hostname>", Rgba::RED);
	}
}

CONSOLE_COMMAND(net_join) //<username> <hostaddr> <hostport> 
{
	args;
	if (g_netSession == nullptr)
		return;
	if (args.m_argList.size() == 3)
	{
		uint16_t port = (uint16_t)atoi(args.m_argList[2].c_str());
		sockaddr_in host = g_networkSystem->StringToSockAddrIPv4(args.m_argList[1].c_str(), port);

		g_netSession->Join(args.m_argList[0].c_str(), host);
	}
	else
	{
		g_theConsole->ConsolePrint("ERROR: Args are: <Hostname> <HostAddr> <HostPort>", Rgba::RED);
	}
}

CONSOLE_COMMAND(net_leave)
{
	args;
	if (g_netSession == nullptr)
		return;

	g_netSession->Leave();
}