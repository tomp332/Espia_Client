#include "Edge.h"

Tool::Browsers::Edge::Edge(Tool::Configuration::config_t MainContext_t) :Chromium(MainContext_t) {
	this->sBrowserIdentifier = STR("Edge");
	std::string sAppDataPath;
	Tool::Utils::GetLocalAppDataPath(sAppDataPath);
	this->sBrowserRootPath = std::format(STR("{}\\Microsoft\\Edge\\User Data"), sAppDataPath);
	this->bIsBrowserInstalled = Tool::Utils::IsApplicationInstalled(this->sBrowserRootPath);
	if (!this->bIsBrowserInstalled) {
		Tool::Utils::print(STR("Edge not installed"));
		return;
	}
	Tool::Utils::print(STR("Edge installed"));
	this->CreateBrowserProductDir();
}