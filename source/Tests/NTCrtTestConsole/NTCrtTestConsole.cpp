#define _CRT_SECURE_NO_WARNINGS
#include <NTLib.h>
#include <stdio.h>
#include <assert.h>

/*TODO:
- Enable SAFE-SEH for Win32->Release
*/

#define NTLIB_ASSERT(x) if (!(x)) DbgBreakPoint()


// Disable optimization for the right test results
#pragma optimize("", off)

//TOOD: delete me
//#pragma runtime_checks( "", off )


// Stack runtime check testing.
//  _RTC_CheckEsp, _RTC_CheckStackVars

//#pragma runtime_checks("s", restore)
void test_runtimechecks_stack()
{
    char buffer[0x60000];
    ULONG written = 0;
    auto status = NtQuerySystemInformation(SystemProcessInformation, buffer, sizeof(buffer), &written);
    //call _RTC_CheckEsp()
    RTL_ASSERTMSG("Validate status to be sure _RTC_CheckEsp() didn't corrupt a EAX/RAX", status == STATUS_SUCCESS);
    //call _RTC_CheckEsp()
    //call _RTC_CheckStackVars()
}
//#pragma runtime_checks("", restore)


void test_runtimechecks()
{
    test_runtimechecks_stack();
}

// Turn back a default optimization options
#pragma optimize("", on)

int wmain(int argc, wchar_t* argv[])
{
    /*char buffer[100] = {0};

    EXCEPTION_RECORD a = {0};
    HANDLE process;
    __try
    {
        NtlPsOpenProcess(&process, ::GetCurrentProcessId(), PROCESS_QUERY_LIMITED_INFORMATION);
        NtlUtilIsProcessFeaturePresented(PF_FASTFAIL_AVAILABLE);

        memset(&buffer, 0, 101);
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {

    }*/

    test_runtimechecks();

    char buffer[0x60000];
    ULONG written = 0;

    auto statys = NtQuerySystemInformation(SystemProcessInformation, buffer, sizeof(buffer), &written);

    sprintf(buffer, "lolka%d", 1);
    return 0;
}