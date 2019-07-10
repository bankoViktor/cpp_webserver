#include <stdio.h>
#include <istream>
#include <thread>
#include <sstream>	// stringstream
#include <list>
#include <string>
#include <fstream>	// fsteam work with file

#include "Server.h"
#include "Request.h"


MIMEType::MIMEType(const char* pszType, const char* pszSubType)
{
	Type = string(pszType);
	SubType = string(pszSubType);
}

MIMEType::MIMEType(const char* pszMimeType)
{
	string mimeType(pszMimeType);

	size_t idx_type_sep = mimeType.find('/');

	if (idx_type_sep != -1)
	{
		Type = mimeType.substr(0, idx_type_sep);
		SubType = mimeType.substr((size_t)idx_type_sep + 1, (size_t)mimeType.size() - idx_type_sep - 1);
	}
}

string MIMEType::GetString()
{
	stringstream ss;
	ss << Type << "/" << SubType;
	return ss.str();
}

MIMEType MIMEType::GetMEMIType(string sExt)
{
	const char* pszApplication = "application";
	const char* pszText = "text";
	const char* pszImage = "image";
	const char* pszUnknown = "unknown";

	// application
	if (sExt == "json")
		return MIMEType(pszApplication, "json");
	else if (sExt == "js")
		return MIMEType(pszApplication, "javascript");
	else if (sExt == "pdf")
		return MIMEType(pszApplication, "pdf");

	// text
	else if (sExt == "html" || sExt == "htm")
		return MIMEType(pszText, "html");
	else if (sExt == "css")
		return MIMEType(pszText, "css");
	else if (sExt == "xml")
		return MIMEType(pszText, "xml");
	else if (sExt == "txt")
		return MIMEType(pszText, "plain");

	// image
	else if (sExt == "ico")
		return MIMEType(pszImage, "x-icon");
	else if (sExt == "png")
		return MIMEType(pszImage, "png");
	else if (sExt == "jpg" || sExt == "jpeg")
		return MIMEType(pszImage, "jpeg");
	else if (sExt == "svg")
		return MIMEType(pszImage, "svg+xml");
	else
		return MIMEType(pszApplication, pszUnknown);
}




Server::Server(unsigned int port, string* psResourcePath, list<pair<string, string>>* pRedirectionList)
{
	m_port = port;
	m_psResourcePath = psResourcePath;
	m_pRedirectionList = pRedirectionList;

	int iResult;
	WSADATA wsaData;
	
	// Инициализация WSA
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		//printf("WSAStartup failed: %d\n", iResult);
		throw exception("WSAStartup failed", iResult);
	}

	// Определяем значения в структуре addrinfo (pResult)
	ADDRINFO hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = PF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	ADDRINFO* pResult;
	string sPort = to_string(m_port);

	iResult = GetAddrInfo(NULL, sPort.data(), &hints, &pResult);
	if (iResult != 0)
	{
		//printf("Getaddrinfo failed: %d\n", iResult);
		WSACleanup();
		throw exception("Getaddrinfo failed", iResult);
	}

	// Создаем socket
	m_socket = socket(pResult->ai_family, pResult->ai_socktype, pResult->ai_protocol);
	if (m_socket == INVALID_SOCKET)
	{
		freeaddrinfo(pResult);
		WSACleanup();
		throw exception("Creating socket failed", iResult);
	}

	// Связываем socket с ip-адрессом и портом
	iResult = bind(m_socket, pResult->ai_addr, (int)pResult->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		int errn = WSAGetLastError();
		freeaddrinfo(pResult);
		closesocket(m_socket);
		WSACleanup();
		throw exception("Binding socket failed", errn);
	}

	// После связывания информация о адресе не нужна, освободжаем ее
	FreeAddrInfo(pResult);

	// Включаем прослушивание socket
	iResult = listen(m_socket, m_maxConn);
	if (iResult == SOCKET_ERROR)
	{
		closesocket(m_socket);
		WSACleanup();
		throw exception("Listen socket failed", iResult);
	}

	const char* pszFormatStr = "%-25s %s\n";
	const char* pszFormatInt = "%-25s %d\n";
	printf(pszFormatStr, "Server use", wsaData.szDescription);
	printf(pszFormatInt, "Port", m_port);
	printf(pszFormatStr, "ResourcePath", m_psResourcePath->c_str());
	printf(pszFormatInt, "Redirection List Size", pRedirectionList->size());
	printf("\n");
}

Server::~Server()
{
	WSACleanup();
}

void Server::Run()
{
	thread loop(AcceptingLoop, this);
	SetThreadDescription(loop.native_handle(), L"Accepting Loop");
	loop.join();
}

int Server::AcceptingLoop(Server * pServer)
{
	printf("Server Running\n\n");

	SOCKET ClientSocket = INVALID_SOCKET;

	while (1)
	{
		ClientSocket = accept(pServer->m_socket, NULL, NULL);
		if (ClientSocket == INVALID_SOCKET) {
			closesocket(pServer->m_socket);
			WSACleanup();
			throw exception("Accept failed", WSAGetLastError());
		}
		
		thread Haldle(RequestHandling, pServer, ClientSocket);
		wstring descr = L"Client " + to_wstring(ClientSocket);
		SetThreadDescription(Haldle.native_handle(), descr.c_str());
		
		Haldle.detach();
	}

	return 0;
}

