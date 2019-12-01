#pragma once

#include <NTLib.h>

#ifdef __cplusplus
extern "C" {
#endif

extern DECLSPEC_SELECTANY uintptr_t __security_cookie;
extern DECLSPEC_SELECTANY uintptr_t __security_cookie_complement;

void  __fastcall __security_check_cookie(uintptr_t cookie);

#ifdef _M_AMD64

typedef struct _GS_HANDLER_DATA *PGS_HANDLER_DATA;

uintptr_t __GSHandlerCheck(
    PVOID Unknown,
    PVOID EstablisherFrame,
    PCONTEXT ContextRecord,
    PDISPATCHER_CONTEXT DispatcherContext);

EXCEPTION_DISPOSITION __GSHandlerCheck_SEH(
    PEXCEPTION_RECORD ExceptionRecord,
    PVOID EstablisherFrame,
    PCONTEXT ContextRecord,
    PDISPATCHER_CONTEXT DispatcherContext);

#endif

void NtCrtReportGSFailure(uintptr_t cookie);

#ifdef __cplusplus
};
#endif