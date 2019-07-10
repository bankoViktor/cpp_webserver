#pragma once

#pragma comment(lib, "Ws2_32.lib")	// Winsock Library

#include <WinSock2.h>	// Winsock 2
#include <WS2tcpip.h>	// Winsock 2 TCP/IP

#include "Responce.h"

using namespace std;


#define MAX_PACKET_LEN	1500


class MIMEType
{
public:
	string Type;
	string SubType;

public:
	MIMEType(const char* pszType, const char* pszSubType);
	MIMEType(const char* pszMimeType);
	string GetString();
	static MIMEType GetMEMIType(string sExt);
};


class Server
{
public:
	list<pair<string, string>>* m_pRedirectionList;

public:
	Server(unsigned int port, string* psResourcePath, list<pair<string, string>>* pRedirectionList);
	~Server();
	void Run();
	string Receive(SOCKET socket);
	void Transmit(SOCKET socket, Response* pResponse);
	void Transmit(SOCKET socket, const char* pData, const size_t size);
	void MakeResponse(Response* pResponse);

private:
	unsigned int m_port;
	SOCKET m_socket;
	int m_maxConn = 5;
	string* m_psResourcePath;

private:
	string GetTruePathToResource(string pszResourceName);
	bool SSI_Handle(char** ppData, int* pSize);
	static int AcceptingLoop(Server* pServer);
	static int RequestHandling(Server* pServer, SOCKET clientSocket);
	static void Replace(string* pStr, int offset, char oldChar, char newChar);
	static bool LoadFile(const char* pszFileName, char** ppData, int* pSize, const char* pMode);
};
