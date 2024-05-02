#include "common.h"

int InitializeWinsock()
{
	WSADATA wsa;
	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		printf("Failed. Error Code: %d", WSAGetLastError());
		return 0;
	}
	printf("Initialised.");
	return 1;
}

SOCKET CreateConnect(char* SVRIPAddress, u_short port)
{
	SOCKET s;
	struct sockaddr_in server;
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		printf("Could not create socket : %d", WSAGetLastError());
		exit(-3);
	}
	printf("Socket created.\n");
	server.sin_addr.s_addr = inet_addr(SVRIPAddress);
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	if (connect(s, (struct sockaddr*)&server, sizeof(server)) < 0) {
		printf("Connect %s error !", SVRIPAddress);
		exit(-4);
	}
	printf("Connected to Server %s", SVRIPAddress);
	return s;
}