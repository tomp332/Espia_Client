#include "Configuration.h"


Tool::Configuration::Configuration(std::string jConfigBuffer) {
	InitializeConfig(jConfigBuffer);
	SetupEnvironment();
}

void Tool::Configuration::SetupEnvironment() {

	WCHAR CurrentDir_W[MAX_PATH];
	SHGetSpecialFolderPathW(NULL, CurrentDir_W, CSIDL_LOCAL_APPDATA, false);
	std::wstring wCurrentDir(CurrentDir_W);
	std::string sCurrentDir = std::string(wCurrentDir.begin(), wCurrentDir.end());

	// If products path not specified saving to AppData\UpdateProducts by default
	if (this->MainContext_t.sMainDirectory.empty())
		this->MainContext_t.sMainDirectory = std::format(STR("{}\\UpdateProducts"), sCurrentDir);
	std::wstring wMainDirectory(this->MainContext_t.sMainDirectory.begin(), this->MainContext_t.sMainDirectory.end());
	if (!CreateDirectoryW(wMainDirectory.c_str(), NULL))
	{
		if (GetLastError() != ERROR_ALREADY_EXISTS)
		{
			Tool::Utils::print_error(STR("Error creating environment directory, droping to AppData"));
			MainContext_t.sMainDirectory = sCurrentDir;
		}
	}
}

void Tool::Configuration::InitializeConfig(std::string sJsonStringBuffer) {
	try {
		awesomejson::json jDecoded = awesomejson::json::parse(sJsonStringBuffer);
		this->MainContext_t.sDomain = jDecoded[std::string(STR("Domain"))];
		std::string sPort = jDecoded[std::string(STR("Port"))];
		this->MainContext_t.dMainPort = std::stoi(sPort);
		this->MainContext_t.sMainDirectory = jDecoded[std::string(STR("ProductsPath"))];
		this->MainContext_t.hUserSession = HttpCommunication::HttpRequests::CreateFirstConnection(this->MainContext_t.sDomain, this->MainContext_t.dMainPort); 
		Tool::SystemInfo::GenerateUUID(this->MainContext_t.sSessionId);
	}
	catch (...) {
		std::cout << "Error parsing json config" << std::endl;
		Tool::Utils::print_error(std::format(STR("Json parse for config buffer: {}"), sJsonStringBuffer));
	}
}