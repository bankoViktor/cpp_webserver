#include "Request.h"

int Request::TryParse(string content)
{
	if (content.empty())
		return 8;

	size_t idx_resource = content.find(" ");
	if (idx_resource == -1)
		return 1;
	m_sMethod = content.substr(0, idx_resource);

	size_t idx_protocol = content.find(" ", idx_resource + 1);
	if (idx_protocol == -1)
		return 2;
	string uri = content.substr(idx_resource + 1, idx_protocol - idx_resource - 1);
	
	m_sResource = uri;
	size_t idx_uri_param_begin = m_sResource.find('?');
	if (idx_uri_param_begin != -1)
	{
		m_sResource = uri.substr(0, idx_uri_param_begin);
		
		if ((uri.size() - idx_uri_param_begin - 1) == 0)
			return 3;

		size_t offset = idx_uri_param_begin + 1;
		size_t idx_uri_param_sep = 0;

		while (idx_uri_param_sep != -1)
		{
			idx_uri_param_sep = uri.find('&', offset);
			
			size_t idx_uri_value_sep = uri.find('=', offset);
			if (idx_uri_value_sep != -1)
			{
				string key = uri.substr(offset, idx_uri_value_sep - offset);
				string value = uri.substr(idx_uri_value_sep + 1,
					((idx_uri_param_sep != -1) ? idx_uri_param_sep : (int)uri.size()) - idx_uri_value_sep - 1);
				
				if (value.empty())
					return 4;

				for (pair<string, string> item : m_lsParams)
					if (item.first == key)
						return 9;

				m_lsParams.push_back(pair<string, string>(key, value));
			}
			else
				return 5;
				
			if (idx_uri_param_sep != -1)
				offset = idx_uri_param_sep + 1;
		}
	}

	size_t idx_protocol_sep = content.find("/", idx_protocol + 1);
	if (idx_protocol_sep == -1)
		return 6;
	m_sProtocolName = content.substr(idx_protocol + 1, idx_protocol_sep - idx_protocol - 1);

	size_t idx_firstEnd = content.find("\r\n");
	if (idx_firstEnd == -1)
		return 7;
	m_sProtocolVersion = content.substr(idx_protocol_sep + 1, idx_firstEnd - idx_protocol_sep - 1);

	size_t idx_begin = idx_firstEnd + 2;
	size_t idx_end = 0;

	while (idx_end + 4 < (int)content.length())
	{
		idx_end = content.find("\r\n", idx_begin);
		size_t idx_separator = content.find(":", idx_begin);
		if (idx_separator != -1)
		{
			m_lsHeaders.push_back(pair<string, string>(
				content.substr(idx_begin, idx_separator - idx_begin),
				content.substr(idx_separator + 2, idx_end - idx_separator - 2)));
		}
		idx_begin = idx_end + 2;
	}

	return 0;
}

const char* Request::GetMethod()
{
	return m_sMethod.c_str();
}

const char* Request::GetProtocolName()
{
	return m_sProtocolName.c_str();
}

const char* Request::GetProtocolVersion()
{
	return m_sProtocolVersion.c_str();
}

const char* Request::GetResourceName()
{
	return m_sResource.c_str();
}

const list<pair<string, string>>* Request::GetHeaders()
{
	return &m_lsHeaders;
}

const list<pair<string, string>>* Request::GetParams()
{
	return &m_lsParams;
}

const char * Request::GetErrorDescription(int err)
{
	const char * ppszResult;

	switch (err)
	{
	case 0:
		ppszResult = "Succsess.";
		break;
	case 1:
		ppszResult = "Method parse failed.";
		break;
	case 2:
		ppszResult = "URI parse failed.";
		break;
	case 3:
		ppszResult = "Marker of begin of list parameters is detected, but parameters not found.";
		break;
	case 4:
		ppszResult = "Value of parameter is empty.";
		break;
	case 5:
		ppszResult = "Parameter parse failed.";
		break;
	case 6:
		ppszResult = "Protocol parse failed.";
		break;
	case 7:
		ppszResult = "Protocol version parse failed.";
		break;
	case 8:
		ppszResult = "Have not the data for parse.";
		break;
	case 9:
		ppszResult = "Parameter repeated.";
		break;
	default:
		ppszResult = "Unknown Error";
		break;
	}

	return ppszResult;
}
