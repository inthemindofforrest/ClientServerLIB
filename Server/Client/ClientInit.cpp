#include "ClientInit.h"
#include <cstdlib>
#include <string>

//Nothing is real. We are all concepts.
Positions::Positions() {}
//Init for position
Positions::Positions(int _X, int _Y)
{
	Value[0] = _X;
	Value[1] = _Y;
}

//Default Init for Clients
Client::Client()
{
	winsock_version = 0x202;//Which version of Winsock we are using
	winsock_data;//Struct of data
}

//Function is Obsolete!
void Client::StartClient()
{
	//Init
	WSAStart();
	AssignServerAddress();
	AssignServerSocket();

	//Init Variables
	is_running = true;
	HasSentMessage = false;
	IsConnected = false;

	//Start Recieve thread
	Receive_Thread = std::thread([&]
	{
		while (is_running)
		{
			ReceivePacket();
		}
	});
	printf("\n\nStartClient() is Obsolete, use StartClient(sTUP _ProcessPacketFunction) instead!\n\n");
}
//Starting Function for creating the Client
void Client::StartClient(sTUP _ProcessPacketFunction)
{
	//Init
	WSAStart();
	AssignServerAddress();
	AssignServerSocket();

	//Init Variables
	is_running = true;
	HasSentMessage = false;
	IsConnected = false;

	//Start Recieve thread
	Receive_Thread = std::thread([&]
	{
		while (is_running)
		{
			ReceivePacket(_ProcessPacketFunction);
		}
	});
}

//SAFELY shutdown client
bool Client::StopClient()
{
	//If the shudown is not successful 
	if (shutdown(sock, SD_BOTH) == SOCKET_ERROR)
	{
		//Report to console that shutdown has failed
		printf("Shutdown failed: %d on line: %d", WSAGetLastError(), __LINE__);
		closesocket(sock);
		WSACleanup();
		return false;
	}
	//Report to console, Client has disconnected from server
	printf("Disconnected from server.\n");
	IsConnected = false;
	return true;
}

//Function is Obsolete!
void Client::StartCustomClient()
{
	WSAStart();
	char Address[IDENTIFY_BUFFER_SIZE];
	char Port[IDENTIFY_BUFFER_SIZE];
	printf("IPv4: ");
	gets_s(Address, IDENTIFY_BUFFER_SIZE);
	printf("Port: ");
	gets_s(Port, IDENTIFY_BUFFER_SIZE);

	CustomServerAddress(Address, Port);
	AssignServerSocket();

	is_running = true;
	SendPacket("Join");



	Receive_Thread = std::thread([&]
	{
		while (is_running)
		{
			ReceivePacket();
		}
	});

	printf("\n\nStartCustomClient() is Obsolete, use StartCustomClient(sTUP _ProcessPacketFunction) instead!\n\n");
}
//Starts a custom client to ask for input when called
void Client::StartCustomClient(sTUP _ProcessPacketFunction)
{
	WSAStart();
	char Address[IDENTIFY_BUFFER_SIZE];
	char Port[IDENTIFY_BUFFER_SIZE];
	printf("IPv4: ");
	gets_s(Address, IDENTIFY_BUFFER_SIZE);
	printf("Port: ");
	gets_s(Port, IDENTIFY_BUFFER_SIZE);

	CustomServerAddress(Address, Port);
	AssignServerSocket();

	is_running = true;
	SendPacket("Join");



	Receive_Thread = std::thread([&]
	{
		while (is_running)
		{
			ReceivePacket();
		}
	});


}
//Starts a custom client to requested IP and port
void Client::StartCustomClient(const char* _IP, int _Port)
{
	WSAStart();

	CustomServerAddress(_IP, _Port);
	AssignServerSocket();

	is_running = true;
	SendPacket("Join");



	Receive_Thread = std::thread([&]
	{
		while (is_running)
		{
			ReceivePacket();
		}
	});


}

//Run the Send packet on the MAIN THREAD
void Client::UpdateClient()
{
	if (is_running)
	{
		SendPacket();
	}
}

//Starts up Winsock
bool Client::WSAStart()
{
	int Temp = WSAStartup(winsock_version, &winsock_data);
	if (Temp)//Return 0 if successful
	{
		printf("WSAStartup failed: %d on line: %d, Returned: %d", WSAGetLastError(), __LINE__, Temp);
		return false;
	}
	return true;
}

