#include "Engine/Net/NetworkSystem.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <ws2tcpip.h>
#pragma comment (lib, "ws2_32.lib")
#pragma warning( disable : 4996 )  // deprecated warnings

NetworkSystem::NetworkSystem()
{

}

NetworkSystem::~NetworkSystem()
{

}

bool NetworkSystem::NetworkStartup()
{
	WSADATA wsa_data;

	// Startup Winsock version 2.2
	int error = WSAStartup(MAKEWORD(2, 2), &wsa_data);

	if (error == 0) {
		return true;
	}
	else {
		// Error( "Failed to initialize WinSock.  Error[%u]\n", error );
		return false;
	}

}

void NetworkSystem::NetworkShutdown()
{
	WSACleanup();
}

// Get Local Host Name (stores it in a global buffer - so not thread safe implementation)
char const* NetworkSystem::GetLocalHostName()
{
	static char buffer[256];
	if (::gethostname(buffer, 256) == 0) {
		return buffer;
	}
	else {
		// failed to grab local host name - you can check the error WSAGetLastError()
		// if you're curious as to why.  
		return "localhost";
	}
}

// Get All Addresses that match our criteria
addrinfo* NetworkSystem::AllocAddressesForHost(char const *host, // host, like google.com
	char const *service, // service, usually the port number as a string
	int family,      // Connection Family, AF_INET (IPv4) for this assignment
	int socktype,    // Socket Type, SOCK_STREAM or SOCK_DGRAM (TCP or UDP) for this class
	int flags)  // Search flag hints, we use this for AI_PASSIVE (bindable addresses)
{

	// Also, family of AF_UNSPEC will return all address that support the 
	// sock type (so both IPv4 and IPv6 adddress).

	// Define the hints - this is what it will use
	// for determining what addresses to return
	addrinfo hints;
	memset(&hints, 0, sizeof(hints));

	hints.ai_family = family;
	hints.ai_socktype = socktype;
	hints.ai_flags = flags;

	// This will allocate all addresses into a single linked list
	// with the head put into result.
	addrinfo *result = nullptr;
	int status = getaddrinfo(host, service, &hints, &result);
	if (status != 0) {
		/* Warning( "net", "Failed to find addresses for [%s:%s].  Error[%s]",
		host, service, gai_strerror(status) ); */
		return nullptr;
	}
	return result;
}


