NAME = ircserv

SRCS = srcs/Commandes/TOPIC.cpp srcs/Commandes/JOIN.cpp srcs/Commandes/SIGNAUX.cpp srcs/Commandes/NICK.cpp srcs/Commandes/PRIVMSG.cpp srcs/Commandes/MODE.cpp srcs/Commandes/INVITE.cpp srcs/Commandes/utils.cpp srcs/Commandes/KICK.cpp srcs/Commandes/USERandNICK.cpp srcs/main.cpp srcs/Classes/Channel.cpp srcs/Classes/Client.cpp srcs/Classes/Serveur.cpp 

FLAGS = -std=c++98 -Wall -Werror -Wextra -g
all : $(NAME)

$(NAME) : $(SRCS)
	c++ $(FLAGS) $(SRCS) -o $@

clean :

fclean : clean
	rm -rf $(NAME) 

re : fclean all

