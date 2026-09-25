/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nofelten <nofelten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/23 14:19:04 by nofelten          #+#    #+#             */
/*   Updated: 2026/09/25 15:09:18 by nofelten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <iostream>
# include <string>

class Client
{
	public:
		Client();
		Client(int fd);
		~Client();
		Client(const Client& copy);

		Client& operator=(const Client& copy);

		int	getFd() const;
		bool	setIsRegistered() const;

		void	setPass();
		void	setNickName(std::string const& nickName);
		void	setUserName(std::string const& userName);
		void	setRealName(std::string const& realName);

		void		appendToIn(std::string const& data);
		bool		hasCompleteCommand() const;
		std::string	extractCommand();

		

	private:
		int		_fd;
		std::string	_ipAddr;

		std::string	_bufferIn;
		std::string	_bufferOut;

		std::string	_nickname;
		std::string	_username;
		std::string	_realname;

		bool		_hasSetPass;
		bool		_hasSetNick;
		bool		_hasSetUser;
		bool		_isOpperator;
		bool		_toDisconnect;
};

#endif
