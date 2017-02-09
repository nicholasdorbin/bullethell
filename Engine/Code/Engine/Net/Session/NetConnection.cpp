#include "Engine/Net/Session/NetConnection.hpp"
#include "Engine/Net/Session/NetSession.hpp"
#include "Engine/Net/Session/NetMessage.hpp"
#include "Engine/Net/Session/NetPacket.hpp"
#include "Engine/Utils/ByteUtils.hpp"
#include "Engine/Core/Time.hpp"


NetConnection::NetConnection() :
	m_index(0),
	m_session(nullptr),
	m_oldestUnconfirmedReliableID(0),
	m_nextExpectedReliableID(0),
	m_nextSentReliableID(0), 
	m_nextSentAckID(0),
	m_connectionState(eNetConnectionState_UNINITIALIZED),
	m_lastReceivedTimestamp(GetCurrentTimeSeconds()),
	m_lastSentTimestamp(GetCurrentTimeSeconds()),
	m_nextSentSequenceID(0), 
	m_expectedReceivedSequenceID(0),
	m_rtt(0.15) //150 ms,
	
{

}

NetConnection::NetConnection(uint8_t index, sockaddr_in address, const char* guid, NetSession* session) :
	m_index(index),
	m_address(address),
	m_session(session),
	m_oldestUnconfirmedReliableID(0),
	m_nextExpectedReliableID(0),
	m_nextSentReliableID(0),
	m_highestReceivedAckID(0xffff),
	m_nextSentAckID(0),
	m_prevHighestReceivedAcksBitfield(0),
	m_connectionState(eNetConnectionState_UNINITIALIZED),
	m_lastReceivedTimestamp(GetCurrentTimeSeconds()),
	m_lastSentTimestamp(GetCurrentTimeSeconds()), 
	m_nextSentSequenceID(0),
	m_expectedReceivedSequenceID(0),
	m_rtt(0.15) //150 ms
	
{
	size_t len = strlen(guid);

	if (len > MAX_GUID_LENGTH)
	{
		len = MAX_GUID_LENGTH;
	}

	strncpy_s(m_guid, guid, len);

	for (size_t i = 0; i < MAX_ACK_BUNDLES; i++)
	{
		m_ackBundles[i] = AckBundle();
	}
}

NetConnection::~NetConnection()
{
	for each (NetMessage* msg in m_unreliables)
	{
		delete msg;
	}


	for each (NetMessage* msg in m_inOrderUnproccesedMessages)
	{
		delete msg;
	}

	while (!m_unsentReliables.empty())
	{
		NetMessage* msgReliable = m_unsentReliables.front();
		delete msgReliable;
		m_unsentReliables.pop();
	}

	while (!m_sentReliables.empty())
	{
		NetMessage* msgReliable = m_sentReliables.front();
		delete msgReliable;
		m_sentReliables.pop();
	}
}

void NetConnection::AddMessage(NetMessage* msg)
{
// 	msg.finalize(session); //defines the definition in this message
// 	Unreliables.push_back(CreateCopy(msg));
	
	if (msg->m_definition == nullptr)
	{
		return;
	}


	if (msg->IsReliable() ||  (msg->IsInOrder()))
	{
		if (msg->IsReliable())
		{
			msg->m_reliableID = 0;
 			msg->WriteReliableID();
			//m_nextSentReliableID++;

			
		}
		if (msg->IsInOrder())
		{
			msg->m_sequenceID = m_nextSentSequenceID;

			msg->WriteSequenceID();
			m_nextSentSequenceID++;
		}
		
		m_unsentReliables.push(msg);
	}
	else
	{
		m_unreliables.push_back(msg);
	}

}

bool NetConnection::CanWriteMessageToPacket(NetPacket* packet, NetMessage* msg)
{
	return (packet->GetWriteableBytes() >= msg->m_currentContentSize);
}

bool NetConnection::WriteMessageToPacket(NetPacket* packet, NetMessage* msg)
{
	if (!CanWriteMessageToPacket(packet, msg))
	{
		return false;
	}

	short msgLen;
	msg->Read<short>(&msgLen);
	packet->Write<short>(&msgLen);

	byte_t msgID;
	msg->Read<byte_t>(&msgID);

	packet->Write<byte_t>(&msgID);

	byte_t msgData[MESSAGE_MTU];


	//This already handles the reliable / unreliable stuff that may or may not be there, since it writes stuff w/o needing to interpret
	msg->ReadForward(&msgData, msgLen - 1);

	packet->WriteForward(&msgData, msgLen - 1);

	return true;
}

