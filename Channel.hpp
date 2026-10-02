#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#ifndef DEBUG
# define DEBUG 1
#endif

# include "Operator.hpp"
# include <string>

class Channel
{
    private:
    int             _fd;
    std::string     _name;
    std::string     _channelKey;
    bool            _isInviteOnly;
    bool            _hasTopicRestrictions
    bool            _hasChannelKey;


	public:
		/* ----- Orthodox Canonical Form ----- */
		~Channel();
		Channel();
		Channel(int fd);
		Channel(const Channel& copy);
		Channel& Channel=(const Channel& copy);

		/* ----- Methods ----- */


};

#endif
