/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acohaut <acohaut@learner.42.tech>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/16 17:05:34 by acohaut           #+#    #+#             */
/*   Updated: 2026/09/24 12:40:35 by acohaut          ###   ########.fr       */
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
# include <cstring> //-> used for c string utilities like memset() or strcmp()
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
		void ConnectionWithClients();

	private:
		//Attributes
		short					_port; // listening port (for initialing communications)
		std::string				_password; // password used for connections server/clients
		int						_socketServer; // initial socket for clients connections
		int						_lastFd; // temporary
		std::map<int, Client*>	_clients; // all clients connected
		sockaddr_in				_server; // struct server
};

#endif
