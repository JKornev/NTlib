#include "runtime-checks.h"

#if defined (_M_IX86)

void   __cdecl _RTC_CheckEsp()
{
    //TODO: isn't implemented yet
}

#endif

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