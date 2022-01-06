#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <winhttp.h>
#include <iostream>
#include "json.hpp"
#include "Connection.h"


#define BUFFER_SIZE_1KB 1024

#pragma comment(lib, "Winhttp")

namespace Tool
{
	namespace HttpCommunication
	{
		class HttpRequests
		{
		public:
			static std::wstring GetUserHeaderW(std::string& sSessionId);

			static HINTERNET AddHeader(HINTERNET Wrequest, std::wstring wHeader);

			static HINTERNET AddDefaultHeaders(HINTERNET Wrequest, bool ifFile = false);

			static HINTERNET  CreateFirstConnection(std::string fullDomain, DWORD port);

			static bool UploadFile(std::string filePath, std::string sUriPath, std::string sSessionId, HINTERNET hUserSession = NULL, std::string sDomain = "", DWORD port = 0);

			static bool SendPostRequest(const awesomejson::json& data, std::string sUriPath, std::string sSessionId, HINTERNET hUserSession = NULL, std::string sDomain = "", DWORD port = 0);

			static bool HandleResponse(HINTERNET& wRequest);
		};
	}

}
