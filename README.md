_This project has been created as part of the 42 curriculum by nofelten, ccauderl and acohaut._

# FT\_IRC

_nofelten, ccauderl & acohaut_  
_(piscine of september's 2025 - 42 PARIS)_  


<ins>starting date</ins> : 16/09/2026  
<ins>last update</ins> : 22/10/2026  

[![Quality gate status](https://sonarcloud.io/api/project_badges/measure?project=DisSaith_42_ft_irc&metric=alert_status)](https://sonarcloud.io/summary/new_code?id=DisSaith_42_ft_irc)

## Mandatory
This project is about to create our own IRC server !   
We have to use an actual IRC client to connect to our server and test it.   

## Description  
The Internet is governed by solid standard protocols that allow connected computers to interact with each other. So, it's always beneficial to understand these protocols.   
We based our project mainly on [RFC 1459](https://datatracker.ietf.org/doc/html/rfc1459) that explains all about Internet Relay Chat Protocol.

Skills learning / improvement :
- Understanding TCP/IP communication between client and server.
- Implementing non-blocking I/O operations and multiplexing with poll().
- Writing clean code that complies with the C++ 98 standard.
- Strengthening computational thinking, problem-solving, and collaboration.
- Coding a moderation bot from scratch

## Instructions  
***Usage***  
1) Run "make" to compile the source files using the provided Makefile. You can run "make bonus" to compile the IRC server and its moderation bot.
2) Execute the server by running :
```bash
   ./ircserv <port> <password>.
```
In another terminal, execute the bot by running :
```bash
   ./ircbot <ip> <port> <password> <bot name> <nb_of_warnings>
```
3) Connect to the server using your reference IRC client. You can also test the connection manually using :
```bash
   nc 127.0.0.1 <port>
```
4) Then follow these instructions :
```bash
  PASS <password of the server>
  NICK <your nickname>
  USER <username> <hostname> <servername> <realname>
```
5) Examples of possible commands (channel name must begin by '&' or '#'):
```bash
  JOIN <channel name>
  PRIVMSG <user nickname> <message>
```
6) After joining a channel invite the bot :
```bash
  INVITE <bot name> <channel name>
```
After this the bot will moderate the channel based on the dictionary given in bonus/resources. If a user has too many warnings for bad language, it will be kicked from the channel.

***Check leaks***   
valgrind --leak-check=full --show-leak-kinds=all ./ircserv "port" "password"     


***Tips for adding \\r or \\n in commands***   
- CTRL + V + M = \r
- CTRL + V + J = \n


## Ressources  
<ins>RFC 1459 - IRC Protocol</ins> : (https://datatracker.ietf.org/doc/html/rfc1459)   
<ins>French ft_irc github project</ins> : (https://github.com/raveriss/ft_irc)   
<ins>Creating a TCP Server in C++</ins> : (https://www.youtube.com/watch?v=cNdlrbZSkyQ&list=PLHBVNH27RbWqGTL-AYMylWkNck45cxPnG)   
<ins>C++ Networking</ins> : (https://www.youtube.com/watch?v=gntyAFoZp-E&list=PLHBVNH27RbWqGTL-AYMylWkNck45cxPnG&index=3)    
<ins>Other ressources</ins> : Githubs and IA -> for researches and understanding concepts or examples usages for functions allowed by the subject    

