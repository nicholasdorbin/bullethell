#pragma once
#include <vector>

class TCPConnection;
class TCPListener;
class RemoteServiceConnection;
struct Event;

enum eConnectionStatus
{
	eConnectionStatus_Disconnected = 0,
	eConnectionStatus_Host,
	eConnectionStatus_Client
};

class RemoteCommandService
{
public:
	//#TODO Client / Host / Not Connected state


	unsigned char const MSG_ECHO = 1; //Just prints
	unsigned char const MSG_COMMAND = 2;
	unsigned char const MSG_RENAME = 3; //gives the client a name


	typedef RemoteServiceConnection CONN;
	TCPListener* m_listener;
	std::vector<RemoteServiceConnection*> m_connections; //TCPConnections is what TCPSockets were from Day 1
	eConnectionStatus m_status;
	

	void OnMessage(Event* eventData);
	//TEvent<CONN*> on_connection_join;
	//Tevent<CONN*> on_connection_leave;
	//Tevent<CONN*, unsigned char const message_id, char const* message> on_message;
	
	RemoteCommandService();//makes this disconnected
	void InitEvents();
	bool Host(char const* host_name); //just create a listener
	bool Join(char const* host_name); //create a TCP connection and add it to the connection list

	void StopAllConnections();
	
	
	
	void SendCommand(unsigned char command_id, char const * command); //Sends to server, server tells people things
	
	void Step();
	void CheckForConnection(); //accepts 
	void CheckForMessages(); //received
	void CheckForDisconnection(); //is_disconnected
};
