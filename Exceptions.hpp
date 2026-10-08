#ifndef EXCEPTIONS_HPP
# define EXCEPTIONS_HPP

# include <exception>

class IrcException : public std::exception
{
	public:
		IrcException( int code, std::string const& target, std::string const& text, int const& fd )
			: _code(code), _target(target), _text(text), _fd(fd) {}

		virtual ~IrcException() throw() {}

		int					getCode() const     { return _code; }
		std::string const&	getTarget() const   { return _target; }
        int                 getFd() const       { return _fd; }
        std::string const&  getText() const     { return _text; }

	private:
		int			_code;
		std::string	_target;
		std::string	_text;
        int         _fd;
};

#endif