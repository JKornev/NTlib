#include "security-check.h"
#include "helper-asm.h"

#define STATUS_SECURITY_CHECK_FAILURE STATUS_STACK_BUFFER_OVERRUN

#ifdef _WIN64
#define DEFAULT_SECURITY_COOKIE 0x00002B992DEFA232
#else
#define DEFAULT_SECURITY_COOKIE 0xBB04E64E
#endif

DECLSPEC_SELECTANY uintptr_t __security_cookie = DEFAULT_SECURITY_COOKIE;
DECLSPEC_SELECTANY uintptr_t __security_cookie_complement = ~(DEFAULT_SECURITY_COOKIE);

static EXCEPTION_RECORD         s_exception_record;
static CONTEXT                  s_exception_context;
static const EXCEPTION_POINTERS s_exception_pointers = {
    &s_exception_record,
    &s_exception_context
};

//TODO: remove this shit
//uintptr_t __GSHandlerCheck = 0;

void report_gs_failure(uintptr_t code);

#if defined (_M_IX86)
void __declspec(naked) __fastcall __security_check_cookie(uintptr_t cookie)
{
    __asm {
        cmp ecx, __security_cookie
        jne failure
        rep ret // REP to avoid AMD branch prediction penalty
    failure :
        push ecx
        call report_gs_failure
    }
}

#else

void __fastcall __security_check_cookie(uintptr_t cookie)
{
    //TODO: implement me, possibly in ASM
    /*  LEAF_ENTRY __security_check_cookie, _TEXT
        cmp rcx, __security_cookie      ; check cookie value in frame
        jne ReportFailure               ; if ne, cookie check failure
        rol rcx, 16                     ; make sure high word is zero
        test cx, -1
        jne RestoreRcx
        db 0f3h                         ; (encode REP for REP RET)
        ret                             ; no overrun, use REP RET to avoid AMD
                                        ; branch prediction flaw after Jcc
;
; The cookie check failed.
;
RestoreRcx:
        ror rcx, 16
ReportFailure:
        jmp __report_gsfailure          ; overrun found
        LEAF_END __security_check_cookie, _TEXT
        end */
}

void __GSHandlerCheckCommon(
    PVOID EstablisherFrame, 
    PDISPATCHER_CONTEXT DispatcherContext, 
    PGS_HANDLER_DATA GSHandlerData)
{
    //TODO: implement me
}

#endif

static void report_gs_failure(uintptr_t cookie)
{
    volatile uintptr_t cookies[2];

    if (NtlUtilIsProcessFeaturePresented(PF_FASTFAIL_AVAILABLE))
        __fastfail(FAST_FAIL_STACK_COOKIE_CHECK_FAILURE);

    cookies[0] = __security_cookie;
    cookies[1] = __security_cookie_complement;

#ifdef _M_IX86
    //TODO: capture context
    capture_context86(&s_exception_context);
    s_exception_context.Ecx = cookie;
#else
    //TODO: capture context
    capture_context64(&s_exception_context);
    s_exception_context.Rcx = cookie;
#endif

    s_exception_record.ExceptionCode = STATUS_SECURITY_CHECK_FAILURE;
    s_exception_record.ExceptionFlags = EXCEPTION_NONCONTINUABLE;
    s_exception_record.NumberParameters = 1;
    s_exception_record.ExceptionInformation[0] = FAST_FAIL_STACK_COOKIE_CHECK_FAILURE;

    RtlRaiseException(&s_exception_record);
    RtlExitUserProcess(STATUS_FAIL_FAST_EXCEPTION);
}
