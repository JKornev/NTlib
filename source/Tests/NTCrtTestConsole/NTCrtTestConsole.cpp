#include <NTLib.h>

int wmain(int argc, wchar_t* argv[])
{
    HANDLE process;
    NtlPsOpenProcess(&process, ::GetCurrentProcessId(), PROCESS_QUERY_LIMITED_INFORMATION);
    return 0;
}
/*
int __stdcall wmainCRTStartup(void)
{
    wmain(0, 0);
    return 0;
}*/


//#pragma comment(linker, "/ENTRY:DllMainEntry")

BOOL WINAPI DllMainEntry(HMODULE hmodule, DWORD reason, LPVOID reserved)
{
    switch (reason) {
    case DLL_PROCESS_ATTACH:
    case DLL_PROCESS_DETACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    };

    return TRUE;
}