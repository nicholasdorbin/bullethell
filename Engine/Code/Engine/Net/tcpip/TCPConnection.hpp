#pragma once
#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <cstdint>

class TCPConnection
{
public:
	SOCKET m_socket;
	sockaddr_in m_addr;
	char const* m_addrName;

	// can make whole class private and make this a friend of TCPListener to make yourself safe from yourself
	TCPConnection(SOCKET s, sockaddr_in& addr);  // Just set
	TCPConnection(char const* host, uint16_t port); // Socket connect

	void Disconnect(); // close socket
	size_t Send(void const* data, size_t size); // send on socket
	size_t Receive(void* buf, size_t bufSize); // receive on socket

	bool IsConnected(); // m_socket !+ INVALID_SOCKET
};