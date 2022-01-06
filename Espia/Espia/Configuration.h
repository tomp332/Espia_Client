#pragma once
#include <Windows.h>
#include <iostream>
#include "Utils.hpp"
#include "json.hpp"
#include "HttpRequests.h"
#include "SystemInfo.h"
#include <format>
#pragma comment(lib, "rpcrt4.lib")
using namespace awesomejson;

namespace Tool
{
	class Configuration {
	public:
		Configuration(std::string jConfigBuffer);
		// Global struct for main context
		struct config_t {
			std::string sDomain;
			DWORD dMainPort = 0;
			std::string sMainDirectory;
			HINTERNET hUserSession;
			std::string sSessionId;
		};
		config_t MainContext_t;
	private:
		void SetupEnvironment();
		void InitializeConfig(std::string sJsonConfigBuffer);
	};
}
