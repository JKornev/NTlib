#include "runtime-checks.h"
#include <intrin.h>

void __fastcall _RTC_CheckStackVars(void *_Esp, _RTC_framedesc *_Fd)
{
    int i, *guard1, *guard2;

    for (i = 0; i < _Fd->varCount; i++)
    {
        guard1 = (int*)((char*)_Esp + _Fd->variables[i].addr - sizeof(*guard1));
        guard2 = (int*)((char*)_Esp + _Fd->variables[i].addr + _Fd->variables[i].size);

        if ((*guard1 != 0xCCCCCCCC) || (*guard2 != 0xCCCCCCCC))
            _RTC_Failure(_ReturnAddress(), _RTC_ErrorNumber::_RTC_CORRUPT_STACK);
    }
}

void __fastcall _RTC_CheckStackVars2(void *_Esp, _RTC_framedesc *_Fd, _RTC_ALLOCA_NODE *_AllocaList)
{
    _RTC_ALLOCA_NODE *current;
    int *guard;

    _RTC_CheckStackVars(_Esp, _Fd);

    for (current = _AllocaList; current != 0; current = current->next)
    {
        guard = (int*)((char*)current + current->allocaSize - sizeof(*guard));

        if ((current->guard1 != 0xCCCCCCCC) ||
            (current->guard2[0] != 0xCCCCCCCC) ||
            (current->guard2[1] != 0xCCCCCCCC) ||
            (current->guard2[2] != 0xCCCCCCCC) ||
            (*guard != 0xCCCCCCCC)
        )
        _RTC_Failure(_ReturnAddress(), _RTC_ErrorNumber::_RTC_CORRUPT_STACK);
    }
}

void __fastcall _RTC_AllocaHelper(_RTC_ALLOCA_NODE *_PAllocaBase, size_t _CbSize, _RTC_ALLOCA_NODE **_PAllocaInfoList)
{
    if (_PAllocaBase && _CbSize && _PAllocaInfoList)
    {
        memset(_PAllocaBase, 0xCC, _CbSize);

        _PAllocaBase->allocaSize = _CbSize;
        _PAllocaBase->next = *_PAllocaInfoList;
        *_PAllocaInfoList = _PAllocaBase;
    }
}


void __cdecl _RTC_Shutdown(void)
{ // Stub
    NTLIB_DBG_BREAK;
}

void __cdecl _RTC_InitBase(void)
{ // Stub
    NTLIB_DBG_BREAK;
}

void __cdecl _RTC_Failure(void* retaddr, int errnum)
{
    static void* s_rtc_retaddr = retaddr;
    static int s_rtc_errnum = errnum;
    ::RtlRaiseStatus(STATUS_STACK_BUFFER_OVERRUN);
    ::ZwTerminateProcess(NtlPsGetCurrentProcess(), 0xBADBAD + s_rtc_errnum);
}

_RTC_error_fn __cdecl _CRT_RTC_INIT(void *_Res0, void **_Res1, int _Res2, int _Res3, int _Res4)
{
    NTLIB_DBG_BREAK;
    return 0;
}

_RTC_error_fnW __cdecl _CRT_RTC_INITW(void *_Res0, void **_Res1, int _Res2, int _Res3, int _Res4)
{
    NTLIB_DBG_BREAK;
    return 0;
}
