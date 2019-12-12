#pragma once

#include <NTLib.h>

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
};
#endif