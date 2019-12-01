#include "runtime-checks.h"


void __fastcall _RTC_CheckStackVars(void *_Esp, _RTC_framedesc *_Fd)
{
    //TODO: isn't implemented yet
}

void __fastcall _RTC_CheckStackVars2(void *_Esp, _RTC_framedesc *_Fd, _RTC_ALLOCA_NODE *_AllocaList)
{
    //TODO: isn't implemented yet
}

void __cdecl _RTC_Shutdown(void)
{
    //TODO: isn't implemented yet
}

void __cdecl _RTC_InitBase(void)
{
    //TODO: isn't implemented yet
}

void __cdecl _RTC_Failure(void* retaddr, int errnum)
{
    ::RtlRaiseStatus(STATUS_STACK_BUFFER_OVERRUN);
    ::ZwTerminateProcess(NtlPsGetCurrentProcess(), 0xBADBAD + errnum);
}
