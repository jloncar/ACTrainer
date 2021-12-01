#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <fstream>

#define LOGMSG(msg) std::wcout << msg << std::endl
#define LOGERR(err) std::wcout << err << std::endl;\
                    system("pause");\
                    return 1

DWORD GetProcessId(const wchar_t* exeName)
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    PROCESSENTRY32W pe;
    pe.dwSize = sizeof(PROCESSENTRY32W);

    if (!Process32FirstW(hSnapshot, &pe)) {
        LOGMSG("Unable to open processlist.");
        return 0;
    }

    while (Process32NextW(hSnapshot, &pe))
    {
        if (_wcsicmp(exeName, pe.szExeFile) == 0)
        {
            LOGMSG("Process found.");
            CloseHandle(hSnapshot);
            return pe.th32ProcessID;
        }
    }
    CloseHandle(hSnapshot);
    return 0;
}


std::wstring getExePath() {
    TCHAR buffer[MAX_PATH] = { 0 };
    GetModuleFileName(NULL, buffer, MAX_PATH);
    std::string::size_type pos = std::wstring(buffer).find_last_of(L"\\/");
    return std::wstring(buffer).substr(0, pos);
}

bool fileExists(std::wstring filePath) {
    std::ifstream f;
    f.open(filePath);
    bool result = f.is_open();
    f.close();
    return result;
}


int main()
{
    DWORD processId;
    std::wstring dllPath = getExePath() + L"\\Trainer.dll";
    SIZE_T dllPathSize = dllPath.size() * sizeof(wchar_t);

    if (!fileExists(dllPath))
    {
        LOGERR("DLL NOT FOUND: " << dllPath);
    }

    do {
        std::cout << "Waiting for game to load...\r";
        processId = GetProcessId(L"ac_client.exe");
        Sleep(300);
    } while (processId == 0);

    std::cout << std::endl;

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, processId);
    if (hProcess == 0)
    {
        LOGERR("Unable to open process.");
    }

    LPVOID dllPathAddr = VirtualAllocEx(hProcess, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (dllPathAddr == 0)
    {
        LOGERR("Unable to allocate space for dll path.");
    }

    if (!WriteProcessMemory(hProcess, dllPathAddr, dllPath.c_str(), dllPathSize, 0))
    {
        LOGERR("Unable to write to process memory.");
    }
    
    HANDLE hThread = CreateRemoteThread(hProcess, 0, 0, (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleW(L"Kernel32.dll"), "LoadLibraryW"), dllPathAddr, 0, 0);

    if (hThread == 0)
    {
        LOGERR("Unable to spawn thread.");
    }

    // Should I free memory allocated by VirtualAllocEx ??

    CloseHandle(hThread);
    CloseHandle(hProcess);
}
