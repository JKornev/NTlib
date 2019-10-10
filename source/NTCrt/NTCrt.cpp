#include <NTLib.h>

/*TODO:
- Call original main, wmain, WinMain etc. from these stubs
- Check how security check works
*/

#ifdef __cplusplus
extern "C" {
#endif

// DLL entry

BOOL WINAPI _DllMainCRTStartup(HANDLE  hDllHandle, DWORD dwReason, LPVOID lpreserved)
{
    return 0;
}

// Console entry

int mainCRTStartup(void)
{
    return 0;
}

int wmainCRTStartup(void)
{
    return 0;
}

// Windows app entry

int WinMainCRTStartup(void)
{
    return 0;
}

int wWinMainCRTStartup(void)
{
    return 0;
}

#ifdef __cplusplus
};
#endif
