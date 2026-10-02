/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acohaut <acohaut@learner.42.tech>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/16 17:05:39 by acohaut           #+#    #+#             */
/*   Updated: 2026/10/02 17:27:12 by acohaut          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IrcServer.hpp"
#include <cstdio>
#include <cstring>

int	main(int ac, char **av)
{
	IrcServer server;
	
	try 
	{
		if (ac == 3)
		{
			server.InitServer( av[1], av[2] );
			server.CreateServer();
			server.ConnectionWithClients();
		}
		else
		{
			std::cout << RED << "Error: " << RESET
				<< "port and password needed !" << std::endl;
			return 1;
		}
	}
	catch( std::exception & e )
	{
		std::cout << RED << "Error: " << RESET
			<< e.what() << std::endl;
	}
	
	server.CloseFds();
	return 0;
}
