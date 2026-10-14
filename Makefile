CC			= c++
CFLAGS		= -Wall -Wextra -Werror -std=c++98 -g
NAME		= ircserv
NAME_BOT	= ircbot
SRCS		= 	main.cpp \
				IrcServer.cpp \
				Utils.cpp \

SRC_BOT	= 	Bot.cpp Utils.cpp

OBJS		= $(SRCS:%.cpp=objs/%.o)

OBJ_BOT		= $(SRC_BOT:%.cpp=objs/%.o)

all:		$(NAME)

objs:
		mkdir -p objs

objs/%.o: 	%.cpp  | objs
		$(CC) $(CFLAGS) -c $< -o $@

$(NAME):	$(OBJS)
		$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

$(NAME_BOT):	$(OBJ_BOT)
		$(CC) $(CFLAGS) $(OBJ_BOT) -o $(NAME_BOT)

debug: CFLAGS += -DDEBUG=1
debug: fclean all $(NAME_BOT)

bonus:	fclean all $(NAME_BOT)

clean:
		rm -rf $(OBJS) ./objs

fclean:		clean
		rm -f $(NAME) $(NAME_BOT)

re:		fclean all

.PHONY:		all clean fclean re bonus
