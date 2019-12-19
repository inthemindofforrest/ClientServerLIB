#pragma once
#include <WinSock2.h>
#include <stdio.h>
#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include <iostream>
#include <list>

#pragma warning(disable:4996) 

typedef unsigned int int32;
typedef char int8;
using sTUP = void(*)(std::string *);


struct Positions
{
public:
	SOCKADDR_IN Address;
	int Value[2]{ 0 };
	int Color;
	bool Active;

	Positions();
	Positions(int _X, int _Y);
};


class Client
{
	WORD winsock_version;
	WSADATA winsock_data;
	SOCKET sock;
	SOCKADDR_IN server_address;

	int8 buffer[IDENTIFY_BUFFER_SIZE];
	int flags;

	SOCKADDR_IN from;
	int from_size;
	int bytes_received;

	char RecievedData[IDENTIFY_BUFFER_SIZE];
	char SentData[IDENTIFY_BUFFER_SIZE];

	std::atomic<bool> HasSentMessage;

public:
	Client();

	bool IsConnected;

	std::thread Receive_Thread;

	std::atomic<bool> is_running;
	std::mutex IsDrawing;

	void StartClient();
	bool StopClient();
	void StartCustomClient();
	void StartCustomClient(const char* _IP, int _Port);
	void UpdateClient();

	bool WSAStart();
	void AssignServerAddress();
	void AssignServerSocket();
	
	void CustomServerAddress(char* _Address, char* _Port);
	void CustomServerAddress(const char* _Address, int _Port);

	bool SendPacket();
	bool SendPacket(const char* _data);
	bool SendPacket(const char* _data, int _Size, int _NumData);
	bool ForceSendPacket(const char* _data, int _Size, int _NumData);
	bool ReceivePacket();
	bool ClientConsole(char* _Message);

	bool CompareAddresses(SOCKADDR_IN _First, SOCKADDR_IN _Second);

	void DisplayConnection(const char * _data);
	void ProcessPacket(char* _Data);
	void ProcessPacket(char* _Data, sTUP _Function);

	void CloseAllThreads();


	std::string ParsePacket(std::string* _Packet);//Should change Packet
	std::string ParsePacket(std::string _Packet);//Should change Packet

	void ClearArray(char* _Array, int _Size);

};