#include "Utils.hpp"


/* ======================== Static Methods ======================== */

namespace Utils 
{
	int	stoi( std::string const& s )
	{
		int i;

		std::istringstream(s) >> i;
	
		return i;
	}

	long	stol( std::string const& s )
	{
		long	i;

		std::istringstream(s) >> i;
	
		return i;
	}

	std::string		itos( int const& value )
	{
		std::string			s_;
		std::stringstream	ss;
		ss << value;
		ss >> s_;
		return s_;
	}

	bool isMaskChar( char const& c)
	{
		if (c == '#' || c == '&')
			return true;
		return false;
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
			if (token.empty() == false)
				result.push_back(token);
			j = i + 1;
			i = s.find(c, j);
			free(token);
		}
		i = s.find('\r');
		if (i == std::string::npos)
			i = s.find('\n');
		result.push_back(s.substr(j, i - j));
		return result;
	}
}
