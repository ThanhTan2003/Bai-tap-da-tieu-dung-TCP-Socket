#include "common.h"

int InitializeWinsock()
{
	WSADATA wsa;
	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code: %d", WSAGetLastError());
		return 0;
	}
	printf("Initialised.");
	return 1;
}

SOCKET CreateBindListen(u_short port)
{
	SOCKET s;
	struct sockaddr_in server;
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket: %d", WSAGetLastError());
		exit(-1);
	}
	printf("Socket created.\n");
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(port);
	if (bind(s, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d", WSAGetLastError());
		exit(-2);
	}
	puts("Bind done");
	listen(s, 30);
	puts("Waiting for incoming connections...");
	return s;
}
