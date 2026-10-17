#include "../includes/IrcServer.hpp"
#include <netdb.h>
#include <fstream>
#include <ctime>

bool	sendData(int sock, const std::string& msg)
{
	int total_sent = 0;
	int length = msg.length();
	while (total_sent < length)
	{
		int bytes_sent = send(sock, msg.c_str() + total_sent, length - total_sent, MSG_NOSIGNAL);
		if (bytes_sent == -1)
		{
			std::cerr << "Send error." << std::endl;
			return false;
		}
		total_sent += bytes_sent;
	}
	return true;
}

std::list<std::string> readDictionnary()
{
	std::string current_line;
	std::list<std::string> output;
	std::ifstream inFile("./bonus/dictionary");

	if (!inFile.is_open())
	{
		std::cerr << YELLOW << "Warning: " << RESET << "Unable to open dictionary" << std::endl;
		return output;
	}
	while (getline (inFile, current_line)) 
	{
		if (!current_line.empty())
		{
			if (current_line[current_line.length() - 1] == '\r')
				current_line.erase(current_line.length() - 1);
  			output.push_back(current_line);
		}
	}
	inFile.close();

	if (output.empty())
		std::cerr << YELLOW << "Warning: " << RESET << " Empty dictionnary" << std::endl;

	return output;
}

bool checkMessage(std::list<std::string> message, std::list<std::string> dictionnary, char* const& buffer)
{
	bool found = false;

	for (std::list<std::string>::iterator message_it = message.begin(); message_it != message.end(); message_it++)
	{
		if (std::find(dictionnary.begin(), dictionnary.end(), *message_it) != dictionnary.end())
		{
			found = true;
			std::cout << RED << "Bad Language: " << RESET << buffer;
		}
	}
	return found;
}

bool	connectBot(std::string const& password, int const& port, std::string const& ip, std::string const& botName, int const& sock)
{
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	inet_pton(AF_INET, ip.c_str(), &server.sin_addr);
	if (inet_pton(AF_INET, ip.c_str(), &server.sin_addr) <= 0)
	{
		std::cerr << "Invalid IP address format: " << ip << std::endl;
		close(sock);
		return false;
	}
	if (connect(sock, (struct sockaddr *)&server, sizeof(server)) == -1)
	{
		std::cerr << "Error: Unable to connect to " << ip << ":" << port << std::endl;
		close(sock);
		return false;
	}
	std::cout << "Connected to server. Authenticating..." << std::endl;
	if (!password.empty())
		sendData(sock, "PASS " + password + "\r\n");
	sendData(sock, "NICK " + botName + "\r\n");
	sendData(sock, "USER " + botName + " 0 * :" + botName + " C++\r\n");
	return true;
}

int main(int argc, char* argv[])
{ 
	if (argc != 6)
	{
		std::cerr << "Usage: " << argv[0] << " <IP> <PORT> <PASSWORD> <BOT NAME> <MAX NB OF WARNINGS>" << std::endl;
		return 1;
	}

	std::string	ip = argv[1];
	int			port = Utils::stoi(argv[2]);
	std::string	password = argv[3];
	std::string	botName = argv[4];
	int			maxNbWarnings = Utils::stoi(argv[5]);
	int			sock = socket(AF_INET, SOCK_STREAM, 0);
	std::map<std::string, int>		_usersWarnings;
	std::string lastInviteNickname;
	
	if (sock == -1)
	{
		std::cerr << "Socket error" << std::endl;
		return 1;
	}

	connectBot(password, port, ip, botName, sock);

	char buffer[4096];
	std::list<std::string> dictionnary = readDictionnary();
	std::ofstream ofs("log", std::ios::trunc);
	if (!ofs.is_open())
		std::cerr << "Warning: Unable to open log file." << std::endl;
	sendData(sock, "OPER " + botName + "\r\n");
	while (true)
	{
		int bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
		if (bytes_received <= 0)
		{
			std::cerr << "Connection lost with the server." << std::endl;
			break;
		}
		buffer[bytes_received] = '\0';
		std::list<std::string> data = Utils::split(buffer, ' ');
		std::cout << buffer;

		std::list<std::string>::iterator data_it = std::find(data.begin(), data.end(), "PRIVMSG");
		if (data_it != data.end())
		{
			data_it++;
			if (((*(data_it))[0] == '&' || (*(data_it))[0] == '#') && checkMessage(data, dictionnary, buffer))
			{
				std::string BadUserName = data.front();		

				BadUserName.erase(0, 1);
				sendData(sock, "PRIVMSG " + BadUserName + " WARNING BAD LANGUAGE" + "\r\n");
				_usersWarnings[BadUserName]++;
				std::cout << BadUserName << " is at warning number " << _usersWarnings[BadUserName] << "/" << maxNbWarnings << "." << std::endl;
				if (_usersWarnings[BadUserName] > maxNbWarnings)
					sendData(sock, "KICK " + *(data_it) + " " + BadUserName + "Bad Language" + "\r\n");
			}
		}
		
		if (std::find(data.begin(), data.end(), "464") != data.end())
		{
			std::cerr << "\n[FATAL ERROR] Incorrect password (464)." << std::endl;
			break ;
		}
		if (std::find(data.begin(), data.end(), "433") != data.end())
		{
			std::cerr << "\n[FATAL ERROR] Bot name '" << botName << "' is already in use on this server." << std::endl;
			std::cerr << "Please restart the program with a different name." << std::endl;
			break ;
		}
		if (std::find(data.begin(), data.end(), "001") != data.end())
			std::cout << "Authentication successful! Name validated." << std::endl;
		if (std::find(data.begin(), data.end(), "INVITE") != data.end())
		{
			std::vector<std::string> vdata(data.begin(), data.end());
			lastInviteNickname = vdata[0];
			lastInviteNickname.erase(0, 1);
			sendData(sock, "JOIN " + vdata[3] + "\r\n");
		}
		if (std::find(data.begin(), data.end(), ":Cannot join channel") != data.end())
			sendData(sock, "PRIVMSG " + lastInviteNickname + " Cannot join channel" + "\r\n");
		if (ofs.is_open())
		{
			char timeBuf[42];
			time_t now = time(NULL);
			if (ctime_r(&now, timeBuf) != NULL)
			{
				std::string timestamp(timeBuf);
				//ctime add a \n at the end that I remove
				if (timestamp.empty() == false && timestamp[timestamp.length() - 1] == '\n')
					timestamp.erase(timestamp.length() - 1);
				ofs << "[" << timestamp << "]: "<< buffer;
			}
			ofs.flush();
		}
	}
	close(sock);
	return 0;
}
