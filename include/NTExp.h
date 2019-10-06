#pragma once

#include <Windows.h>

// ----------------------------------------
//   API Prolog

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _M_IX86
  #pragma pack(push, 4)
#elif _M_AMD64
  #pragma pack(push, 8)
#else
  #error NTLIB: Error, unknown architecture
#endif

#define NTCALL __stdcall

#ifdef NTLIB_COMPILATION
  #define NTDEF __declspec(dllexport) NTCALL
#else
  #define NTDEF __declspec(dllimport) NTCALL
#endif

#ifdef NTLIB_COMPILATION
  #define NATIVE_API(type, name, params) \
    type NTDEF Zw##name params { return (type)0; } \
    type NTDEF Nt##name params { return (type)0; }
  #define NATIVE_API_VOID(name, params) \
    VOID NTDEF name params{ return; }
  #define NTDLL_API(type, name, params) \
    type NTDEF name params { return (type)0; }
  #define NTDLL_API_VOID(name, params) \
    VOID NTDEF name params { return; }
#else
  #define NATIVE_API(type, name, params) \
    type NTDEF Zw##name params; \
    type NTDEF Nt##name params;
  #define NATIVE_API_VOID(name, params) \
    VOID NTDEF name params;
  #define NTDLL_API(type, name, params) \
    type NTDEF name params;
  #define NTDLL_API_VOID(name, params) \
    VOID NTDEF name params;
#endif

#define NTLIB_WIN_2K      0x0400
#define NTLIB_WIN_XP      0x0500
#define NTLIB_WIN_VISTA   0x0600
#define NTLIB_WIN_7       0x0601
#define NTLIB_WIN_8       0x0602
#define NTLIB_WIN_8_1     0x0603
#define NTLIB_WIN_10_TH1  0x0A00
#define NTLIB_WIN_10_TH2  0x0A01
#define NTLIB_WIN_10_RS1  0x0A02
#define NTLIB_WIN_10_RS2  0x0A03
#define NTLIB_WIN_10_RS3  0x0A04
#define NTLIB_WIN_10_RS4  0x0A05
#define NTLIB_WIN_10_RS5  0x0A06
#define NTLIB_WIN_10_19H1 0x0A07
#define NTLIB_WIN_MAX     0xFFFF

#ifndef NTLIB_WIN_VERSION
  #define NTLIB_WIN_VERSION NTLIB_WIN_MAX
#endif

#define NTLIB_KERNEL_MODE 0
#define NTLIB_USER_MODE   1

#ifndef NTLIB_CPU_MODE
  #define NTLIB_CPU_MODE NTLIB_USER_MODE
#endif

// PHNT compilation settings
#ifdef NTLIB_COMPILATION
  // This macro make PHNT headers compatible with NTExp.lib compilation
  #define PHNT_COMPILE 1
  // Disable inline functions
  #define PHNT_NO_INLINE_INIT_STRING
#endif

// ----------------------------------------
//   Include zone

#include <ntexp\ntcompatibility.h>

#include <ntexp\ntstatus.h>
#include <ntexp\ntcommon.h>
#include <ntexp\ntcompatibility.h>
#include <ntexp\ntdbg.h>
#include <ntexp\ntexapi.h>
#include <ntexp\ntgdi.h>
#include <ntexp\ntioapi.h>
#include <ntexp\ntkeapi.h>
#include <ntexp\ntldr.h>
#include <ntexp\ntlpcapi.h>
#include <ntexp\ntmisc.h>
#include <ntexp\ntmmapi.h>
#include <ntexp\ntnls.h>
#include <ntexp\ntobapi.h>
#include <ntexp\ntpebteb.h>
#include <ntexp\ntpfapi.h>
#include <ntexp\ntpnpapi.h>
#include <ntexp\ntpoapi.h>
#include <ntexp\ntpsapi.h>
#include <ntexp\ntregapi.h>
#include <ntexp\ntrtl.h>
#include <ntexp\ntsam.h>
#include <ntexp\ntseapi.h>
#include <ntexp\ntsmss.h>
#include <ntexp\nttmapi.h>
#include <ntexp\nttp.h>
#include <ntexp\ntwow64.h>
#include <ntexp\ntxcapi.h>
#include <ntexp\subprocesstag.h>
#include <ntexp\winsta.h>


// ----------------------------------------
//   API Epilog

#undef NTLIB_WIN_XP
#undef NTLIB_WIN_VISTA
#undef NTLIB_WIN_7
#undef NTLIB_WIN_8
#undef NTLIB_WIN_8_1
#undef NTLIB_WIN_10_TH1
#undef NTLIB_WIN_MAX
#undef NTLIB_WIN_VERSION 

#undef NTCALL
#undef NTDEF

#undef NATIVE_API

#undef NTDLL_API
#undef NTDLL_API_VOID

#undef LDR_API

#pragma pack(pop)

#ifdef __cplusplus
}
#endif
