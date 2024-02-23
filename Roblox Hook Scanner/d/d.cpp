// This code is garbage enjoy!
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

void ListLoadedModules(DWORD processId) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId);
    if (hSnapshot != INVALID_HANDLE_VALUE) {
        MODULEENTRY32 moduleEntry;
        moduleEntry.dwSize = sizeof(MODULEENTRY32);
        if (Module32First(hSnapshot, &moduleEntry)) {
            do {
                loadedModules.push_back({ moduleEntry.szModule, moduleEntry.szExePath });
            } while (Module32Next(hSnapshot, &moduleEntry));
        }
        CloseHandle(hSnapshot);
    }
}

int main() {
    const wchar_t* robloxProcessName = L"RobloxPlayerBeta.exe";
    DWORD robloxProcessId = 0;


    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32 processEntry;
        processEntry.dwSize = sizeof(PROCESSENTRY32);
        if (Process32First(hSnapshot, &processEntry)) {
            do {
                if (_wcsicmp(processEntry.szExeFile, robloxProcessName) == 0) {
                    robloxProcessId = processEntry.th32ProcessID;
                    break;
                }
            } while (Process32Next(hSnapshot, &processEntry));
        }
        CloseHandle(hSnapshot);
    }

    if (robloxProcessId == 0) {
        cerr << "Failed to find the Roblox process." << endl;
        return 1;
    }

    ListLoadedModules(robloxProcessId);

    cout << "Loaded modules of Roblox process:" << endl;
    for (const auto& module : loadedModules) {
        wcout << L"Module Name: " << module.moduleName << L", Path: " << module.modulePath << endl;
    }


    cout << "Press any key to exit..." << endl;
    cin.get();

    return 0;
}
