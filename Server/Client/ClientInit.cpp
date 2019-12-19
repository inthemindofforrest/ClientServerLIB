#include "ClientInit.h"
#include <cstdlib>
#include <string>

Positions::Positions(){}
Positions::Positions(int _X, int _Y)
{
	Value[0] = _X;
	Value[1] = _Y;
}


Client::Client()
{
	winsock_version = 0x202;//Which version of Winsock we are using
	winsock_data;//Struct of data
}

void Client::StartClient()
{
	WSAStart();
	AssignServerAddress();
	AssignServerSocket();

	is_running = true;
	HasSentMessage = false;
	IsConnected = false;
	//printf("Client: %d.%d.%d.%d:%d trying to join Server: %d.%d.%d.%d:%d")

	Receive_Thread = std::thread([&]
	{
		while (is_running)
		{
			ReceivePacket();
		}
	});
}

bool Client::StopClient()
{
	if (shutdown(sock, SD_BOTH) == SOCKET_ERROR)
	{
		printf("Shutdown failed: %d on line: %d", WSAGetLastError(), __LINE__);
		closesocket(sock);
		WSACleanup();
		return false;
	}
	printf("Disconnected from server.\n");
	IsConnected = false;
	return true;
}

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


}
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

void Client::UpdateClient()
{
	if (is_running)
	{
		SendPacket();
	}
}

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

void Client::AssignServerAddress()
{
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(9999);
	server_address.sin_addr.S_un.S_addr = inet_addr("10.15.20.13");
}

void Client::AssignServerSocket()
{
	int address_family = AF_INET;//Acts as the IPv4 address
	int type = SOCK_DGRAM;// Type is a UDP socket(SOCK_DGRAM)
	int protocol = IPPROTO_UDP;//
	sock = socket(address_family, type, protocol);
}

void Client::CustomServerAddress(char * _Address, char* _Port)
{
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(atoi(_Port));
	server_address.sin_addr.S_un.S_addr = inet_addr(_Address);
}
void Client::CustomServerAddress(const char * _Address, int _Port)
{
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(_Port);
	server_address.sin_addr.S_un.S_addr = inet_addr(_Address);
}

bool Client::SendPacket()
{
	gets_s(SentData, IDENTIFY_BUFFER_SIZE);

	if (ClientConsole(SentData))
	{
		return false;
	}

	if (is_running)
	{
		flags = 0;
		if (sendto(sock, SentData, sizeof(SentData), flags, (SOCKADDR*)&server_address, sizeof(server_address)) == SOCKET_ERROR)
		{
			printf("sendto failed: %d on line: %d", WSAGetLastError(), __LINE__);

			return false;
		}
		else
		{
			printf("Sending Package to: %d.%d.%d.%d:%d\n",
				server_address.sin_addr.S_un.S_un_b.s_b1,
				server_address.sin_addr.S_un.S_un_b.s_b2,
				server_address.sin_addr.S_un.S_un_b.s_b3,
				server_address.sin_addr.S_un.S_un_b.s_b4,
				server_address.sin_port);
		}
		HasSentMessage = true;
	}
	return true;
}
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
		else
		{
			/*printf("Sending Package to: %d.%d.%d.%d:%d\n",
				server_address.sin_addr.S_un.S_un_b.s_b1,
				server_address.sin_addr.S_un.S_un_b.s_b2,
				server_address.sin_addr.S_un.S_un_b.s_b3,
				server_address.sin_addr.S_un.S_un_b.s_b4,
				server_address.sin_port);*/
		}
		HasSentMessage = true;
	}
	return true;
}
bool Client::SendPacket(const char * _data, int _Size, int _NumData)
{
	if (is_running)
	{
		int Index = 0;
		char temp[IDENTIFY_BUFFER_SIZE]{'\0'};

		memcpy(&temp[Index], _data, _Size);
		Index += _Size;

		flags = 0;
		if (sendto(sock, temp, sizeof(temp), flags, (SOCKADDR*)&server_address, sizeof(server_address)) == SOCKET_ERROR)
		{
			printf("sendto failed: %d on line: %d", WSAGetLastError(), __LINE__);

			return false;
		}
		else
		{
			printf("Sending Package to: %d.%d.%d.%d:%d\n",
				server_address.sin_addr.S_un.S_un_b.s_b1,
				server_address.sin_addr.S_un.S_un_b.s_b2,
				server_address.sin_addr.S_un.S_un_b.s_b3,
				server_address.sin_addr.S_un.S_un_b.s_b4,
				server_address.sin_port);
		}
		HasSentMessage = true;

		DisplayConnection(_data);
	}
	return true;
}
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
	else
	{
		printf("Sending Package to: %d.%d.%d.%d:%d\n",
			server_address.sin_addr.S_un.S_un_b.s_b1,
			server_address.sin_addr.S_un.S_un_b.s_b2,
			server_address.sin_addr.S_un.S_un_b.s_b3,
			server_address.sin_addr.S_un.S_un_b.s_b4,
			server_address.sin_port);
	}
}


bool Client::ReceivePacket()
{
	if (is_running && HasSentMessage)
	{
		//Waiting for reply ;)
		flags = 0;
		from_size = sizeof(from);

		bytes_received = recvfrom(sock, buffer, 1024, flags, (SOCKADDR*)&from, &from_size);

		if (bytes_received == SOCKET_ERROR)
		{
			printf("recvfrom returned SOCKET_ERROR, WSAGetLastError() %d on line: %d\n", WSAGetLastError(), __LINE__);
			return false;
		}
		ProcessPacket(buffer);
	}
	return true;
}

bool Client::ClientConsole(char * _Message)
{
	if (is_running)
	{
		if (strcmp(_Message, "shutdown") == 0)
		{
			SendPacket("Remove");
			StopClient();
			is_running = false;

			printf("\n\n");
			system("PAUSE");
			//exit(0);
			return true;
		}
		return false;
	}
}

bool Client::CompareAddresses(SOCKADDR_IN _First, SOCKADDR_IN _Second)
{
	return (_First.sin_addr.S_un.S_un_b.s_b1 == _Second.sin_addr.S_un.S_un_b.s_b2 &&
		_First.sin_addr.S_un.S_un_b.s_b2 == _Second.sin_addr.S_un.S_un_b.s_b2 &&
		_First.sin_addr.S_un.S_un_b.s_b3 == _Second.sin_addr.S_un.S_un_b.s_b3 &&
		_First.sin_addr.S_un.S_un_b.s_b4 == _Second.sin_addr.S_un.S_un_b.s_b4);
}

void Client::DisplayConnection(const char * _data)
{
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

void Client::ProcessPacket(char * _Data)
{
	std::string DataCopy = _Data;
	std::string Command;
	int SizeOfCommand;
	ClearArray(buffer, IDENTIFY_BUFFER_SIZE);

	Command = ParsePacket(&DataCopy);

	if (Command.compare("AmountOfPlayers") == 0)
	{
		printf("Recieved");
	}
}

void Client::ProcessPacket(char * _Data, sTUP _Function)
{
	std::string DataCopy = _Data;
	_Function(&DataCopy);
}

void Client::CloseAllThreads()
{
	if (Receive_Thread.joinable())
	{
		Receive_Thread.join();
	}
}

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

void Client::ClearArray(char * _Array, int _Size)
{
	for (int i = 0; i < _Size; i++)
	{
		_Array[i] = '\0';
	}
}
