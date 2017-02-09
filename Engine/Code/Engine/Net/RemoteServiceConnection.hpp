#pragma once

#include <vector>
#include "Engine/Core/Events/EventSystem.hpp"

class TCPConnection;
class RemoteServiceConnection;

struct RCSMessageEvent : Event
{
	RemoteServiceConnection* rcs;
	unsigned char msg_id;
	const char* msg;
};

class RemoteServiceConnection
{
public:
	void Send(unsigned char msg_id, char const* msg);
	TCPConnection* m_tcpConn;
	RemoteServiceConnection(TCPConnection* tcpconn);

	std::vector<char> next_message;//not char*. We are constructing a string on the fly
	//TEvent<RSC*, byte_t, char const*> on_message;
	void Receive();
	char const* GetAddrName();
};
