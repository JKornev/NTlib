#define _CRT_SECURE_NO_WARNINGS
#include <NTLib.h>
#include <stdio.h>
#include <assert.h>
#include <malloc.h>

/*TODO:
- Enable SAFE-SEH for Win32->Release
*/

#pragma comment(lib, "ntdll.lib")

// 
// test_runtimechecks()
//
//   Stack runtime check testing.
//      _RTC_CheckEsp, _RTC_CheckStackVars

#pragma runtime_checks("s", restore)
void test_runtimechecks()
{
    const int bufferSize = 0x60000;
    char* buffer = reinterpret_cast<char*>(_alloca(bufferSize));
    //call __chkstk
    //call _RTC_AllocaHelper
    ULONG written = 0;
    auto status = NtQuerySystemInformation(SystemProcessInformation, buffer, bufferSize, &written);
    //call _RTC_CheckEsp()
    RTL_ASSERTMSG("Validate status to be sure _RTC_CheckEsp() didn't corrupt a EAX/RAX", status == STATUS_SUCCESS);
    //call _RTC_CheckEsp()
    //call _RTC_CheckStackVars2()
}
#pragma runtime_checks("", restore)

int wmain(int argc, wchar_t* argv[])
{
    test_runtimechecks();

    char buffer[0x60000];
    ULONG written = 0;

    auto statys = NtQuerySystemInformation(SystemProcessInformation, buffer, sizeof(buffer), &written);

    sprintf(buffer, "lolka%d", 1);
    return 0;
}