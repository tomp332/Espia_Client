#include "HttpRequests.h"

std::wstring Tool::HttpCommunication::HttpRequests::GetUserHeaderW(std::string& sSessionId)
{
	std::string sTempHeader = std::format(STR("Session: {}"), sSessionId);
	return std::wstring(sTempHeader.begin(), sTempHeader.end());
}


HINTERNET Tool::HttpCommunication::HttpRequests::AddHeader(HINTERNET Wrequest, std::wstring wHeader)
{
	if (!WinHttpAddRequestHeaders(Wrequest, wHeader.c_str(), (ULONG)-1L, WINHTTP_ADDREQ_FLAG_ADD & WINHTTP_ADDREQ_FLAG_REPLACE))
	{
		Tool::Utils::print_error(STR("adding header to request"));
		return NULL;
	}
	return Wrequest;
}

HINTERNET Tool::HttpCommunication::HttpRequests::AddDefaultHeaders(HINTERNET Wrequest, bool ifFile)
{
	HINTERNET newSession;
	DWORD dwFlags =
		SECURITY_FLAG_IGNORE_UNKNOWN_CA |
		SECURITY_FLAG_IGNORE_CERT_WRONG_USAGE |
		SECURITY_FLAG_IGNORE_CERT_CN_INVALID |
		SECURITY_FLAG_IGNORE_CERT_DATE_INVALID;
	if (!WinHttpSetOption(Wrequest, WINHTTP_OPTION_SECURITY_FLAGS, &dwFlags, sizeof(dwFlags)))
	{
		Tool::Utils::print_error(STR("adding default headers to request"));
		return NULL;
	}
	if (ifFile)
	{
		if (!WinHttpAddRequestHeaders(Wrequest, L"Content-Type:multipart/form-data", (ULONG)-1L, WINHTTP_ADDREQ_FLAG_ADD & WINHTTP_ADDREQ_FLAG_REPLACE))
		{
			Tool::Utils::print_error(STR("adding file headers to request"));
			return NULL;
		}
	}
	else
	{
		if (!WinHttpAddRequestHeaders(Wrequest, L"Content-Type:application/json", (ULONG)-1L, WINHTTP_ADDREQ_FLAG_ADD & WINHTTP_ADDREQ_FLAG_REPLACE))
		{
			Tool::Utils::print_error(STR("adding json header to request"));
			return NULL;
		}
	}
	return Wrequest;
}

