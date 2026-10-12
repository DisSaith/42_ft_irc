CC			= c++
CFLAGS		= -Wall -Wextra -Werror -std=c++98 -g
NAME		= ircserv

SRCS		= 	main.cpp \
				IrcServer.cpp \
				Utils.cpp \

OBJS		= $(SRCS:%.cpp=objs/%.o)

all:		$(NAME)

objs:
		mkdir -p objs

objs/%.o: 	%.cpp  | objs
		$(CC) $(CFLAGS) -c $< -o $@

$(NAME):	$(OBJS)
		$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

debug: CFLAGS += -DDEBUG=1
debug: fclean all

bot: SRCS += Bot.cpp
bot: fclean all

clean:
		rm -rf $(OBJS) ./objs

fclean:		clean
		rm -f $(NAME)

re:		fclean all

.PHONY:		all clean fclean re bonus
