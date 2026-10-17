#include "../includes/IrcServer.hpp"
#include <cstdio>
#include <cstring>

int	main(int ac, char **av)
{
	IrcServer server;
	
	IrcServer::GetPtrServer(&server);
	signal( SIGINT, IrcServer::signalHandler );
	signal( SIGQUIT, IrcServer::signalHandler );

	try 
	{
		if (ac == 3)
		{
			server.InitServer( av[1], av[2] );
			server.CreateServer();
			server.ConnectionWithClients();
		}
		else
		{
			std::cout << RED << "Error: " << RESET
				<< "port and password needed !" << std::endl;
			return 1;
		}
	}
	catch( std::exception & e )
	{
		std::cout << RED << "Error: " << RESET
			<< e.what() << std::endl;
	}
	
	server.DeleteChannels();
	server.CloseFds();
	return 0;
}
