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

#ifdef NTLIB_COMPILATION
  #define EXPORT_API(name) \
    void __cdecl ntexport_##name() {}
#else
  #define EXPORT_API(name)
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
#define NTLIB_WIN_10_19H2 0x0A08
#define NTLIB_WIN_10_20H1 0x0A09
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
  #define PHNT_COMPILE
  // Disable inline functions
  #define PHNT_NO_INLINE_INIT_STRING
  // Enable Rtl swap bytes
  #define PHNT_RTL_BYTESWAP
#endif

// ----------------------------------------
//   Include zone

//INCLUDE:ZONE

// ----------------------------------------
//   API Epilog

#undef NTLIB_WIN_2K
#undef NTLIB_WIN_XP
#undef NTLIB_WIN_VISTA
#undef NTLIB_WIN_7
#undef NTLIB_WIN_8
#undef NTLIB_WIN_8_1
#undef NTLIB_WIN_10_TH1
#undef NTLIB_WIN_10_TH2
#undef NTLIB_WIN_10_RS1
#undef NTLIB_WIN_10_RS2
#undef NTLIB_WIN_10_RS3
#undef NTLIB_WIN_10_RS4
#undef NTLIB_WIN_10_RS5
#undef NTLIB_WIN_10_19H1
#undef NTLIB_WIN_10_19H2
#undef NTLIB_WIN_10_20H1
#undef NTLIB_WIN_MAX
#undef NTLIB_WIN_VERSION 

#undef NTLIB_KERNEL_MODE
#undef NTLIB_USER_MODE

#pragma pack(pop)

#ifdef __cplusplus
}
#endif
