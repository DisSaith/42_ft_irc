/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acohaut <acohaut@learner.42.tech>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/17 11:53:41 by acohaut           #+#    #+#             */
/*   Updated: 2026/09/18 10:46:59 by acohaut          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Utils.hpp"


/* ======================== Static Methods ======================== */

namespace Utils 
{
	int	stoi( std::string const& s )
	{
		int i;

		std::istringstream(s) >> i;
	
		return (i);
	}

	std::string		itos( int const& value )
	{
		std::string			s_;
		std::stringstream	ss;
		ss << value;
		ss >> s_;
		return ( s_ );
	}

	std::list<std::string> split(std::string const& s, char const& c)
	{
		std::string				token;
		std::list<std::string>	result;
		size_t					i = s.find(c);
		size_t					j = 0;

		while (i != std::string::npos)
		{
			token = s.substr(j, i - j);
			result.push_back(token);
			j = i + 1;
			i = s.find(c, j);
		}
		i = s.find('\r');
		if (i == std::string::npos)
			i = s.find('\n');
		result.push_back(s.substr(j, i - j));
		return (result);
	}
}
