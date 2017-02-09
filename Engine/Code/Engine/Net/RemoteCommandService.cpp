#include "Engine/Net/RemoteCommandService.hpp"
#include "Engine/Net/NetworkSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Net/tcpip/TCPListener.hpp"
#include "Engine/Net/tcpip/TCPConnection.hpp"
#include "Engine/Net/RemoteServiceConnection.hpp"
#include "Engine/Net/NetworkSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Events/EventSystem.hpp"
#include "Engine/Core/Console.hpp"



void RemoteCommandService::OnMessage(Event* eventData)
{
	RCSMessageEvent* messageEventData = (RCSMessageEvent*)eventData;

	if (messageEventData->msg_id == MSG_COMMAND)
	{
		//ProcessCommand
		g_theConsole->ExecuteCommand(messageEventData->msg);
	}
	else //MSG_ECHO
	{
		g_theConsole->ConsolePrint(messageEventData->msg);
	}
}

RemoteCommandService::RemoteCommandService()
	: m_listener(nullptr)
	, m_status(eConnectionStatus_Disconnected)
{

}

void RemoteCommandService::InitEvents()
{
	EventSystem* eventSys = EventSystem::GetInstance();
	eventSys->RegisterEvent<RemoteCommandService, &RemoteCommandService::OnMessage>(std::string("onMessage"), this);
}

bool RemoteCommandService::Host(char const* host_name)
{
	//Make a listener and open it
	m_listener = new TCPListener(host_name, DEFAULT_PORT, 2);
	
	return m_listener->IsListening();
}

bool RemoteCommandService::Join(char const* host_name)
{
	TCPConnection* connection = new TCPConnection(host_name, DEFAULT_PORT);

	if (connection->IsConnected())
	{
		RemoteServiceConnection* newConnection = new RemoteServiceConnection(connection);
		m_connections.push_back(newConnection);
		return true;
	}
	return false;
}

void RemoteCommandService::StopAllConnections()
{
	for each(CONN* conn in m_connections)
	{
		//send on that socket
		RemoteServiceConnection* c = conn;

		c->m_tcpConn->Disconnect();
	}
}

void RemoteCommandService::SendCommand(unsigned char command_id, char const * command)
{
	for each(CONN* conn in m_connections)
	{
		//send on that socket
		RemoteServiceConnection* c = conn;

		c->Send(command_id, command);
	}

}

void RemoteCommandService::Step()
{
	if (m_status != eConnectionStatus_Disconnected)
	{
		CheckForConnection(); //accepts 
		CheckForMessages(); //received
		CheckForDisconnection(); //is_disconnected
	}
}

void RemoteCommandService::CheckForConnection()
{
	if (m_listener == nullptr)
		return;
	TCPConnection* newTCPConnection =  m_listener->Accept();

	if (newTCPConnection)
	{
		RemoteServiceConnection* newConnection = new RemoteServiceConnection(newTCPConnection);
		m_connections.push_back(newConnection);
	}
}

void RemoteCommandService::CheckForMessages()
{

// 	size_t const BUFFER_SIZE = 1024;
// 	unsigned char* buffer[BUFFER_SIZE];

	for each(CONN* conn in m_connections)
	{
		conn->Receive();
	}
}

void RemoteCommandService::CheckForDisconnection()
{
	std::vector<CONN*>::iterator removeIter = m_connections.begin();
	while (removeIter != m_connections.end()) {
		CONN* b = *removeIter;
		if (!b->m_tcpConn->IsConnected())
		{
			removeIter = m_connections.erase(removeIter);
			delete b;
		}
		else
			++removeIter;
	}
}

CONSOLE_COMMAND(command_server_host)
{
	args;
	//Make this service be the host
	const char * localHost = g_networkSystem->GetLocalHostName();
	if (g_networkService->Host(localHost))
	{
		const sockaddr* addr = (sockaddr*)&g_networkService->m_listener->m_addr;
		std::string hostName = g_networkSystem->SockAddrToString(addr);
		g_theConsole->ConsolePrint(hostName);
		g_networkService->m_status = eConnectionStatus_Host;
	}

}