string Server::Receive(SOCKET socket)
{
	string message;
	int receivedByte = 0;
	char recvbuf[MAX_PACKET_LEN + 1];

	while (1)
	{
		receivedByte = recv(socket, recvbuf, MAX_PACKET_LEN, 0);
		*(recvbuf + receivedByte) = 0;
		if (receivedByte > 0)
		{
			string rec(recvbuf, receivedByte);

			size_t pos = rec.find("\r\n\r\n");
			message += rec;

			if (pos != -1 || receivedByte < 5)
				break;
		}
		else if (receivedByte == 0)
			break;
		else if (receivedByte < 0)
			printf("Recv failed: %d\n", WSAGetLastError());
	}

	return message;
}

void Server::Transmit(SOCKET socket, Response* pResponse)
{
	size_t size = 0;
	char* txbuf;

	pResponse->GetData(&txbuf, (int*)&size);
	Transmit(socket, txbuf, size);
	delete[] txbuf;

	if (pResponse->HasContent())
	{
		char* buf;
		size = pResponse->GetContent(&buf);

		Transmit(socket, buf, size);
	}
}

void Server::Transmit(SOCKET socket, const char * pData, const size_t size)
{
	int totalSendedByte = 0;

	if (size > INT_MAX)
		throw exception("Size data for transmitting is too large.");
	else
	{
		while ((unsigned int)totalSendedByte < size)
		{
			int sendedByte = send(socket, pData + totalSendedByte, (int)size - totalSendedByte, NULL);
			if (sendedByte == SOCKET_ERROR)
				throw exception("Send failed", WSAGetLastError());
			totalSendedByte += sendedByte;
		}
	}
}

int Server::RequestHandling(Server* pServer, SOCKET clientSocket)
{
	Request request;
	Response response(&request);
	string message = pServer->Receive(clientSocket);

	if (message.empty())
		return 0;

	int iResult = request.TryParse(message);
	
	// Стандартные заголовки ответа
	response.GetHeaders()->push_back(pair<string, string>("Host", "127.0.0.1"));
	response.GetHeaders()->push_back(pair<string, string>("Connection", "close"));
	response.GetHeaders()->push_back(pair<string, string>("Server", "MyServer"));

	if (iResult != 0)
		response.m_status = HTTPSTATUS::BadRequest;
	else
		pServer->MakeResponse(&response);

	pServer->Transmit(clientSocket, &response);
	
	printf("Client %-4d %-10s %-30s %d %-15s %s %s\n", 
		(int)clientSocket, 
		request.GetMethod(), 
		request.GetResourceName(),
		(int)response.m_status,
		response.GetStatusDescription(response.m_status),
		(iResult == 0) ? " " : string("\n------ Error: " + to_string(iResult)).c_str(),
		(iResult == 0) ? " ": Request::GetErrorDescription(iResult));
	
	return 0;
}

void Server::MakeResponse(Response * pResponse)
{
	Request * pReq = pResponse->GetRequest();

	// Сервер доступен?
	if (false)
	{
		pResponse->m_status = HTTPSTATUS::ServiceUnavailable;
		return;
	}

	// URI слишком длинный?
	if (false)
	{
		pResponse->m_status = HTTPSTATUS::URITooLong;
		return;
	}

	// Запрос плохо сформирован?
	if (false)
	{
		pResponse->m_status = HTTPSTATUS::BadRequest;
		return;
	}

	// Требуется авторизация?
	if (false)
	{
		pResponse->m_status = HTTPSTATUS::Unauthorized;
		return;
	}

	//  Запрос разрешен?
	if (false)
	{
		pResponse->m_status = HTTPSTATUS::Forbidden;
		return;
	}
	
	//  Полезной нагрузки слишком много?
	if (false)
	{
		pResponse->m_status = HTTPSTATUS::PayloadTooLarge;
		return;
	}

	//  Неизвестный или не поддерживаемый Content-* заголовок?
	if (false)
	{
		pResponse->m_status = HTTPSTATUS::NotImplemented;
		return;
	}
	
	string method(pReq->GetMethod());
	if (method.compare("OPTIONS") == 0)
	{
		// Обработать запрос OPTIONS
		pResponse->m_status = HTTPSTATUS::OK;
		return;
	}
	
	if (method.compare("GET") != 0 && method.compare("POST") != 0)
	{
		pResponse->m_status = HTTPSTATUS::MethodNotAllowed;
		return;
	}
	
	string prot_version(pReq->GetProtocolVersion());
	if (prot_version.compare("1.1") != 0)
	{
		pResponse->m_status = HTTPSTATUS::HTTPVersionNotSupported;
		return;
	}

	// Сверяемся с таблицей перенаправлений
	if (m_pRedirectionList->empty() == false)
	{
		for (pair<string, string> item : *m_pRedirectionList)
		{
			if (item.first == pReq->GetResourceName())
			{
				pResponse->GetHeaders()->push_back(pair<string, string>("Location", item.second));
				pResponse->m_status = HTTPSTATUS::TemporaryRedirect;
				return;
			}
		}
	}

	HTTPSTATUS status = HTTPSTATUS::OK;
	string filename = GetTruePathToResource(pReq->GetResourceName());
	int nContentLength = 0;
	char* pContent;

	if (LoadFile(filename.c_str(), &pContent, &nContentLength, "rb"))
	{
		// Получаем расширение
		string path(pReq->GetResourceName());
		int idx_ext_sep = (int)path.find_last_of('.', (int)path.size());
		string extension = path.substr((size_t)idx_ext_sep + 1, (size_t)path.size() - idx_ext_sep - 1);
		// Определяем тип
		MIMEType contentType = MIMEType::GetMEMIType(extension);
				
		if (contentType.Type == "text" && contentType.SubType == "html")
		{
			string str_content(pContent);

			SSI_Handle(&pContent, &nContentLength);
		}

		pResponse->SetContent(pContent, nContentLength);

		if (nContentLength > 0)
		{
			pResponse->GetHeaders()->push_back(pair<string, string>("Content-Type", contentType.GetString()));
			pResponse->GetHeaders()->push_back(pair<string, string>("Content-Length", to_string(nContentLength)));
		}
		
		pResponse->m_status = status;

		return;
	}
	else
	{
		pResponse->m_status = HTTPSTATUS::NotFound;
		return;
	}
}

