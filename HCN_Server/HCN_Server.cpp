#include "common.h"
#include "hinhchunhat.h"

HANDLE hSemaphore;

DWORD WINAPI FindVmax(LPVOID lpParam)
{
	SOCKET clientSocket = *(SOCKET*)lpParam;
	while (true)
	{
		request_t req;
		reply_t rep;
		int bytesReceived = recv(clientSocket, (char*)&req, sizeof(request_t), 0);
		if (bytesReceived == sizeof(request_t))
		{
			printf("Receive: Width = %3.0lf, Length = %3.0lf\n", req.width, req.length);
			double x = timx(req.width, req.length);
			double Vmax = f(x, req.width, req.length);
			rep.x = x;
			rep.Vmax = Vmax;

			printf("Send: x = %3.5lf, Vmax = %3.5lf\n", rep.x, rep.Vmax);
			printf("............................................................\n");

			send(clientSocket, (char*)&rep, sizeof(reply_t), 0);
		}
		else if (bytesReceived == 0)
		{
			printf("Client disconnected!\n");
			break;
		}
		else
		{
			printf("Receive failed with error: %d\n", WSAGetLastError());
			break;
		}
	}
	closesocket(clientSocket);
	return 0;
}

void AcceptClient(SOCKET s)
{
	struct sockaddr_in client;
	int c = sizeof(struct sockaddr_in);

	while (true) {
		SOCKET clientSocket = accept(s, (struct sockaddr*)&client, &c);
		if (clientSocket == INVALID_SOCKET)
		{
			printf("Accept failed with error: %d\n", WSAGetLastError());
			continue;
		}

		char* client_ip = inet_ntoa(client.sin_addr);
		int client_port = ntohs(client.sin_port);
		printf("Connection accepted from IP: %s, Port: %d\n", client_ip, client_port);

		HANDLE hThread = CreateThread(NULL, 0, FindVmax, &clientSocket, 0, NULL);
		if (hThread != NULL)
			CloseHandle(hThread);
	}
}

int main(int argc, char* argv[])
{
	u_short port = 54321;
	if (argc > 1) port = atoi(argv[1]);

	hSemaphore = CreateSemaphore(NULL, 1, 1, NULL);

	InitializeWinsock();
	SOCKET svrsock = CreateBindListen(port);

	AcceptClient(svrsock);

	CloseHandle(hSemaphore);
	closesocket(svrsock);
	WSACleanup();

	return 0;
}
