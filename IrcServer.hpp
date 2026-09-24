/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acohaut <acohaut@learner.42.tech>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/16 17:05:34 by acohaut           #+#    #+#             */
/*   Updated: 2026/09/24 11:21:21 by nofelten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCSERVER_HPP
# define IRCSERVER_HPP

#define RESET   "\033[0m" //-> reset color
#define RED		"\e[1;31m" //-> red color
#define WHITE	"\e[0;37m" //-> white color
#define GREEN	"\e[1;32m" //-> green color
#define YELLOW	"\e[1;33m" //-> yellow color

# include <iostream>
# include <string>
# include <exception> //-> handling exceptions
# include <sys/socket.h> //-> communications by sockets
# include <errno.h> //-> errors management
# include <unistd.h> //-> close()
# include <arpa/inet.h> //-> conversion Home/Server
# include <netinet/in.h> //-> for struct sockaddr_in
# include <cstring>
# include <cstdio>//
# include <map> //-> for map container
# include "Utils.hpp" //-> namespace with utils functions
# include "Client.hpp" //-> Class Client

using namespace Utils;


class IrcServer 
{
	public:
		/* ----- Orthodox Canonical Form ----- */
		~IrcServer(); //Destructor
		IrcServer(); //Default Constructor
		IrcServer( IrcServer const& copy ); //Copy Constructor
		IrcServer& operator=( IrcServer const& copy ); //Overloard operator=

		IrcServer( char* const& port, char* const& password ); // Main Constructor

		//Methods
		bool CheckServerPort( std::string const& port );
		bool CreateServer();
		void test();

	private:
		//Attributes
		short				_port; // listening port (for initialing communications)
		std::string			_password; // password used for connections server/clients
		int				_socketServer;
		int				_lastFd;
		std::map<int, Client*>		_clients;
		sockaddr_in			_server;
};

#endif
