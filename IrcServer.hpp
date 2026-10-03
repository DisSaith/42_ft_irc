/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acohaut <acohaut@learner.42.tech>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/16 17:05:34 by acohaut           #+#    #+#             */
/*   Updated: 2026/10/02 12:59:16 by acohaut          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCSERVER_HPP
# define IRCSERVER_HPP

#define RESET   "\033[0m" //-> reset color
#define RED		"\e[1;31m" //-> red color
#define WHITE	"\e[0;37m" //-> white color
#define GREEN	"\e[1;32m" //-> green color
#define YELLOW	"\e[1;33m" //-> yellow color

#ifndef DEBUG
# define DEBUG 0
#endif

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
# include <list> //-> for list container
# include <poll.h> // poll() for multiple clients
# include <vector> //-> for vector container
# include <fcntl.h> //-> fcntl()
# include "Utils.hpp" //-> namespace with utils functions
# include "Client.hpp" //-> Client class
# include "Operator.hpp" //-> Operator class
# include "Channel.hpp" //-> Operator class

using namespace Utils;

class IrcServer 
{
	public:
		/* ----- Main Constructor & Destructor ----- */
		~IrcServer();
		IrcServer( char* const& port, char* const& password );

		/* ----- Methods ----- */
		bool CheckServerPort( std::string const& port );
		bool CreateServer();
		void InitSetCommands();
		void ConnectionWithClients();
		void TokenizerRecv( std::string const& buffer );
		void ParsingRecv(int const& clientFd);
		
		/* ----- IRC Commands ----- */
		void PASS(int const& fd);
		void NICK(int const& fd);
		void JOIN(int const& fd);
		void CHANMSG(int const& fd);

		// typedef for commands map container
		typedef void (IrcServer::*cmdFunction)(int const& fd);

	private:
		/* ----- Attributes ----- */
		// password used for connections server/clients
		std::string							_password;
		// all clients connected
		std::map<int, Client*>				_clients;
		// all channels (accessible by name)
		std::map<std::string, Channel*>				_channels;
		// current client recv
		std::list<std::string>				_recv;
		// all commands of IRC server
		std::map<std::string, cmdFunction>	_commands;
		std::vector<struct pollfd>			_pollFds;
		// struct server
		sockaddr_in							_server;
		// initial socket for clients connections
		int									_socketServer;
		// listening port (for initialing communications)
		short								_port;
		
		/* ----- Orthodox Canonical Form ----- */
		IrcServer(); //Default Constructor
		IrcServer( IrcServer const& copy ); //Copy Constructor
		IrcServer& operator=( IrcServer const& copy ); //Overloard operator=
};

#endif