//Assigns the Server address that should be connecting to
void Client::AssignServerAddress()
{
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(9999);
	server_address.sin_addr.S_un.S_addr = inet_addr("10.15.20.13");
}

//Creates Socket that server is opened with
void Client::AssignServerSocket()
{
	int address_family = AF_INET;//Acts as the IPv4 address
	int type = SOCK_DGRAM;// Type is a UDP socket(SOCK_DGRAM)
	int protocol = IPPROTO_UDP;//
	sock = socket(address_family, type, protocol);
}

//Assigns the server with a given IP and Port (NOT Const)
void Client::CustomServerAddress(char * _Address, char* _Port)
{
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(atoi(_Port));
	server_address.sin_addr.S_un.S_addr = inet_addr(_Address);
}
//Assigns the server with a given IP and Port (Const)
void Client::CustomServerAddress(const char * _Address, int _Port)
{
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(_Port);
	server_address.sin_addr.S_un.S_addr = inet_addr(_Address);
}

//Sends a packet from input sent through Console
bool Client::SendPacket()
{
	//Get Input
	gets_s(SentData, IDENTIFY_BUFFER_SIZE);

	//Check to see if Input is Command
	if (ClientConsole(SentData))
	{
		//Return if the input was a command
		return false;
	}

	return SendPacket(SentData);
}
//Sends a pakcet given a char*
bool Client::SendPacket(const char* _data)
{
	strcpy(SentData, _data);
	if (is_running)
	{
		flags = 0;
		if (sendto(sock, SentData, sizeof(SentData), flags, (SOCKADDR*)&server_address, sizeof(server_address)) == SOCKET_ERROR)
		{
			printf("sendto failed: %d on line: %d", WSAGetLastError(), __LINE__);

			return false;
		}
		HasSentMessage = true;
	}
	return true;
}
//Is Obsolete, and refers back to ^
bool Client::SendPacket(const char * _data, int _Size, int _NumData)
{
	return SendPacket(_data);
}
//Sends data, No matter if there is a connection or not.
bool Client::ForceSendPacket(const char* _data, int _Size, int _NumData)
{
	int Index = 0;
	char temp[IDENTIFY_BUFFER_SIZE]{ '\0' };

	memcpy(&temp[Index], _data, _Size);
	Index += _Size;

	flags = 0;
	if (sendto(sock, temp, sizeof(temp), flags, (SOCKADDR*)&server_address, sizeof(server_address)) == SOCKET_ERROR)
	{
		printf("sendto failed: %d on line: %d", WSAGetLastError(), __LINE__);

		return false;
	}
}

//Function is OBSOLETE
bool Client::ReceivePacket()
{
	//If Server has established a connection
	if (is_running && HasSentMessage)
	{
		//Reset Flag
		flags = 0;
		//Get the size of the address of incoming address
		from_size = sizeof(from);

		//Saves packet to buffer, and saves result
		bytes_received = recvfrom(sock, buffer, 1024, flags, (SOCKADDR*)&from, &from_size);

		//If result has errored, Error
		if (bytes_received == SOCKET_ERROR)
		{
			printf("recvfrom returned SOCKET_ERROR, WSAGetLastError() %d on line: %d\n", WSAGetLastError(), __LINE__);
			return false;
		}
		//Process the information that was recieved
		ProcessPacket(buffer);
	}
	printf("\n\nRecievePacket() is obsolete, use ReceivePacket(sTUP _ProcessPacketFunction) instead!\n\n");
	return true;
}
//Recieves Packet with custom function to handle process
bool Client::ReceivePacket(sTUP _Function)
{
	//If Server has established a connection
	if (is_running && HasSentMessage)
	{
		//Reset Flag
		flags = 0;
		//Get the size of the address of incoming address
		from_size = sizeof(from);

		//Saves packet to buffer, and saves result
		bytes_received = recvfrom(sock, buffer, 1024, flags, (SOCKADDR*)&from, &from_size);

		//If result has errored, Error
		if (bytes_received == SOCKET_ERROR)
		{
			printf("recvfrom returned SOCKET_ERROR, WSAGetLastError() %d on line: %d\n", WSAGetLastError(), __LINE__);
			return false;
		}
		//Process the information that was recieved
		ProcessPacket(buffer, _Function);
	}
	return true;
}

