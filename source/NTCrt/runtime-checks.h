#pragma once

#include <NTLib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum _RTC_ErrorNumber {
    _RTC_CHKSTK = 0,
    _RTC_CVRT_LOSS_INFO,
    _RTC_CORRUPT_STACK,
    _RTC_UNINIT_LOCAL_USE,
    _RTC_CORRUPTED_ALLOCA,
    _RTC_ILLEGAL
} _RTC_ErrorNumber;

typedef struct _RTC_vardesc {
    int addr;
    int size;
    char *name;
} _RTC_vardesc;

typedef struct _RTC_framedesc {
    int varCount;
    _RTC_vardesc *variables;
} _RTC_framedesc;

#pragma pack(push, 1)
typedef struct _RTC_ALLOCA_NODE {
    __int32 guard1;
    struct _RTC_ALLOCA_NODE *next;
#if defined (_M_IX86) || defined (_M_ARM)
    __int32 dummypad;
#endif
    size_t allocaSize;
#if defined (_M_IX86) || defined (_M_ARM)
    __int32 dummypad2;
#endif
    __int32 guard2[3];
} _RTC_ALLOCA_NODE;
#pragma pack(pop)

void __fastcall _RTC_CheckStackVars(void *_Esp, _RTC_framedesc *_Fd);
void __fastcall _RTC_CheckStackVars2(void *_Esp, _RTC_framedesc *_Fd, _RTC_ALLOCA_NODE *_AllocaList);
void __fastcall _RTC_AllocaHelper(_RTC_ALLOCA_NODE *_PAllocaBase, size_t _CbSize, _RTC_ALLOCA_NODE **_PAllocaInfoList);

void __cdecl _RTC_Failure(void* retaddr, int errnum);

void __cdecl _RTC_Shutdown(void);
void __cdecl _RTC_InitBase(void);

typedef int(__cdecl *_RTC_error_fn)(int, const char *, int, const char *, const char *, ...);
typedef int(__cdecl *_RTC_error_fnW)(int, const wchar_t *, int, const wchar_t *, const wchar_t *, ...);

_RTC_error_fn __cdecl _CRT_RTC_INIT(void *_Res0, void **_Res1, int _Res2, int _Res3, int _Res4);
_RTC_error_fnW __cdecl _CRT_RTC_INITW(void *_Res0, void **_Res1, int _Res2, int _Res3, int _Res4);

#ifdef __cplusplus
};
#endif