void NetConnection::FreeAllUnreliables()
{
	for each (NetMessage* msg in m_unreliables)
	{
		delete msg;
		msg = nullptr;
	}
	m_unreliables.clear();
}

void NetConnection::SendPacket()
{
	NetPacket packet;

	//Write the header
	//ConnIndex
	packet.Write<uint8_t>(&m_session->m_myConnection->m_index);


	//next Sent Ack
	packet.Write<uint16_t>(&m_nextSentAckID);

	//Highest Received Ack
	packet.Write<uint16_t>(&m_highestReceivedAckID);

	//Prev Highest Received Acks
	packet.Write<uint16_t>(&m_prevHighestReceivedAcksBitfield);

	//NumMessages Bookmark
	size_t numMessagesBookmark = packet.m_offset;
	uint8_t numMessagesSent = 0u;
	packet.Write<uint8_t>(&numMessagesSent);



	//Messages

	AckBundle* bundle = CreateAckBundle(m_nextSentAckID);
	m_nextSentAckID++;
	double currentTime = GetCurrentTimeSeconds();
	bundle->timestamp = currentTime;


	numMessagesSent += (uint8_t)AttachOldSentReliables(packet, bundle, currentTime);
	numMessagesSent += (uint8_t)AttachUnsentReliables(packet, bundle, currentTime);

	//Unreliables
	for (unsigned int i = 0; i < m_unreliables.size(); i++)
	{
		NetMessage* msg = m_unreliables[i];
		if (!WriteMessageToPacket(&packet, msg))
		{
			//No more room in packet
			//#TODO free specific unreliables
			//#TODO How do only free the unreliables we did?
			break;
		}
		numMessagesSent++;

	}

	FreeAllUnreliables();
	packet.OverWrite<uint8_t>(numMessagesBookmark, &numMessagesSent);

	//Send the packet
	m_session->m_sessionSocket.SendTo(m_address, packet.m_buffer, packet.GetTotalReadableBytes());
	m_lastSentTimestamp = currentTime;
	m_session->SetLastSentTimeStamp(currentTime);
}

void NetConnection::SentHeartbeat()
{
	if (m_lastSentTimestamp < HEARTBEAT_THRESHOLD)
	{
		return;
	}

	NetPacket packet;
	double currentTime = GetCurrentTimeSeconds();
	packet.Write<uint8_t>(&m_session->m_myConnection->m_index);


	//next Sent Ack
	packet.Write<uint16_t>(&m_nextSentAckID);

	//Highest Received Ack
	packet.Write<uint16_t>(&m_highestReceivedAckID);

	//Prev Highest Received Acks
	packet.Write<uint16_t>(&m_prevHighestReceivedAcksBitfield);

	uint8_t numMessagesSent = 0u;
	packet.Write<uint8_t>(&numMessagesSent);

	FreeAllUnreliables();

	m_session->m_sessionSocket.SendTo(m_address, packet.m_buffer, packet.GetTotalReadableBytes());
	m_lastSentTimestamp = currentTime;
	m_session->SetLastSentTimeStamp(currentTime);
}

bool NetConnection::CanAttachNewReliable()
{
	return true;//We will change this on a later assignment
}

size_t NetConnection::GetLiveReliableRange()
{
	return (m_nextSentReliableID - 1) - m_oldestUnconfirmedReliableID;
}

