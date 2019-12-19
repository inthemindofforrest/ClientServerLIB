#include "ServerInit.h"
#include <cstdlib>
#include <string.h>
#include <Windows.h>


//Shutdown the Server SAFELY
void Server::Shutdown()
{
	//if the shutdown fails
	if (shutdown(sock, SD_BOTH) == SOCKET_ERROR)
	{
		//Display Shutdown Failure
		printf("Shutdown failed: %d on line: %d\n", WSAGetLastError(), __LINE__);
		closesocket(sock);
		WSACleanup();
		return;
	}
	//Inform Server of Shutdown
	printf("Shutdown Server\n");
	//shutdown the other Threads
	is_running = false;

	CloseAllThreads();//Closes all open threads safely

	printf("\n\n");
	system("PAUSE");
	exit(0);
}

Server::Server()
{
}

//Initialize the Server
Server::Server(sTUP _ConsoleFunction, TUP _SendClientFunction, sTUP _ProcessPacketFunction)
{
	winsock_version = 0x202;//Which version of Winsock we are using
	winsock_data;//Struct of data

	is_running = true;//Allows the other Threads to run

	//Sets what function will run when Packets are processed
	ProcessPacketFunction = _ProcessPacketFunction;

	//Starts the Console_Thread
	Console_Thread = std::thread([&]
	{
		while (is_running)
		{
			//Run the function that tracks Console commands
			ServerConsole(_ConsoleFunction);
		}
	});
	SendClientInfo_Thread = std::thread([&] 
	{
		while (is_running)
		{
			//function the Developer can use to send data to Client
			_SendClientFunction();
			//Once every frame or so
			std::this_thread::sleep_for(std::chrono::milliseconds(16));
		}
	});
}

//Starts the server and opens/sets Sockets
void Server::StartServer()
{
	//Starts up the Winsock 
	WSAStart();
	//
	CreateSocket();
	SetSocketAddress();

}
//Starts Server from a specific IP and Port
void Server::StartServer(char * _Address, char* _Port)
{
	WSAStart();
	CreateSocket();
	SetSocketAddress(_Address, atoi(_Port));
}

//Allows User Input to Init Server
void Server::StartConfigServer()
{
	char IP[IDENTIFY_BUFFER_SIZE];
	printf("IPv4: ");
	gets_s(IP, IDENTIFY_BUFFER_SIZE);
	char Port[IDENTIFY_BUFFER_SIZE];
	printf("Port: ");
	gets_s(Port, IDENTIFY_BUFFER_SIZE);
	StartServer(IP, Port);
}

//Updates the Server on the main thread
void Server::ServerUpdate()
{
	if (is_running)
	{
		//Recieves packets from incoming clients
		RecievedPacket();
	}
}

//Starts Winsock with Version and Data Struct
bool Server::WSAStart()
{
	if (WSAStartup(winsock_version, &winsock_data))
	{
		printf("WSAStartup failed: %d", WSAGetLastError());
		return false;
	}
	else
	{
		printf("Starting WSA\n");
	}
	return true;
}

//Creates a socket with settings (Unasigned Port)
bool Server::CreateSocket()
{
	//Creates socket
	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	//If Socket Failure, Inform Server
	if (sock == INVALID_SOCKET)//If INVALID_SOCKET then it has failed
	{
		printf("socket failed: %d", WSAGetLastError());
		return false;
	}
	else
	{
		printf("Opening Socket\n");
	}
	return true;
}
//Creates a socket with Personal Settings (Unasigned Port)
bool Server::CreateSocket(int _AddressFamily, int _Type, int _Protocol)
{
	//Creates the Socket
	sock = socket(_AddressFamily, _Type, _Protocol);
	//IF the Socket Fails, inform Server
	if (sock == INVALID_SOCKET)//If INVALID_SOCKET then it has failed
	{
		printf("socket failed: %d", WSAGetLastError());
		return false;
	}
	else
	{
		printf("Opening Socket\n");
	}
	return true;
}

