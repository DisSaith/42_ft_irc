/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acohaut <acohaut@learner.42.tech>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/16 17:05:34 by acohaut           #+#    #+#             */
/*   Updated: 2026/10/08 11:06:24 by nofelten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCSERVER_HPP
# define IRCSERVER_HPP

# define RESET   "\033[0m" //-> reset color
# define RED		"\e[1;31m" //-> red color
# define WHITE   "\033[1m\033[37m" //-> white and bold color
# define BLUE    "\033[34m" //-> blue color
# define GREEN	"\e[1;32m" //-> green color
# define YELLOW	"\e[1;33m" //-> yellow color

# ifndef DEBUG
#  define DEBUG 0
# endif

# include <iostream>
# include <string>
# include <exception> //-> handling exceptions
# include <sys/socket.h> //-> communications by sockets
# include <errno.h> //-> errors management
# include <unistd.h> //-> close()
# include <arpa/inet.h> //-> conversion Home/Server
# include <netinet/in.h> //-> for struct sockaddr_in
# include <poll.h> // poll() for multiple clients
# include <fcntl.h> //-> fcntl()
# include <climits> //-> used for int and uint limits
# include <cstring> //-> used for c string utilities like memset() or strcmp()
# include <csignal> //-> used for signal handling
# include <map> //-> for map container
# include <list> //-> for list container
# include <vector> //-> for vector container
# include <set> //-> for set container
# include "Utils.hpp" //-> namespace with utils functions
# include "Client.hpp" //-> Client class
# include "Operator.hpp" //-> Operator class
# include "Channel.hpp" //-> Operator class
# include "Exceptions.hpp"

using namespace Utils;

class IrcServer 
{
	public:
		/* ----- Default Constructor & Destructor ----- */
		~IrcServer();
		IrcServer();

		/* ----- IRC Server ----- */
		void InitServer( char* const& port, char* const& password );
		void CreateServer();
		void ConnectionWithClients();
		
		/* ----- Handling Clients Messages ----- */
		void TokenizerRecv( std::string const& buffer );
		void ParsingRecv(int const& clientFd);
		
		/* ----- IRC Commands ----- */
		void PASS(int const& fd);
		void NICK(int const& fd);
		void JOIN(int const& fd);
		void USER(int const& fd);
		void PRIVMSG(int const& fd);
		void PART(int const& fd);
		void NAMES(int const& fd);
		void QUIT(int const& fd);
		void KICK(int const& fd);
		void MODE(int const& fd);

		/* ----- Signals ----- */
		static void signalINT( int signal );

		/* ----- Utils ----- */
		static IrcServer*	GetPtrServer( IrcServer *server );
		bool				isAnyOperator(int const& fd, std::string const& channelName);
		bool				CheckServerPort( std::string const& port );
		void				InitMapCommands();
		void				CloseFds();
		void				RemoveClient(int fd);
		std::string			buildMessage( int code, std::string target, std::string text, int fd );

		// typedef for commands map container
		typedef void (IrcServer::*cmdFunction)(int const& fd);

	private:
		/* ----- Attributes ----- */
		// password used for connections server/clients
		std::string							_password;
		// all clients connected
		std::map<int, Client*>				_clients;
		// all channels (accessible by name)
		std::map<std::string, Channel*>		_channels;
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
		IrcServer( IrcServer const& copy ); //Copy Constructor
		IrcServer& operator=( IrcServer const& copy ); //Overloard operator=
};

#endif
