#pragma once

#include <NTLib.h>

#ifdef __cplusplus
extern "C" {
#endif

extern DECLSPEC_SELECTANY UINT_PTR __security_cookie;
extern DECLSPEC_SELECTANY UINT_PTR __security_cookie_complement;

#if defined (_M_IX86)
void  __fastcall __security_check_cookie(UINT_PTR cookie);
#endif

#ifdef __cplusplus
};
#endif