//Assign Socket information with IP and Port
bool Server::SetSocketAddress()
{
	//Assigns the  
	local_address.sin_family = AF_INET;
	local_address.sin_port = htons(9999);//Packet needs to be bound to a port. Ports below 1024 are reserved. Chose 9999 - ... just a high number 
	local_address.sin_addr.s_addr = INADDR_ANY;//Local Address

	printf("Binding Socket to 10.15.20.13:%d\n", local_address.sin_port);

	//Binds the Socket with a SOCKADDR_IN
	if (bind(sock, (SOCKADDR*)&local_address, sizeof(local_address)) == SOCKET_ERROR)
	{
		printf("bind failed: %d", WSAGetLastError());
		return false;
	}
	else
	{
		printf("Server Finished Init!\n");
		printf("Server IP: %d.%d.%d.%d:%d\n",
			local_address.sin_addr.S_un.S_un_b.s_b1,
			local_address.sin_addr.S_un.S_un_b.s_b2,
			local_address.sin_addr.S_un.S_un_b.s_b3,
			local_address.sin_addr.S_un.S_un_b.s_b4,
			local_address.sin_port);
	}
	return true;
}
//Assign Socket information with Personal Address and Port
bool Server::SetSocketAddress(char* _Address, int _Port)
{
	local_address.sin_family = AF_INET;//IPV4 internet Protocol
	local_address.sin_port = htons(_Port);//Packet needs to be bound to a port. Ports below 1024 are reserved. Chose 9999 - ... just a high number 
	local_address.sin_addr.s_addr = inet_addr(_Address);//Sets the Address

	printf("Binding Socket to %s:%d\n",_Address, _Port);

	//Binds the Socket with the SOCKADDR_IN
	if (bind(sock, (SOCKADDR*)&local_address, sizeof(local_address)) == SOCKET_ERROR)
	{
		printf("bind failed: %d", WSAGetLastError());
		return false;
	}
	else
	{
		printf("Server Finished Init!\n");
		printf("Server IP: %d.%d.%d.%d:%d\n",
			local_address.sin_addr.S_un.S_un_b.s_b1,
			local_address.sin_addr.S_un.S_un_b.s_b2,
			local_address.sin_addr.S_un.S_un_b.s_b3,
			local_address.sin_addr.S_un.S_un_b.s_b4,
			local_address.sin_port);
	}
	return true;
}

//Retrieves the packets that are inbound
bool Server::RecievedPacket()
{
	//Gets the size of the packet that is incomming
	from_size = sizeof(from);
	//Recieves the packet and stores it into "buffer" and Returns Error results
	bytes_recieved = recvfrom(sock, buffer, IDENTIFY_BUFFER_SIZE, flags, (SOCKADDR*)&from, &from_size);

	//Error check the results of the socket
	if (bytes_recieved == SOCKET_ERROR)
	{
		printf("Recvfrom returned SOCKET_ERROR, WSAGetLastError() %d", WSAGetLastError());
		return false;
	}

	//Process the packet that has just came inbound
	ProcessPacket();

	return true;
}

//Processes the packet with Default commands and with a personal Developer functions
void Server::ProcessPacket()
{
	/*Process of the PACKETS*/

	//Convert Char* to std::string
	std::string CopiedData = buffer;
	//Creates a new string to store commands
	std::string Command;

	//Sets the command to the first 
	Command = ParsePacket(CopiedData);

	if (Command.compare("Join") == 0)
	{
		CheckForSession(from);
	}
 	else if (Command.compare("Disconnect") == 0)
	{
		DisconnectFromSessions(from);
	}
	//Function to process packets
	ProcessPacketFunction(&CopiedData);
}

//Creates a packet to send to the client
void Server::CreatePacket()
{
	//Create return Packet
	strcpy(SendingData, buffer);
	int32 write_index = 0;
	memcpy(&buffer[write_index], &SendingData, sizeof(SendingData));
}
//Creates a Packet with a custom Message
void Server::CreatePacket(const char * Message)
{
	strcpy(SendingData, Message);
	int32 write_index = 0;
	memcpy(&buffer[write_index], &SendingData, sizeof(SendingData));
}

//Sends the Packet that has been created
bool Server::SendPacket()
{
	return 	SendPacket(buffer, sizeof(SendingData), from);
}
//Sends the Packet of a custom message
bool Server::SendPacket(const char* _Message, int _Size)
{
	return SendPacket(_Message, _Size, from);
}
//Sends the Packet of a custom message to a custom reciever
bool Server::SendPacket(const char* _Message, int _Size, SOCKADDR_IN _Return)
{
	//Sets the flag to Zero by default
	flags = 0;
	//RETURN TO SENDER
	SOCKADDR* to = (SOCKADDR*)&_Return;
	//Length of the to Address
	int to_length = sizeof(_Return);
	//Send the buffer to the to address
	if (sendto(sock, _Message, _Size, flags, to, to_length) == SOCKET_ERROR)
	{
		printf("sendto failed: %d", WSAGetLastError());
		return false;
	}
	return true;
}

