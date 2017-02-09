#pragma once
#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <cstdint>

#define DEFAULT_PORT 4325

class NetworkSystem
{
public:
	NetworkSystem();
	~NetworkSystem();

	bool NetworkStartup();
	void NetworkShutdown();
	char const* GetLocalHostName();
	addrinfo* AllocAddressesForHost(char const *host,  char const *service,  int family,  int socktype,  int flags = 0);
	SOCKET CreateListenSocket(char const *addr, char const *service, /* who we're trying to connect to */ sockaddr_in *out_addr, int queue_count = 1); // address we actually connected to.;
	void FreeAddresses(addrinfo* addresses);
	SOCKET AcceptConnection(SOCKET host_socket, sockaddr_in *out_their_addr);
	SOCKET SocketJoin(char const *addr, char const *service, sockaddr_in *out_addr);
	void CloseSocket(SOCKET sock);
	size_t SocketSend(bool *out_should_disconnect, SOCKET my_socket, void const *data, size_t const data_size);
	size_t SocketReceive(bool *out_should_disconnect, SOCKET my_socket, void *buffer, size_t const buffer_size);
	char const* SockAddrToString(sockaddr  const *addr);
	sockaddr_in StringToSockAddrIPv4(const char* ip, uint16_t const port);

	void* GetInAddr(sockaddr const *sa);
	bool SocketErrorShouldDisconnect(uint32_t const error);
	const char* GetPortAsString(uint16_t port);

	//UDP
	SOCKET CreateUDPSocket(char const *addr, char const *service, /* who we're trying to connect to */ sockaddr_in *out_addr); // address we actually connected to.

	size_t SocketSendTo(SOCKET my_socket, sockaddr_in &to_addr, void const *data, size_t const data_size);
	size_t SocketReceiveFrom(sockaddr_in *from_addr, SOCKET my_socket, void *buffer, size_t const buffer_size);
};