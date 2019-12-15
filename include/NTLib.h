#pragma once

#include "NTExp.h"

// ==========================
//   Debug routines

#ifdef _DEBUG
# define _NTLIB_DEBUG_BREAK_ROUTINE __debugbreak()
#else
# define _NTLIB_DEBUG_BREAK_ROUTINE
#endif

#define NTLIB_DBG_BREAK _NTLIB_DEBUG_BREAK_ROUTINE

// ==========================
//   Process routines

HANDLE NtlPsGetCurrentProcess();

NTSTATUS NtlPsCreateProcess(
    _Out_    PHANDLE Handle,
    _In_     LPWSTR Path,
    _In_opt_ LPWSTR CommandLine,
    _In_     ACCESS_MASK Access,
    _In_     BOOLEAN InheritObjects,
    _In_opt_ POBJECT_ATTRIBUTES Attributes = NULL
);

NTSTATUS NtlPsOpenProcess(
    _Out_    PHANDLE Handle,
    _In_     DWORD ProcessID,
    _In_     ACCESS_MASK Access,
    _In_opt_ POBJECT_ATTRIBUTES Attributes = NULL
);

BOOL NtlUtilIsProcessFeaturePresented(BYTE feature);
