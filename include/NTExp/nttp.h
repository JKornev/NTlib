/*
 * This file is part of the Process Hacker project - https://processhacker.sf.io/ 
 *
 * You can redistribute this file and/or modify it under the terms of the 
 * Attribution 4.0 International (CC BY 4.0) license. 
 * 
 * You must give appropriate credit, provide a link to the license, and 
 * indicate if changes were made. You may do so in any reasonable manner, but 
 * not in any way that suggests the licensor endorses you or your use.
 */

#ifndef _NTTP_H
#define _NTTP_H

// Some types are already defined in winnt.h.

typedef struct _TP_ALPC TP_ALPC, *PTP_ALPC;

// private
typedef VOID (NTAPI *PTP_ALPC_CALLBACK)(
    _Inout_ PTP_CALLBACK_INSTANCE Instance,
    _Inout_opt_ PVOID Context,
    _In_ PTP_ALPC Alpc
    );

// rev
typedef VOID (NTAPI *PTP_ALPC_CALLBACK_EX)(
    _Inout_ PTP_CALLBACK_INSTANCE Instance,
    _Inout_opt_ PVOID Context,
    _In_ PTP_ALPC Alpc,
    _In_ PVOID ApcContext
    );

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_VISTA)

// private
_Check_return_
NTDLL_API(NTSTATUS, __stdcall, TpAllocPool, (
    _Out_ PTP_POOL *PoolReturn,
    _Reserved_ PVOID Reserved
))

// winbase:CloseThreadpool
NTDLL_API_VOID(__stdcall, TpReleasePool, (
    _Inout_ PTP_POOL Pool
))

// winbase:SetThreadpoolThreadMaximum
NTDLL_API_VOID(__stdcall, TpSetPoolMaxThreads, (
    _Inout_ PTP_POOL Pool,
    _In_ LONG MaxThreads
))

// private
NTDLL_API(NTSTATUS, __stdcall, TpSetPoolMinThreads, (
    _Inout_ PTP_POOL Pool,
    _In_ LONG MinThreads
))

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_7)
// rev
NTDLL_API(NTSTATUS, __stdcall, TpQueryPoolStackInformation, (
    _In_ PTP_POOL Pool,
    _Out_ PTP_POOL_STACK_INFORMATION PoolStackInformation
))
#endif

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_7)
// rev
NTDLL_API(NTSTATUS, __stdcall, TpSetPoolStackInformation, (
    _Inout_ PTP_POOL Pool,
    _In_ PTP_POOL_STACK_INFORMATION PoolStackInformation
))
#endif

// private
_Check_return_
NTDLL_API(NTSTATUS, __stdcall, TpAllocCleanupGroup, (
    _Out_ PTP_CLEANUP_GROUP *CleanupGroupReturn
))

// winbase:CloseThreadpoolCleanupGroup
NTDLL_API_VOID(__stdcall, TpReleaseCleanupGroup, (
    _Inout_ PTP_CLEANUP_GROUP CleanupGroup
))

// winbase:CloseThreadpoolCleanupGroupMembers
NTDLL_API_VOID(__stdcall, TpReleaseCleanupGroupMembers, (
    _Inout_ PTP_CLEANUP_GROUP CleanupGroup,
    _In_ LOGICAL CancelPendingCallbacks,
    _Inout_opt_ PVOID CleanupParameter
))

// winbase:SetEventWhenCallbackReturns
NTDLL_API_VOID(__stdcall, TpCallbackSetEventOnCompletion, (
    _Inout_ PTP_CALLBACK_INSTANCE Instance,
    _In_ HANDLE Event
))

// winbase:ReleaseSemaphoreWhenCallbackReturns
NTDLL_API_VOID(__stdcall, TpCallbackReleaseSemaphoreOnCompletion, (
    _Inout_ PTP_CALLBACK_INSTANCE Instance,
    _In_ HANDLE Semaphore,
    _In_ LONG ReleaseCount
))

