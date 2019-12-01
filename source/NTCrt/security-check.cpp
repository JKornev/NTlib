#include "security-check.h"
#include "helper-asm.h"

// ========================
//  Security cookie

#define STATUS_SECURITY_CHECK_FAILURE STATUS_STACK_BUFFER_OVERRUN

#ifdef _WIN64
#define DEFAULT_SECURITY_COOKIE 0x00002B992DEFA232
#else
#define DEFAULT_SECURITY_COOKIE 0xBB04E64E
#endif

DECLSPEC_SELECTANY uintptr_t __security_cookie = DEFAULT_SECURITY_COOKIE;
DECLSPEC_SELECTANY uintptr_t __security_cookie_complement = ~(DEFAULT_SECURITY_COOKIE);

// ========================
//  GS failure vars

static EXCEPTION_RECORD         s_exception_record;
static CONTEXT                  s_exception_context;
static const EXCEPTION_POINTERS s_exception_pointers = {
    &s_exception_record,
    &s_exception_context
};

// ========================
//  Internal declaration

#if defined (_M_AMD64)
void NtCrtGSHandlerCheckCommon(
    PVOID EstablisherFrame, 
    PDISPATCHER_CONTEXT DispatcherContext, 
    PGS_HANDLER_DATA GSHandlerData);
#endif

// ========================

#if defined (_M_IX86)
void __declspec(naked) __fastcall __security_check_cookie(uintptr_t cookie)
{
    __asm {
        cmp ecx, __security_cookie
        jne failure
        rep ret // REP to avoid AMD branch prediction penalty
    failure :
        push ecx
        call NtCrtReportGSFailure
    }
}

#else

uintptr_t __GSHandlerCheck(
    PVOID Unknown, 
    PVOID EstablisherFrame,
    PCONTEXT ContextRecord, 
    PDISPATCHER_CONTEXT DispatcherContext)
{
    NtCrtGSHandlerCheckCommon(EstablisherFrame, DispatcherContext, (PGS_HANDLER_DATA)DispatcherContext->HandlerData);
    return 1;
}

EXCEPTION_DISPOSITION __GSHandlerCheck_SEH(
    PEXCEPTION_RECORD ExceptionRecord,
    PVOID EstablisherFrame,
    PCONTEXT ContextRecord,
    PDISPATCHER_CONTEXT DispatcherContext)
{
    PGS_HANDLER_DATA GSHandlerData;
    ULONG GSUnwindInfo;
    EXCEPTION_DISPOSITION Disposition;

    //
    // Retrieve a pointer to the start of that part of the handler data used
    // to locate the local security cookie in the local frame.  That is found
    // following the image-relative offset to the FuncInfo table used by
    // __CxxFrameHandler3.
    //

    GSHandlerData = (PGS_HANDLER_DATA)((PULONG)DispatcherContext->HandlerData + 1);

    //
    // Perform the actual cookie check.
    //

    NtCrtGSHandlerCheckCommon(
        EstablisherFrame,
        DispatcherContext,
        GSHandlerData
    );

    //
    // If the cookie check succeeds, call the normal C++ EH handler if we're
    // supposed to on this exception pass.  Find the EHANDLER/UHANDLER flags
    // controlling that in the first ULONG of our part of the handler data.
    //

    GSUnwindInfo = *(PULONG)GSHandlerData;
    if (IS_DISPATCHING(ExceptionRecord->ExceptionFlags)
        ? (GSUnwindInfo & UNW_FLAG_EHANDLER)
        : (GSUnwindInfo & UNW_FLAG_UHANDLER))
    {
        /*Disposition = __CxxFrameHandler3(
            ExceptionRecord,
            EstablisherFrame,
            ContextRecord,
            DispatcherContext
            );*/
        Disposition = ExceptionContinueSearch;
    }
    else
    {
        Disposition = ExceptionContinueSearch;
    }

    return Disposition;
}

#endif

#if defined (_M_AMD64)
static void NtCrtGSHandlerCheckCommon(
    PVOID EstablisherFrame, 
    PDISPATCHER_CONTEXT DispatcherContext, 
    PGS_HANDLER_DATA GSHandlerData)
{
    //TODO: implement me
}
#endif

void NtCrtReportGSFailure(uintptr_t cookie)
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
