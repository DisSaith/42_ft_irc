/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acohaut <acohaut@learner.42.tech>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/17 11:03:07 by acohaut           #+#    #+#             */
/*   Updated: 2026/09/28 19:04:31 by nofelten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IrcServer.hpp"
#include "Client.cpp"

/* ======================== Constructors / Destructor ======================== */

IrcServer::~IrcServer() {} //Destructor

//Constructors
IrcServer::IrcServer() : _port(0), _password(""), _socketServer(0), _clients(), _recv(), _server() {}

IrcServer::IrcServer( IrcServer const& copy ) : 
	_port(copy._port), _password(copy._password), _socketServer(copy._socketServer), _clients(copy._clients), _recv(copy._recv), _server(copy._server) {}

	//Main Constructor
IrcServer::IrcServer( char* const& port, char* const& password ) 
	: _port(0), _password(""), _socketServer(0), _clients(), _recv(), _server()
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
		this->_port = copy._port;
		this->_password = copy._password;
		this->_socketServer = copy._socketServer;
		this->_clients = copy._clients;
		this->_server = copy._server;
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
	for ( size_t i = 0 ; i < port.length() ; i++ )                            
	{  
		if (port[0] == '-')
			throw std::invalid_argument("This IRC Server port is out of range.");
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
					char	buffer[1024];

					int bytesRead = recv(_pollFds[i].fd, buffer, sizeof(buffer) - 1, 0);
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
						std::string data(buffer, bytesRead);
						_clients[_pollFds[i].fd]->appendToIn(data);
						while (_clients[_pollFds[i].fd]->hasCompleteCommand())
						{
							std::string cmd = _clients[_pollFds[i].fd]->extractCommand();
							std::cout << "[Client " << _pollFds[i].fd << "] a envoyé : " << cmd << std::endl;
							ParsingRecv(cmd);
						}
					}
				}	
			}
		}
	}
}

void IrcServer::ParsingRecv(std::string buffer)
{
	std::string		parsed;
	size_t			j = 0;
	bool			inWord = false;

	if (_recv.empty() == false)
		_recv.clear();
	for ( size_t i = 0 ; i < buffer.length() ; i++)
	{
		if ( buffer[i] != ' ' && inWord == false )
		{
			inWord = true;
			j = i;
		}
		else if ( (buffer[i] == ' ' || i == buffer.length() - 1) && inWord == true )
		{
			inWord = false;
			parsed = buffer.substr(j, i - j);
			_recv.push_back(parsed);
		}
	}
	// Tests pour afficher les tokens de la list
	/* int i = 0;
	   for ( std::list<std::string>::iterator it = _recv.begin() ; it != _recv.end() ; ++it )
	   {
	   std::cout << i << ": " << *it << std::endl;
	   i++;
	   }*/
}
