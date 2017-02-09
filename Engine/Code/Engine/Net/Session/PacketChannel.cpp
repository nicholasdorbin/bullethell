#include "Engine/Net/Session/PacketChannel.hpp"

#include "Engine/MathUtils.hpp"
#include "Engine/Core/Time.hpp"

#include "Engine/Net/Session/NetPacket.hpp"

#define DROPRATE 0.01f
#define MINRANGE  0//(100.f / 1000.f)
#define MAXRANGE  0//(150.f / 1000.f)


PacketChannel::PacketChannel():
	m_pool(1024),
	m_lagRange(Range<float>(MINRANGE, MAXRANGE))
	, m_dropRate(DROPRATE)
{

}

PacketChannel::PacketChannel(const char* addr, const char* service):
	m_pool(1024),
	m_lagRange(Range<float>(MINRANGE, MAXRANGE))
	, m_socket(addr, service)
	, m_dropRate(DROPRATE)
{

}

void PacketChannel::SetSimLagRate(float& minRate, float& maxRate)
{
	m_lagRange.min = minRate;
	m_lagRange.max = maxRate;
}

void PacketChannel::SetSimDropRate(float& dropRate)
{
	m_dropRate = dropRate;
}

size_t PacketChannel::SendTo(sockaddr_in& toAddr, void const *data, size_t const dataSize)
{
	return m_socket.SendTo(toAddr, data, dataSize);
}

size_t PacketChannel::ReceiveFrom(sockaddr_in* outFromAddr, void const *buffer, size_t const bufferSize)
{
	TimeStampedPacket* timeStampPacket = m_pool.Alloc();
	sockaddr_in from_addr;
	size_t read = m_socket.ReceiveFrom(&from_addr, timeStampPacket->packet.m_buffer, bufferSize);

	if(read > 0)
	{
		if(GetRandomTrueOrFalseWithinProbability(m_dropRate))
		{
			m_pool.Delete(timeStampPacket);
		}
		else
		{
			double delay = m_lagRange.get_random();
			timeStampPacket->packet.m_currentContentSize = read;
			timeStampPacket->timeToProcess = GetCurrentTimeSeconds() + delay;
			timeStampPacket->from_addr = from_addr;
			m_inboundPackets.insert(TimeStampedPacketPair(timeStampPacket->timeToProcess, timeStampPacket));
			//InsertInOrder(inbound_packet, packet);
		}
	}
	else
	{
		m_pool.Delete(timeStampPacket);
	}


	if(m_inboundPackets.size() > 0)
	{
		double cur_time = GetCurrentTimeSeconds();
		TimeStampedPacket *inboundPacket = m_inboundPackets.begin()->second;
		if(cur_time >= inboundPacket->timeToProcess)
		{

			int addrlen = sizeof(inboundPacket->from_addr);
			memcpy(outFromAddr, &inboundPacket->from_addr, addrlen);
			size_t size = inboundPacket->packet.m_currentContentSize;
			memcpy((char*)buffer, inboundPacket->packet.m_buffer, size);
			m_pool.Delete(inboundPacket);
			m_inboundPackets.erase(m_inboundPackets.begin());
			return size;

		}
	}
	return 0;

}

size_t PacketChannel::PacketReceiveFrom(sockaddr_in* outFromAddr, NetPacket* packetToWriteTo, size_t const bufferSize)
{
	TimeStampedPacket* timeStampPacket = m_pool.Alloc();
	sockaddr_in from_addr;
	size_t read = m_socket.ReceiveFrom(&from_addr, timeStampPacket->packet.m_buffer, bufferSize);

	if (read > 0)
	{
		if (GetRandomTrueOrFalseWithinProbability(m_dropRate))
		{
			m_pool.Delete(timeStampPacket);
		}
		else
		{
			double delay = m_lagRange.get_random();
			timeStampPacket->packet.m_currentContentSize = read;
			timeStampPacket->timeToProcess = GetCurrentTimeSeconds() + delay;
			timeStampPacket->from_addr = from_addr;
			m_inboundPackets.insert(TimeStampedPacketPair(timeStampPacket->timeToProcess, timeStampPacket));
			//InsertInOrder(inbound_packet, packet);
		}
	}
	else
	{
		m_pool.Delete(timeStampPacket);
	}


	if (m_inboundPackets.size() > 0)
	{
		double cur_time = GetCurrentTimeSeconds();
		TimeStampedPacket *inboundPacket = m_inboundPackets.begin()->second;
		if (cur_time >= inboundPacket->timeToProcess)
		{

			int addrlen = sizeof(inboundPacket->from_addr);
			memcpy(outFromAddr, &inboundPacket->from_addr, addrlen);
			size_t size = inboundPacket->packet.m_currentContentSize;
			packetToWriteTo->m_timeRecv = inboundPacket->timeToProcess;
			memcpy((char*)packetToWriteTo->m_buffer, inboundPacket->packet.m_buffer, size);
			m_pool.Delete(inboundPacket);
			m_inboundPackets.erase(m_inboundPackets.begin());
			return size;

		}
	}
	return 0;

}

bool PacketChannel::IsBound()
{
	return (m_socket.IsBound());
}

void PacketChannel::CloseSocket()
{
	m_socket.CloseSocket();
}
