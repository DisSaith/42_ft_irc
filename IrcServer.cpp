/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acohaut <acohaut@learner.42.tech>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/17 11:03:07 by acohaut           #+#    #+#             */
/*   Updated: 2026/09/30 17:18:36 by nofelten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IrcServer.hpp"
#include "Client.cpp"

/* ======================== Constructors / Destructor ======================== */

IrcServer::~IrcServer() {} //Destructor

//Constructors
IrcServer::IrcServer() : 
	_password(""), _clients(), _recv(), _pollFds(), _server(), _socketServer(0), _port(0) {}

IrcServer::IrcServer( IrcServer const& copy ) : 
	_password(copy._password), _clients(copy._clients), _recv(copy._recv), _pollFds(copy._pollFds), _server(copy._server), _socketServer(copy._socketServer), _port(copy._port) {}

//Main Constructor
IrcServer::IrcServer( char* const& port, char* const& password ) 
	: _password(""), _clients(), _recv(), _server(), _socketServer(0), _port(0)
{
	std::string string_port = std::string(port);

	if ( CheckServerPort(string_port) == true )
	{
		this->_port = ::stoi(string_port);
		this->_password = std::string(password);
	}
	else
	{
		std::cout << RED << "Error: " << RESET
			<< "IRC Server port not valid." << std::endl;
		return ;
	}

}

//Overload operator=
IrcServer& IrcServer::operator=( IrcServer const& copy )
{
	if (this != &copy)
	{
		this->_password = copy._password;
		this->_clients = copy._clients;
		this->_recv = copy._recv;
		this->_pollFds = copy._pollFds;
		this->_server = copy._server;
		this->_socketServer = copy._socketServer;
		this->_port = copy._port;
	}
	return *this;
}


/* ======================== Methods ======================== */

// Ports between 0 and 1023 need root permission
// only ports between 1024 and 65 535 are allowed
bool IrcServer::CheckServerPort( std::string const& port )
{
	int converted_port;

	if ( port.empty() == true )
		throw std::invalid_argument("This IRC Server port is invalid.");
	if (port[0] == '-')
			throw std::invalid_argument("This IRC Server port is out of range.");
	for ( size_t i = 0 ; i < port.length() ; i++ )                            
	{  
		if (!std::isdigit(port[i]))
			throw std::invalid_argument("This IRC Server port is invalid.");
	}
	converted_port = ::stoi(port);
	if ( converted_port >= 0 && converted_port < 1024 )
		throw std::out_of_range( "This IRC Server port needs root permission." );
	else if ( converted_port < 0 || converted_port > 65535 )
		throw std::out_of_range( "This IRC Server port is out of range." );
	else if ( converted_port >= 1024 && converted_port <= 65535 )
		return (true);

	return (false);
}

bool IrcServer::CreateServer()
{
	// Config Server
	_server.sin_addr.s_addr = INADDR_ANY;
	_server.sin_family = AF_INET;
	_server.sin_port = htons(_port);

	// Creation of the initial Socket Server
	_socketServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_socketServer == -1)
		throw std::runtime_error( "Failed during the creation of the server socket." );
	if (bind(_socketServer, (struct sockaddr*)&_server, sizeof(_server)) == -1)
		throw std::runtime_error( "Failed to bind the server socket." );
	if (listen(_socketServer, SOMAXCONN) == -1)
		throw std::runtime_error( "Failed of the listen() function." );
	fcntl(_socketServer, F_SETFL, O_NONBLOCK);
	std::cout << GREEN << "IRC Server created !\n" << RESET
		<< "port: " << this->_port << std::endl
		<< "password: " << this->_password << std::endl;
	return (true);
}

