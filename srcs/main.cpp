#include "../includes/Client.hpp"
#include "../includes/Serveur.hpp"

int	main(int ac, char **av)
{
	struct pollfd server_poll;
	size_t	len;
	ssize_t	bytes;
	char	*buff;

	if (ac != 3)
	{
		std::cout << "the program must be run like this < ./ircserv <port> <password> >\n";
		return (1);
	}
	if (prohibitedCharacterServerPassword((std::string)av[2]))
	{
		std::cout << "The password contains prohibited characters.\n";
		return (1);
	}
	if (!checkNum((std::string)av[1]) || std::atoi(av[1]) < 1024
		|| std::atoi(av[1]) > 65535)
	{
		std::cout << "The port is not correct\n";
		return (1);
	}
	Server server(av);
	if (server.createServer())
		return (1);
	while (1)
	{
		std::cout << "test reset\n";
		if (poll(&server.getFds()[0], server.getFds().size(), -1) > 0)
		{	if (server.getFds()[0].revents & POLLIN)
			{
				std::cout << "\n\ntest1\n\n";
				Client*client = new Client(server.getPort());
                std::cout << "\n\ntest2\n\n";
				client->setCliSocket(accept(server.getFds()[0].fd, NULL, NULL));
                std::cout << "\n\ntest553\n\n";
				if (client->getSocket() > 0)
				{
					std::cout << "\n\ntest3\n\n";
					server_poll.fd = client->getSocket();
					server_poll.events = POLLIN;
					client->setServsocket(server.getFds()[0].fd);
					server.getClients().push_back(client);
					server.getFds().push_back(server_poll);
				}
                else
                {
                    std::cout << "error addind client in vector\n\n";
                }
			}}
            // std::cout << "\n\ntest4\n\n";
		if (server.getClients().size())
		{
			for (size_t i = 1; i < server.getFds().size(); i++)
			{
				//std::cout << "\n\ntest5\n\n";
				Client &tmp = *server.getClients()[i - 1];
				//std::cout << "\n " << i << " client size : " << server.getClients().size() << "\ntest6\n\n";
				if (server.getFds()[i].revents & POLLIN)
				{
					std::cout << "\n\ntest7 fd\n" ;
					std::cout << " " + tmp.getRegister() << std::endl;
					std::cout << "size of vector client " << server.getClients().size() << std::endl; 
					std::cout << server.getClients()[i - 1]->getSocket() << "\n\n";                
					std::cout << "\n\ntest7.5\n\n";
					buff = tmp.getBuffer();
					memset(buff, 0, strlen(buff));
					bytes = recv(server.getFds()[i].fd, buff, 512, 0);
					std::cout << "\n\ntest7.7\n\n";
					if (bytes > 0)
					{				
						std::cout << "\n\ntest8\n\n";
						buff[bytes - 2] = '\0';
						std::cout << "\n\ntest9\n\n";
						if (!tmp.getGivenPassword())
						{
							std::cout << "\n\ntest 10 BUFFER: " << buff << "\n\n";
							std::string str(buff);
							if (!strncmp(buff, "PASS ", 5))
							{
								std::cout << "\n\ntest extract pass: " << extractPass(str) << " PASS DU SERVER : " << server.getPass() << "\n\n";
								if (extractPass(str) == server.getPass())
									tmp.onPass();
								else
								{
									std::cout << "\n\ntest 11 \n\n";
									server.sendMessage("464 * :Password incorrect\r\n", tmp);
									close(tmp.getSocket());
									server.getClients().erase(server.getClients().begin() + (i - 1));
									server.getFds().erase(server.getFds().begin() + i);
									delete &tmp;
									i--;
									continue;
								}
							}
							else 
							{
								server.sendMessage("464 * :Password incorrect\r\n", tmp);
								close(tmp.getSocket());
								server.getClients().erase(server.getClients().begin() + i);
								server.getFds().erase(server.getFds().begin() + (i - 1));
								delete &tmp;
								i--;
								continue;
								std::cout << "Rejet de connexion. FD: <client_fd>. Première commande invalide: " << buff << "\n";
							}
						}
						else if (!tmp.getRegister())
						{
							std::cout << "usernick entree\n";
							if (!strncmp(buff, "USER ", 5) || !strncmp(buff, "NICK ", 5))
							{
								setUserAndNick(tmp, server, buff);
								std::cout << "setusernick fini11\n";
							}
							else
							{
								std::cout << "Not registered client\n";
								server.sendMessage("451 " + (std::string)buff + " :You have not registered\r\n", tmp);
							}
							std::cout << "setusernick fini\n";
						}
						else
						{ // METTRE EN PLACE POINTEUR SUR METHODES
							if (!strncmp(buff, "JOIN ", 5))
								JOIN(tmp, server, buff);
							else if (!strncmp(buff, "KICK ", 5))
							{
							}
							else if (!strncmp(buff, "INVITE ", 7))
							{
							}
							else if (!strncmp(buff, "TOPIC ", 6))
							{
							}
							else if (!strncmp(buff, "MODE ", 5))
							{
							}
							else if (!strncmp(buff, "USER ", 5))
							{
								server.sendMessage("462" + (std::string)buff + ":You may not reregister\r\n", tmp);
							}
							else if (!strncmp(buff, "NICK ", 5))
							{
							}
							else
							{
							}
						}
					}
					else if (bytes == 0)
					{
						std::cout << "\n\ntest9\n\n";
						std::cout << "Client " << tmp.getSocket() << " déconnecté." << std::endl;
						close(tmp.getSocket());
						server.getClients().erase(server.getClients().begin() + (i - 1));
						server.getFds().erase(server.getFds().begin() + i);
						delete &tmp;
						i--;
						// errno(); client deco
					}
					else
					{
						std::cout << "\n\ntest91\n\n";
						// error
					}
				}
				else if (server.getFds()[i].revents & POLLERR)
				{
					std::cout << "\n\ntest99\n\n";
				}
				else if (server.getFds()[i].revents & POLLHUP)
				{
					std::cout << "\n\ntest999\n\n";
				}
			}
		}
	}
	return (0);
}
