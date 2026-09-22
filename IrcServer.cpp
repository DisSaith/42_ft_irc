/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acohaut <acohaut@learner.42.tech>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/17 11:03:07 by acohaut           #+#    #+#             */
/*   Updated: 2026/09/22 17:51:33 by acohaut          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IrcServer.hpp"


/* ======================== Constructors / Destructor ======================== */

IrcServer::~IrcServer() {} //Destructor

//Constructors
IrcServer::IrcServer() : _port(0), _password(""), _socket(0), _server() {}

IrcServer::IrcServer( IrcServer const& copy ) : 
	_port(copy._port), _password(copy._password), _socket(copy._socket), _server(copy._server) {}

//Main Constructor
IrcServer::IrcServer( char* const& port, char* const& password ) 
	: _port(0), _password(""), _socket(0), _server()
{
	std::string string_port = std::string(port);
	
	if ( CheckServerPort(string_port) == true )
	{
		this->_port = stoi(string_port);
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
		this->_socket = copy._socket;
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
	  if (!std::isdigit(port[i]) && port[i] != '-')
			throw std::invalid_argument("This IRC Server port is invalid.");
	}

	converted_port = stoi(port);

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

	// Creation of the initial Socket 
	_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_socket == -1)
		throw std::runtime_error( "Failed during the creation of the server socket." );
	if (bind(_socket, (struct sockaddr*)&_server, sizeof(_server)) == -1)
		throw std::runtime_error( "Failed to bind the server socket." );
	if (listen(_socket, SOMAXCONN) == -1)
		throw std::runtime_error( "Failed of the listen() function." );

	std::cout << GREEN << "IRC Server created !\n" << RESET
			<< "port: " << this->_port << std::endl
			<< "password: " << this->_password << std::endl;
	return (true);
}
