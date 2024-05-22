/*
*
*   Welcome to Hook-Scanner 
*	License: https://github.com/GavinCoded/Roblox-Hook-Scanner/blob/main/LICENSE
*   CONTACT: For any questions contact me at contact@gavinstrikes.wtf
*/

#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include <vector>
#include <string>

using namespace std;

struct ModuleInfo {
    wstring moduleName;
    wstring modulePath;
};

vector<ModuleInfo> loadedModules;

void listmods(DWORD processId) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        printf("failed to create module snapshot. %lu\n", GetLastError());
        return;
    }

    MODULEENTRY32 moduleEntry;
    moduleEntry.dwSize = sizeof(MODULEENTRY32);
    if (Module32First(hSnapshot, &moduleEntry)) {
        do {
            loadedModules.push_back({ moduleEntry.szModule, moduleEntry.szExePath });
        } while (Module32Next(hSnapshot, &moduleEntry));
    }
    else {
        printf("failed to get first module. %lu\n", GetLastError());
    }

    CloseHandle(hSnapshot);
}

DWORD getpid(const wchar_t* processName) {
    DWORD processId = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        printf("failed to create process snapshot. %lu\n", GetLastError());
        return 0;
    }

    PROCESSENTRY32 processEntry;
    processEntry.dwSize = sizeof(PROCESSENTRY32);
    if (Process32First(hSnapshot, &processEntry)) {
        do {
            if (_wcsicmp(processEntry.szExeFile, processName) == 0) {
                processId = processEntry.th32ProcessID;
                break;
            }
        } while (Process32Next(hSnapshot, &processEntry));
    }
    else {
        printf("failed to get first process. %lu\n", GetLastError());
    }

    CloseHandle(hSnapshot);
    return processId;
}

int main() {
    const wchar_t* processname = L"RobloxPlayerBeta.exe";
    DWORD processpid = getpid(processname);

    if (processpid == 0) {
        printf("Process Not Found\n");
        Sleep(3000);
        return 1;
    }

    listmods(processpid);

    printf("Loaded modules of process:\n");
    for (const auto& module : loadedModules) {
        wprintf(L"Module Name: %s, Path: %s\n", module.moduleName.c_str(), module.modulePath.c_str());
    }

    printf("Press any key to exit...\n");
    cin.get();

    return 0;
}