// Binding a TCP Socket for Listening Purposes
SOCKET NetworkSystem::CreateListenSocket(char const *addr, char const *service, // who we're trying to connect to
	sockaddr_in *out_addr, int queue_count) // address we actually connected to.
{
	// First, try to get network addresses for this
	addrinfo *info_list = AllocAddressesForHost(addr, // an address for this machine
		service, // service, which for TCP/IP is the port as a string (ex: "80")
		AF_INET, // We're doing IPv4 in class
		SOCK_STREAM, // TCP for now
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
		// AF_INET, SOCK_STREAM, IPPROTO_TCP
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

				// Set it to listen - this will allow people to connect to us
				result = listen(my_socket, queue_count); //2nd arg is queue_count
				ASSERT_OR_DIE(result != SOCKET_ERROR, "Socket Error."); // sanity check

												// Save off the address if available.
				ASSERT_OR_DIE(iter->ai_addrlen == sizeof(sockaddr_in), "Addlen is different.");
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
	FreeAddresses(info_list);

	// Return the socket we created.
	return my_socket;
}


void NetworkSystem::FreeAddresses(addrinfo *addresses)
{
	if (nullptr != addresses) {
		freeaddrinfo(addresses);
	}
}



// Accepting a Connection on a Listening Socket
// (Called on the host - this will Okay a socket
// trying to connect with ::connect in the code above.
SOCKET NetworkSystem::AcceptConnection(SOCKET host_socket, sockaddr_in *out_their_addr)
{
	sockaddr_storage their_addr;
	int their_addr_len = sizeof(their_addr);

	SOCKET their_socket = ::accept(host_socket, (sockaddr*)&their_addr, &their_addr_len);
	if (their_socket != INVALID_SOCKET) {

		// If you want to support IPv6, this is no longer a valid check
		if (out_their_addr != nullptr) {
			ASSERT_OR_DIE(their_addr_len == sizeof(sockaddr_in), "Addr Len different.");
			memcpy(out_their_addr, &their_addr, their_addr_len);
		}

		return their_socket;
	}
	else {
		// if we fail to accept, it might be we lost
		// connection - you can check the same we we'll do it
		// for send and recv below, and potentially return 
		// that error code somehow (if you move this code into a method
		// you could disonnect directly)
		/*
		int err = WSAGetLastError();
		if (SocketErrorShouldDisconnect(err)) {
		disconnect();
		}
		*/
	}

	return INVALID_SOCKET;
}


// Similar to Hosting - with a few changes internally
// Comments are only around places that change.
SOCKET NetworkSystem::SocketJoin(char const *addr, char const *service, sockaddr_in *out_addr)
{
	// First, we don't look for AI_PASSIVE connections - we don't bind this one
	// We'll be sending to it.
	addrinfo *info_list = AllocAddressesForHost(addr, service, AF_INET, SOCK_STREAM, 0);
	if (info_list == nullptr) {
		return false;
	}

	SOCKET my_socket = INVALID_SOCKET;
	addrinfo *iter = info_list;
	while ((iter != nullptr) && (my_socket == INVALID_SOCKET)) {
		my_socket = socket(iter->ai_family, iter->ai_socktype, iter->ai_protocol);
		if (my_socket != INVALID_SOCKET) {

			// Instead of binding - we call connect, which will do the TCP/IP
			// handshake.  Be aware this can block and cause a frame hickup, which
			// is fine for now.
			int result = ::connect(my_socket, iter->ai_addr, (int)(iter->ai_addrlen));
			if (SOCKET_ERROR != result) {
				u_long non_blocking = 1;
				ioctlsocket(my_socket, FIONBIO, &non_blocking);

				// We do not listen on on this socket - we are not
				// accepting new connections.

				ASSERT_OR_DIE(iter->ai_addrlen == sizeof(sockaddr_in), "Addr len different.");
				if (nullptr != out_addr) {
					memcpy(out_addr, iter->ai_addr, iter->ai_addrlen);
				}
			}
			else {
				int error = WSAGetLastError();
				ERROR_RECOVERABLE(Stringf("WSA Socket error: %i", error));
				closesocket(my_socket);
				my_socket = INVALID_SOCKET;
			}
		}
		iter = iter->ai_next;
	}

	FreeAddresses(info_list);

	return my_socket;
}

void NetworkSystem::CloseSocket(SOCKET sock)
{
	closesocket(sock);
}

size_t NetworkSystem::SocketSend(bool *out_should_disconnect,
	SOCKET my_socket,
	void const *data,
	size_t const data_size)
{
	*out_should_disconnect = false;
	if (my_socket != INVALID_SOCKET) {
		// send will return the amount of data actually sent.
		// It SHOULD match, or be an error.  
		int size = ::send(my_socket, (char const*)data, (int)data_size, 0);
		if (size < 0) {
			int32_t error = WSAGetLastError();
			if (SocketErrorShouldDisconnect(error)) {
				// If the error is critical - disconnect this socket
				*out_should_disconnect = true;
			}
		}
		else {
			ASSERT_OR_DIE((size_t)size == data_size, "Size does not equal data size.");
		}

		return (size_t)size;
	}
	return 0u;
}

size_t NetworkSystem::SocketReceive(bool *out_should_disconnect,
	SOCKET my_socket,
	void *buffer,
	size_t const buffer_size)
{
	*out_should_disconnect = false;
	if (my_socket != INVALID_SOCKET) {
		// recv will return amount of data read, should always be <= buffer_size
		// Also, if you send, say, 3 KB with send, recv may actually
		// end up returning multiple times (say, 1KB, 512B, and 1.5KB) because 
		// the message got broken up - so be sure you application watches for it
		int size = ::recv(my_socket, (char*)buffer, buffer_size, 0);
		if (size < 0) {
			int32_t error = WSAGetLastError();
			if (SocketErrorShouldDisconnect(error)) {
				*out_should_disconnect = true;
			}
			return 0U;
		}
		else {
			return (size_t)size;
		}
	}
	else {
		return 0U;
	}
}


// Converting a sockaddr_in to a String
// Again, doing this in a global buffer so it is not thread safe
char const* NetworkSystem::SockAddrToString(sockaddr  const *addr)
{
	static char buffer[256];

	// Hard coding this for sockaddr_in for brevity
	// You can make this work for IPv6 as well
	sockaddr_in *addr_in = (sockaddr_in*)addr;

	// inet_ntop converts an address type to a human readable string,
	// ie 0x7f000001 => "127.0.0.1"
	// GetInAddr (defined below) gets the pointer to the address part of the sockaddr
	char hostname[256];
	inet_ntop(addr_in->sin_family, GetInAddr(addr), hostname, 256);

	// Combine the above with the port.  
	// Port is stored in network order, so convert it to host order
	// using ntohs (Network TO Host Short)
	sprintf_s(buffer, 256, "%s:%u", hostname, ntohs(addr_in->sin_port));

	// buffer is static - so will not go out of scope, but that means this is not thread safe.
	return buffer;
}


sockaddr_in NetworkSystem::StringToSockAddrIPv4(const char* ip, uint16_t const port)
{
	sockaddr_in addr;
	memset(&addr, 0, sizeof(sockaddr_in));
	addr.sin_addr.S_un.S_addr = inet_addr(ip);
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;

	return addr;

}

// get address part of a sockaddr, IPv4 or IPv6:
void* NetworkSystem::GetInAddr(sockaddr const *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((sockaddr_in*)sa)->sin_addr);
	}
	else {
		return &(((sockaddr_in6*)sa)->sin6_addr);
	}
}


