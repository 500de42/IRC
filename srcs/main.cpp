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
		if (poll(&server.getFds()[0], server.getFds().size(), -1) > 0)
			if (server.getFds()[0].revents & POLLIN)
			{std::cout << "\n\ntest1\n\n";
				Client*client = new Client(server.getPort());
                std::cout << "\n\ntest2\n\n";
				client->setCliSocket(accept(server.getFds()[0].fd, NULL, NULL));
                std::cout << "\n\ntest553\n\n";
				if (client->getSocket() > 0)
				{std::cout << "\n\ntest3\n\n";
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
			}
            std::cout << "\n\ntest4\n\n";
		for (size_t i = 1; i < server.getFds().size(); i++)
		{std::cout << "\n\ntest5\n\n";
			len = sizeof(server.getClients()[i]->getBuffer());
            std::cout << "\n\ntest6\n\n";
			if (server.getFds()[i].revents & POLLIN)
			{
                std::cout << "\n\ntest7 fd" << server.getClients()[i]->getSocket() << "\n\n" ;
				bytes = recv(server.getFds()[i].fd, server.getClients()[i]->getBuffer(), len, 0);
                std::cout << "\n\ntest7.5\n\n";
				if (bytes > 0)
				{std::cout << "\n\ntest8\n\n";
					buff = server.getClients()[i]->getBuffer();
					Client &tmp = *server.getClients()[i];
                    std::cout << "\n\ntest9\n\n";
                    if (!tmp.getGivenPassword())
                    {
                        std::string str(buff);
						if (!str.rfind("PASS", 0))
						{std::cout << "\n\ntest\n\n";
							if (extractPass(str) == server.getPass())
								tmp.onPass();
							else
							{
								server.sendMessage("464 * :Password incorrect\r\n", tmp);
								close(tmp.getSocket());
								server.getClients().erase(server.getClients().begin()
									+ i);
								server.getFds().erase(server.getFds().begin()
									+ i);
							}
						}
                    }
					else if (!tmp.getRegister())
						setUserAndNick(tmp, server);
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
							server.sendMessage("462" + (std::string)buff
								+ ":You may not reregister\r\n", tmp);
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
					// errno(); client deco
				}
				else
				{
					// error
				}
			}
			else if (server.getFds()[i].revents & POLLERR)
			{
			}
			else if (server.getFds()[i].revents & POLLHUP)
			{
			}
		}
	}
	return (0);
}
