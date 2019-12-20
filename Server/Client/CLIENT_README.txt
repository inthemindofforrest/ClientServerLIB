--------------------------------------------------------------------
Client Functions or Needed Functions:
--------------------------------------------------------------------


 - Developer made
-------------------------------
(_ProcessPacketFunction)
{
The Function for starting a client takes in a std::string and is able to manipulate the string. 
This function is designed to allow the developer to customize what happends with the data processed from a packet. 
The Developer should read from the string that is provided, muniplate (delete parts of string), and act on it. 

Examples:
"PlayerPos", "PlayerScore", "SessionClientCount"...

Every Packet check is done each from on a seperate Thread as the main. This allows the Developer or Client to read
packets without the distruption of the main Thread.
}

-------------------------------

- Included!

-------------------------------
(StartServer(char * _Address, char* _Port))
{
This version of Start server allows the developer to choose which IP and port they would like to use. The IP 
will only correctly start if it is the same as the Device it is being ran on. And the Port needs to be a number
above 1024 as those ports may be reserved.
}
-------------------------------