// Ignoring Non-Critical Errors
// These errors are non-fatal and are more or less ignorable.
bool NetworkSystem::SocketErrorShouldDisconnect(uint32_t const error)
{
	switch (error) {
	case WSAEWOULDBLOCK: // nothing to do - would've blocked if set to blocking
	case WSAEMSGSIZE:    // UDP message too large - ignore that packet.
	case WSAECONNRESET:  // Other side reset their connection.
		return false;

	default:
		return true;
	}
}

const char* NetworkSystem::GetPortAsString(uint16_t port)
{
	static char portAsString[6];
	sprintf_s(portAsString, 6, "%u", port);
	return portAsString;
}



// Binding a TCP Socket for Listening Purposes
SOCKET NetworkSystem::CreateUDPSocket(char const *addr, char const *service, // who we're trying to connect to
	sockaddr_in *out_addr) // address we actually connected to.
{
	// First, try to get network addresses for this
	addrinfo *info_list = AllocAddressesForHost(addr, // an address for this machine
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
				ASSERT_OR_DIE(iter->ai_addrlen == sizeof(sockaddr_in), "addrlen != sockaddr size");
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
	FreeAddresses(info_list);

	// Return the socket we created.
	return my_socket;
}


size_t NetworkSystem::SocketSendTo(SOCKET my_socket,
	sockaddr_in &to_addr,
	void const *data,
	size_t const data_size)
{
	if (my_socket != INVALID_SOCKET) {
		// send will return the amount of data actually sent.
		// It SHOULD match, or be an error.  
		int size = ::sendto(my_socket,
			(char const*)data,      // payload
			(int)data_size,         // payload size
			0,                      // flags - unused
			(sockaddr const*)&to_addr, // who we're sending to
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

size_t NetworkSystem::SocketReceiveFrom(sockaddr_in *from_addr,
	SOCKET my_socket,
	void *buffer,
	size_t const buffer_size)
{
	if (my_socket != INVALID_SOCKET) {
		// recv will return amount of data read, should always be <= buffer_size
		// Also, if you send, say, 3 KB with send, recv may actually
		// end up returning multiple times (say, 1KB, 512B, and 1.5KB) because 
		// the message got broken up - so be sure you application watches for it

		sockaddr_storage addr;
		int addrlen = sizeof(addr);

		int size = ::recvfrom(my_socket,
			(char*)buffer,    // what we're reading into
			buffer_size,      // max data we can read
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

			memcpy(from_addr, &addr, addrlen);
			return size;
		}
	}

	// Again, I don't particularly care about the 
	// error code for now.  It may tell us
	// the guy we're sending to is bad, but we can't really
	// do anything with that yet. 
	return 0U;
}
