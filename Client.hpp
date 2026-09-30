/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nofelten <nofelten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/23 14:19:04 by nofelten          #+#    #+#             */
/*   Updated: 2026/09/29 16:45:01 by acohaut          ###   ########.fr       */
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
		/* ----- Orthodox Canonical Form ----- */
		~Client();
		Client();
		Client(int fd);
		Client(const Client& copy);
		Client& operator=(const Client& copy);

		/* ----- Getters / Setters ----- */
		int		getFd() const;
		bool	setIsRegistered() const;
		void	setPass();
		void	setNickName(std::string const& nickName);
		void	setUserName(std::string const& userName);
		void	setRealName(std::string const& realName);

		/* ----- Methods ----- */
		void		appendToIn(std::string const& data);
		bool		hasCompleteCommand() const;
		std::string	extractCommand();

	private:
		/* ----- Attributes ----- */
		std::string	_ipAddr;
		std::string	_bufferIn;
		std::string	_bufferOut;
		std::string	_nickname;
		std::string	_username;
		std::string	_realname;

		int			_fd;
		
		bool		_hasSetPass;
		bool		_hasSetNick;
		bool		_hasSetUser;
		bool		_isOpperator;
		bool		_toDisconnect;
};

#endif
