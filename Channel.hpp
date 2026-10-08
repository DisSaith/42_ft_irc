#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#ifndef DEBUG
# define DEBUG 0
#endif

# include <string>
# include <iostream>
# include <map>

class Client;

class Channel
{
    private:
    	std::string     _name;
    	std::string     _channelKey;
    	bool            _isInviteOnly;
    	bool            _hasTopicRestrictions;
    	bool            _hasChannelKey;

		// all clients connected, accessible by nickname
		std::map<int, Client*> 			_members;	
		// all channel operators
		std::map<int, Client*> 			_operators;
		
		Channel();

	public:
		/* ----- Orthodox Canonical Form ----- */
		
		~Channel();
		Channel( std::string name, Client *owner );
		Channel(const Channel& copy);
		Channel& operator=(const Channel& copy);

		/* ----- Accessors ----- */

		bool					hasMember(int fd) const;
		std::map<int, Client*>	getMembers() const;
		bool					getIsChannelOperator( int const& fd );
		bool					getHasChannelKey() const;
		std::string				getChannelKey() const;

		/* ----- Setters ----- */

		void					setHasChannelKey(bool status);
		void					setChannelKey(std::string key);


		/* ----- Methods ----- */

		void					sendMessageToMembers(std::string const& message, int const& sender_fd);
		void					addNewMember(Client *newMember);
		bool					removeMember( int const& fd );
		void					addNewOperator(Client *newMember);
		void					removeOperator( int const& fd );
		void					displayOperators( void );
		void					displayMembers( void );
		void					displayMembers( int const& fd );

};

#endif
