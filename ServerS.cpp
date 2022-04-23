#include "stdafx.h"

#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <iostream>

#pragma warning(disable: 4996)

SOCKET Connections[100];
int Counter = 0;

int print_ips_with_gethostbyname(const std::string &host_name)
{
    std::cout
        << "Getting name for \"" << host_name << "\"...\n"
        << "Using gethostbyname() function." << std::endl;

    socket_wrapper::SocketWrapper sock_wrap;
    const hostent *remote_host { gethostbyname(host_name.c_str()) };

    if (nullptr == remote_host)
    {
        if (sock_wrap.get_last_error_code())
        {
            std::cerr << sock_wrap.get_last_error_string() << std::endl;
        }

        return EXIT_FAILURE;
    }

    std::cout << "Official name: " << remote_host->h_name << "\n";

    for (const char* const* p_alias = const_cast<const char* const*>(remote_host->h_aliases); *p_alias; ++p_alias)
    {
        std::cout << "# Alternate name: \"" <<  *p_alias << "\"\n";
    }

    std::cout << "Address type: ";
    if (AF_INET == remote_host->h_addrtype)
    {
        std::cout << "AF_INET\n";
        std::cout << "\nAddress length: " << remote_host->h_length << "\n";

        in_addr addr = {0};

        for (int i = 0; remote_host->h_addr_list[i]; ++i)
        {
            addr.s_addr = *reinterpret_cast<const u_long* const>(remote_host->h_addr_list[i]);
            std::cout << "IP Address: " << inet_ntoa(addr) << "\n";
        }
    }
    // Событие ниже никогда не случится. gethostbyname() считается deprecated функцией.
    // Он не работает с IPv6.
    else if (AF_INET6 == remote_host->h_addrtype)
    {
        std::cout << "AF_INET6\n";
    }
    else
    {
        std::cout << remote_host->h_addrtype << "\n";
    }

    std::cout << std::endl;

    return EXIT_SUCCESS;
}


void ClientHandler(int index) {
	int msg_size;
	while (true) {
		recv(Connections[index], (char*)&msg_size, sizeof(int), NULL);
		char* msg = new char[msg_size + 1];
		msg[msg_size] = '\0';
		recv(Connections[index], msg, msg_size, NULL);
		for (int i = 0; i < Counter; i++) {
			if (i == index) {
				continue;
			}

			send(Connections[i], (char*)&msg_size, sizeof(int), NULL);
			send(Connections[i], msg, msg_size, NULL);
		}
		delete[] msg;
	}
}

int main(int argc, char* argv[]) {
	//WSAStartup
	WSAData wsaData;
	WORD DLLVersion = MAKEWORD(2, 1);
	if (WSAStartup(DLLVersion, &wsaData) != 0) {
		std::cout << "Error" << std::endl;
		exit(1);
	}

	SOCKADDR_IN addr;
	int sizeofaddr = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(1111);
	addr.sin_family = AF_INET;

	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);
	bind(sListen, (SOCKADDR*)&addr, sizeof(addr));
	listen(sListen, SOMAXCONN);

	SOCKET newConnection;
	
	 if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " <hostname>" << std::endl;
        return EXIT_FAILURE;
    }

    const std::string host_name = { argv[1] };
	
	for (int i = 0; i < 100; i++) {
		newConnection = accept(sListen, (SOCKADDR*)&addr, &sizeofaddr);

		if (newConnection == 0) {
			std::cout << "Error #2\n";
		}
		else {
			std::cout << "Client Connected!\n";
			std::string msg = "Hello";
			int msg_size = msg.size();
			send(newConnection, (char*)&msg_size, sizeof(int), NULL);
			send(newConnection, msg.c_str(), msg_size, NULL);

			Connections[i] = newConnection;
			Counter++;
			print_ips_with_getaddrinfo(host_name);
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, (LPVOID)(i), NULL, NULL);
		}
	}


	system("pause");
	return 0;
}