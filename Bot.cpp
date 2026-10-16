#include "IrcServer.hpp"
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
	std::ifstream inFile("dictionnary");

	while (getline (inFile, current_line)) 
  		output.push_back(current_line);
	inFile.close();

	if (output.empty())
		std::cerr << YELLOW << "Warning: " << RESET << " Empty dictionnary" << std::endl;

	return output;
}

bool checkMessage(std::list<std::string> message, std::list<std::string> dictionnary, char* buffer)
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

int main(int argc, char* argv[])
{ 
	if (argc != 6)
	{
		std::cerr << "Usage: " << argv[0] << " <IP> <PORT> <PASSWORD> <BOT NAME> <MAX NB OF WARNINGS>" << std::endl;
		return 1;
	}
	
	time_t now;
	std::string	ip = argv[1];
	int			port = Utils::stoi(argv[2]);
	std::string	password = argv[3];
	std::string	bot_name = argv[4];
	int			maxNbWarnings = Utils::stoi(argv[2]);
	int			sock = socket(AF_INET, SOCK_STREAM, 0);
	std::map<std::string, int>		_usersWarnings;
	
	if (sock == -1)
	{
		std::cerr << "Socket error" << std::endl;
		return 1;
	}
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	inet_pton(AF_INET, ip.c_str(), &server.sin_addr);
	if (inet_pton(AF_INET, ip.c_str(), &server.sin_addr) <= 0)
	{
		std::cerr << "Invalid IP address format: " << ip << std::endl;
		close(sock);
		return 1;
	}
	if (connect(sock, (struct sockaddr *)&server, sizeof(server)) == -1)
	{
		std::cerr << "Error: Unable to connect to " << ip << ":" << port << std::endl;
		close(sock);
		return 1;
	}
	std::cout << "Connected to server. Authenticating..." << std::endl;
	if (!password.empty())
		sendData(sock, "PASS " + password + "\r\n");
	sendData(sock, "NICK " + bot_name + "\r\n");
	sendData(sock, "USER " + bot_name + " 0 * :" + bot_name + " C++\r\n");

	char buffer[4096];
	std::list<std::string> dictionnary = readDictionnary();
	std::ofstream ofs("log", std::ios::trunc);
	if (!ofs.is_open())
		std::cerr << "Warning: Unable to open log file." << std::endl;
	
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

		if (checkMessage(data, dictionnary, buffer))
		{
			std::string BadUserName = data.front();
			BadUserName.erase(0, 1);
			sendData(sock, "PRIVMSG " + BadUserName + " WARNING BAD LANGUAGE" + "\r\n");
			_usersWarnings[BadUserName]++;
			if (_usersWarnings[BadUserName] > maxNbWarnings)
				sendData(sock, "KICK #chan " + BadUserName + " :Bad Language" + "\r\n");
		}
		
		if (std::find(data.begin(), data.end(), "464") != data.end())
		{
			std::cerr << "\n[FATAL ERROR] Incorrect password (464)." << std::endl;
			break ;
		}
		if (std::find(data.begin(), data.end(), "433") != data.end())
		{
			std::cerr << "\n[FATAL ERROR] Bot name '" << bot_name << "' is already in use on this server." << std::endl;
			std::cerr << "Please restart the program with a different name." << std::endl;
			break ;
		}
		if (std::find(data.begin(), data.end(), "001") != data.end())
			std::cout << "Authentication successful! Name validated." << std::endl;
		if (std::find(data.begin(), data.end(), " INVITE ") != data.end())

		
		if (ofs.is_open())
		{
			time(&now);
			std::string timestamp = ctime(&now);
			//ctime add a \n at the end that I remove
			if (timestamp.empty() == false && timestamp[timestamp.length() - 1] == '\n')
				timestamp.erase(timestamp.length() - 1);
			ofs << "[" << timestamp << "]: "<< buffer;
			ofs.flush();
		}
	}
	close(sock);
	return 0;
}