void NetConnection::MarkReliableConfirmed(uint16_t const reliable_id)
{
	if (CycleGreaterThan(m_oldestUnconfirmedReliableID, reliable_id))
	{
		return; //We already know it was confirmed--because it's below the oldest unconfirmed we have.
	}
	else if (reliable_id == m_oldestUnconfirmedReliableID)
	{
		//The main reason we insert rid just to remove it, is to increment m_oldest correctly.
		std::set<uint16_t>::iterator ridIter = m_confirmedReliableIDs.insert(reliable_id).first; //Iter to inserted key.

	//This may not be as simple as just incrementing once.
	//e.g. m_oldestUnconfirmedReliableID was 3, and now we've received 2 for the first time.
	//So, we need to advance as many times as it takes to find m_oldest.

//Each time we remove at the oldest, we'll increment oldest for the next go-around, so we'll remove 0, 1, 2, but not 4 if find(3) fails.
		while (ridIter != m_confirmedReliableIDs.end())
		{
			m_confirmedReliableIDs.erase(ridIter);
			++m_oldestUnconfirmedReliableID; //Be sure it comes after the erase!
			ridIter = m_confirmedReliableIDs.find(m_oldestUnconfirmedReliableID);
		}
	}
	else //The oldest is staying the oldest, but the new received id exceeds it. This above is when we receive 3, but still are missing 2.
	{ 
			m_confirmedReliableIDs.insert(reliable_id);
	}
}

bool NetConnection::IsReliableConfirmed(uint16_t const reliable_id)
{
	std::set<uint16_t>::iterator ridIter = m_confirmedReliableIDs.find(reliable_id);
	if (ridIter != m_confirmedReliableIDs.end())
	{
		return true;
	}
	if (CycleGreaterThan(m_oldestUnconfirmedReliableID, reliable_id))
	{
		return true; //We already know it was confirmed--because it's below the oldest unconfirmed we have.
	}
	return false;
}

bool NetConnection::CanProcessMessage(NetSender& from,  NetMessage const &msg)
{
	if (from.connection == nullptr)
	{
		return (msg.m_definition->control_flags == eControlFlagID_CONNECTIONLESS);
		//return !msg.RequiresConnection();
	}
	else //Can put the below inside a from.connection->Can/*Connection*/ProcessMessage() and just call that here.
	{
		if (msg.IsReliable()) //Instead of contentFlags and contentOptions, the spec/doc has options and controls.
		{
			return !(from.connection->HaveReceivedReliable(msg.m_reliableID));
			//return !from.connection->HasReceivedReliable(msg);
		}
		else
		{
			return true;
		}
	}
}

bool NetConnection::HaveReceivedReliable(uint16_t const reliable_id)
{
	for (uint16_t i = 0; i < m_receivedReliableIDs.size(); i++)
	{
		if (m_receivedReliableIDs[i] == reliable_id)
		{
			return true;
		}
	}
	return false;
}

void NetConnection::MarkReliableReceived(uint16_t const reliable_id)
{
	if(CycleGreaterThanEqual(reliable_id, m_nextExpectedReliableID))
	{
		uint16_t distance = reliable_id - m_nextExpectedReliableID;
		if(distance >= MAX_RELIABLE_RANGE) {
			return; //ASSERT
		}
		m_nextExpectedReliableID = reliable_id + 1;
		RemoveAllReliableIdsLessThan(reliable_id - MAX_RELIABLE_RANGE);
		m_receivedReliableIDs.push_back(reliable_id);
	}
	else
	{
		uint16_t diff = m_nextExpectedReliableID - reliable_id;
	
		if (diff < MAX_RELIABLE_RANGE)
		{
			if (!HaveReceivedReliable(reliable_id))
			{
				m_receivedReliableIDs.push_back(reliable_id);
			}
		}
	}	

}

bool NetConnection::ProcessMessage(NetSender const &from, NetMessage &msg)
{
	if (m_connectionState != eNetConnectionState_LOCAL)
	{
		m_connectionState = eNetConnectionState_CONFIRMED;
	}

	

	m_lastReceivedTimestamp = GetCurrentTimeSeconds();
	m_session->SetLastRecvTimeStamp(m_lastReceivedTimestamp);

	if (msg.IsInOrder())
	{
		ProcessInOrder(from, msg);
	}
	else
	{
		msg.m_definition->cb(from, msg);
	}
	//msg.Process(from); //Calls the defns's cb on that message.

	MarkMessageReceived(msg);
	return true;
}

