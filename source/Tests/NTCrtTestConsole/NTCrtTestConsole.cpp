#include <NTLib.h>

int wmain(int argc, wchar_t* argv[])
{
    HANDLE process;
    NtlPsOpenProcess(&process, ::GetCurrentProcessId(), PROCESS_QUERY_LIMITED_INFORMATION);
    return 0;
}