// winbase:ReleaseMutexWhenCallbackReturns
NTDLL_API_VOID(__stdcall, TpCallbackReleaseMutexOnCompletion, (
    _Inout_ PTP_CALLBACK_INSTANCE Instance,
    _In_ HANDLE Mutex
))

// winbase:LeaveCriticalSectionWhenCallbackReturns
NTDLL_API_VOID(__stdcall, TpCallbackLeaveCriticalSectionOnCompletion, (
    _Inout_ PTP_CALLBACK_INSTANCE Instance,
    _Inout_ PRTL_CRITICAL_SECTION CriticalSection
))

// winbase:FreeLibraryWhenCallbackReturns
NTDLL_API_VOID(__stdcall, TpCallbackUnloadDllOnCompletion, (
    _Inout_ PTP_CALLBACK_INSTANCE Instance,
    _In_ PVOID DllHandle
))

// winbase:CallbackMayRunLong
NTDLL_API(NTSTATUS, __stdcall, TpCallbackMayRunLong, (
    _Inout_ PTP_CALLBACK_INSTANCE Instance
))

// winbase:DisassociateCurrentThreadFromCallback
NTDLL_API_VOID(__stdcall, TpDisassociateCallback, (
    _Inout_ PTP_CALLBACK_INSTANCE Instance
))

// winbase:TrySubmitThreadpoolCallback
_Check_return_
NTDLL_API(NTSTATUS, __stdcall, TpSimpleTryPost, (
    _In_ PTP_SIMPLE_CALLBACK Callback,
    _Inout_opt_ PVOID Context,
    _In_opt_ PTP_CALLBACK_ENVIRON CallbackEnviron
))

// private
_Check_return_
NTDLL_API(NTSTATUS, __stdcall, TpAllocWork, (
    _Out_ PTP_WORK *WorkReturn,
    _In_ PTP_WORK_CALLBACK Callback,
    _Inout_opt_ PVOID Context,
    _In_opt_ PTP_CALLBACK_ENVIRON CallbackEnviron
))

// winbase:CloseThreadpoolWork
NTDLL_API_VOID(__stdcall, TpReleaseWork, (
    _Inout_ PTP_WORK Work
))

// winbase:SubmitThreadpoolWork
NTDLL_API_VOID(__stdcall, TpPostWork, (
    _Inout_ PTP_WORK Work
))

// winbase:WaitForThreadpoolWorkCallbacks
NTDLL_API_VOID(__stdcall, TpWaitForWork, (
    _Inout_ PTP_WORK Work,
    _In_ LOGICAL CancelPendingCallbacks
))

// private
_Check_return_
NTDLL_API(NTSTATUS, __stdcall, TpAllocTimer, (
    _Out_ PTP_TIMER *Timer,
    _In_ PTP_TIMER_CALLBACK Callback,
    _Inout_opt_ PVOID Context,
    _In_opt_ PTP_CALLBACK_ENVIRON CallbackEnviron
))

// winbase:CloseThreadpoolTimer
NTDLL_API_VOID(__stdcall, TpReleaseTimer, (
    _Inout_ PTP_TIMER Timer
))

// winbase:SetThreadpoolTimer
NTDLL_API_VOID(__stdcall, TpSetTimer, (
    _Inout_ PTP_TIMER Timer,
    _In_opt_ PLARGE_INTEGER DueTime,
    _In_ LONG Period,
    _In_opt_ LONG WindowLength
))

// winbase:IsThreadpoolTimerSet
NTDLL_API(LOGICAL, __stdcall, TpIsTimerSet, (
    _In_ PTP_TIMER Timer
))

// winbase:WaitForThreadpoolTimerCallbacks
NTDLL_API_VOID(__stdcall, TpWaitForTimer, (
    _Inout_ PTP_TIMER Timer,
    _In_ LOGICAL CancelPendingCallbacks
))

// private
_Check_return_
NTDLL_API(NTSTATUS, __stdcall, TpAllocWait, (
    _Out_ PTP_WAIT *WaitReturn,
    _In_ PTP_WAIT_CALLBACK Callback,
    _Inout_opt_ PVOID Context,
    _In_opt_ PTP_CALLBACK_ENVIRON CallbackEnviron
))

