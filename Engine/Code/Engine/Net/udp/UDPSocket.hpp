#pragma once

#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <cstdint>


class UDPSocket
{
public:
	SOCKET m_socket;
	sockaddr_in m_addrIn;
	const char* m_port;


	UDPSocket();
	UDPSocket(const char* addr, const char* service); //CreateUDPSocket

	SOCKET CreateUDPSocket(char const *addr, char const *service, sockaddr_in *out_addr);
	void CloseSocket();
	size_t SendTo(sockaddr_in& toAddr, void const *data, size_t const dataSize);
	size_t ReceiveFrom(sockaddr_in* outFromAddr, void const *buffer, size_t const bufferSize);

	bool IsBound();

};