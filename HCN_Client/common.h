#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#pragma comment(lib,"Ws2_32.lib")

typedef struct _request_t
{
	double width;
	double length;
} request_t, *prequest_t;

typedef struct _reply_t
{
	double x;
	double Vmax;
} reply_t, *preply_t;

typedef struct _HCN_Max
{
	request_t req;
	reply_t rep;
} HCN_Max, *PHCN_Max;

int InitializeWinsock();
SOCKET CreateConnect(char* SVRIPAddress, u_short port);
