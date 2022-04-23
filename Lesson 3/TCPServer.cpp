#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable: 4996)
#define BACKLOG 10
#define PORT "1111"


int Connections[100];
int Counter = 0;

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
	WSAData wsaData;
	WORD DLLVersion = MAKEWORD(2, 1);
	if (WSAStartup(DLLVersion, &wsaData) != 0) {
		std::cout << "Error" << std::endl;
		exit(1);
	}

	int status, sListen, newConnection;
	struct sockaddr_storage their_addr;
	socklen_t addr_size;
	struct addrinfo hints;
	struct addrinfo* servinfo;  
	memset(&hints, 0, sizeof hints); 
	hints.ai_family = AF_UNSPEC;     
	hints.ai_socktype = SOCK_STREAM; 
	hints.ai_flags = AI_PASSIVE;     
	if ((status = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
	fprintf(stderr, "getaddrinfo error : %sn", gai_strerror(status));
	exit(1);
}
	freeaddrinfo(servinfo);

	sListen = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
	bind(sListen, servinfo->ai_addr, servinfo->ai_addrlen);
	listen(sListen, BACKLOG);

	addr_size = sizeof their_addr;
	for (int i = 0; i < 100; i++) {
		newConnection = accept(sListen, (struct sockaddr*)&their_addr, &addr_size);

		if (newConnection == 0) {
			std::cout << "Error #2\n";
		}
		else {
			std::cout << "Client Connected!\n";
			std::string msg = "Hello. It`s my first network program!";
			int msg_size = msg.size();
			send(newConnection, (char*)&msg_size, sizeof(int), NULL);
			send(newConnection, msg.c_str(), msg_size, NULL);

			Connections[i] = newConnection;
			Counter++;
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, (LPVOID)(i), NULL, NULL);
		}
	}

	shutdown(sListen, 0);
	system("pause");
	return 0;
}