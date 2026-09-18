/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acohaut <acohaut@learner.42.tech>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/16 17:05:39 by acohaut           #+#    #+#             */
/*   Updated: 2026/09/18 12:51:57 by acohaut          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IrcServer.hpp"

int	main(int ac, char **av)
{
	try 
	{
		if (ac == 3)
		{
			IrcServer server( av[1], av[2] );
		}
		else
			std::cout << RED << "Error: " << RESET
						<< "port and password needed !" << std::endl;
	}
	catch( std::exception & e )
	{
		std::cout << RED << "Error: " << RESET
					<< e.what() << std::endl;
	}

	return (0);
}
