/*
*
*   Welcome to Hook-Scanner
*   License: https://github.com/GavinCoded/Roblox-Hook-Scanner/blob/main/LICENSE
*   CONTACT: For any questions contact me at contact@gavinstrikes.wtf
*/

#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include <vector>
#include <string>
#include <fstream>
#include <ctime>
#include <chrono>

using namespace std;
using namespace chrono;

struct ModuleInfo {
    wstring moduleName;
    wstring modulePath;
};

vector<ModuleInfo> loadedModules;

void setConsoleColor(WORD color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

void print_time(const char* message, WORD color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE) {
    time_t now = time(0);
    tm timeinfo;
    localtime_s(&timeinfo, &now);
    char timeStr[9]; 
    strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &timeinfo);

    setConsoleColor(FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    printf("[%s] ", timeStr);
    setConsoleColor(color);
    printf("%s\n", message);
}

void printmod(const ModuleInfo& module) {
    time_t now = time(0);
    tm timeinfo;
    localtime_s(&timeinfo, &now);
    char timeStr[9]; 
    strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &timeinfo);

    setConsoleColor(FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    wprintf(L"[%S] ", timeStr);
    setConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    wprintf(L"Module Name: %s, Path: %s\n", module.moduleName.c_str(), module.modulePath.c_str());
}

void listmods(DWORD processId) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        print_time("failed to create module snapshot.", FOREGROUND_RED | FOREGROUND_INTENSITY);
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
        print_time("failed to get first module.", FOREGROUND_RED | FOREGROUND_INTENSITY);
    }

    CloseHandle(hSnapshot);
}

DWORD getpid(const wchar_t* processName) {
    DWORD processId = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        print_time("failed to create process snapshot.", FOREGROUND_RED | FOREGROUND_INTENSITY);
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
        print_time("failed to get first process.", FOREGROUND_RED | FOREGROUND_INTENSITY);
    }

    CloseHandle(hSnapshot);
    return processId;
}

void savefile(const string& filename) {
    ofstream outFile(filename);
    if (outFile.is_open()) {
        time_t now = time(0);
        tm timeinfo;
        localtime_s(&timeinfo, &now);
        char timeStr[9]; 
        strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &timeinfo);
        outFile << "Scan Time: " << timeStr << "\n";
        for (const auto& module : loadedModules) {
            outFile << "Module Name: " << string(module.moduleName.begin(), module.moduleName.end())
                << ", Path: " << string(module.modulePath.begin(), module.modulePath.end()) << "\n";
        }
        outFile.close();
        print_time(("Saved to " + filename).c_str());
    }
    else {
        print_time(("failed to open file " + filename + " for writing.").c_str());
    }
}

int main() {
    SetConsoleTitle(L"Hook-Scanner");
    const wchar_t* processname = L"RobloxPlayerBeta.exe";
    DWORD processpid = getpid(processname);

    if (processpid == 0) {
        print_time("Process Not Found");
        Sleep(3000);
        return 1;
    }

    auto start = high_resolution_clock::now();

    listmods(processpid);

    auto end = high_resolution_clock::now();
    duration<double> elapsed = end - start;

    print_time("Loaded modules of process:");
    for (const auto& module : loadedModules) {
        printmod(module);
    }



    print_time("Do you want to save the module list to a file? (y/n): ");
    char choice;
    cin >> choice;

    if (choice == 'y' || choice == 'Y') {
        print_time("Enter the filename: ");
        string filename;
        cin >> filename;
        savefile(filename);
    }

    print_time("press any key to exit...");
    cin.ignore();
    cin.get();

    return 0;
}
