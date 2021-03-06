/*

  All code has been written by Karsten Vermeulen and may be used freely by anyone. It is by no
  means perfect and there is certainly room for improvement in some parts. As a whole, the code
  has been created as part of an engine for the purposes of educating other fellow programmers,
  and anyone else wishing to learn C++ and OOP. Feel free to use, copy, break, update and do as
  you wish with this code - it is there for all!

  UPDATED : January 2016

  -----------------------------------------------------------------------------------------------

-
-
-

*/

#ifndef CLIENT_H
#define CLIENT_H

#include <SDL_net.h>
#include <string>

class Client
{

public:

	Client();

public:

	bool Initialize();
	bool Connect(const std::string& serverName, int serverPort);

private:

	int m_serverPort;
	std::string m_serverName;

	IPaddress m_IP;
	TCPsocket m_socket;

};

#endif