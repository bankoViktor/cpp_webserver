
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
	bool sectionNeeded = false;
	bool keyNeeded = false;
	char __ch = 0;
	char ch = fgetc(pFile);

	fseek(pFile, 0, SEEK_SET);
	
	while (ch != EOF)
	{
		switch (ch)
		{
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
			type = VALUE;
			counter = 0;
			if (sectionNeeded && keyNeeded)
				pointer = _pszValue;
			break;
		case '\n':
			// TODO учеть вместо перевода строки - конец файла
			if (type == VALUE && sectionNeeded && keyNeeded)
			{
				_pszValue[counter] = '\0';
				return string(_pszValue);
			}
			type = NONE;
			break;
		case ';':
		case '#':
			type = COMMENT;
			matchPointer = 0;
			counter = 0;
			pointer = nullptr;
			break;
		default:
			if (__ch == '\n')
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

				// TODO строка образец параметра больше имеющейся в файле
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
	}

	return string(pszDef);
}

inline int INI_GetInt(FILE* pFile, const char* pszSection, const char* pszKey, int nDef)
{
	string val = INI_GetString(pFile, pszSection, pszKey, "-");

	return (val == "-") ? nDef : atoi(val.c_str());
}

int main()
{
	char szFilename[ MAX_PATH ];
	GetModuleFileNameA(NULL, szFilename, MAX_PATH);

	string path(szFilename);
	int sep = path.find_last_of('\\', path.size());
	string dir = path.substr(0, sep);

	FILE* ini;
	string iniName = dir + "\\config.ini";
	errno_t err = fopen_s(&ini, iniName.c_str(), "rt");
	if (err == 0 && ini != 0)
	{
		//string ResourcePath = GetString(ini, "General", "ResourcePath");
		string Port1 = INI_GetString(ini, "General", "ServerName1", "NULL");
		string Port2 = INI_GetString(ini, "General", "ServerName", "NULL");
		int Port3 = INI_GetInt(ini, "General", "Portoo", 80); // TODO исправить
		fclose(ini);
	}
	else
	{

	}


	// Таблица перанаправлений
	//server.RedirectionList.push_back(pair<string, string>("/", "/index.html"));

	Server server(
		63000,
		"C:\\Users\\Winston\\source\repos\\cpp_webserver\\Resources"
	);

	server.Run();

	return 0;
}
