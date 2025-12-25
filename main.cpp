#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include <shlobj.h>
#include <fstream>
#include <string>
#include <cstdlib>

void systemInfo() {
    SYSTEM_INFO si;
    GetSystemInfo(&si);

    MEMORYSTATUSEX mem;
    mem.dwLength = sizeof(mem);
    GlobalMemoryStatusEx(&mem);

    char username[256];
    DWORD username_len = 256;
    GetUserNameA(username, &username_len);

    char computerName[256];
    DWORD computerName_len = 256;
    GetComputerNameA(computerName, &computerName_len);

    OSVERSIONINFOEXA osvi;
    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEXA));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXA);
    GetVersionExA((OSVERSIONINFOA*)&osvi);

    DWORD uptime_ms = GetTickCount();
    int uptime_sec = uptime_ms / 1000;
    int hours = uptime_sec / 3600;
    int minutes = (uptime_sec % 3600) / 60;
    int seconds = uptime_sec % 60;

    system("cls");
    std::cout << "\n==== System Info ====\n";
    std::cout << "Computer Name: " << computerName << "\n";
    std::cout << "Username: " << username << "\n";
    std::cout << "OS Version: " << osvi.dwMajorVersion << "." << osvi.dwMinorVersion << "\n";
    std::cout << "Architecture: " << (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ? "x64" : "x86") << "\n";
    std::cout << "CPU Cores: " << si.dwNumberOfProcessors << "\n";
    std::cout << "RAM: " << mem.ullTotalPhys / (1024 * 1024) << " MB\n";
    std::cout << "Uptime: " << hours << "h " << minutes << "m " << seconds << "s\n";
    system("pause");
    system("cls");
}

void listProcesses() {
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(pe);

    std::cout << "\nPID\tProcess\n";
    if (Process32First(snap, &pe)) {
        do {
            std::wcout << pe.th32ProcessID << L"\t" << pe.szExeFile << L"\n";
        } while (Process32Next(snap, &pe));
    }
    CloseHandle(snap);
}

void killProcess() {
    int pid;
    std::cout << "PID : ";
    std::cin >> pid;

    HANDLE h = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if (h) {
        TerminateProcess(h, 0);
        CloseHandle(h);
        std::cout << "Process killed\n";
    } else {
        std::cout << "Failed to kill process\n";
    }
}

void deleteFolder(const std::string& folderPath) {
    WIN32_FIND_DATAA fd;
    HANDLE hFind = FindFirstFileA((folderPath + "\\*").c_str(), &fd);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            std::string filePath = folderPath + "\\" + fd.cFileName;
            if (strcmp(fd.cFileName, ".") == 0 || strcmp(fd.cFileName, "..") == 0)
                continue;
            if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                deleteFolder(filePath);
                RemoveDirectoryA(filePath.c_str());
            } else {
                DeleteFileA(filePath.c_str());
            }
        } while (FindNextFileA(hFind, &fd));
        FindClose(hFind);
    }
}

void cleanTemp() {
    char tempPath[MAX_PATH];
    GetTempPathA(MAX_PATH, tempPath);

    std::cout << "Cleaning " << tempPath << "\n";
    deleteFolder(tempPath);

    std::cout << "Cleaning C:\\Windows\\Temp\n";
    deleteFolder("C:\\Windows\\Temp");

    std::cout << "Cleaning Prefetch\n";
    deleteFolder("C:\\Windows\\Prefetch");

    std::cout << "Running Disk Cleanup\n";
    system("cleanmgr /sagerun:1");

    std::cout << "Temp cleaning finished!\n";
}

void credits() {
    system("cls");
    std::cout << "Developed by x.skuno\nhttps://salvatorerusso.xyz\nhttps://github.com/skun0\n";
    system("pause");
    system("cls");
}
int main() {
    int choice;

    while (true) {
        std::cout << "\n==== System Utility ====\n";
        std::cout << "1. System Info\n";
        std::cout << "2. List Processes\n";
        std::cout << "3. Kill Process\n";
        std::cout << "4. Clean Temp Files\n";
        std::cout << "5. Credits\n";
        std::cout << "0. Exit\n";
        std::cout << ":";

        std::cin >> choice;

        switch (choice) {
        case 1: systemInfo(); break;
        case 2: listProcesses(); break;
        case 3: killProcess(); break;
        case 4: cleanTemp(); break;
        case 5: credits(); break;
        case 0: return 0;
        default: std::cout << "Invalid input\n";
        }
    }
}
