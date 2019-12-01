#pragma once

#include <NTLib.h>

#define ASMCALL __cdecl

#ifdef _M_IX86
extern "C" void ASMCALL capture_context86(CONTEXT* context);
extern "C" void ASMCALL _RTC_CheckEsp();
#else
extern "C" void ASMCALL capture_context64(CONTEXT* context);
#endif

#undef ASMCALL
