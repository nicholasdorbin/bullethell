#pragma once
#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <cstdint>
class TCPConnection;

class TCPListener
{
public:
	SOCKET m_socket;
	sockaddr_in m_addr;

	TCPListener(char const* host, uint16_t port, int queueCount = 1); // bind & listen
	TCPListener(uint16_t port, int queuecount = 1);

	void Stop(); // close socket

	bool IsListening(); // same as is connected

	TCPConnection* Accept(); // ::accept and creates socket
};