string Server::GetTruePathToResource(string pszResourceName)
{
	stringstream ss;
	ss << *m_psResourcePath << pszResourceName;
	return ss.str();
}

 void Server::Replace(string * pStr, int offset, char oldChar, char newChar)
 {
	 size_t cur_pos = offset;

	 while (1)
	 {
		 cur_pos = pStr->find(oldChar, (size_t)cur_pos + 1);
		 if (cur_pos == -1)
			 break;

		 char buf[] = { newChar, 0 };

		 pStr->replace(cur_pos, 1, buf);
	 }
 }
 
 bool Server::LoadFile(const char* pszFileName, char** ppData, int* pSize, const char* pMode)
 {
	 FILE* pFile = NULL;

	 errno_t Error = fopen_s(&pFile, pszFileName, pMode);
	 if (Error != 0 || pFile == 0)
	 {
		 *ppData = nullptr;
		 *pSize = 0;
		 return false;
	 }

	 fseek(pFile, 0, SEEK_END);
	 long size = ftell(pFile) + 1;
	 char* pData = new char[size];
	 ZeroMemory(pData, size);

	 fseek(pFile, 0, SEEK_SET);

	 int iResult = fgetc(pFile);
	 char* pCurr = pData;
	 while (iResult != EOF)
	 {
		 *pCurr++ = (char)iResult;
		 iResult = fgetc(pFile);
	 }

	 fclose(pFile);
	 
	 *ppData = pData;
	 *pSize = size;
	 return true;
 }

 bool Server::SSI_Handle(char** ppData, int* pSize)
 {
	 string pString(*ppData);
	 size_t offset = 0;

	 while (offset < pString.size())
	 {
		 size_t i_command = pString.find('#', offset);
		 if (i_command != -1)
		 {
			 size_t i_commentBegin = pString.find_last_of("<!--", i_command) - 3;
			 size_t i_commentEnd = pString.find("-->", i_command) + 3;
			 if (i_commentBegin != -1 && i_commentEnd != -1)
			 {
				 size_t i_space = pString.find(' ', i_command);
				 string command = pString.substr(i_command + 1, i_space - i_command - 1);
				 if (command == "include")
				 {
					 size_t i_begin = pString.find('\"', i_space);
					 if (i_begin != -1)
					 {
						 size_t i_end = pString.find('\"', i_begin + 1);
						 if (i_end != -1)
						 {
							 string value = pString.substr(i_begin + 1, i_end - i_begin - 1);

							 int nSubFileSize = 0;
							 char* pSubFileData = nullptr;
							 string filename = GetTruePathToResource('/' + value);
							 if (LoadFile(filename.c_str(), &pSubFileData, &nSubFileSize, "r"))
							 {
								 string sub(pSubFileData);

								 string result = pString.substr(0, i_commentBegin) + sub + pString.substr(i_commentEnd + 1, pString.size() - i_commentEnd - 1);
								 size_t size = result.size();
								 SafeReleaseArray(*ppData);
								 *ppData = new char[size + 1];
								 ZeroMemory(*ppData, size + (size_t)1);
								 CopyMemory(*ppData, result.c_str(), size);
								 *pSize = (int)size;
								 return true;
							 }
						 }
					 }
				 }
			 }
		 }
		 offset = i_command;
	 }
	 return false;
 }
 
 // TODO сразу освободить памать загруженного файла
 // TODO слеш в конце пути, если есть - убрать