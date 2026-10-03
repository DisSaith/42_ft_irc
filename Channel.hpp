#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#ifndef DEBUG
# define DEBUG 1
#endif

# include "Client.hpp"
# include <string>
# include <iostream>
# include <map>

class Channel
{
    private:
    	std::string     _name;
    	std::string     _channelKey;
    	bool            _isInviteOnly;
    	bool            _hasTopicRestrictions;
    	bool            _hasChannelKey;
		Client*			_owner;

		// all clients connected, accessible by nickname
		std::map<int, Client*> 			_members;		

	public:
		/* ----- Orthodox Canonical Form ----- */
		~Channel();
		Channel( std::string name, Client *owner );
		Channel(const Channel& copy);
		Channel& operator=(const Channel& copy);

		/* ----- Methods ----- */

		void	sendMessageToMembers(std::string const& message, int const& sender_fd);
		void	addNewMember(Client *newMember);
		void	displayMembers( void );
};

#endif
