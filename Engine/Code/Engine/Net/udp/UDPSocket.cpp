#include "Engine/Net/udp/UDPSocket.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <ws2tcpip.h>
#pragma comment (lib, "ws2_32.lib")

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Net/NetworkSystem.hpp"


UDPSocket::UDPSocket()
{
	m_socket = INVALID_SOCKET;
	m_addrIn;// #TODO Find a easy way to initialize this
}

UDPSocket::UDPSocket(const char* addr, const char* service)
{
	m_port = service;
	m_socket = CreateUDPSocket(addr, service, &m_addrIn);
	//Make sure IsBound is called after construction to ensure the Socket got bound.
}

SOCKET UDPSocket::CreateUDPSocket(char const *addr, char const *service, sockaddr_in *out_addr)
{
	// First, try to get network addresses for this
	addrinfo *info_list = g_networkSystem->AllocAddressesForHost(addr, // an address for this machine
		service, // service, which for TCP/IP is the port as a string (ex: "80")
		AF_INET, // We're doing IPv4 in class
		SOCK_DGRAM, // UDP for now
		AI_PASSIVE);  // And something we can bind (and therefore listen on)


	if (info_list == nullptr) {
		// no addresses match - FAIL
		return false;
	}

	// Alright, try to create a SOCKET from this addr info
	SOCKET my_socket = INVALID_SOCKET;
	addrinfo *iter = info_list;
	while ((iter != nullptr) && (my_socket == INVALID_SOCKET)) {

		// First, create a socket for this address.
		// family, socktype, and protocol are provided by the addrinfo
		// if you wanted to be manual, for an TCP/IPv4 socket you'd use
		// AF_INET, SOCK_DGRAM, IPPROTO_UDP
		my_socket = socket(iter->ai_family, iter->ai_socktype, iter->ai_protocol);
		if (my_socket != INVALID_SOCKET) {
			// Okay, we were able to create it,
			// Now try to bind it (associates the address (ex: 192.168.1.52:4325) to this 
			// socket so it will receive information for it.
			int result = bind(my_socket, iter->ai_addr, (int)(iter->ai_addrlen));
			if (SOCKET_ERROR != result) {

				// Set it to non-block - since we'll be working with this on our main thread
				u_long non_blocking = 1;
				ioctlsocket(my_socket, FIONBIO, &non_blocking);

				// Save off the address if available.
				ASSERT_OR_DIE(iter->ai_addrlen == sizeof(sockaddr_in), "addrLen != Size of SockAddr_in");
				if (nullptr != out_addr) {
					memcpy(out_addr, iter->ai_addr, iter->ai_addrlen);
				}
			}
			else {
				// Cleanup on Fail.
				closesocket(my_socket);
				my_socket = INVALID_SOCKET;
			}
		}
		iter = iter->ai_next;
	}

	// If we allocted, we must free eventually
	g_networkSystem->FreeAddresses(info_list);

	// Return the socket we created.
	return my_socket;
}

void UDPSocket::CloseSocket()
{
	g_networkSystem->CloseSocket(m_socket);
}

size_t UDPSocket::SendTo(sockaddr_in& toAddr, void const *data, size_t const dataSize)
{
	if (m_socket != INVALID_SOCKET) {
		// send will return the amount of data actually sent.
		// It SHOULD match, or be an error.  
		int size = ::sendto(m_socket,
			(char const*)data,      // payload
			(int)dataSize,         // payload size
			0,                      // flags - unused
			(sockaddr const*)&toAddr, // who we're sending to
			sizeof(sockaddr_in));  // size of that structure

		if (size > 0) {
			return size;
		}
	}

	// Not particularly interested in errors - you can 
	// check this though if you want to see if something
	// has happened to your socket.
	return 0;

}

size_t UDPSocket::ReceiveFrom(sockaddr_in* outFromAddr, void const *buffer, size_t const bufferSize)
{
	if (m_socket != INVALID_SOCKET) {
		// recv will return amount of data read, should always be <= buffer_size
		// Also, if you send, say, 3 KB with send, recv may actually
		// end up returning multiple times (say, 1KB, 512B, and 1.5KB) because 
		// the message got broken up - so be sure you application watches for it

		sockaddr_storage addr;
		int addrlen = sizeof(addr);

		int size = ::recvfrom(m_socket,
			(char*)buffer,    // what we're reading into
			bufferSize,      // max data we can read
			0,                // optional flags (see docs if you're curious)
			(sockaddr*)&addr, // Who sent the message
			&addrlen);       // length of their address

		if (size > 0) {
			// We're only doing IPv4 - if we got a non-IPv4 address
			// assume it's garbage
			if (addrlen != sizeof(sockaddr_in))
			{
				return 0;
			}

			memcpy(outFromAddr, &addr, addrlen);
			return size;
		}
	}

	// Again, I don't particularly care about the 
	// error code for now.  It may tell us
	// the guy we're sending to is bad, but we can't really
	// do anything with that yet. 
	return 0U;

}

bool UDPSocket::IsBound()
{
	return (m_socket != INVALID_SOCKET);
}

