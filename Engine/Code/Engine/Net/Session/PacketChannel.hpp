#pragma once
#include <stdint.h>
#include <vector>
#include <map>
#include "Engine/Core/Memory/ObjectPool.hpp"
#include "Engine/Net/udp/UDPSocket.hpp"
#include "Engine/Math/Range.hpp"
#include "Engine/Net/Session/NetPacket.hpp"

class NetSession;
class NetMessage;
class NetPacket;


struct TimeStampedPacket
{
	NetPacket packet;
	double timeToProcess;
	sockaddr_in from_addr;

};

 typedef  std::pair<double, TimeStampedPacket*> TimeStampedPacketPair;
 typedef  std::map<double, TimeStampedPacket*>  TimeStampedPacketMap;
// typedef  TimeStampedPacketMap::iterator  TimeStampedPacketIter;


class PacketChannel
{
public:
	PacketChannel();
	PacketChannel(const char* addr, const char* service);
	UDPSocket m_socket;
	Range<float> m_lagRange;
	float m_dropRate;
	ObjectPool<TimeStampedPacket> m_pool;


	void SetSimLagRate(float& minRate, float& maxRate);
	void SetSimDropRate(float& dropRate);


	size_t SendTo(sockaddr_in& toAddr, void const *data, size_t const dataSize);
	size_t ReceiveFrom(sockaddr_in* outFromAddr, void const *buffer, size_t const bufferSize); //This one changes


	size_t PacketReceiveFrom(sockaddr_in* outFromAddr, NetPacket* packetToWriteTo, size_t const bufferSize);
	bool IsBound();
	void CloseSocket();
	TimeStampedPacketMap m_inboundPackets;
	//std::vector<TimeStampedPacket*> m_inboundPackets;

};