//Sends a Generic Packet with Session Size
void Server::SendClientsInfo()
{
	//Gets amount of sessions
	int Size = sizeof(Sessions) / sizeof(Session);
	//For Amount of sessions
	for (int i = 0; i < Size; i++)//Sessions
	{
		//For Client in Sessions
		for (int j = 0; j < Sessions[i].CurrentClientAmount; j++)//Each Client
		{
			char Message[IDENTIFY_BUFFER_SIZE];//Message to send
			int Index = 0;
			int CommandLength = 15;//Command Size

			//Attaches size of the command
			memcpy(&Message[Index], &CommandLength, sizeof(CommandLength));
			Index += sizeof(CommandLength);

			//Attaches Command Text
			memcpy(&Message[Index], "AmountOfPlayers", sizeof("AmountOfPlayers"));
			Index += sizeof("AmountOfPlayers");

			//Attaches size of amount
			memcpy(&Message[Index], std::to_string(Sessions[i].CurrentClientAmount).c_str(), sizeof(std::to_string(Sessions[i].CurrentClientAmount).c_str()));
			Index += sizeof(std::to_string(Sessions[i].CurrentClientAmount).c_str());

			//Sends the packet to the current client in the session
			SendPacket(Message,Index, Sessions[i].Clients[j]);
		}
	}
}

//Check Sessions for availability, and place client into session
void Server::CheckForSession(SOCKADDR_IN _Address)
{
	//Create and send generic packet to client
	CreatePacket();
	SendPacket();

	//If the client is currenly not in a session
	for (int i = 0; i < SessionsAmount; i++)
	{
		if (Sessions[i].CheckForClient(_Address))
		{
			return;
		}
	}

	//Find the Session that is available and place the player into session
	for (int i = 0; i < SessionsAmount; i++)
	{
		if (Sessions[i].AvailableSpace())//If there is space in the Session
		{
			//Place the use into that session
			Sessions[i].AddClientToSession(_Address);
			printf("Client IP: %d.%d.%d.%d:%d joined the session\n",
				_Address.sin_addr.S_un.S_un_b.s_b1,
				_Address.sin_addr.S_un.S_un_b.s_b2,
				_Address.sin_addr.S_un.S_un_b.s_b3,
				_Address.sin_addr.S_un.S_un_b.s_b4,
				_Address.sin_port);

			return;
		}
	}
}

//Checks a specific Session for availability and place client into session
void Server::CheckForSession(SOCKADDR_IN _Address, int _SessionIndex)
{
	//Check to make sure client is not in session currently
	for (int i = 0; i < SessionsAmount; i++)
	{
		if (Sessions[i].CheckForClient(_Address))
		{
			return;
		}
	}

	//If there is space in the Session
	if (Sessions[_SessionIndex].AvailableSpace())
	{
		//Place the use into that session
		Sessions[_SessionIndex].AddClientToSession(_Address);
		printf("Client IP: %d.%d.%d.%d:%d joined the session\n",
			_Address.sin_addr.S_un.S_un_b.s_b1,
			_Address.sin_addr.S_un.S_un_b.s_b2,
			_Address.sin_addr.S_un.S_un_b.s_b3,
			_Address.sin_addr.S_un.S_un_b.s_b4,
			_Address.sin_port);

		return;
	}
}

//Gets the amount of clients in each server
int* Server::GetSessionCount()
{
	int Total[5];//Creates array of Client amounts

	//Itterates through each session and gets its size
	for (int i = 0; i < 5; i++)
	{
		Total[i] = Sessions[i].CurrentClientAmount;
	}

	return Total;
}

//Disconnects client from the session they are in
void Server::DisconnectFromSessions(SOCKADDR_IN _Address)
{
	//Gets the amount of sessions
	int Size = sizeof(Sessions) / sizeof(Session);
	//For each session
	for (int i = 0; i < Size; i++)
	{
		//If the Client is in the session and they are removed correctly
		if (Sessions[i].RemoveClientFromSession(_Address))
		{
			printf("%d.%d.%d.%d:%d disconnected from Session(%d)\n",
				_Address.sin_addr.S_un.S_un_b.s_b1, _Address.sin_addr.S_un.S_un_b.s_b2,
				_Address.sin_addr.S_un.S_un_b.s_b3, _Address.sin_addr.S_un.S_un_b.s_b4,
				_Address.sin_port, i);
			return;
		}
	}
	printf("Client not connected to Server...\n");
}

