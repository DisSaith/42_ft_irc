#ifndef UTILS_HPP
# define UTILS_HPP

# include <iostream>
# include <string>
# include <sstream>
# include <cctype>
# include <list>

namespace Utils 
{
		long stol( std::string const& s );
		int stoi( std::string const& s );
		std::string	itos( int const& value );
		std::list<std::string> split(std::string const& s, char const& c);
		bool isMaskChar( char const& c);
}

#endif
