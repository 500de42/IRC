NAME = ircserv
SRCS = srcs/Commandes/JOIN.cpp srcs/Commandes/MODE.cpp srcs/Commandes/utils.cpp srcs/main.cpp srcs/Classes/Channel.cpp srcs/Classes/Client.cpp srcs/Classes/Serveur.cpp
OBJS = $(SRCS:.cpp=.o)
FLAGS = -std=c++98 -Wall -Werror -Wextra 
all : $(NAME)

$(NAME) : $(OBJS)
	c++ $(OBJS) -o $@

%.o : %.cpp
	c++ $(FLAGS) -c $< -o $@  # ⬅️ Compile chaque .cpp en .o

clean :
	rm -rf *.o

fclean : clean
	rm -rf $(NAME)

re : fclean all

