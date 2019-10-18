#define _CRT_SECURE_NO_WARNINGS
#include <NTLib.h>
#include <stdio.h>

/*TODO:
- Enable SAFE-SEH for Win32->Release
*/

int wmain(int argc, wchar_t* argv[])
{
    EXCEPTION_RECORD a = {0};
    HANDLE process;
    NtlPsOpenProcess(&process, ::GetCurrentProcessId(), PROCESS_QUERY_LIMITED_INFORMATION);
    NtlUtilIsProcessFeaturePresented(PF_FASTFAIL_AVAILABLE);
    memset(&process, 0, 4);

    char buffer[100] = {0};
    sprintf(buffer, "lolka%d", 1);
    return 0;
}