void NetConnection::ProcessInOrder(NetSender const &from, NetMessage &msg)
{
	if (msg.m_sequenceID == m_expectedReceivedSequenceID)
	{
		m_expectedReceivedSequenceID++;
		msg.m_definition->cb(from, msg);

		//Loop thru the vector and see if there are more things to process

		auto counterIter = m_inOrderUnproccesedMessages.begin();
		while (counterIter != m_inOrderUnproccesedMessages.end())
		{
			NetMessage* thisMsg = *counterIter;
			if (thisMsg->m_sequenceID == m_expectedReceivedSequenceID)
			{

				thisMsg->m_definition->cb(from, *thisMsg);
				counterIter = m_inOrderUnproccesedMessages.erase(counterIter);
				delete thisMsg;
				m_expectedReceivedSequenceID++;
				counterIter = m_inOrderUnproccesedMessages.begin();
			}
			else
			{
				++counterIter;
			}
		}
	}
	else
	{
		//Store the message
		NetMessage* copy = new NetMessage(msg);
		m_inOrderUnproccesedMessages.push_back(copy);
	}
}

void NetConnection::MarkMessageReceived(NetMessage const &msg)
{
	if (msg.IsReliable())
	{
		MarkReliableReceived(msg.m_reliableID);
	}
	
}


void NetConnection::MarkPacketReceived(NetPacket* packet)
{
	NetPacket::Header ph = packet->m_header;
	UpdateAckBitField(m_highestReceivedAckID, m_prevHighestReceivedAcksBitfield, ph.ackID);

	AckBundle* correspondingBundle = this->FindAckBundle(ph.highestReceivedAckID);

	if (correspondingBundle == nullptr)
	{
		//Bundle was skipped, and was assumed to be dropped, but we got a confirmation for it too late :(
		return;
	}
	packet->m_rtt = packet->m_timeRecv - correspondingBundle->timestamp;
	m_rtt = (0.9f * m_rtt) + (0.1f * packet->m_rtt);
	if (m_rtt < 0)
	{
		m_rtt = 0;
	}

	ConfirmAck(ph.highestReceivedAckID);
	for (size_t bitIndex = 0; bitIndex < sizeof(ph.prevHighestReceivedAcksBitfield); ++bitIndex)
	{
		if (IsBitSetAtIndex(bitIndex, ph.prevHighestReceivedAcksBitfield)) //IsBitSetAtIndex( size_t idx, size_t bitfield ) { return ( ( 1<<idx ) & bitfield ) != 0 ; }
		{
				ConfirmAck(ph.highestReceivedAckID - (ph.prevHighestReceivedAcksBitfield + 1)); //You're sending in "confirm me as true" for one index of the field?
		}
	}
}

AckBundle* NetConnection::CreateAckBundle(uint16_t ack)
{
	//don't wanna new anything //want the oldest bundle //use circular buffer
	//mod it by size of MAX_BUNDLES to make it fit within buffer
	uint16_t idx = ack % MAX_ACK_BUNDLES;
	AckBundle* bundle = &(m_ackBundles[idx]);
	bundle->sentReliables.clear();
	bundle->ackID = ack;
	bundle->reliableCount = 0;
	return bundle;

}

AckBundle* NetConnection::FindAckBundle(uint16_t ack)
{
	size_t index = ack % MAX_ACK_BUNDLES;
	AckBundle* bundle = &(m_ackBundles[index]);
	return ((bundle->ackID == ack) ? bundle : nullptr);
}

size_t NetConnection::AttachOldSentReliables(NetPacket& packet, AckBundle* bundle, double currentTime)
{
	size_t messagesSent = 0;
	while(!m_sentReliables.empty())
	{
		NetMessage* msg = m_sentReliables.front(); //Sees the first thing but doesn't pop it
												 //
		if(IsReliableConfirmed(msg->m_reliableID))
		{
			//Cleanup
			m_sentReliables.pop();
			//FreeMessage(msg);
			delete msg;
			msg = nullptr;
			continue;
		}
		if(IsOld(msg, currentTime))
		{

			if (!WriteMessageToPacket(&packet, msg))
			{
				break;
			}
			else
			{
				//sending
				m_sentReliables.pop();
				msg->m_lastSentTimestampMS = currentTime;
				bundle->AddReliable(msg->m_reliableID);
				msg->ResetOffset();
				m_sentReliables.push(msg);
				//m_sentReliables.enqueue(msg);
				messagesSent++;
			}
		}
		else //We don't need to keep checking because everything after this is older
		{
			break;
		}
	}

	return messagesSent;
}

