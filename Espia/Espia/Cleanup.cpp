#include "Cleanup.h"

void Tool::Cleanup::CleanAll(std::string& sMainProductsDir) {
	Tool::Utils::RemoveDirTree(sMainProductsDir);
	Tool::Cleanup::CleanupTool();
}

void Tool::Cleanup::CleanupTool() {
	std::string sToolPath;
	Tool::Utils::GetCurrentFileName(sToolPath);
	STARTUPINFO info = { sizeof(info) };
	PROCESS_INFORMATION processInfo;
	std::string params = std::format(STR("-c Start-Sleep -s 0.5;rm {}"), sToolPath);
	std::string sApp = STR("C:\\Windows\\System32\\WindowsPowerShell\\v1.0\\powershell.exe");
	std::wstring wParams(params.begin(), params.end());
	std::wstring wApp(sApp.begin(), sApp.end());
	bool openedProcess = CreateProcessW(wApp.c_str(), (LPWSTR)wParams.c_str(), NULL, NULL, TRUE, NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW | CREATE_NEW_PROCESS_GROUP, NULL, NULL, &info, &processInfo);
	if (!openedProcess){
		Tool::Utils::print_error(STR("Unable to remove tool file"));
	}
	Tool::Utils::print(STR("Launched process to remove tool"));
	CloseHandle(processInfo.hProcess);
	CloseHandle(processInfo.hThread);
}