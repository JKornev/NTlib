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
    type NTDEF Zw##name params { return 0; } \
    type NTDEF Nt##name params { return 0; }
  #define NTDLL_API(type, name, params) \
    type NTDEF name params { return 0; }
  #define NTDLL_API_VOID(name, params) \
    VOID NTDEF name params { return; }
#else
  #define NATIVE_API(type, name, params) \
    type NTDEF Zw##name params; \
    type NTDEF Nt##name params;
  #define NTDLL_API(type, name, params) \
    type NTDEF name params;
  #define NTDLL_API_VOID(name, params) \
    VOID NTDEF name params;
#endif

#define NTLIB_WIN_XP     0x0500
#define NTLIB_WIN_VISTA  0x0600
#define NTLIB_WIN_7      0x0601
#define NTLIB_WIN_8      0x0602
#define NTLIB_WIN_8_1    0x0603
#define NTLIB_WIN_10_TH1 0x0A00
#define NTLIB_WIN_MAX    0xFFFF

#ifndef NTLIB_WIN_VERSION
  #define NTLIB_WIN_VERSION NTLIB_WIN_MAX
#endif

// ----------------------------------------
//   Include zone

//INCLUDE:ZONE

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
