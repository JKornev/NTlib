#include <Windows.h>
#include <stdio.h>
#include <NTLib.h>

#pragma comment(lib, "NTExp.lib")
#pragma comment(lib, "NTLib.lib")

int wmain(int argc, wchar_t* argv[])
{
    printf("ZwClose returned %x\n", ZwClose(0));
    getchar();
    return 0;
}