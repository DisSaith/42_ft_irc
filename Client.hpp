/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nofelten <nofelten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/23 14:19:04 by nofelten          #+#    #+#             */
/*   Updated: 2026/10/03 16:06:41 by nofelten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

#ifndef DEBUG
# define DEBUG 0
#endif

# include <iostream>
# include <string>

class Client
{
	public:
		/* ----- Main Constructor & Destructor ----- */
		~Client();
		Client(int fd);
		
		/* ----- Getters / Setters ----- */
		std::string	getNickname() const;
		std::string	getCurrentChannelName() const;
		int			getFd() const;
		bool		getHasSetPass() const;
		bool		getIsOperator() const;
		bool		getToDisconnect() const;
		bool		setIsRegistered() const;
		void		setPass();
		void		setNickName(std::string const& nickName);
		void		setUserName(std::string const& userName);
		void		setRealName(std::string const& realName);
		void		setNewChannel(std::string const& channelName);
		void		setIsOperator(bool status);
		void		setIsDisconnect(bool status);
		
		/* ----- Methods ----- */
		std::string	extractCommand();
		bool		hasCompleteCommand() const;
		void		appendToIn(std::string const& data);

	private:
		/* ----- Attributes ----- */
		std::string	_ipAddr;
		std::string	_bufferIn;
		std::string	_bufferOut;
		std::string	_nickname;
		std::string	_username;
		std::string	_realname;
		std::string	_currentchannelname;

		int			_fd;
		
		bool		_hasSetPass;
		bool		_hasSetNick;
		bool		_hasSetUser;
		bool		_isOperator;
		bool		_toDisconnect;

		/* ----- Orthodox Canonical Form ----- */
		Client();
		Client(const Client& copy);
		Client& operator=(const Client& copy);
};

#endif
