/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acohaut <acohaut@learner.42.tech>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/17 12:02:27 by acohaut           #+#    #+#             */
/*   Updated: 2026/09/18 10:38:51 by acohaut          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

# include <iostream>
# include <string>
# include <sstream>
# include <cctype>
# include <list>

namespace Utils 
{
		int stoi( std::string const& s );
		std::string	itos( int const& value );
		std::list<std::string> split(std::string const& s, char const& c);
		bool isMaskChar( char const& c);
}

#endif
