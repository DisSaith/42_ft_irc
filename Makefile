CC			= c++
CFLAGS		= -Wall -Wextra -Werror -std=c++98 -g
NAME		= ircserv
NAME_BOT	= ircbot
SRCS		= 	srcs/main.cpp \
				srcs/IrcServer.cpp \
				srcs/Utils.cpp \

SRC_BOT	= 	bonus/Bot_main.cpp srcs/Utils.cpp

OBJS		= $(SRCS:srcs/%.cpp=objs/%.o)

OBJ_BOT		= $(SRC_BOT:srcs/%.cpp=objs/%.o)

all:		$(NAME)

objs:
		mkdir -p objs

objs/%.o: 	srcs/%.cpp  | objs
		$(CC) $(CFLAGS) -c $< -o $@

$(NAME):	$(OBJS)
		$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

$(NAME_BOT):	$(OBJ_BOT)
		$(CC) $(CFLAGS) $(OBJ_BOT) -o $(NAME_BOT)

debug: CFLAGS += -DDEBUG=1
debug: fclean all $(NAME_BOT)

bonus:	all $(NAME_BOT)

clean:
		rm -rf $(OBJS) ./objs

fclean:		clean
		rm -f $(NAME) $(NAME_BOT) log

re:		fclean all

.PHONY:		all clean fclean re bonus
