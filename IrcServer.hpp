/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acohaut <acohaut@learner.42.tech>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/16 17:05:34 by acohaut           #+#    #+#             */
/*   Updated: 2026/09/17 12:09:06 by acohaut          ###   ########.fr       */
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
# include "Utils.hpp"

class IrcServer 
{
	public:
		//Orthodox Form
		~IrcServer(); //Destructor
		IrcServer(); //Default Constructor
		IrcServer( IrcServer const& copy ); //Copy Constructor
		IrcServer& operator=( IrcServer const& copy ); //Overloard operator=

		IrcServer( int port, int password ); //Main Constructor
		IrcServer( std::string port, std::string password ); //Main Constructor

		//Methods

	private:
		//Attributes
		int _port;
		int	_password;
};

#endif
