#pragma once

#include "Request.h"

#define SafeReleaseArray(ptr)		if ((ptr) != nullptr) { delete[] (ptr); (ptr) = nullptr; }

typedef enum _HTTPSTATUS
{
	None = 0,					// �������� �� �����������
	// 1** ��������������
	Continue = 100,				// ���������
	SwitchingProtocol = 101,	// ������������ ����������	
	Processing = 102,			// ���� ���������
	// 2** ��������
	OK = 200,					// �������
	Created = 201,				// �������
	Accepted = 202,				// �������
	NonAuthoritativeInformation = 203,	// ���������� �� �����������
	NoContent = 204,			// ��� �����������
	ResetContent = 205,			// �������� ����������
	PartialContent = 206,		// ��������� ����������
	MultiStatus = 207,			// ��������������
	AlreadyReported = 208,		// ��� ����������
	IMUsed = 209,				// ������������ IM
	// 3** ���������������
	MultipleChoices = 300,		// ��������� �������
	MovedPermanently = 301,		// ���������� ��������
	Found = 302,				// �������
	SeeOther = 303,				// �������� ������
	NotModified = 304,			// �� ����������
	UseProxy = 305,				// ������������ ������
	SwitchProxy = 306,			// �� ������������. ����������� ������� ������ ������������ ��������� ������.
	TemporaryRedirect = 307,	// ��������� ���������������
	PermanentRedirect = 308,	// ���������� ���������������
	// 4** ������ �������
	BadRequest = 400,			// ������, �������� ������
	Unauthorized = 401,			// �� ����������� (�� ������������)
	PaymentRequired = 402,		// ���������� ������
	Forbidden = 403,			// ��������� (�� �����������)
	NotFound = 404,				// �� �������
	MethodNotAllowed = 405,		// ����� �� ��������������
	NotAcceptable = 406,		// �����������
	ProxyAuthenticationRequired= 407,	// ���������� �������������� ������
	RequestTimeout = 408,		// ������� ����� ��������
	Conflict = 409,				// ��������
	Gone = 410,					// �����
	LengthRequired = 411,		// ���������� �����
	PreconditionFailed = 412,	// ������� �����
	PayloadTooLarge = 413,		// �������� �������� ������� ������
	URITooLong = 414,			// URI ������� �������
	UnsupportedMediaType = 415,	// ���������������� ��� ������
	RangeNotSatisfiable = 416,	// �������� �� ��������
	ExpectationFailed = 417,	// �������� �� �������
	Im_a_teapot = 418,			// � � ������
	AuthenticationTimeout = 419,// ������ ������ �������� CSRF
	MisdirectedRequest = 421,	// 
	UnprocessableEntity = 422,	// ���������������� ���������
	Locked = 423,				// �������������
	FailedDependency = 424,		// ������������� �����������
	UpgradeRequired = 426,		// ���������� ����������
	PreconditionRequired = 428,	// ���������� �����������
	TooManyRequests = 429,		// ������� ����� ��������
	RequestHeaderFieldsTooLarge = 431,	// ���� ��������� ������� ������� �������
	RetryWith = 449,			// ��������� �
	UnavailableForLegalReasons = 451,	// ���������� �� ����������� ��������
	BadSendedRequest = 452,		// ���������� ������������ ������
	ClientClosedRequest = 499,	// ������ ������ ����������
	// 5** ������ �������
	InternalServerError = 500,	// ���������� ������ �������
	NotImplemented = 501,		// �� �����������
	BadGateway = 502,			// ������, ��������� ����
	ServiceUnavailable = 503,	// ������ ����������
	GatewayTimeout = 504,		// ���� �� ��������
	HTTPVersionNotSupported = 505,	// ������ HTTP �� ��������������
	VariantAlsoNegotiates = 506,// ������� ���� �������� ������������
	InsufficientStorage = 507,	// ������������ ���������
	LoopDetected = 508,			// ���������� ����������� ���������������
	BandwidthLimitExceeded = 509,	// ��������� ���������� ������ ������
	NotExtended = 510,			// �� ���������
	NetworkAuthenticationRequired = 511,	// ��������� ������� ��������������
	UnknownError = 520,			// ����������� ������
	WebServerIsDown = 521,		// ��� - ������ �� ��������
	ConnectionTimedOut = 522,	// ���������� �� ��������
	OriginIsUnreachable = 523,	// �������� ����������
	ATimeoutOccurred = 524,		// ����� �������� �������
	SSLHandshakeFailed = 525,	// ������������ SSL �� �������
	InvalidSSLCertificate = 526	// ���������������� ���������� SSL
} HTTPSTATUS, *PHTTPSTATUS;

class Response
{
public:
	HTTPSTATUS m_status;

public:
	Response(Request* pReq);
	~Response();
	list<pair<string, string>>* GetHeaders();
	Request* GetRequest();
	void SetContent(const char* pData, const int len);
	int GetContent(char** ppBuf);
	bool HasContent();
	void GetData(char** ppResponseData, int* pLen);
	static const char* GetStatusDescription(HTTPSTATUS status);

private:
	Request* m_pRequest;
	list<pair<string, string>> m_lsHeaders;
	char* m_pContent;
	int m_ContentLength;
};

