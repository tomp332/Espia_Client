#include "SystemInfo.h"

void Tool::SystemInfo::GetUserDomain(std::wstring& wUserDomain)
{
    DWORD dwLevel = 102;
    LPWKSTA_INFO_102 pBuf = NULL;
    NET_API_STATUS nStatus;
    LPWSTR pszServerName = NULL;
    nStatus = NetWkstaGetInfo(pszServerName,
        dwLevel,
        (LPBYTE*)&pBuf);
    if (nStatus == NERR_Success)
        wUserDomain = pBuf->wki102_langroup;
}

void Tool::SystemInfo::GetCurrentComputerName(std::wstring& wUserComputerName)
{
    wchar_t buffer[MAX_COMPUTERNAME_LENGTH + 1] = { 0 };
    DWORD cchBufferSize = sizeof(buffer) / sizeof(buffer[0]);
    if (!GetComputerNameW(buffer, &cchBufferSize))
        throw std::runtime_error("GetComputerName() failed.");
    wUserComputerName = &buffer[0];
}

void Tool::SystemInfo::GetCurrentUserName(std::wstring& wUsername)
{
    WCHAR username[UNLEN + 1];
    DWORD username_len = UNLEN + 1;
    GetUserNameW(username, &username_len);
    wUsername = username;
}

std::string Tool::SystemInfo::GetFallbackUUID() {
    std::wstring wTempUserName;
    Tool::SystemInfo::GetCurrentUserName(wTempUserName);
    return std::format("x{}192mj-asj2", std::string(wTempUserName.begin(), wTempUserName.end()));
}

void Tool::SystemInfo::GenerateUUID(std::string& sUUID) {
    UUID uuid;
    std::string sTempUuid;
    char* sUuid;
    if (UuidCreate(&uuid) != RPC_S_OK) {
        sUUID = GetFallbackUUID();
        return;
    }
    if (UuidToStringA(&uuid, (RPC_CSTR*)&sUuid) != RPC_S_OK) {
        sUUID = GetFallbackUUID();
        return;
    }
    sUUID = sUuid;
    RpcStringFreeA((RPC_CSTR*)&sUuid);
}