CONSOLE_COMMAND(command_server_stop)
{
	args;
	//Stop the Listener and stop all my connections
	if (g_networkService->m_status == eConnectionStatus_Host)
	{
		g_networkService->m_listener->Stop();
		g_networkService->StopAllConnections();
		g_networkService->m_status = eConnectionStatus_Disconnected;
		g_theConsole->ConsolePrint("Stopped hosting.");
	}

}

CONSOLE_COMMAND(command_server_join)
{
	if (args.m_argList.size() == 1 && g_networkService->m_status == eConnectionStatus_Disconnected)
	{
		const char* hostName = args.m_argList[0].c_str();
		if (g_networkService->Join(hostName))
		{
			g_networkService->m_status = eConnectionStatus_Client;
			g_theConsole->ConsolePrint("Connected to " + args.m_argList[0]);
		}
	}
}


CONSOLE_COMMAND(command_server_leave)
{
	args;
	//Stop the Listener and stop all my connections
	if (g_networkService->m_status == eConnectionStatus_Client)
	{
		g_networkService->StopAllConnections();
		g_networkService->m_status = eConnectionStatus_Disconnected;
		g_theConsole->ConsolePrint("Stopped connecting to host.");
	}
}

CONSOLE_COMMAND(send_command)
{
	if (args.m_argList.size() > 1 && g_networkService->m_status != eConnectionStatus_Disconnected)
	{
		unsigned int command_id = atoi(args.m_argList[0].c_str());
		const char* command = args.m_argList[1].c_str();
		g_networkService->SendCommand((unsigned char)command_id, command);
	}
}

CONSOLE_COMMAND(command_server_info)
{
	args;
	g_theConsole->ConsolePrint("Connection status:", Rgba::GREEN);
	if (g_networkService->m_status == eConnectionStatus_Host)
	{
		const sockaddr* addr = (sockaddr*)&g_networkService->m_listener->m_addr;
		std::string hostName = g_networkSystem->SockAddrToString(addr);
		g_theConsole->ConsolePrint("Hosting @ "  + hostName, Rgba::GREEN);
		g_theConsole->ConsolePrint("Connections:", Rgba::GREEN);
		int index = 1;
		for each (RemoteServiceConnection* conn in g_networkService->m_connections)
		{
			std::string connStr = conn->GetAddrName();
			g_theConsole->ConsolePrint((index + " " + connStr), Rgba::GREEN);
			index++;
		}

	}
	else if (g_networkService->m_status == eConnectionStatus_Client)
	{
		if (g_networkService->m_connections.size() > 0)
		{
			std::string connStr = g_networkService->m_connections[0]->GetAddrName();
			g_theConsole->ConsolePrint("Client @ " + connStr, Rgba::GREEN);
		}
		else
		{
			g_theConsole->ConsolePrint("BUG: Client has no connections", Rgba::RED);
		}
		
	}
	else
	{
		g_theConsole->ConsolePrint("Not Connected", Rgba::GREEN);
	}
}

CONSOLE_COMMAND(net_list_tcp_addresses)
{
	args;
	if (args.m_argList.size() > 0)
	{
		const char* hostName = g_networkSystem->GetLocalHostName();
		const char* portName = args.m_argList[0].c_str();
		addrinfo *info_list = g_networkSystem->AllocAddressesForHost(hostName, // an address for this machine
			portName, // service, which for TCP/IP is the port as a string (ex: "80")
			AF_INET, // We're doing IPv4 in class
			SOCK_STREAM, // TCP for now
			AI_PASSIVE);

		addrinfo* thisInfo = info_list;
		g_theConsole->ConsolePrint("Available TCP addresses:", Rgba::GREEN);
		while (thisInfo != nullptr)
		{
			const char* tcpName = g_networkSystem->SockAddrToString(thisInfo->ai_addr);
			g_theConsole->ConsolePrint(tcpName, Rgba::GREEN);
			thisInfo = thisInfo->ai_next;
		}
	}
	
}