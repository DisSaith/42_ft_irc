#ifndef CLIENT_HPP
# define CLIENT_HPP

#ifndef DEBUG
# define DEBUG 0
#endif

# include <iostream>
# include <string>
# include "Channel.hpp"

class Client
{
	public:
		/* ----- Main Constructor & Destructor ----- */
		~Client();
		explicit Client(int fd);
		Client(int fd, std::string nickname, std::string username, std::string realname);

		/* ----- Getters / Setters ----- */
		std::string	getNickname() const;
		std::map<std::string, Channel*>	getChannels() const;
		int			getFd() const;
		bool		getHasSetPass() const;
		bool		getIsServerOperator() const;
		bool		getToDisconnect() const;
		bool		setIsRegistered() const;
		void		setPass();
		void		setNickName(std::string const& nickName);
		void		setUserName(std::string const& userName);
		void		setRealName(std::string const& realName);
		void		setNewChannel(std::string const& channelName, Channel *channel);
		void		removeChannel(std::string const& channelName);
		void		setIsOperator(bool status);
		void		setIsDisconnect(bool status);

		/* ----- Methods ----- */
		std::string	extractCommand();
		bool		hasCompleteCommand() const;
		void		appendToIn(std::string const& data);

	private:
		/* ----- Attributes ----- */
		std::string							_ipAddr;
		std::string							_bufferIn;
		std::string							_bufferOut;
		std::string							_nickname;
		std::string							_username;
		std::string							_realname;
		std::map<std::string, Channel*>		_channels;

		int									_fd;
		
		bool								_hasSetPass;
		bool								_hasSetNick;
		bool								_hasSetUser;
		bool								_isOperator;
		bool								_toDisconnect;

		/* ----- Orthodox Canonical Form ----- */
		Client();
		Client(const Client& copy);
		Client& operator=(const Client& copy);
};

#endif
