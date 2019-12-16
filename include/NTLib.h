#pragma once

#include "NTExp.h"

// ==========================
//   Debug routines

// 
#define NTLIB_INT3 __debugbreak()
// 
#define NTLIB_ASSERT(expr) if (!(expr)) NTLIB_INT3;

#ifdef _DEBUG
# define _NTLIB_DEBUG_BREAK_ROUTINE NTLIB_INT3
#else
# define _NTLIB_DEBUG_BREAK_ROUTINE
#endif
// 
#define NTLIB_DBG_BREAK _NTLIB_DEBUG_BREAK_ROUTINE
// 
#define NTLIB_DBG_ASSERT(expr)  if (!(expr)) NTLIB_DBG_BREAK;

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
