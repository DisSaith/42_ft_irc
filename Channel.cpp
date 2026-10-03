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