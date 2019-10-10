#include "security-check.h"
#include <Windows.h>

#ifdef _WIN64
#define DEFAULT_SECURITY_COOKIE 0x00002B992DDFA232
#else
#define DEFAULT_SECURITY_COOKIE 0xBB40E64E
#endif

DECLSPEC_SELECTANY UINT_PTR __security_cookie = DEFAULT_SECURITY_COOKIE;
DECLSPEC_SELECTANY UINT_PTR __security_cookie_complement = ~(DEFAULT_SECURITY_COOKIE);

#if defined (_M_IX86)
void __declspec(naked) __fastcall __security_check_cookie(UINT_PTR cookie)
{
    __asm {
        cmp ecx, __security_cookie
        jne failure
        rep ret /* REP to avoid AMD branch prediction penalty */
        failure :
        int 3
    }
}
#endif