//Console side of the server, with custom function
void Server::ServerConsole(sTUP _ConsoleFunction)
{
	//Creates a message for the user to enter a command
	char Message[IDENTIFY_BUFFER_SIZE];
	gets_s(Message, IDENTIFY_BUFFER_SIZE);
	//if the Server is still running
	if (is_running)
	{
		//Check Generic Commands
		if (strcmp(Message, "session") == 0)
		{
			for (int i = 0; i < SessionsAmount; i++)
			{
				Sessions[i].DisplayConnectedClients(i);
			}
			if (SessionsAmount == 0)
			{
				printf("No Sessions Opened\n");
			}
		}
		else if (strcmp(Message, "shutdown") == 0)
		{
			Shutdown();
		}
		else if(strcmp(Message, "send") == 0)
		{
			SendClientsInfo();
		}
		//Else convert the Command to a string
		std::string StringOfMessage = Message;
		//Send New String Message to the function of Developers choice
		_ConsoleFunction(&StringOfMessage);
	}
}

//Checks if two client SOCKADDR_IN addresses are the same
bool Server::MatchingSockAddress(SOCKADDR_IN _First, SOCKADDR_IN _Second)
{
	return(_First.sin_addr.S_un.S_un_b.s_b1 == _Second.sin_addr.S_un.S_un_b.s_b1 &&
		_First.sin_addr.S_un.S_un_b.s_b2 == _Second.sin_addr.S_un.S_un_b.s_b2 &&
		_First.sin_addr.S_un.S_un_b.s_b3 == _Second.sin_addr.S_un.S_un_b.s_b3 &&
		_First.sin_addr.S_un.S_un_b.s_b4 == _Second.sin_addr.S_un.S_un_b.s_b4 &&
		_First.sin_port == _Second.sin_port);
}

//Changes a given Sock address to a string
std::string Server::SockAddrInToString(SOCKADDR_IN _Address)
{
	std::string Text;
	Text.append(std::to_string(_Address.sin_addr.S_un.S_un_b.s_b1));
	Text.append(".");
	Text.append(std::to_string(_Address.sin_addr.S_un.S_un_b.s_b2));
	Text.append(".");
	Text.append(std::to_string(_Address.sin_addr.S_un.S_un_b.s_b3));
	Text.append(".");
	Text.append(std::to_string(_Address.sin_addr.S_un.S_un_b.s_b4));

	return Text;
}

//Sets all the values of the SOCKADDR_IN to Zero's
bool Server::ResetSockAddress(SOCKADDR_IN * _Address)
{
	(*_Address).sin_addr.S_un.S_un_b.s_b1 = 0;
	(*_Address).sin_addr.S_un.S_un_b.s_b2 = 0;
	(*_Address).sin_addr.S_un.S_un_b.s_b3 = 0;
	(*_Address).sin_addr.S_un.S_un_b.s_b4 = 0;
	(*_Address).sin_port = 0;
	return false;
}

//Gets the client IP from a session, given the session and Client Index's
SOCKADDR_IN Server::ClientIPFromSession(int _SessionID, int _ClientIndex)
{
	return Sessions[_SessionID].ClientIP(_ClientIndex);
}

//Gets all IP's from given Session
std::list<SOCKADDR_IN> Server::AllAvailableAddresses(int _SessionID)
{
	return Sessions[_SessionID].AvailableClientIP();
}

//Parses the packet by retrieving the first command and removing it from the string
std::string Server::ParsePacket(std::string* _Packet)
{
	//Final Parsed String
	std::string Parsed;

	//While the string does not hit end character
	while ((*_Packet)[0] != ',' && (*_Packet)[0] != ';' && (*_Packet)[0] != '\0')
	{
		//Add the current character into the Final string
		Parsed += (*_Packet)[0];
		//Erase the Character from the old string
		(*_Packet).erase(0, 1);
	}

	//If the character is currently a end character string, remove it
	if ((*_Packet)[0] == ',' || (*_Packet)[0] == ';')
		(*_Packet).erase(0, 1);

	return Parsed;
}
//Parses the packet by retrieving the first command, leaving the string alone
std::string Server::ParsePacket(std::string _Packet)
{
	//Final string to return
	std::string Parsed;
	//Counter to itterate the string
	int i = 0;

	//While the character is not a final Character end
	while (_Packet[i] != ',' && _Packet[i] != ';' && _Packet[i] != '\0')
	{
		//Add the character to the string
		Parsed += _Packet[i];
	}

	return Parsed;
}

//Closes all of the threads that are open from the server
void Server::CloseAllThreads()
{
	if (SendClientInfo_Thread.joinable())
	{
		SendClientInfo_Thread.join();
	}
	if (Console_Thread.joinable())
	{
		Console_Thread.join();
	}
}