// winbase:CloseThreadpoolWait
NTDLL_API_VOID(__stdcall, TpReleaseWait, (
    _Inout_ PTP_WAIT Wait
))

// winbase:SetThreadpoolWait
NTDLL_API_VOID(__stdcall, TpSetWait, (
    _Inout_ PTP_WAIT Wait,
    _In_opt_ HANDLE Handle,
    _In_opt_ PLARGE_INTEGER Timeout
))

// winbase:WaitForThreadpoolWaitCallbacks
NTDLL_API_VOID(__stdcall, TpWaitForWait, (
    _Inout_ PTP_WAIT Wait,
    _In_ LOGICAL CancelPendingCallbacks
))

// private
typedef VOID (NTAPI *PTP_IO_CALLBACK)(
    _Inout_ PTP_CALLBACK_INSTANCE Instance,
    _Inout_opt_ PVOID Context,
    _In_ PVOID ApcContext,
    _In_ PIO_STATUS_BLOCK IoSB,
    _In_ PTP_IO Io
    );

// private
_Check_return_
NTDLL_API(NTSTATUS, __stdcall, TpAllocIoCompletion, (
    _Out_ PTP_IO *IoReturn,
    _In_ HANDLE File,
    _In_ PTP_IO_CALLBACK Callback,
    _Inout_opt_ PVOID Context,
    _In_opt_ PTP_CALLBACK_ENVIRON CallbackEnviron
))

// winbase:CloseThreadpoolIo
NTDLL_API_VOID(__stdcall, TpReleaseIoCompletion, (
    _Inout_ PTP_IO Io
))

// winbase:StartThreadpoolIo
NTDLL_API_VOID(__stdcall, TpStartAsyncIoOperation, (
    _Inout_ PTP_IO Io
))

// winbase:CancelThreadpoolIo
NTDLL_API_VOID(__stdcall, TpCancelAsyncIoOperation, (
    _Inout_ PTP_IO Io
))

// winbase:WaitForThreadpoolIoCallbacks
NTDLL_API_VOID(__stdcall, TpWaitForIoCompletion, (
    _Inout_ PTP_IO Io,
    _In_ LOGICAL CancelPendingCallbacks
))

// private
NTDLL_API(NTSTATUS, __stdcall, TpAllocAlpcCompletion, (
    _Out_ PTP_ALPC *AlpcReturn,
    _In_ HANDLE AlpcPort,
    _In_ PTP_ALPC_CALLBACK Callback,
    _Inout_opt_ PVOID Context,
    _In_opt_ PTP_CALLBACK_ENVIRON CallbackEnviron
))

#if (NTLIB_WIN_VERSION >= NTLIB_WIN_7)
// rev
NTDLL_API(NTSTATUS, __stdcall, TpAllocAlpcCompletionEx, (
    _Out_ PTP_ALPC *AlpcReturn,
    _In_ HANDLE AlpcPort,
    _In_ PTP_ALPC_CALLBACK_EX Callback,
    _Inout_opt_ PVOID Context,
    _In_opt_ PTP_CALLBACK_ENVIRON CallbackEnviron
))
#endif

// private
NTDLL_API_VOID(__stdcall, TpReleaseAlpcCompletion, (
    _Inout_ PTP_ALPC Alpc
))

// private
NTDLL_API_VOID(__stdcall, TpWaitForAlpcCompletion, (
    _Inout_ PTP_ALPC Alpc
))

// private
typedef enum _TP_TRACE_TYPE
{
    TpTraceThreadPriority = 1,
    TpTraceThreadAffinity,
    MaxTpTraceType
} TP_TRACE_TYPE;

// private
NTDLL_API_VOID(__stdcall, TpCaptureCaller, (
    _In_ TP_TRACE_TYPE Type
))

// private
NTDLL_API_VOID(__stdcall, TpCheckTerminateWorker, (
    _In_ HANDLE Thread
))

#endif

#endif
