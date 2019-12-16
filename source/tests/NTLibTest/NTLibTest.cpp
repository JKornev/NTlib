#include <Windows.h>
#include <stdio.h>
#include <assert.h>
#include <NTLib.h>

#pragma comment(lib, "NTLib.lib")
#pragma comment(lib, "ntdll.lib")

void TestNtlPsRoutines()
{
    HANDLE process;
    printf("Running process routines tests\n");
    NTLIB_ASSERT(NT_SUCCESS(NtlPsOpenProcess(&process, ::GetCurrentProcessId(), PROCESS_QUERY_LIMITED_INFORMATION)));
    NTLIB_ASSERT(NT_ERROR(NtlPsOpenProcess(&process, 0, PROCESS_QUERY_LIMITED_INFORMATION)));
    printf("passed\n");
}

int wmain(int argc, wchar_t* argv[])
{
    TestNtlPsRoutines();
    return 0;
}
