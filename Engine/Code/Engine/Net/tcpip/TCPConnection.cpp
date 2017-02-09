#include "Engine/Net/tcpip/TCPConnection.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Net/NetworkSystem.hpp"

TCPConnection::TCPConnection(SOCKET s, sockaddr_in& addr)
{
	m_socket = s;
	m_addr = addr;
	const sockaddr* sockaddrVar = (sockaddr*)&m_addr;
	m_addrName = g_networkSystem->SockAddrToString(sockaddrVar);
}

TCPConnection::TCPConnection(char const* host, uint16_t port)
{
	const char* portAsString =  g_networkSystem->GetPortAsString(port);
	
	m_socket = g_networkSystem->SocketJoin(host, portAsString, &m_addr);
	const sockaddr* sockaddrVar = (sockaddr*)&m_addr;
	m_addrName = g_networkSystem->SockAddrToString(sockaddrVar);
}

void TCPConnection::Disconnect()
{
	g_networkSystem->CloseSocket(m_socket);
}

size_t TCPConnection::Send(void const* data, size_t size)
{
	bool shouldDisconnect = false;
	size_t dataSent = g_networkSystem->SocketSend(&shouldDisconnect, m_socket, data, size);

	if (shouldDisconnect)
	{
		Disconnect();
	}
	return dataSent;
}

size_t TCPConnection::Receive(void* buf, size_t bufSize)
{
	bool shouldDisconnect = false;
	size_t dataRecv = g_networkSystem->SocketReceive(&shouldDisconnect, m_socket, buf, bufSize);
	if (shouldDisconnect)
	{
		Disconnect();
	}
	return dataRecv;
}

bool TCPConnection::IsConnected()
{
	return (m_socket != INVALID_SOCKET);
}
