#include "Channel.hpp"

Channel::Channel( std::string name, Client *owner ): _name(name), _channelKey(""), _isInviteOnly(false), _hasTopicRestrictions(false), _hasChannelKey(false),  _owner(owner)
{
	_members[owner->getFd()] = owner;
	if (DEBUG)
		std::cout << "Channel parametric constructor called" << std::endl;
}

Channel::Channel( Channel const& copy )
{
    *this = copy;
	if (DEBUG)
		std::cout << "Channel copy constructor called" << std::endl;
}

Channel::~Channel( void )
{}

//Overload operator=
Channel&	Channel::operator=( const Channel& copy )
{
	if (DEBUG)
		std::cout << "Channel copy assignment operator called" << std::endl;
	if (this != &copy)
	{
        _channelKey = copy._channelKey;
        _isInviteOnly = copy._isInviteOnly;
        _hasTopicRestrictions = copy._hasTopicRestrictions;
        _hasChannelKey = copy._hasChannelKey;
		_members = copy._members;
	}
	return *this;
}

void	Channel::sendMessageToMembers(std::string const& message, int const& sender_fd)
{
	std::map<int, Client*>::const_iterator 	it;	
	for (it = _members.begin(); it != _members.end(); ++it)
    {
		if (it->first != sender_fd)
			send(it->first, message.c_str(), message.length(), 0);
	}

}

void	Channel::addNewMember(Client *newMember)
{
	_members[newMember->getFd()] = newMember;
	this->displayMembers();
}

void	Channel::displayMembers( void )
{
	std::map<int, Client*>::const_iterator it;
	std::cout << "Members of channel " << _name << std::endl;
    for (it = _members.begin(); it != _members.end(); ++it)
        std::cout << "	FD: " << it->second << "	Nickname: " << it->second->getNickname() << std::endl;
}

