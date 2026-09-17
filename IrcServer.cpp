/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acohaut <acohaut@learner.42.tech>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/17 11:03:07 by acohaut           #+#    #+#             */
/*   Updated: 2026/09/17 16:05:18 by acohaut          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IrcServer.hpp"


/* ======================== Constructors / Destructor ======================== */

IrcServer::~IrcServer() {} //Destructor

//Constructors
IrcServer::IrcServer() : _port(0), _password(0) {}

IrcServer::IrcServer( IrcServer const& copy ) : _port(copy._port), _password(copy._password) {}

IrcServer::IrcServer( char* const& port, char* const& password )
{
	std::string string_port = std::string(port);
	
	if ( CheckServerPort(string_port) )
	{
		this->_port = stoi(string_port);
		this->_password = std::string(password);
	}
	else
	{
		std::cout << RED << "Error: " << RESET
					<< "port not valid." << std::endl;
		return ;
	}

	std::cout << GREEN << "IRC Server created !\n" << RESET
				<< "port: " << this->_port << std::endl
				<< "password: " << this->_password << std::endl;
}

//Overload operator=
IrcServer& IrcServer::operator=( IrcServer const& copy )
{
    if (this != &copy)
    {
		this->_port = copy._port;
		this->_password = copy._password;
    }
    return *this;
}


/* ======================== Methods ======================== */

bool IrcServer::CheckServerPort( std::string const& port )
{
	int converted_port = stoi(port);

	if ( converted_port >= 0 && converted_port < 1024 )
		throw std::out_of_range( "This port needs root permission." );
	else if ( converted_port < 0 || converted_port > 65535 )
		throw std::out_of_range( "This port is out of range." );
	else if ( converted_port >= 1024 && converted_port <= 65535 )
		return (true);
		
	return (false);
}
