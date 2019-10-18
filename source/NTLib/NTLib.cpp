#include "NTLib.h"

NTSTATUS NtlPsCreateProcess(
    _Out_    PHANDLE Handle,
    _In_     LPWSTR Path,
    _In_opt_ LPWSTR CommandLine,
    _In_     ACCESS_MASK Access,
    _In_     BOOLEAN InheritObjects,
    _In_opt_ POBJECT_ATTRIBUTES Attributes)
{
    OBJECT_ATTRIBUTES attribs;

    if (!Attributes)
    {
        InitializeObjectAttributes(&attribs, NULL, 0, NULL, NULL);
        Attributes = &attribs;
    }

    //TODO: implement me

    return STATUS_UNSUCCESSFUL;
}

NTSTATUS NtlPsOpenProcess(
    _Out_    PHANDLE Handle,
    _In_     DWORD ProcessID,
    _In_     ACCESS_MASK Access,
    _In_opt_ POBJECT_ATTRIBUTES Attributes)
{
    OBJECT_ATTRIBUTES attribs;
    
    if (!Attributes)
    {
        InitializeObjectAttributes(&attribs, NULL, 0, NULL, NULL);
        Attributes = &attribs;
    }

    CLIENT_ID id = { (PVOID)ProcessID, 0 };
    return ::ZwOpenProcess(Handle, Access, Attributes, &id);
}

BOOL NtlUtilIsProcessFeaturePresented(BYTE feature)
{
    if (feature != RTL_FIELD_SIZE(KUSER_SHARED_DATA, ProcessorFeatures))
        return FALSE;

    return (USER_SHARED_DATA->ProcessorFeatures[feature] ? TRUE : FALSE);
}
