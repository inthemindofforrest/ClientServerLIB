#include "Session.h"
#pragma warning(disable:4996) 

//Inits Posions and color with default value
Positions::Positions()
{
	Value[0] = 0;
	Value[1] = 0;
	Color = 0;
}
//Inits Positions with given values
Positions::Positions(int _X, int _Y, int _Color)
{
	Value[0] = _X;
	Value[1] = _Y;
	Color = _Color;
}

//Checks if two Positions (IP, Color, and Positions) are equal
bool Positions::operator==(const Positions _Other)
{
	return
		(Address.sin_addr.S_un.S_un_b.s_b1 == _Other.Address.sin_addr.S_un.S_un_b.s_b1 &&
			Address.sin_addr.S_un.S_un_b.s_b2 == _Other.Address.sin_addr.S_un.S_un_b.s_b2 &&
			Address.sin_addr.S_un.S_un_b.s_b3 == _Other.Address.sin_addr.S_un.S_un_b.s_b3 &&
			Address.sin_addr.S_un.S_un_b.s_b4 == _Other.Address.sin_addr.S_un.S_un_b.s_b4 &&
			Address.sin_port == _Other.Address.sin_port) &&
		(Color == _Other.Color) &&
		(Value[0] == _Other.Value[0] && Value[1] == _Other.Value[1]);
}

//Inits sessions with Blank Clients
Session::Session()
{
	for (int i = 0; i < MaxClientAmount; i++)
	{
		Clients[i].sin_addr.S_un.S_addr = inet_addr("0.0.0.0");
		Clients[i].sin_port = htons(0);
	}
}

//Updates sessions with a given function from the Developer
void Session::SessionUpdate(TUP _Function)
{
	_Function();
}

//Adds given client to a session
bool Session::AddClientToSession(SOCKADDR_IN _Address)
{
	//Checks all spaces to see if they are empty
	for (int i = 0; i < MaxClientAmount; i++)
	{
		//If they are empty, place new client in
		if (Clients[i].sin_port == 0)
		{
			Clients[i] = _Address;
			CurrentClientAmount++;
			ClientPositions[i].Address = _Address;
			return true;
		}
	}
	//If they are all not empty, This is a problem and the Developer needs to
	//Catch this and compensate for it
	return false;
}

//Removes the Client from the session
bool Session::RemoveClientFromSession(SOCKADDR_IN _Address)
{
	//Checks all the Clients
	for (int i = 0; i < MaxClientAmount; i++)
	{
		//If current client is the one to remove
		if (Clients[i].sin_addr.S_un.S_un_b.s_b1 == _Address.sin_addr.S_un.S_un_b.s_b1 &&
			Clients[i].sin_addr.S_un.S_un_b.s_b2 == _Address.sin_addr.S_un.S_un_b.s_b2 &&
			Clients[i].sin_addr.S_un.S_un_b.s_b3 == _Address.sin_addr.S_un.S_un_b.s_b3 &&
			Clients[i].sin_addr.S_un.S_un_b.s_b4 == _Address.sin_addr.S_un.S_un_b.s_b4 &&
			Clients[i].sin_port == _Address.sin_port)
		{
			//Remove client
			Clients[i].sin_port = 0;
			CurrentClientAmount--;
			return true;
		}
	}
	//If client was not found, Developer needs to compensate for this
	return false;
}

//Checks the Session for an available space
bool Session::AvailableSpace()
{
	return CurrentClientAmount < MaxClientAmount;
}

//Gets all of the connected clients
bool Session::DisplayConnectedClients(int _SessionID)
{
	for (int i = 0; i < MaxClientAmount; i++)
	{
		if (Clients[i].sin_port != 0)
		{
			printf("Session(%d) Client(%d) IP: %d.%d.%d.%d:%d\n",_SessionID,i,
				Clients[i].sin_addr.S_un.S_un_b.s_b1,
				Clients[i].sin_addr.S_un.S_un_b.s_b2,
				Clients[i].sin_addr.S_un.S_un_b.s_b3,
				Clients[i].sin_addr.S_un.S_un_b.s_b4,
				Clients[i].sin_port);
		}
		if (i == MaxClientAmount - 1)
		{
			return true;
		}
	}
	return false;
}

//Checks for a given client in the session
bool Session::CheckForClient(SOCKADDR_IN _Address)
{
	for (int i = 0; i < CurrentClientAmount; i++)
	{
		if (Clients[i].sin_addr.S_un.S_un_b.s_b1 == _Address.sin_addr.S_un.S_un_b.s_b1 &&
			Clients[i].sin_addr.S_un.S_un_b.s_b2 == _Address.sin_addr.S_un.S_un_b.s_b2 &&
			Clients[i].sin_addr.S_un.S_un_b.s_b3 == _Address.sin_addr.S_un.S_un_b.s_b3 &&
			Clients[i].sin_addr.S_un.S_un_b.s_b4 == _Address.sin_addr.S_un.S_un_b.s_b4 &&
			Clients[i].sin_port == _Address.sin_port)
		{
			return true;
			//Client Already Exists
		}
	}
	return false;
}

//Checks two Socket addresses to see if they match
bool Session::MatchingSockAddress(SOCKADDR_IN _First, SOCKADDR_IN _Second)
{
	return(_First.sin_addr.S_un.S_un_b.s_b1 == _Second.sin_addr.S_un.S_un_b.s_b1 &&
		_First.sin_addr.S_un.S_un_b.s_b2 == _Second.sin_addr.S_un.S_un_b.s_b2 &&
		_First.sin_addr.S_un.S_un_b.s_b3 == _Second.sin_addr.S_un.S_un_b.s_b3 &&
		_First.sin_addr.S_un.S_un_b.s_b4 == _Second.sin_addr.S_un.S_un_b.s_b4 &&
		_First.sin_port == _Second.sin_port);
}

//Retrieves all Client positions in Session
std::list<Positions> Session::RetrieveClientPositions()
{
	std::list<Positions> ClientPos;
	for (int i = 0; i < MaxClientAmount; i++)
	{
		if (Clients[i].sin_port == 0)//If client space is empty
			continue;//Continue searching
		ClientPos.push_back(ClientPositions[i]);
	}
	return ClientPos;
}

//Gets a clientIP from a given ClientIndex
SOCKADDR_IN Session::ClientIP(int _ClientIndex)
{
	return Clients[_ClientIndex];
}

//Get all clients on Session
std::list<SOCKADDR_IN> Session::AvailableClientIP()
{
	//Creates an empty list of Socket Addresses
	std::list<SOCKADDR_IN> TempList;

	//For Every Client spot
	for (int i = 0; i < MaxClientAmount; i++)
	{
		//Get Client Socket Info
		SOCKADDR_IN TempAdd = ClientIP(i);
		if (TempAdd.sin_port == 0)//If Port == 0, Session space is empty
			continue;

		//If they exist, Push them to the lsit
		TempList.push_back(TempAdd);
	}
	return TempList;
}

//Parse a packet and remove the Phrases
std::string Session::ParsePacket(std::string* _Packet)
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
