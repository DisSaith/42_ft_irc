#include "../includes/Utils.hpp"


/* ======================== Static Methods ======================== */

namespace Utils 
{
	int	stoi( std::string const& s )
	{
		long value = Utils::stol(s);
		if (value > INT_MAX || value < INT_MIN)
			return 0;

		return static_cast<int>(value);
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
		std::list<std::string>	result;
    	size_t end = s.find_last_not_of("\r\n");
		std::string const token = s.substr(0, end + 2);
		size_t					i;
		size_t					j = 0;

		while ((i = token.find(c, j)) != std::string::npos)
    	{
       		if (i > j)
            	result.push_back(token.substr(j, i - j));
        	j = i + 1;
    	}
		i = s.find('\r');
		if (i == std::string::npos)
			i = s.find('\n');
		result.push_back(s.substr(j, i - j));
		return result;
	}
}
