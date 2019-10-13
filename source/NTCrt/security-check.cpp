#include "security-check.h"
#include "helper-asm.h"

#define STATUS_SECURITY_CHECK_FAILURE STATUS_STACK_BUFFER_OVERRUN

#ifdef _WIN64
#define DEFAULT_SECURITY_COOKIE 0x00002B992DEFA232
#else
#define DEFAULT_SECURITY_COOKIE 0xBB04E64E
#endif

DECLSPEC_SELECTANY UINT_PTR __security_cookie = DEFAULT_SECURITY_COOKIE;
DECLSPEC_SELECTANY UINT_PTR __security_cookie_complement = ~(DEFAULT_SECURITY_COOKIE);

static EXCEPTION_RECORD         s_exception_record;
static CONTEXT                  s_exception_context;
static const EXCEPTION_POINTERS s_exception_pointers = {
    &s_exception_record,
    &s_exception_context
};

void report_gs_failure(uintptr_t code);

#if defined (_M_IX86)
void __declspec(naked) __fastcall __security_check_cookie(UINT_PTR cookie)
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
#endif

static void report_gs_failure(uintptr_t cookie)
{
    volatile UINT_PTR cookies[2];

    if (NtlKusdIsProcessFeaturePresented(PF_FASTFAIL_AVAILABLE))
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