size_t NetConnection::AttachUnsentReliables(NetPacket& packet, AckBundle* bundle, double currentTime)
{
	size_t messagesSent = 0;

	while (!m_unsentReliables.empty())
	{
		uint16_t aliveReliableIDs = m_nextSentReliableID - m_oldestUnconfirmedReliableID;
		if (aliveReliableIDs >= MAX_RELIABLE_RANGE) {
			break;
		}

		NetMessage* msg = m_unsentReliables.front();
		msg->m_lastSentTimestampMS = currentTime;
		if (CanWriteMessageToPacket(&packet, msg)) {
			m_unsentReliables.pop();

			msg->m_reliableID = m_nextSentReliableID;
			++m_nextSentReliableID;
			msg->WriteReliableID();


			WriteMessageToPacket(&packet, msg);

			bundle->AddReliable(msg->m_reliableID);
			msg->ResetOffset();
			m_sentReliables.push(msg);
			messagesSent++;
		}
		else 
		{ 
			break; 
		}
	}
	return messagesSent;
}

bool NetConnection::IsOld(NetMessage* message, double currentTime)
{
	
	if ((currentTime - message->m_lastSentTimestampMS) > MESSAGE_OLD_THRESHOLD) //Old Message
	{
		return true;
	}

	return false;
}

void NetConnection::RemoveAllReliableIdsLessThan(uint16_t minimumReliableID)
{
	auto ridIter = m_receivedReliableIDs.begin();

	while (ridIter != m_receivedReliableIDs.end())
	{
		if (!CycleGreaterThanEqual((*ridIter), minimumReliableID))
		{
			//Remove
			ridIter = m_receivedReliableIDs.erase(ridIter);
		}
		else
		{
			++ridIter;
		}
	}
}

void NetConnection::ConfirmAck(uint16_t ack)
{
	//Find the ack bundle.
	AckBundle* correspondingBundle = this->FindAckBundle(ack); //Using the bundles[] on NetConnection.
	if (correspondingBundle != nullptr)
	{
		for each (uint16_t id in correspondingBundle->sentReliables)
		{
			MarkReliableConfirmed(id); //confirmedIds.push_back() but more logic.
		}
	}

	//Find all the messages associated with that bundle.
}

bool NetConnection::IsBitSetAtIndex(size_t index, uint16_t bitfield)
{
	return ((1 << index) & bitfield) != 0;
}


void NetConnection::UpdateAckBitField(uint16_t &highest, uint16_t &previous, uint16_t new_value)
{

	if (CycleGreaterThan(new_value, highest))
	{
		uint16_t shift = new_value - highest;
		previous = previous << shift;
		highest = new_value;
		SetBit(shift - 1, previous);
	}
	else
	{
		uint16_t index = highest - new_value;
		SetBit(index - 1, previous);
	}
}

bool NetConnection::IsReceived(uint16_t& value, uint16_t& highest, uint16_t& previous_bf)
{
	if (CycleGreaterThan(value, highest)) 
		return false;
	else if (value == highest)
	{
		return true;
	}
	else
	{
		uint16_t index = highest - value;
		return IsBitSetAtIndex(index - 1, previous_bf);
	}
}

void NetConnection::SetBit(uint16_t value, uint16_t& target)
{
	target |= BIT(value);
}

bool NetConnection::HasMessagesToSend()
{

	if (m_unreliables.size() > 0 || m_sentReliables.size() > 0 || m_unsentReliables.size() > 0)
	{
		return true;
	}

	return false;
}

std::string NetConnection::GetConnectionStateName()
{
	if (m_connectionState == eNetConnectionState_UNINITIALIZED)
	{
		return "UNINIT";
	}
	else if (m_connectionState == eNetConnectionState_UNCONFIRMED)
	{
		return "UNCONF";
	}
	else if (m_connectionState == eNetConnectionState_CONFIRMED)
	{
		return "CONF";
	}
	else if (m_connectionState == eNetConnectionState_LOCAL)
	{
		return "LOCAL";
	}
	else if (m_connectionState == eNetConnectionState_BAD)
	{
		return "BAD";
	}
	else if (m_connectionState == eNetConnectionState_DEAD)
	{
		return "DEAD";
	}
	else
	{
		return "?";
	}
}
