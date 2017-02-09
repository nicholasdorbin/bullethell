#include "Engine/Net/RemoteServiceConnection.hpp"
#include "Engine/Net/tcpip/TCPConnection.hpp"


void RemoteServiceConnection::Send(unsigned char msg_id, char const* msg)
{
	m_tcpConn->Send(&msg_id, 1); //one byte long

	m_tcpConn->Send(msg, strlen(msg));//send the actual message
	char nill = NULL;
	m_tcpConn->Send(&nill, 1);//Determined internally when a message is done
}

RemoteServiceConnection::RemoteServiceConnection(TCPConnection* tcpconn)
{
	m_tcpConn = tcpconn;
}

void RemoteServiceConnection::Receive()
{
	
	if (!m_tcpConn->IsConnected())
		return;
	size_t const BUFFER_SIZE = 1024;
	unsigned char buffer[BUFFER_SIZE];

	size_t read = m_tcpConn->Receive(buffer, BUFFER_SIZE);

	while(read > 0)
	{
		for(size_t i = 0; i < read; ++i)
		{
			unsigned char c = buffer[i];
			next_message.push_back(c);
			if(c == NULL)
			{
				//On_message.trigger(this, next_message[0], &next_message[1]);
				RCSMessageEvent eventData;
				eventData.rcs = this;
				eventData.msg_id = next_message[0];
				eventData.msg = &next_message[1];
				EventSystem::GetInstance()->TriggerEvent("onMessage", &eventData);
				next_message.clear();
			}
		}
		read = m_tcpConn->Receive(&buffer, BUFFER_SIZE);
	}
	
}

char const* RemoteServiceConnection::GetAddrName()
{
	return m_tcpConn->m_addrName;
}