void	IrcServer::ConnectionWithClients()
{
	struct pollfd	serverFd;
	serverFd.fd = _socketServer;
	serverFd.events = POLLIN;
	serverFd.revents = 0;
	_pollFds.push_back(serverFd);

	std::cout << "Serveur en écoute. En attente de connexions..." << std::endl;

	while (1)
	{
		if (poll(&_pollFds[0], _pollFds.size(), -1) == -1)
		{
			std::cerr << "Erreur critique sur poll()" << std::endl;
			break ;
		}
		for (size_t i = 0; i < _pollFds.size(); i++)
		{
			if (_pollFds[i].revents & POLLIN)
			{
				if (_pollFds[i].fd == _socketServer)
				{
					int newClientFd = accept(_socketServer, NULL, NULL);
					if (newClientFd == -1)
					{
						std::cerr << "Erreur critique sur acept()" << std::endl;
						continue ;
					}
					fcntl(newClientFd, F_SETFL, O_NONBLOCK);
					_clients[newClientFd] = new Client(newClientFd);
					struct pollfd	clientFd;
					clientFd.fd = newClientFd;
					clientFd.events = POLLIN;
					clientFd.revents = 0;
					_pollFds.push_back(clientFd);				
				}
				else
				{
					char	buffer[4096];

					int bytesRead = recv(_pollFds[i].fd, buffer, sizeof(buffer) - 1, 0);
					if (DEBUG)
						std::cout << "bytesRead = " << bytesRead << std::endl;
					if (bytesRead <= 0)
					{
						close(_pollFds[i].fd);
						delete(_clients[_pollFds[i].fd]);
						_clients.erase(_pollFds[i].fd);
						_pollFds.erase(_pollFds.begin() + i);
						i--;
					}
					else
					{
						std::string data(buffer, 0, 510);
						_clients[_pollFds[i].fd]->appendToIn(data);
						while (_clients[_pollFds[i].fd]->hasCompleteCommand())
						{
							std::string cmd = _clients[_pollFds[i].fd]->extractCommand();
							if (DEBUG)
								std::cout << "[Client " << _pollFds[i].fd << "] a envoyé : " << cmd;
							TokenizerRecv(cmd);
							ParsingRecv(_pollFds[i].fd);
						}
					}
				}	
			}
		}
	}
}

void IrcServer::TokenizerRecv(std::string const& buffer)
{
	std::string		token;
	size_t			j = 0;
	bool			inWord = false;

	if (_recv.empty() == false)
		_recv.clear();
	for ( size_t i = 0 ; i < buffer.length() ; i++)
	{
		if ( buffer[i] != ' ' && buffer[i] != '\n' && buffer[i] != '\r' && inWord == false )
		{
			inWord = true;
			j = i;
		}
		if ( (buffer[i] == ' ' || buffer[i] == '\n' || buffer[i] == '\r') && inWord == true )
		{
			inWord = false;
			token = buffer.substr(j, i - j);
			_recv.push_back(token);
		}
	}

	/*if (DEBUG) // display list tokens
	{
		int i = 0;
		std::cout << std::endl << "[List Tokens] " << std::endl;
		for ( std::list<std::string>::iterator it = _recv.begin() ; it != _recv.end() ; ++it )
		{
			std::cout << i << ": " << *it << std::endl;
			i++;
		}
	}*/
}

void IrcServer::ParsingRecv( int const& clientFd )
{
	(void)clientFd;
	int i = 0;

	if (_recv.front()[0] == ':')
		_recv.pop_front(); // delate prefix if it exists
	for ( std::list<std::string>::iterator it = _recv.begin() ; it != _recv.end() ; ++it )
	{
		if (DEBUG)
			std::cout << i << ": " << *it << std::endl;
		i++;
	}

}

/* ======================== IRC Commands ======================== */

void	IrcServer::PASS(const int fd)
{
	if (_recv.front() == "PASS")
	{
		if (_recv.size() < 2)
		{
			std::string errorMsg = ":localhost 461 * PASS :Not enough parameters\r\n";
			send(fd, errorMsg.c_str(), errorMsg.length(), 0);
			return ;

		}
		std::list<std::string>::iterator it = _recv.begin();
		it++;
		if (*it != _password)
		{
			std::string errorMsg = ":localhost 464 * :Password incorrect\r\n";
			send(fd, errorMsg.c_str(), errorMsg.length(), 0);
			return ;
		}
		_clients[fd]->setPass();
	}
}

void	IrcServer::NICK(const int fd)
{
	if (_clients[fd]->getHasSetPass())
	{
		if (_recv.front() == "NICK")
		{
			if (_recv.size() < 2)
			{
				std::string errorMsg = ":localhost 431 * :No nickname given\r\n";
				send(fd, errorMsg.c_str(), errorMsg.length(), 0);
				return ;
			}
			std::list<std::string>::iterator it = _recv.begin();
			it++;
			std::map<int, Client*>::iterator mapIt;
			for (mapIt = _clients.begin(); mapIt != _clients.end(); ++mapIt)
			{
				if (*it == mapIt->second->getNickname())
				{
					std::string errorMsg = ":localhost 433 * " + *it + " :Nickname is already in use\r\n";
					send(fd, errorMsg.c_str(), errorMsg.length(), 0);
  					return ;
				}
			}
			_clients[fd]->setNickName(*it);
		}
	}
}
