
#include <iostream>
#include <stdio.h>
#include <sstream>

#include "Server.h"


string INI_GetString(FILE* pFile, const char* pszSection, const char* pszKey, const char* pszDef)
{
	enum BlockType {
		NONE,
		SECTION,
		KEY,
		VALUE,
		COMMENT
	}	type = NONE;

	int counter = 0;
	char* pointer = nullptr;
	const char* matchPointer = nullptr;
	const int value_max_len = 512;
	char _pszValue[value_max_len];
	bool quotationMark = false; // кавычка
	bool sectionNeeded = false;
	bool keyNeeded = false;

	fseek(pFile, 0, SEEK_SET);
	
	char __ch = 0;
	char ch = fgetc(pFile);
	do {
		switch (ch)
		{
		case '\"':
		case '\'':
			quotationMark = !quotationMark;
			if (quotationMark && sectionNeeded && keyNeeded)
			{
				counter = 0;
				pointer = _pszValue;
				matchPointer = nullptr;
			}
			break;
		case '[':
			type = SECTION;
			if (sectionNeeded)
				return string(pszDef);
			sectionNeeded = true;
			counter = 0;
			matchPointer = pszSection;
			break;
		case ']':
			// конец имени секции в файле, но имя требуемой секции длинее
			if (matchPointer && matchPointer[counter] != '\0')
				sectionNeeded = false;
			matchPointer = nullptr;
			break;
		case '=':
			if (type == KEY && matchPointer && matchPointer[counter] != '\0')
			{
				keyNeeded = false;
				counter = 0;
				matchPointer = nullptr;
			}
			type = VALUE;
			counter = 0;
			break;
		case ' ':
			if (type == VALUE && quotationMark == false && pointer)
				goto case_new_line;
			
			if (__ch != '\"' && __ch != '\'' && __ch != ']')
			{
				if (__ch == '\n')
					type = NONE;
				break;
			}
		case '-1':
		case '\n':
			case_new_line:
			if (type == VALUE && sectionNeeded && keyNeeded)
			{
				_pszValue[counter] = '\0';
				return string(_pszValue);
			}
			type = NONE;
			quotationMark = false;
			break;
		case ';':
		case '#':
			type = COMMENT;
			matchPointer = 0;
			counter = 0;
			pointer = nullptr;
			break;
		default:
			if (type == VALUE && sectionNeeded && keyNeeded)
			{
				pointer = _pszValue;
				if (ch == -1)
					goto case_new_line;
			}

			if (type == KEY && ch != ' ' && matchPointer == nullptr )
			{
				keyNeeded = true;
				counter = 0;
				matchPointer = pszKey;
			}

			if (type == NONE && __ch == ' ' || __ch == '\n')
			{
				type = KEY;
				keyNeeded = true;
				counter = 0;
				matchPointer = pszKey;
			}

			if (matchPointer)
			{
				if (ch != matchPointer[counter++])
				{
					if (type == SECTION)
						sectionNeeded = false;

					if (type == KEY)
						keyNeeded = false;

					counter = 0;
					matchPointer = nullptr;
					break;
				}

				if (matchPointer[counter] != '\0')
				{
					if (sectionNeeded && ch == '\0')
						sectionNeeded = false;
					if (keyNeeded && ch == '=')
						keyNeeded = false;
				}
			}
			break;
		}

		if (pointer)
			pointer[counter++] = ch;

		__ch = ch;
		ch = fgetc(pFile);
	} while (__ch != EOF);

	return string(pszDef);
}

inline int INI_GetInt(FILE* pFile, const char* pszSection, const char* pszKey, int nDef)
{
	string val = INI_GetString(pFile, pszSection, pszKey, "-");
	return (val == "-") ? nDef : atoi(val.c_str());
}

list<pair<string, string>> RedirectionList;

int main(int argc, CHAR argv[])
{
	string sINIFileName;

	char pszConfigPath[MAX_PATH];
	ZeroMemory(pszConfigPath, MAX_PATH);
	GetModuleFileNameA(NULL, pszConfigPath, MAX_PATH);
	string path(pszConfigPath);
	int sep = path.find_last_of('\\', path.size());
	string dir = path.substr(0, sep);

	if (argc > 1)
		sINIFileName = (char*)(*((int*)argv + 1));
	else
		sINIFileName = dir + "\\config.ini";

	int Port = 0;
	string sResourcePath;
	string sRedirectionListPath;

	FILE* pFile;
	errno_t err = fopen_s(&pFile, sINIFileName.c_str(), "rt");
	if (err == 0 && pFile != 0)
	{
		Port = INI_GetInt(pFile, "General", "Port", 63000);
		sResourcePath = INI_GetString(pFile, "General", "ResourcePath", dir.c_str());
		sRedirectionListPath = INI_GetString(pFile, "General", "RedirectionListPath", "");

		int pcnt1 = sRedirectionListPath.find('%');
		if (pcnt1 != -1)
		{
			int pcnt2 = sRedirectionListPath.find('%', pcnt1 + 1);
			string valname = sRedirectionListPath.substr(pcnt1 + 1, pcnt2 - pcnt1 - 1);
			if (valname == "ResourcePath")
			{
				if (sResourcePath.at(0) == '\"')
					sResourcePath = sResourcePath.substr(1, sResourcePath.size() - 2);

				if (sResourcePath.at(sResourcePath.size() - 1) == '\\')
					sResourcePath = sResourcePath.substr(0, sResourcePath.size() - 1);

				if (sRedirectionListPath.at(pcnt2 + 1) == '\\')
					sRedirectionListPath = sRedirectionListPath.substr(pcnt2 + 2, sResourcePath.size() - pcnt2 - 2);
				
				sRedirectionListPath = sResourcePath + '\\' + sRedirectionListPath;
			}
			
		}
		fclose(pFile);
	}
	
	
	err = fopen_s(&pFile, sRedirectionListPath.c_str(), "rt");
	if (err == 0 && pFile != 0)
	{
		stringstream ss;
		char ch = fgetc(pFile);
		while (ch != EOF)
		{
			if (ch == -1)
				break;
			ss << ch;
			ch = fgetc(pFile);
		}
		fclose(pFile);

		string redir = ss.str();
		int n1 = 0;
		while (n1 < (int)redir.size())
		{
			int n2 = redir.find('\n', n1);
			if (n2 == -1)
				n2 = redir.size();

			string line = redir.substr(n1, n2 - n1);

			int sep = line.find('=');
			if (sep != -1)
			{
				RedirectionList.push_back(pair<string, string>(
					line.substr(0, sep),
					line.substr(sep + 1, line.size() - sep)
					));
			}
			n1 = n2;
		}
	}

	Server server(
		Port,
		&sResourcePath,
		&RedirectionList
	);

	server.Run();

	return 0;
}
