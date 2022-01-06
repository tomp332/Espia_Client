#pragma once
#include <windows.h>
#include <iostream>
#include <format>
#include <lm.h>
#include <sddl.h>
#pragma comment(lib, "netapi32.lib")

namespace Tool
{
	class SystemInfo {
	public:
		static void GetUserDomain(std::wstring& wUserDomain);
		static void GetCurrentComputerName(std::wstring& wUserComputerName);
		static void GetCurrentUserName(std::wstring& wUsername);
		static void GenerateUUID(std::string& sUUID);
		static std::string GetFallbackUUID();
	};

}
