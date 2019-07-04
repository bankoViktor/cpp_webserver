#include "Responce.h"

#include <sstream>

Response::Response(Request* pReq)
{
	m_pRequest = pReq;
	m_status = HTTPSTATUS::None;
	m_pContent = nullptr;
	m_ContentLength = 0;
}

Response::~Response()
{
	SafeReleaseArray(m_pContent);
}

void Response::GetData(char** ppResponseData, int* pLen) // возращенный указатель нужно освобождать вручную
{
	ostringstream ss;
	ss
		<< m_pRequest->GetProtocolName() << "/"
		<< m_pRequest->GetProtocolVersion() << " "
		<< to_string((int)m_status) << " "
		<< Response::GetStatusDescription(m_status) << "\r\n";

	if (m_lsHeaders.empty() == false)
	{
		for (pair<string, string> item : m_lsHeaders)
			ss << item.first << ": " << item.second << "\r\n";
	}

	ss << "\r\n";

	string responseHead = ss.str();
	int size = responseHead.size();

	char* buf = new char[size + 1];
	buf[size] = 0;
	memcpy(buf, responseHead.c_str(), size);

	*pLen = size;
	*ppResponseData = buf;
}

const char* Response::GetStatusDescription(HTTPSTATUS status)
{
	const char* pszResult = nullptr;

	switch (status)
	{
	case HTTPSTATUS::None: pszResult = ""; break;
		// 1** Информационные
	case HTTPSTATUS::Continue: pszResult = "Continue"; break;
	case HTTPSTATUS::SwitchingProtocol: pszResult = "SwitchingProtocol"; break;
	case HTTPSTATUS::Processing: pszResult = "Processing"; break;
		// 2** Успешные
	case HTTPSTATUS::OK: pszResult = "OK"; break;
	case HTTPSTATUS::Created: pszResult = "Created"; break;
	case HTTPSTATUS::Accepted: pszResult = "Accepted"; break;
	case HTTPSTATUS::NonAuthoritativeInformation: pszResult = "NonAuthoritativeInformation"; break;
	case HTTPSTATUS::NoContent: pszResult = "NoContent"; break;
	case HTTPSTATUS::ResetContent: pszResult = "ResetContent"; break;
	case HTTPSTATUS::PartialContent: pszResult = "PartialContent"; break;
	case HTTPSTATUS::MultiStatus: pszResult = "MultiStatus"; break;
	case HTTPSTATUS::AlreadyReported: pszResult = "AlreadyReported"; break;
	case HTTPSTATUS::IMUsed: pszResult = "IMUsed"; break;
		// 3** Перенаправления
	case HTTPSTATUS::MultipleChoices: pszResult = "MultipleChoices"; break;
	case HTTPSTATUS::MovedPermanently: pszResult = "MovedPermanently"; break;
	case HTTPSTATUS::Found: pszResult = "Found"; break;
	case HTTPSTATUS::SeeOther: pszResult = "SeeOther"; break;
	case HTTPSTATUS::NotModified: pszResult = "NotModified"; break;
	case HTTPSTATUS::UseProxy: pszResult = "UseProxy"; break;
	case HTTPSTATUS::SwitchProxy: pszResult = "SwitchProxy"; break;
	case HTTPSTATUS::TemporaryRedirect: pszResult = "TemporaryRedirect"; break;
	case HTTPSTATUS::PermanentRedirect: pszResult = ""; break;
		// 4** Ошибка клиента
	case HTTPSTATUS::BadRequest: pszResult = "BadRequest"; break;
	case HTTPSTATUS::Unauthorized: pszResult = "Unauthorized"; break;
	case HTTPSTATUS::PaymentRequired: pszResult = "PaymentRequired"; break;
	case HTTPSTATUS::Forbidden: pszResult = "Forbidden"; break;
	case HTTPSTATUS::NotFound: pszResult = "NotFound"; break;
	case HTTPSTATUS::MethodNotAllowed: pszResult = "MethodNotAllowed"; break;
	case HTTPSTATUS::NotAcceptable: pszResult = "NotAcceptable"; break;
	case HTTPSTATUS::ProxyAuthenticationRequired: pszResult = "ProxyAuthenticationRequired"; break;
	case HTTPSTATUS::RequestTimeout: pszResult = "RequestTimeout"; break;
	case HTTPSTATUS::Conflict: pszResult = "Conflict"; break;
	case HTTPSTATUS::Gone: pszResult = "Gone"; break;
	case HTTPSTATUS::LengthRequired: pszResult = "LengthRequired"; break;
	case HTTPSTATUS::PreconditionFailed: pszResult = "PreconditionFailed"; break;
	case HTTPSTATUS::PayloadTooLarge: pszResult = "PayloadTooLarge"; break;
	case HTTPSTATUS::URITooLong: pszResult = "URITooLong"; break;
	case HTTPSTATUS::UnsupportedMediaType: pszResult = "UnsupportedMediaType"; break;
	case HTTPSTATUS::RangeNotSatisfiable: pszResult = "RangeNotSatisfiable"; break;
	case HTTPSTATUS::ExpectationFailed: pszResult = "ExpectationFailed"; break;
	case HTTPSTATUS::Im_a_teapot: pszResult = "Im_a_teapot"; break;
	case HTTPSTATUS::AuthenticationTimeout: pszResult = "AuthenticationTimeout"; break;
	case HTTPSTATUS::MisdirectedRequest: pszResult = "MisdirectedRequest"; break;
	case HTTPSTATUS::UnprocessableEntity: pszResult = "UnprocessableEntity"; break;
	case HTTPSTATUS::Locked: pszResult = "Locked"; break;
	case HTTPSTATUS::FailedDependency: pszResult = "FailedDependency"; break;
	case HTTPSTATUS::UpgradeRequired: pszResult = "UpgradeRequired"; break;
	case HTTPSTATUS::PreconditionRequired: pszResult = "PreconditionRequired"; break;
	case HTTPSTATUS::TooManyRequests: pszResult = "TooManyRequests"; break;
	case HTTPSTATUS::RequestHeaderFieldsTooLarge: pszResult = "RequestHeaderFieldsTooLarge"; break;
	case HTTPSTATUS::RetryWith: pszResult = "RetryWith"; break;
	case HTTPSTATUS::UnavailableForLegalReasons: pszResult = "UnavailableForLegalReasons"; break;
	case HTTPSTATUS::BadSendedRequest: pszResult = "BadSendedRequest"; break;
	case HTTPSTATUS::ClientClosedRequest: pszResult = "ClientClosedRequest"; break;
		// 5** Ошибка сервера
	case HTTPSTATUS::InternalServerError: pszResult = "InternalServerError"; break;
	case HTTPSTATUS::NotImplemented: pszResult = "NotImplemented"; break;
	case HTTPSTATUS::BadGateway: pszResult = "BadGateway"; break;
	case HTTPSTATUS::ServiceUnavailable: pszResult = "ServiceUnavailable"; break;
	case HTTPSTATUS::GatewayTimeout: pszResult = "GatewayTimeout"; break;
	case HTTPSTATUS::HTTPVersionNotSupported: pszResult = "HTTPVersionNotSupported"; break;
	case HTTPSTATUS::VariantAlsoNegotiates: pszResult = "VariantAlsoNegotiates"; break;
	case HTTPSTATUS::InsufficientStorage: pszResult = "InsufficientStorage"; break;
	case HTTPSTATUS::LoopDetected: pszResult = "LoopDetected"; break;
	case HTTPSTATUS::BandwidthLimitExceeded: pszResult = "BandwidthLimitExceeded"; break;
	case HTTPSTATUS::NotExtended: pszResult = "NotExtended"; break;
	case HTTPSTATUS::NetworkAuthenticationRequired: pszResult = "NetworkAuthenticationRequired"; break;
	case HTTPSTATUS::UnknownError: pszResult = "UnknownError"; break;
	case HTTPSTATUS::WebServerIsDown: pszResult = "WebServerIsDown"; break;
	case HTTPSTATUS::ConnectionTimedOut: pszResult = "ConnectionTimedOut"; break;
	case HTTPSTATUS::OriginIsUnreachable: pszResult = "Im_a_teapot"; break;
	case HTTPSTATUS::ATimeoutOccurred: pszResult = "ATimeoutOccurred"; break;
	case HTTPSTATUS::SSLHandshakeFailed: pszResult = "SSLHandshakeFailed"; break;
	case HTTPSTATUS::InvalidSSLCertificate: pszResult = "InvalidSSLCertificate"; break;
	default: throw exception("Value out of range.");
	}

	return pszResult;
}

list<pair<string, string>>* Response::GetHeaders()
{
	return &m_lsHeaders;
}

Request* Response::GetRequest()
{
	return m_pRequest;
}

void Response::SetContent(const char* pData, const int len)
{
	SafeReleaseArray(m_pContent);
	m_pContent = (char*) pData;
	m_ContentLength = len;
}

int Response::GetContent(char** ppBuf)
{
	char* ptr = nullptr;
	int size = 0;

	if (HasContent())
	{
		ptr = m_pContent;
		size = m_ContentLength;
	}

	*ppBuf = ptr;
	return size;
}

bool Response::HasContent()
{
	return m_pContent != nullptr && m_ContentLength > 0;
}
