#pragma once

#include <string>
#include <list>

using namespace std;

class Request
{
public:
	int TryParse(string content);
	const char* GetMethod();
	const char* GetProtocolName();
	const char* GetProtocolVersion();
	const char* GetResourceName();
	const list<pair<string, string>>* GetHeaders();
	const list<pair<string, string>>* GetParams();
	static const char * GetErrorDescription(int err);

private:
	string m_sMethod;
	string m_sProtocolName;
	string m_sProtocolVersion;
	string m_sResource;
	list<pair<string, string>> m_lsHeaders;
	list<pair<string, string>> m_lsParams;
};

