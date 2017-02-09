#pragma once
#include <stdint.h>
#include <vector>
#include <set>
#include <queue>
#include "Engine/Net/Session/PacketChannel.hpp"

class NetSession;
class NetMessage;
class NetPacket;
struct NetSender;

#define MAX_GUID_LENGTH 32
#define MAX_RELIABLES_PER_PACKET 32
#define MAX_RELIABLE_RANGE 1024
#define INVALID_PACKET_ACK 0xffff
#define  INVALID_CONNECTION_INDEX 0xf
#define MAX_ACK_BUNDLES 128
#define MESSAGE_OLD_THRESHOLD 0.15f
#define HEARTBEAT_THRESHOLD 4.f


enum eNetConnectionState
{
	eNetConnectionState_UNINITIALIZED = 0,
	eNetConnectionState_UNCONFIRMED,
	eNetConnectionState_CONFIRMED,
	eNetConnectionState_LOCAL,
	eNetConnectionState_BAD,
	eNetConnectionState_DEAD
};


struct AckBundle
{
	uint16_t ackID;
	std::vector<uint16_t> sentReliables; //can use vector
	uint32_t reliableCount;
	ULONG timestamp;

	AckBundle()
	{
		ackID = INVALID_PACKET_ACK;
	}


	void AddReliable(uint16_t reliableID)
	{
		//if (reliableCount < MAX_RELIABLES_PER_PACKET)
		{
			sentReliables.push_back(reliableID);
			reliableCount++;
		}
	}
};



class NetConnection
{
public:
	NetConnection();
	NetConnection(uint8_t index, sockaddr_in address, const char* guid,  NetSession* session);
	~NetConnection();
	uint8_t m_index;
	sockaddr_in m_address; //Target address
	char m_guid[MAX_GUID_LENGTH]; //use 32
	NetSession* m_session;

	eNetConnectionState m_connectionState;


	// = ACKS =
	// sending: acks
	uint16_t m_nextSentAckID;
	AckBundle m_ackBundles[MAX_ACK_BUNDLES];

	// recving: acks
	//uint16_t m_nextExpectedAckID; // should always be highest_received_ack + 1.  
	uint16_t m_highestReceivedAckID; // so there's no real need for both
	uint16_t m_prevHighestReceivedAcksBitfield; // bitfield of previous received acks

	


	//Messages
	std::vector<NetMessage*> m_unreliables;
	// sending:  reliable traffic
	uint16_t m_nextSentReliableID;
	uint16_t m_oldestUnconfirmedReliableID;
	std::set<uint16_t> m_confirmedReliableIDs; 	// vector is easiest method - but not very effecient

	std::queue<NetMessage*> m_unsentReliables;
	std::queue<NetMessage*> m_sentReliables;

	// recving:  reliable_traffic
	uint16_t m_nextExpectedReliableID;
	
	std::vector<uint16_t> m_receivedReliableIDs; 	// vector is easiest method, but not very efficient

	//InOrder
	uint16_t m_nextSentSequenceID;
	uint16_t m_expectedReceivedSequenceID;
	std::vector<NetMessage*> m_inOrderUnproccesedMessages;

	double m_lastReceivedTimestamp;
	double m_lastSentTimestamp;

	double m_rtt;


	void AddMessage(NetMessage* msg);

	bool CanWriteMessageToPacket(NetPacket* packet, NetMessage* msg);
	bool WriteMessageToPacket(NetPacket* packet, NetMessage* msg);
	void FreeAllUnreliables();

	void SendPacket();
	void SentHeartbeat();



	bool CanAttachNewReliable(); // determines if we can send a new reliable message (have reliables IDs to spare)
	size_t GetLiveReliableRange(); // distance between last_sent_reliable_id, and oldest_unconfirmed_reliable_id

	// Mark a reliable ID as confirmed - that is, we know the other guy has processed it
	void MarkReliableConfirmed(uint16_t const reliable_id);
	bool IsReliableConfirmed(uint16_t const reliable_id);

	// recv_side: reliable traffic
	bool CanProcessMessage(NetSender& from, NetMessage const &msg);  	// should we process this message (checks controls and records)
														// such as it already being received
	bool HaveReceivedReliable(uint16_t const reliable_id);	// check if a reliable_id is marked as received
	void MarkReliableReceived(uint16_t const reliable_id); 	// after processing a message, mark it as received

	// Called if we can process a message
	// and will mark the message as 
	bool ProcessMessage(NetSender const &from, NetMessage &msg);
	void ProcessInOrder(NetSender const &from, NetMessage &msg);
	void MarkMessageReceived(NetMessage const &msg); // called from process message, does record keeping
	void MarkPacketReceived(NetPacket* packet);

	//Bundles
	AckBundle* CreateAckBundle(uint16_t ack);
	AckBundle* FindAckBundle(uint16_t ack);

	size_t AttachOldSentReliables(NetPacket& packet, AckBundle* bundle, double currentTime);
	size_t AttachUnsentReliables(NetPacket& packet, AckBundle* bundle, double currentTime);
	bool IsOld(NetMessage* message, double currentTime);
	void RemoveAllReliableIdsLessThan(uint16_t minimumReliableID);

	void NetConnection::ConfirmAck(uint16_t ack);
	bool IsBitSetAtIndex(size_t index, uint16_t bitfield);
	void UpdateAckBitField(uint16_t &highest, uint16_t &previous, uint16_t new_value);
	bool IsReceived(uint16_t& value, uint16_t& highest, uint16_t& previous_bf);
	void SetBit(uint16_t value, uint16_t& target);

	bool HasMessagesToSend();
	std::string GetConnectionStateName();
};