#pragma once
#include <WinSock2.h>
#include <stdio.h>
#include <iostream>
#include <list>
#include <string>
#include <mutex>

//#define Server bool AddClientToSession(SOCKADDR_IN _Address);
//#define Server void SessionUpdate(TUP _Function);

struct Positions
{
public:
	SOCKADDR_IN Address;
	int Value[2]{0};
	int Color;

	bool operator==(const Positions _Other);
	Positions();
	Positions(int _X, int _Y, int _Color);
};

using TUP = void(*)();

class Session
{
private:
	static const int MaxClientAmount = 3;//Forever the max players allowed in the session

public:
	Positions ClientPositions[MaxClientAmount];
	SOCKADDR_IN Clients[MaxClientAmount];
	int CurrentClientAmount = 0;

	Session();

	void SessionUpdate(TUP _Function);

	bool AddClientToSession(SOCKADDR_IN _Address);
	bool RemoveClientFromSession(SOCKADDR_IN _Address);
	bool AvailableSpace();
	bool DisplayConnectedClients(int _SessionID);

	bool CheckForClient(SOCKADDR_IN _Address);

	static bool MatchingSockAddress(SOCKADDR_IN _First, SOCKADDR_IN _Second);

	std::list<Positions> RetrieveClientPositions();
	SOCKADDR_IN ClientIP(int _ClientIndex);
	std::list<SOCKADDR_IN> AvailableClientIP();

	std::string ParsePacket(std::string* _Packet);
};