HINTERNET Tool::HttpCommunication::HttpRequests::CreateFirstConnection(std::string fullDomain, DWORD port)
{
	HINTERNET Wopen, Wconnect;
	std::string sUserAgent = STR("Mozilla/5.0 (Windows Phone 10.0; Android 6.0.1; Microsoft; RM-1152) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/52.0.2743.116 Mobile Safari/537.36 Edge/15.15254");
	std::wstring wUserAgent(sUserAgent.begin(), sUserAgent.end());
	Wopen = WinHttpOpen(wUserAgent.c_str(), WINHTTP_ACCESS_TYPE_NO_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
	if (Wopen == NULL){
		Tool::Utils::print_error(STR("opening http request handler"));
		return NULL;
	}
	INTERNET_PORT iPort = port;
	Wconnect = WinHttpConnect(Wopen, Tool::Utils::s2ws(fullDomain).c_str(), port, 0);
	if (Wconnect == NULL){
		return NULL;
	}
	return Wconnect;
}

bool Tool::HttpCommunication::HttpRequests::SendPostRequest(const awesomejson::json &data, std::string sUriPath, std::string sSessionId, HINTERNET hUserSession, std::string sDomain, DWORD port) {
	Tool::Connection::ConnectionFuncs::WaitForConnection();
	std::string fullData;
	HINTERNET Wrequest;
	if ((hUserSession == NULL) && ((!sDomain.empty()) && (port != 0))) {
		hUserSession = Tool::HttpCommunication::HttpRequests::CreateFirstConnection(sDomain, port);
	}
	Wrequest = WinHttpOpenRequest(hUserSession, L"POST", Tool::Utils::s2ws(sUriPath).c_str(), NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
	if (Wrequest == NULL) {
		return NULL;
	}
	Wrequest = Tool::HttpCommunication::HttpRequests::AddDefaultHeaders(Wrequest);
	Tool::HttpCommunication::HttpRequests::AddHeader(Wrequest, GetUserHeaderW(sSessionId));
	std::string sData = data.dump();
	if (!WinHttpSendRequest(Wrequest, WINHTTP_NO_ADDITIONAL_HEADERS, -1L, (LPVOID)sData.c_str(), (DWORD)sData.length(), (DWORD)sData.length(), 0)) {
		Tool::Utils::print_error(STR("Unable to send request"));
		return NULL;
	}
	return Tool::HttpCommunication::HttpRequests::HandleResponse(Wrequest);
}

bool Tool::HttpCommunication::HttpRequests::HandleResponse(HINTERNET& wRequest) {
	DWORD dwTemp, dwStatusCode;
	if (!WinHttpReceiveResponse(wRequest, NULL)) {
		Tool::Utils::print_error(STR("Unable to receive response"));
		return false;
	}
	else {
		dwTemp = sizeof(dwStatusCode);
		if (!WinHttpQueryHeaders(wRequest, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, NULL, &dwStatusCode, &dwTemp, NULL)) {
			Tool::Utils::print_error(STR("Unable to query headers from response"));
			return false;
		}
		if (dwStatusCode == 200 || dwStatusCode == 4) {
			WinHttpCloseHandle(wRequest);
			return true;
		}
		else {
			Tool::Utils::print_error(std::format(STR("Received response code: {}"), dwTemp));
			return false;
		}
	}
}

bool Tool::HttpCommunication::HttpRequests::UploadFile(std::string filePath, std::string sUriPath, std::string sSessionId, HINTERNET hUserSession, std::string sDomain, DWORD port) {
	Tool::Connection::ConnectionFuncs::WaitForConnection();
	bool bResults;
	HINTERNET  hRequest = NULL;
	DWORD dwContentLength = 0;
	DWORD dwBoundaryValue = 0;
	DWORD dwFileSize = 0;
	LPCWSTR wszProxyAddress = 0;
	PCHAR pFormHeader = 0;
	PCHAR pFinalBoundary = 0;
	PUCHAR pBuf = 0;
	wchar_t* pContentHeader = 0;
	int res;
	char* content;
	FILE* new_file;
	long lSize;
	size_t result;
	if ((hUserSession == NULL) && ((!sDomain.empty()) && (port != 0))) {
		hUserSession = CreateFirstConnection(sDomain, port);
	}
	std::filesystem::path pFilePath(filePath);
	DWORD dwStatusCode = 0, dwTemp = 0;
	LPSTR data = (char*)calloc(10000000, sizeof(char));
	std::string pszFormHeader = STR("------Boundary%08X\r\nContent-Disposition: form-data; name=\"fileUpload\"; filename=\"");
	pszFormHeader += pFilePath.filename().string();
	pszFormHeader += "\"";
	pszFormHeader += "\r\nContent-Type:text/plain\r\n\r\n";
	char* pszFinalBoundary = (char*)"\r\n------Boundary%08X--\r\n";
	wchar_t* wszContentHeader = (wchar_t*)L"Content-Type: multipart/form-data; boundary=----Boundary%08X";
	wchar_t wszContentLength[256] = { 0 };

	pFormHeader = (PCHAR)calloc(BUFFER_SIZE_1KB, 1);
	pFinalBoundary = (PCHAR)calloc(BUFFER_SIZE_1KB, 1);
	pContentHeader = (wchar_t*)calloc(BUFFER_SIZE_1KB, 1);
	dwBoundaryValue = GetTickCount64();
	sprintf_s(pFormHeader, BUFFER_SIZE_1KB, pszFormHeader.c_str(), dwBoundaryValue, dwBoundaryValue);
	sprintf_s(pFinalBoundary, BUFFER_SIZE_1KB, pszFinalBoundary, dwBoundaryValue);
	res = wsprintf(pContentHeader, wszContentHeader, dwBoundaryValue);
	if (wszProxyAddress != NULL && wcslen(wszProxyAddress) < 4)
	{
		wszProxyAddress = NULL;
	}
	if (fopen_s(&new_file, filePath.c_str(), "rb") != 0)
	{
		delete pFormHeader;
		delete pFinalBoundary;
		delete pContentHeader;
		return false;
	}

	if (fseek(new_file, 0, SEEK_END) != 0) {
		Tool::Utils::print_error(STR("reading file before sending"));
		return false;
	}
	lSize = ftell(new_file);
	dwFileSize = lSize;
	rewind(new_file);
	content = (char*)malloc(sizeof(char) * (lSize + 1));
	if (content == NULL)
	{
		delete pFormHeader;
		delete pFinalBoundary;
		delete pContentHeader;
		return false;
	}
	result = fread(content, 1, lSize, new_file);
	fflush(stdout);
	if (result != lSize)
	{
		delete pFormHeader;
		delete pFinalBoundary;
		delete pContentHeader;
		return false;
	}
	content[lSize] = '\0';
	fclose(new_file);
	if (dwFileSize == 0)
	{
		delete pFormHeader;
		delete pFinalBoundary;
		delete pContentHeader;
		return false;
	}
	hRequest = WinHttpOpenRequest(hUserSession, L"POST", Tool::Utils::s2ws(sUriPath).c_str(), NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE | WINHTTP_FLAG_REFRESH);
	if (!hRequest)
	{
		delete pFormHeader;
		delete pFinalBoundary;
		delete pContentHeader;
		Tool::Utils::print_error("Unable to send file to server");
		return false;
	}

	hRequest = AddDefaultHeaders(hRequest, true);

	if (!WinHttpAddRequestHeaders(hRequest, pContentHeader, (ULONG)-1L, WINHTTP_ADDREQ_FLAG_ADD | WINHTTP_ADDREQ_FLAG_REPLACE))
	{
		delete pFormHeader;
		delete pFinalBoundary;
		delete pContentHeader;
		return false;
	}
	Tool::HttpCommunication::HttpRequests::AddHeader(hRequest, GetUserHeaderW(sSessionId));
	dwContentLength = (DWORD)(strlen(pFormHeader) + dwFileSize + strlen(pFinalBoundary));
	pBuf = (PUCHAR)calloc(dwContentLength + lSize, 1);
	sprintf((PCHAR)pBuf, "%s", pFormHeader);
	int len = (int)strlen((char*)pBuf);
	memcpy(&pBuf[len], content, lSize);
	sprintf((PCHAR)&pBuf[len + lSize], "%s", pFinalBoundary);
	wsprintf((LPWSTR)wszContentLength, L"Content-Length: %d", dwContentLength);
	bResults = WinHttpSendRequest(hRequest, wszContentLength, -1, pBuf, dwContentLength, dwContentLength, 0);
	if (!bResults) {
		free(pFormHeader);
		free(pFinalBoundary);
		free(pContentHeader);
		return false;
	}
	return Tool::HttpCommunication::HttpRequests::HandleResponse(hRequest);
}