NAME = ircserv
SRCS = srcs/Commandes/JOIN.cpp srcs/Commandes/MODE.cpp srcs/Commandes/utils.cpp srcs/main.cpp srcs/Classes/Channel.cpp srcs/Classes/Client.cpp srcs/Classes/Serveur.cpp
# OBJS = $(SRCS:.cpp=.o)
FLAGS = -std=c++98 -Wall -Werror -Wextra 
all : $(NAME)

$(NAME) : $(SRCS)
	c++ $(SRCS) -o $@


clean :


fclean : clean
	rm -rf $(NAME) 

re : fclean all

