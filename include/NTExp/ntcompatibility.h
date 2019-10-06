#pragma once

#include <Windows.h>

#define MEM_EXTENDED_PARAMETER_TYPE_BITS 8

typedef struct MEM_EXTENDED_PARAMETER {
    struct {
        DWORD64 Type : MEM_EXTENDED_PARAMETER_TYPE_BITS;
        DWORD64 Reserved : 64 - MEM_EXTENDED_PARAMETER_TYPE_BITS;
    } DUMMYSTRUCTNAME;
    union {
        DWORD64 ULong64;
        PVOID   Pointer;
        SIZE_T  Size;
        HANDLE  Handle;
        DWORD   ULong;
    } DUMMYUNIONNAME;
} MEM_EXTENDED_PARAMETER, *PMEM_EXTENDED_PARAMETER;

typedef struct _CFG_CALL_TARGET_INFO {
    ULONG_PTR Offset;
    ULONG_PTR Flags;
} CFG_CALL_TARGET_INFO, *PCFG_CALL_TARGET_INFO;

typedef struct _PROCESS_MITIGATION_CONTROL_FLOW_GUARD_POLICY {
    union {
        DWORD Flags;
        struct {
            DWORD EnableControlFlowGuard : 1;
            DWORD EnableExportSuppression : 1;
            DWORD StrictMode : 1;
            DWORD ReservedFlags : 29;
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;
} PROCESS_MITIGATION_CONTROL_FLOW_GUARD_POLICY, *PPROCESS_MITIGATION_CONTROL_FLOW_GUARD_POLICY;

typedef struct _PROCESS_MITIGATION_FONT_DISABLE_POLICY {
    union {
        DWORD Flags;
        struct {
            DWORD DisableNonSystemFonts : 1;
            DWORD AuditNonSystemFontLoading : 1;
            DWORD ReservedFlags : 30;
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;
} PROCESS_MITIGATION_FONT_DISABLE_POLICY, *PPROCESS_MITIGATION_FONT_DISABLE_POLICY;

typedef struct _PROCESS_MITIGATION_IMAGE_LOAD_POLICY {
    union {
        DWORD Flags;
        struct {
            DWORD NoRemoteImages : 1;
            DWORD NoLowMandatoryLabelImages : 1;
            DWORD PreferSystem32Images : 1;
            DWORD AuditNoRemoteImages : 1;
            DWORD AuditNoLowMandatoryLabelImages : 1;
            DWORD ReservedFlags : 27;
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;
} PROCESS_MITIGATION_IMAGE_LOAD_POLICY, *PPROCESS_MITIGATION_IMAGE_LOAD_POLICY;

typedef struct _PROCESS_MITIGATION_SYSTEM_CALL_FILTER_POLICY {
    union {
        ULONG Flags;
        struct {
            ULONG FilterId : 4;
            ULONG ReservedFlags : 28;
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;
} PROCESS_MITIGATION_SYSTEM_CALL_FILTER_POLICY, *PPROCESS_MITIGATION_SYSTEM_CALL_FILTER_POLICY;

typedef struct _PROCESS_MITIGATION_PAYLOAD_RESTRICTION_POLICY {
    union {
        ULONG Flags;
        struct {
            ULONG EnableExportAddressFilter : 1;
            ULONG AuditExportAddressFilter : 1;
            ULONG EnableExportAddressFilterPlus : 1;
            ULONG AuditExportAddressFilterPlus : 1;
            ULONG EnableImportAddressFilter : 1;
            ULONG AuditImportAddressFilter : 1;
            ULONG EnableRopStackPivot : 1;
            ULONG AuditRopStackPivot : 1;
            ULONG EnableRopCallerCheck : 1;
            ULONG AuditRopCallerCheck : 1;
            ULONG EnableRopSimExec : 1;
            ULONG AuditRopSimExec : 1;
            ULONG ReservedFlags : 20;
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;
} PROCESS_MITIGATION_PAYLOAD_RESTRICTION_POLICY, *PPROCESS_MITIGATION_PAYLOAD_RESTRICTION_POLICY;

typedef struct _PROCESS_MITIGATION_CHILD_PROCESS_POLICY {
    union {
        ULONG Flags;
        struct {
            ULONG NoChildProcessCreation : 1;
            ULONG AuditNoChildProcessCreation : 1;
            ULONG AllowSecureProcessCreation : 1;
            ULONG ReservedFlags : 29;
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;
} PROCESS_MITIGATION_CHILD_PROCESS_POLICY, *PPROCESS_MITIGATION_CHILD_PROCESS_POLICY;

typedef struct _PROCESS_MITIGATION_SIDE_CHANNEL_ISOLATION_POLICY {
    ULONG reserved;
} PROCESS_MITIGATION_SIDE_CHANNEL_ISOLATION_POLICY, *PPROCESS_MITIGATION_SIDE_CHANNEL_ISOLATION_POLICY;

typedef GUID* PGUID;

typedef enum _OS_DEPLOYEMENT_STATE_VALUES
{
    OS_DEPLOYMENT_STANDARD = 1,
    OS_DEPLOYMENT_COMPACT
} OS_DEPLOYEMENT_STATE_VALUES;

typedef enum _PNP_VETO_TYPE {
    PNP_VetoTypeUnknown,
    PNP_VetoLegacyDevice,
    PNP_VetoPendingClose,
    PNP_VetoWindowsApp,
    PNP_VetoWindowsService,
    PNP_VetoOutstandingOpen,
    PNP_VetoDevice,
    PNP_VetoDriver,
    PNP_VetoIllegalDeviceRequest,
    PNP_VetoInsufficientPower,
    PNP_VetoNonDisableable,
    PNP_VetoLegacyDriver,
    PNP_VetoInsufficientRights,
    PNP_VetoAlreadyRemoved
} PNP_VETO_TYPE, *PPNP_VETO_TYPE;

typedef UCHAR SE_SIGNING_LEVEL, *PSE_SIGNING_LEVEL;

typedef _IMAGE_RUNTIME_FUNCTION_ENTRY RUNTIME_FUNCTION, *PRUNTIME_FUNCTION;

#ifndef _WIN64
typedef FARPROC PGET_RUNTIME_FUNCTION_CALLBACK;
#endif

#define SE_SIGNING_LEVEL_UNCHECKED         0x00000000
#define SE_SIGNING_LEVEL_UNSIGNED          0x00000001
#define SE_SIGNING_LEVEL_ENTERPRISE        0x00000002
#define SE_SIGNING_LEVEL_CUSTOM_1          0x00000003
#define SE_SIGNING_LEVEL_AUTHENTICODE      0x00000004
#define SE_SIGNING_LEVEL_CUSTOM_2          0x00000005
#define SE_SIGNING_LEVEL_STORE             0x00000006
#define SE_SIGNING_LEVEL_CUSTOM_3          0x00000007
#define SE_SIGNING_LEVEL_ANTIMALWARE       SE_SIGNING_LEVEL_CUSTOM_3
#define SE_SIGNING_LEVEL_MICROSOFT         0x00000008
#define SE_SIGNING_LEVEL_CUSTOM_4          0x00000009
#define SE_SIGNING_LEVEL_CUSTOM_5          0x0000000A
#define SE_SIGNING_LEVEL_DYNAMIC_CODEGEN   0x0000000B
#define SE_SIGNING_LEVEL_WINDOWS           0x0000000C
#define SE_SIGNING_LEVEL_CUSTOM_7          0x0000000D
#define SE_SIGNING_LEVEL_WINDOWS_TCB       0x0000000E
#define SE_SIGNING_LEVEL_CUSTOM_6          0x0000000F