//Check console message for command
bool Client::ClientConsole(char * _Message)
{
	if (is_running)
	{
		//If command is shutdown
		if (strcmp(_Message, "shutdown") == 0)
		{
			//Tell Server to remove client
			SendPacket("Remove");
			//Stop Client Safely
			StopClient();
			//Stop other Threads
			is_running = false;

			printf("\n\n");
			system("PAUSE");
			//exit(0);
			return true;
		}
		return false;
	}
}

//Compares two SOCKADDR_IN's to see if they are the same
bool Client::CompareAddresses(SOCKADDR_IN _First, SOCKADDR_IN _Second)
{
	return (_First.sin_addr.S_un.S_un_b.s_b1 == _Second.sin_addr.S_un.S_un_b.s_b2 &&
		_First.sin_addr.S_un.S_un_b.s_b2 == _Second.sin_addr.S_un.S_un_b.s_b2 &&
		_First.sin_addr.S_un.S_un_b.s_b3 == _Second.sin_addr.S_un.S_un_b.s_b3 &&
		_First.sin_addr.S_un.S_un_b.s_b4 == _Second.sin_addr.S_un.S_un_b.s_b4);
}

//Display connection with server
void Client::DisplayConnection(const char * _data)
{
	//if Message is Join, Client joined the server
	if (strcmp(_data, "Join") == 0)
	{
		printf("Joined %d.%d.%d.%d:%d\n",
			server_address.sin_addr.S_un.S_un_b.s_b1,
			server_address.sin_addr.S_un.S_un_b.s_b2,
			server_address.sin_addr.S_un.S_un_b.s_b3,
			server_address.sin_addr.S_un.S_un_b.s_b4,
			server_address.sin_port);
		IsConnected = true;
	}
	//If Message is Disconnect, Client disconnected from the server
	if (strcmp(_data, "Disconnect") == 0)
	{
		printf("Disconnected from %d.%d.%d.%d:%d\n",
			server_address.sin_addr.S_un.S_un_b.s_b1,
			server_address.sin_addr.S_un.S_un_b.s_b2,
			server_address.sin_addr.S_un.S_un_b.s_b3,
			server_address.sin_addr.S_un.S_un_b.s_b4,
			server_address.sin_port);
		IsConnected = false;
	}
}

//Function has been obsolete
void Client::ProcessPacket(char * _Data)
{
	//Convert char* to std::string
	std::string DataCopy = _Data;
	//command for which comparing
	std::string Command;
	//Clear the buffer now that we saved the input
	ClearArray(buffer, IDENTIFY_BUFFER_SIZE);

	//Command is the first phrase from the data
	Command = ParsePacket(&DataCopy);

	printf("\n\nFunction is Obsolete. Use ProcessPacket(char * _Data, sTUP _ProcessPacketFunction) instead.\n\n");
}

//Processes the data and allows the developer to choose what happens with data
void Client::ProcessPacket(char * _Data, sTUP _Function)
{
	std::string DataCopy = _Data;
	_Function(&DataCopy);
}

//Closes all the threads that are still open.
//Needs to be ran at end of programs life
void Client::CloseAllThreads()
{
	if (Receive_Thread.joinable())
	{
		Receive_Thread.join();
	}
}

//Parses the packet(Pass by Address)
std::string Client::ParsePacket(std::string* _Packet)
{
	std::string Parsed;

	while ((*_Packet)[0] != ',' && (*_Packet)[0] != ';' && (*_Packet)[0] != '\0')
	{
		Parsed += (*_Packet)[0];
		(*_Packet).erase(0, 1);
	}

	if ((*_Packet)[0] == ',' || (*_Packet)[0] == ';')
		(*_Packet).erase(0, 1);

	return Parsed;
}
//Parses the packet(Pass by copy)
std::string Client::ParsePacket(std::string _Packet)
{
	std::string Parsed;

	while ((_Packet)[0] != ',' && (_Packet)[0] != ';' && (_Packet)[0] != '\0')
	{
		Parsed += (_Packet)[0];
		(_Packet).erase(0, 1);
	}

	return Parsed;
}

//Clears any array with '\0' characters
void Client::ClearArray(char * _Array, int _Size)
{
	for (int i = 0; i < _Size; i++)
	{
		_Array[i] = '\0';
	}
}
