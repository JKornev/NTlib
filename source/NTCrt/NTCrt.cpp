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

#ifdef WPRFLAG
#error "aaa"
#endif

// Console entry

extern int main(int argc, char* argv[]);
extern int wmain(int argc, wchar_t* argv[]);

#pragma comment(linker, "/alternatename:_main=_unspecified_main")
#pragma comment(linker, "/alternatename:_wmain=_unspecified_main")

int unspecified_main(int argc, void* argv)
{
    return 0;
}

int mainCRTStartup(void)
{
    return main(0, 0);
}

int wmainCRTStartup(void)
{
    return wmain(0, 0);
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
