--------------------------------------------------------------------
Server Functions or Needed Functions:
--------------------------------------------------------------------


 - Developer made
-------------------------------
(_ConsoleFunction)
{
The Console function takes in a std::string and is able to manipulate the string. This function is designed to allow the developer to create a list of commands that the console can read from. The Developer should read from the 
string that is provided and act on it. 

Examples:
"Help", "ClearSessions", "DeleteObject", "RemoveClient###"

Every Command check is done each from on a seperate Thread as the main. This allows the Developer or Server Owner to run commands without the distruption of the main Thread.
}

-------------------------------
(_SendClientFunction)
{
The Send Client function takes no parameters and returns nothing. This function is designed to allow the developer to send any given inportant information to the clients. The Developer should put any information that needs to be
sent to the users every tick, inside this one function. 

Examples:
Bullet Positions, Client Positions, Client Colors, Client Score

Every send is done each from a seperate Thread as the main. This allows the server to send Clients important information without the distruption of the main Thread.
}
-------------------------------

-------------------------------
(_ProcessPacketFunction)
{
The Process Packet function takes in a std::string and is able to manipulate the string. This function is designed to allow the developer to take in packets and manipulate them in any way that they please. This could be done
after a command is places, or every tick.


Every Process is done on the main thread, and is only send to process when new new packet is recieved. This allows for the server to Process the information not every frame, but instead when it is most nessasary.
}
-------------------------------

- Included!

-------------------------------
(StartServer(char * _Address, char* _Port))
{
This version of Start server allows the developer to choose which IP and port they would like to use. The IP will only correctly start if it is the same as the Device it is being ran on. And the Port needs to be a number
above 1024 as those ports may be reserved.
}
-------------------------------

-------------------------------
()
{
This version of Start server allows the developer to choose which IP and port they would like to use. The IP will only correctly start if it is the same as the Device it is being ran on. And the Port needs to be a number
above 1024 as those ports may be reserved.
}
-------------------------------