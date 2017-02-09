#include "Engine/Net/tcpip/TCPListener.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Net/NetworkSystem.hpp"
#include "Engine/Net/tcpip/TCPConnection.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <ws2tcpip.h>
#pragma comment (lib, "ws2_32.lib")

TCPListener::TCPListener(char const* host, uint16_t port, int queueCount /*= 1*/)
{
	const char* portAsString = g_networkSystem->GetPortAsString(port);
	m_socket = g_networkSystem->CreateListenSocket(host, portAsString, &m_addr, queueCount);
}

TCPListener::TCPListener(uint16_t port, int queuecount /*= 1*/)
{
	const char* portAsString = g_networkSystem->GetPortAsString(port);
	
	const char* localHostName = g_networkSystem->GetLocalHostName();
	m_socket = g_networkSystem->CreateListenSocket(localHostName, portAsString, &m_addr, queuecount);
}

void TCPListener::Stop()
{
	g_networkSystem->CloseSocket(m_socket);
	m_socket = INVALID_SOCKET;
}

bool TCPListener::IsListening()
{

	if (m_socket != INVALID_SOCKET)
	{
		return true;
	}
	return false;
	
}

TCPConnection* TCPListener::Accept()
{
	//Get other dude's socket
	sockaddr_in their_addr;
	SOCKET theirSocket = g_networkSystem->AcceptConnection(m_socket, &their_addr);

	if (theirSocket != INVALID_SOCKET)
	{
		return new TCPConnection(theirSocket, their_addr);
	}
	return nullptr;
}
