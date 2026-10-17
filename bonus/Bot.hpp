#ifndef BOT_HPP
# define BOT_HPP

#ifndef DEBUG
# define DEBUG 0
#endif

# include "../includes/IrcServer.hpp"
# include <netdb.h>
# include <fstream>
# include <ctime>

class Bot
{
	public:
		/* ----- Main Constructor & Destructor ----- */
		~Bot();
		explicit Bot( char **argv );

		/* ----- Getters / Setters ----- */
        std::string                 getIp() const;
        int                         getPort() const;
        std::string                 getPassword() const;
        std::string                 getBotname() const;
        int                         getMaxNbWarnings() const;
        std::map<std::string, int>  getUsersWarnings() const;
        std::string                 getLastInviteNickname() const;
        int                         getSocket() const;

        void                        setLastInviteNickname(std::string const& name);
        void                        incrementUserWarnings(std::string const& nickname);

	private:
		/* ----- Attributes ----- */

        std::string                     _ip;
	    int                             _port;
	    std::string                     _password;
	    std::string                     _botName;
	    int		                    	_maxNbWarnings;
	    std::map<std::string, int>		_usersWarnings;
	    std::string                     _lastInviteNickname;
	    int			                    _sock;

		/* ----- Orthodox Canonical Form ----- */
		Bot();
		Bot( const Bot& copy );
		Bot& operator=( const Bot& copy );
};

#endif
