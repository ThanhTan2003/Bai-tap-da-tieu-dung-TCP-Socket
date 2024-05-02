#include "common.h"
#include <Windows.h>
#include <stdio.h>
#include <Shellapi.h>

volatile double max_x = 0;
volatile double max_Vmax = 0;
volatile double max_width = 0;
volatile double max_length = 0;

#define NUMSEM 1
HANDLE hSem[NUMSEM];


DWORD WINAPI FindMaxVmax(LPVOID lpParam)
{
	HCN_Max* data = (HCN_Max*)lpParam;
	double x = data->rep.x;
	double Vmax = data->rep.Vmax;
	double width = data->req.width;
	double length = data->req.length;

	WaitForSingleObject(hSem[0], INFINITE);

	if (Vmax > max_Vmax)
	{
		max_x = x;
		max_Vmax = Vmax;
		max_width = width;
		max_length = length;
	}

	ReleaseSemaphore(hSem[0], 1, NULL);

	delete data;
	return 0;
}

DWORD WINAPI ClientThread(LPVOID lpParam)
{
	SOCKET s = *((SOCKET*)lpParam);
	FILE* inputFile = NULL;
	FILE* outputFile = NULL;
	char line[100];
	double WIDTH, LENGTH, x, Vmax;
	errno_t err;

	err = fopen_s(&inputFile, "DanhHCN_31012024.txt", "r");
	if (err != 0 || inputFile == NULL)
	{
		printf("Cannot open file DanhHCN_31012024.txt!\n");
		return 1;
	}

	fgets(line, sizeof(line), inputFile);

	err = fopen_s(&outputFile, "ketqua.csv", "w");
	if (err != 0 || outputFile == NULL)
	{
		printf("Unable to create file ketqua.csv\n");
		fclose(inputFile);
		return 1;
	}

	fprintf(outputFile, "width,length,x,Vmax\n");

	while (fgets(line, sizeof(line), inputFile))
	{
		sscanf_s(line, "%lf\t%lf", &WIDTH, &LENGTH);
		request_t req = { WIDTH, LENGTH };
		reply_t rep;

		printf("Width: %3.0lf, Length: %3.0lf\n", req.width, req.length);

		WaitForSingleObject(hSem[0], INFINITE);

		if (send(s, (char*)&req, sizeof(request_t), 0) != sizeof(request_t))
		{
			printf("Cannot send \n");
		}
		else
			printf("Send ... \n");

		if (recv(s, (char*)&rep, sizeof(reply_t), 0) != sizeof(reply_t))
			printf("Cannot receive \n");
		else
			printf("Receive ... \n");

		printf("Result: x = %3.5lf, Vmax = %3.5lf\n", rep.x, rep.Vmax);
		printf("............................................................\n");
		fprintf(outputFile, "%3.0lf,%3.0lf,%3.5lf,%3.5lf\n", req.width, req.length, rep.x, rep.Vmax);

		ReleaseSemaphore(hSem[0], 1, NULL);

		HCN_Max* Data = new HCN_Max;
		Data->req = req;
		Data->rep = rep;

		HANDLE hThread = CreateThread(NULL, 0, FindMaxVmax, Data, 0, NULL);
		if (hThread)
		{
			WaitForSingleObject(hThread, INFINITE);
			CloseHandle(hThread);
		}
	}

	fclose(inputFile);
	fclose(outputFile);

	printf("Recorded results to file ketqua.csv successfully!\n\n");

	printf("Information on the rectangle with the largest maximum volume:\n");
	printf("Width: %3.0lf Length: %3.0lf, x: %3.5fl, Vmax: %3.5lf\n", max_width, max_length, max_x, max_Vmax);
	printf("............................................................\n\n");
	return 0;
}

int main(int argc, char* argv[])
{
	char* IPServer = (char*)"127.0.0.1";
	u_short port = 54321;
	printf("%s IPServer Port\n", argv[0]);
	if (argc > 2)
	{
		port = atoi(argv[2]);
		IPServer = argv[1];
	}
	printf("IPServer: %s. Port %ld\n", IPServer, (int)port);

	hSem[0] = CreateSemaphore(NULL, 1, 1, NULL);

	InitializeWinsock();
	SOCKET s = CreateConnect(IPServer, port);

	HANDLE hClientThread = CreateThread(NULL, 0, ClientThread, &s, 0, NULL);
	if (hClientThread != NULL)
	{
		WaitForSingleObject(hClientThread, INFINITE);
	}

	CloseHandle(hSem[0]);
	closesocket(s);
	WSACleanup();
	ShellExecute(NULL, "open", "ketqua.csv", NULL, NULL, SW_SHOWNORMAL);
	system("pause");
	return 0;
}
