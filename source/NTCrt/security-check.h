#pragma once

#include <NTLib.h>

#ifdef __cplusplus
extern "C" {
#endif

extern DECLSPEC_SELECTANY uintptr_t __security_cookie;
extern DECLSPEC_SELECTANY uintptr_t __security_cookie_complement;

extern void  __fastcall __security_check_cookie(uintptr_t cookie);

#ifdef _M_AMD64

typedef struct _GS_HANDLER_DATA *PGS_HANDLER_DATA;

//TODO: remove dllexport
extern void _declspec(dllexport) __GSHandlerCheckCommon(
    PVOID EstablisherFrame,
    PDISPATCHER_CONTEXT DispatcherContext,
    PGS_HANDLER_DATA GSHandlerData
);

#endif

#ifdef __cplusplus
};
#endif