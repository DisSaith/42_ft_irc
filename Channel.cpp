#include "Channel.hpp"

Channel::Channel( void ): _name("Channel"), fd(42), _channelKey(""), _isInviteOnly(false), _hasTopicRestrictions(false), _hasChannelKey(false)
{
	if (DEBUG)
		std::cout << "Channel default constructor called" << std::endl;
}

Channel::Channel( std::string name, int fd ): _channelKey(""), _isInviteOnly(false), _hasTopicRestrictions(false), _hasChannelKey(false)
{
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
Channel&	Channel::operator=(const Channel& copy)
{
	if (DEBUG)
		std::cout << "Channel copy assignment operator called" << std::endl;
	if (this != &copy)
	{
		_fd = copy._fd;
        _channelKey = copy._channelKey;
        _isInviteOnly = copy._isInviteOnly;
        _hasTopicRestrictions = copy._hasTopicRestrictions;
        _hasChannelKey = copy._hasChannelKey;
	}
	return *this;
}