#pragma once

#include "Request.h"

#define SafeReleaseArray(ptr)		if ((ptr) != nullptr) { delete[] (ptr); (ptr) = nullptr; }

typedef enum _HTTPSTATUS
{
	None = 0,					// значение не установлено
	// 1** Информационные
	Continue = 100,				// продолжай
	SwitchingProtocol = 101,	// переключение протоколов	
	Processing = 102,			// идет обработка
	// 2** Успешные
	OK = 200,					// успешно
	Created = 201,				// создано
	Accepted = 202,				// принято
	NonAuthoritativeInformation = 203,	// информация не авторитетна
	NoContent = 204,			// нет содержимого
	ResetContent = 205,			// сбросить содержимое
	PartialContent = 206,		// частичное содержимое
	MultiStatus = 207,			// многостатусный
	AlreadyReported = 208,		// уже сообщалось
	IMUsed = 209,				// использовано IM
	// 3** Перенаправления
	MultipleChoices = 300,		// множество выборов
	MovedPermanently = 301,		// перемещено навсегда
	Found = 302,				// найдено
	SeeOther = 303,				// смотреть другое
	NotModified = 304,			// не изменялось
	UseProxy = 305,				// использовать прокси
	SwitchProxy = 306,			// НЕ ИСПОЛЬЗОВАТЬ. последующие запросы должны использовать указанный прокси.
	TemporaryRedirect = 307,	// временное перенаправление
	PermanentRedirect = 308,	// постоянное перенаправление
	// 4** Ошибка клиента
	BadRequest = 400,			// плохой, неверный запрос
	Unauthorized = 401,			// не авторизован (не представился)
	PaymentRequired = 402,		// необходима оплата
	Forbidden = 403,			// запрещено (не уполномочен)
	NotFound = 404,				// не найдено
	MethodNotAllowed = 405,		// метод не поддерживается
	NotAcceptable = 406,		// неприемлемо
	ProxyAuthenticationRequired= 407,	// необходима аутентификация прокси
	RequestTimeout = 408,		// истекло время ожидания
	Conflict = 409,				// конфликт
	Gone = 410,					// удалён
	LengthRequired = 411,		// необходима длина
	PreconditionFailed = 412,	// условие ложно
	PayloadTooLarge = 413,		// полезная нагрузка слишком велика
	URITooLong = 414,			// URI слишком длинный
	UnsupportedMediaType = 415,	// неподдерживаемый тип данных
	RangeNotSatisfiable = 416,	// диапазон не достижим
	ExpectationFailed = 417,	// ожидание не удалось
	Im_a_teapot = 418,			// я — чайник
	AuthenticationTimeout = 419,// обычно ошибка проверки CSRF
	MisdirectedRequest = 421,	// 
	UnprocessableEntity = 422,	// необрабатываемый экземпляр
	Locked = 423,				// заблокировано
	FailedDependency = 424,		// невыполненная зависимость
	UpgradeRequired = 426,		// необходимо обновление
	PreconditionRequired = 428,	// необходимо предусловие
	TooManyRequests = 429,		// слишком много запросов
	RequestHeaderFieldsTooLarge = 431,	// поля заголовка запроса слишком большие
	RetryWith = 449,			// повторить с
	UnavailableForLegalReasons = 451,	// недоступно по юридическим причинам
	BadSendedRequest = 452,		// отправлены неприемлемые данные
	ClientClosedRequest = 499,	// клиент закрыл соединение
	// 5** Ошибка сервера
	InternalServerError = 500,	// внутренняя ошибка сервера
	NotImplemented = 501,		// не реализовано
	BadGateway = 502,			// плохой, ошибочный шлюз
	ServiceUnavailable = 503,	// сервис недоступен
	GatewayTimeout = 504,		// шлюз не отвечает
	HTTPVersionNotSupported = 505,	// версия HTTP не поддерживается
	VariantAlsoNegotiates = 506,// вариант тоже проводит согласование
	InsufficientStorage = 507,	// переполнение хранилища
	LoopDetected = 508,			// обнаружено бесконечное перенаправление
	BandwidthLimitExceeded = 509,	// исчерпана пропускная ширина канала
	NotExtended = 510,			// не расширено
	NetworkAuthenticationRequired = 511,	// требуется сетевая аутентификация
	UnknownError = 520,			// неизвестная ошибка
	WebServerIsDown = 521,		// веб - сервер не работает
	ConnectionTimedOut = 522,	// соединение не отвечает
	OriginIsUnreachable = 523,	// источник недоступен
	ATimeoutOccurred = 524,		// время ожидания истекло
	SSLHandshakeFailed = 525,	// квитирование SSL не удалось
	InvalidSSLCertificate = 526	// недействительный сертификат SSL
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

