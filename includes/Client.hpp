#ifndef CLIENT_HPP
# define CLIENT_HPP

#ifndef DEBUG
# define DEBUG 0
#endif

# include <iostream>
# include <string>
# include <algorithm>
# include "Channel.hpp"

class Client
{
	public:
		/* ----- Main Constructor & Destructor ----- */
		~Client();
		explicit Client( int fd );
		Client( int fd, std::string const& nickname, std::string const& username, std::string const& realname );

		/* ----- Getters / Setters ----- */
		std::string	getNickname() const;
		std::map<std::string, Channel*>	getChannels() const;
		int			getFd() const;
		bool		getHasSetPass() const;
		bool		getIsServerOperator() const;
		bool		getToDisconnect() const;
		bool		getIsInvited( std::string const& channelName ) const;
		bool		getIsRegistered() const;
		std::string	getAllNames() const;
		void		setPass();
		void		setNickName( std::string const& nickName );
		void		setUserName( std::string const& userName );
		void		setRealName( std::string const& realName );
		void		setHostName( std::string const& hostName );
		void		setNewChannel( std::string const& channelName, Channel *channel );
		void		removeChannel( std::string const& channelName );
		void		setIsOperator( bool status );
		void		setIsDisconnect( bool status );

		/* ----- Methods ----- */
		std::string	extractCommand();
		size_t		FindEndFirstCommand();
		bool		hasCompleteCommand() const;
		void		appendToIn( std::string const& data );
		void		addInvite( std::string const& channelName );
		void		removeInvite( std::string const& channelName );
		void		displayClient( void );

	private:
		/* ----- Attributes ----- */
		std::string							_ipAddr;
		std::string							_bufferIn;
		std::string							_bufferOut;
		std::string							_nickname;
		std::string							_username;
		std::string							_realname;
		std::string							_hostname;
		std::map<std::string, Channel*>		_channels;
		std::vector<std::string>			_invitedChannels;

		int									_fd;
		
		bool								_hasSetPass;
		bool								_hasSetNick;
		bool								_hasSetUser;
		bool								_isOperator;
		bool								_toDisconnect;

		/* ----- Orthodox Canonical Form ----- */
		Client();
		Client( const Client& copy );
		Client& operator=( const Client& copy );
};

#endif
