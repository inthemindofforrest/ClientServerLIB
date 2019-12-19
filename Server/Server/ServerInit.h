#pragma once
#include <WinSock2.h>
#include <stdio.h>
#include <string>
#include "Session.h"
#include <thread>
#include <atomic>
#include <iostream>
#include <list>


#pragma warning(disable:4996) 

typedef unsigned int int32;
typedef char int8;

using sTUP = void(*)(std::string*);

class Server
{
#pragma region Networking Specific
	WORD winsock_version;
	WSADATA winsock_data;
    SOCKET sock;
	SOCKADDR_IN local_address;
	int8 buffer[IDENTIFY_BUFFER_SIZE]{""};
	int flags = 0; 
	SOCKADDR_IN from;
	int from_size;
	int bytes_recieved;
#pragma endregion
	char SendingData[IDENTIFY_BUFFER_SIZE];
	
	void Shutdown();

	sTUP ProcessPacketFunction;

public:
	static const int SessionsAmount = 5;
	Session Sessions[SessionsAmount];
	bool Testing = false;

	std::thread SendClientInfo_Thread;
	std::thread Console_Thread;
	std::atomic<bool> is_running;

	Server();
	Server(sTUP _ConsoleFunction, TUP _SendClientFunction, sTUP _ProcessPacketFunction);

	void StartServer();
	void StartServer(char* _Address, char* _Port);
	void StartConfigServer();
	void ServerUpdate();

	bool WSAStart();

	bool CreateSocket();
	bool CreateSocket(int _AddressFamily, int _Type, int _Protocol);

	bool SetSocketAddress();
	bool SetSocketAddress(char* _Address, int _Port);

	bool RecievedPacket();
	void ProcessPacket();

	void CreatePacket();
	void CreatePacket(const char* Message);
	bool SendPacket();
	bool SendPacket(const char* _Message, int _Size);
	bool SendPacket(const char* _Message, int _Size, SOCKADDR_IN _Return);

	void SendClientsInfo();

	void CheckForSession(SOCKADDR_IN _Address);
	void CheckForSession(SOCKADDR_IN _Address, int _SessionIndex);
	int* GetSessionCount();
	void DisconnectFromSessions(SOCKADDR_IN _Address);

	void ServerConsole(sTUP _ConsoleFunction);

	static bool MatchingSockAddress(SOCKADDR_IN _First, SOCKADDR_IN _Second);
	static std::string SockAddrInToString(SOCKADDR_IN _Address);
	static bool ResetSockAddress(SOCKADDR_IN* _Address);

	SOCKADDR_IN ClientIPFromSession(int _SessionID, int _ClientIndex);
	std::list<SOCKADDR_IN> AllAvailableAddresses(int _SessionID);

	std::string ParsePacket(std::string* _Packet);//Change _Packet
	std::string ParsePacket(std::string _Packet);//DO NOT CHANGE _Packet

	void CloseAllThreads();
};