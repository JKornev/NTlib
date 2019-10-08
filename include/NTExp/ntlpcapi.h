/*
 * This file is part of the Process Hacker project - https://processhacker.sf.io/ 
 *
 * You can redistribute this file and/or modify it under the terms of the 
 * Attribution 4.0 International (CC BY 4.0) license. 
 * 
 * You must give appropriate credit, provide a link to the license, and 
 * indicate if changes were made. You may do so in any reasonable manner, but 
 * not in any way that suggests the licensor endorses you or your use.
 */

#ifndef _NTLPCAPI_H
#define _NTLPCAPI_H

// Local Inter-process Communication

#define PORT_CONNECT 0x0001
#define PORT_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE | 0x1)

typedef struct _PORT_MESSAGE
{
    union
    {
        struct
        {
            CSHORT DataLength;
            CSHORT TotalLength;
        } s1;
        ULONG Length;
    } u1;
    union
    {
        struct
        {
            CSHORT Type;
            CSHORT DataInfoOffset;
        } s2;
        ULONG ZeroInit;
    } u2;
    union
    {
        CLIENT_ID ClientId;
        double DoNotUseThisField;
    };
    ULONG MessageId;
    union
    {
        SIZE_T ClientViewSize; // only valid for LPC_CONNECTION_REQUEST messages
        ULONG CallbackId; // only valid for LPC_REQUEST messages
    };
} PORT_MESSAGE, *PPORT_MESSAGE;

typedef struct _PORT_DATA_ENTRY
{
    PVOID Base;
    ULONG Size;
} PORT_DATA_ENTRY, *PPORT_DATA_ENTRY;

typedef struct _PORT_DATA_INFORMATION
{
    ULONG CountDataEntries;
    PORT_DATA_ENTRY DataEntries[1];
} PORT_DATA_INFORMATION, *PPORT_DATA_INFORMATION;

#define LPC_REQUEST 1
#define LPC_REPLY 2
#define LPC_DATAGRAM 3
#define LPC_LOST_REPLY 4
#define LPC_PORT_CLOSED 5
#define LPC_CLIENT_DIED 6
#define LPC_EXCEPTION 7
#define LPC_DEBUG_EVENT 8
#define LPC_ERROR_EVENT 9
#define LPC_CONNECTION_REQUEST 10

#define LPC_KERNELMODE_MESSAGE (CSHORT)0x8000
#define LPC_NO_IMPERSONATE (CSHORT)0x4000

#define PORT_VALID_OBJECT_ATTRIBUTES OBJ_CASE_INSENSITIVE

#ifdef _WIN64
#define PORT_MAXIMUM_MESSAGE_LENGTH 512
#else
#define PORT_MAXIMUM_MESSAGE_LENGTH 256
#endif

#define LPC_MAX_CONNECTION_INFO_SIZE (16 * sizeof(ULONG_PTR))

#define PORT_TOTAL_MAXIMUM_MESSAGE_LENGTH \
    ((PORT_MAXIMUM_MESSAGE_LENGTH + sizeof(PORT_MESSAGE) + LPC_MAX_CONNECTION_INFO_SIZE + 0xf) & ~0xf)

typedef struct _LPC_CLIENT_DIED_MSG
{
    PORT_MESSAGE PortMsg;
    LARGE_INTEGER CreateTime;
} LPC_CLIENT_DIED_MSG, *PLPC_CLIENT_DIED_MSG;

typedef struct _PORT_VIEW
{
    ULONG Length;
    HANDLE SectionHandle;
    ULONG SectionOffset;
    SIZE_T ViewSize;
    PVOID ViewBase;
    PVOID ViewRemoteBase;
} PORT_VIEW, *PPORT_VIEW;

typedef struct _REMOTE_PORT_VIEW
{
    ULONG Length;
    SIZE_T ViewSize;
    PVOID ViewBase;
} REMOTE_PORT_VIEW, *PREMOTE_PORT_VIEW;

// WOW64 definitions

// Except in a small number of special cases, WOW64 programs using the LPC APIs must use the 64-bit versions of the
// PORT_MESSAGE, PORT_VIEW and REMOTE_PORT_VIEW data structures. Note that we take a different approach than the
// official NT headers, which produce 64-bit versions in a 32-bit environment when USE_LPC6432 is defined.

typedef struct _PORT_MESSAGE64
{
    union
    {
        struct
        {
            CSHORT DataLength;
            CSHORT TotalLength;
        } s1;
        ULONG Length;
    } u1;
    union
    {
        struct
        {
            CSHORT Type;
            CSHORT DataInfoOffset;
        } s2;
        ULONG ZeroInit;
    } u2;
    union
    {
        CLIENT_ID64 ClientId;
        double DoNotUseThisField;
    };
    ULONG MessageId;
    union
    {
        ULONGLONG ClientViewSize; // only valid for LPC_CONNECTION_REQUEST messages
        ULONG CallbackId; // only valid for LPC_REQUEST messages
    };
} PORT_MESSAGE64, *PPORT_MESSAGE64;

typedef struct _LPC_CLIENT_DIED_MSG64
{
    PORT_MESSAGE64 PortMsg;
    LARGE_INTEGER CreateTime;
} LPC_CLIENT_DIED_MSG64, *PLPC_CLIENT_DIED_MSG64;

typedef struct _PORT_VIEW64
{
    ULONG Length;
    ULONGLONG SectionHandle;
    ULONG SectionOffset;
    ULONGLONG ViewSize;
    ULONGLONG ViewBase;
    ULONGLONG ViewRemoteBase;
} PORT_VIEW64, *PPORT_VIEW64;

typedef struct _REMOTE_PORT_VIEW64
{
    ULONG Length;
    ULONGLONG ViewSize;
    ULONGLONG ViewBase;
} REMOTE_PORT_VIEW64, *PREMOTE_PORT_VIEW64;

// Port creation

NATIVE_API(NTSTATUS, /*Nt*/CreatePort, (
    _Out_ PHANDLE PortHandle,
    _In_opt_ POBJECT_ATTRIBUTES ObjectAttributes,
    _In_ ULONG MaxConnectionInfoLength,
    _In_ ULONG MaxMessageLength,
    _In_opt_ ULONG MaxPoolUsage
))

NATIVE_API(NTSTATUS, /*Nt*/CreateWaitablePort, (
    _Out_ PHANDLE PortHandle,
    _In_opt_ POBJECT_ATTRIBUTES ObjectAttributes,
    _In_ ULONG MaxConnectionInfoLength,
    _In_ ULONG MaxMessageLength,
    _In_opt_ ULONG MaxPoolUsage
))

// Port connection (client)

NATIVE_API(NTSTATUS, /*Nt*/ConnectPort, (
    _Out_ PHANDLE PortHandle,
    _In_ PUNICODE_STRING PortName,
    _In_ PSECURITY_QUALITY_OF_SERVICE SecurityQos,
    _Inout_opt_ PPORT_VIEW ClientView,
    _Inout_opt_ PREMOTE_PORT_VIEW ServerView,
    _Out_opt_ PULONG MaxMessageLength,
    _Inout_updates_bytes_to_opt_(*ConnectionInformationLength, *ConnectionInformationLength) PVOID ConnectionInformation,
    _Inout_opt_ PULONG ConnectionInformationLength
))

NATIVE_API(NTSTATUS, /*Nt*/SecureConnectPort, (
    _Out_ PHANDLE PortHandle,
    _In_ PUNICODE_STRING PortName,
    _In_ PSECURITY_QUALITY_OF_SERVICE SecurityQos,
    _Inout_opt_ PPORT_VIEW ClientView,
    _In_opt_ PSID RequiredServerSid,
    _Inout_opt_ PREMOTE_PORT_VIEW ServerView,
    _Out_opt_ PULONG MaxMessageLength,
    _Inout_updates_bytes_to_opt_(*ConnectionInformationLength, *ConnectionInformationLength) PVOID ConnectionInformation,
    _Inout_opt_ PULONG ConnectionInformationLength
))

// Port connection (server)

NATIVE_API(NTSTATUS, /*Nt*/ListenPort, (
    _In_ HANDLE PortHandle,
    _Out_ PPORT_MESSAGE ConnectionRequest
))

NATIVE_API(NTSTATUS, /*Nt*/AcceptConnectPort, (
    _Out_ PHANDLE PortHandle,
    _In_opt_ PVOID PortContext,
    _In_ PPORT_MESSAGE ConnectionRequest,
    _In_ BOOLEAN AcceptConnection,
    _Inout_opt_ PPORT_VIEW ServerView,
    _Out_opt_ PREMOTE_PORT_VIEW ClientView
))

NATIVE_API(NTSTATUS, /*Nt*/CompleteConnectPort, (
    _In_ HANDLE PortHandle
))

// General

NATIVE_API(NTSTATUS, /*Nt*/RequestPort, (
    _In_ HANDLE PortHandle,
    _In_reads_bytes_(RequestMessage->u1.s1.TotalLength) PPORT_MESSAGE RequestMessage
))

NATIVE_API(NTSTATUS, /*Nt*/RequestWaitReplyPort, (
    _In_ HANDLE PortHandle,
    _In_reads_bytes_(RequestMessage->u1.s1.TotalLength) PPORT_MESSAGE RequestMessage,
    _Out_ PPORT_MESSAGE ReplyMessage
))

NATIVE_API(NTSTATUS, /*Nt*/ReplyPort, (
    _In_ HANDLE PortHandle,
    _In_reads_bytes_(ReplyMessage->u1.s1.TotalLength) PPORT_MESSAGE ReplyMessage
))

NATIVE_API(NTSTATUS, /*Nt*/ReplyWaitReplyPort, (
    _In_ HANDLE PortHandle,
    _Inout_ PPORT_MESSAGE ReplyMessage
))

NATIVE_API(NTSTATUS, /*Nt*/ReplyWaitReceivePort, (
    _In_ HANDLE PortHandle,
    _Out_opt_ PVOID *PortContext,
    _In_reads_bytes_opt_(ReplyMessage->u1.s1.TotalLength) PPORT_MESSAGE ReplyMessage,
    _Out_ PPORT_MESSAGE ReceiveMessage
))

NATIVE_API(NTSTATUS, /*Nt*/ReplyWaitReceivePortEx, (
    _In_ HANDLE PortHandle,
    _Out_opt_ PVOID *PortContext,
    _In_reads_bytes_opt_(ReplyMessage->u1.s1.TotalLength) PPORT_MESSAGE ReplyMessage,
    _Out_ PPORT_MESSAGE ReceiveMessage,
    _In_opt_ PLARGE_INTEGER Timeout
))

NATIVE_API(NTSTATUS, /*Nt*/ImpersonateClientOfPort, (
    _In_ HANDLE PortHandle,
    _In_ PPORT_MESSAGE Message
))

NATIVE_API(NTSTATUS, /*Nt*/ReadRequestData, (
    _In_ HANDLE PortHandle,
    _In_ PPORT_MESSAGE Message,
    _In_ ULONG DataEntryIndex,
    _Out_writes_bytes_to_(BufferSize, *NumberOfBytesRead) PVOID Buffer,
    _In_ SIZE_T BufferSize,
    _Out_opt_ PSIZE_T NumberOfBytesRead
))

NATIVE_API(NTSTATUS, /*Nt*/WriteRequestData, (
    _In_ HANDLE PortHandle,
    _In_ PPORT_MESSAGE Message,
    _In_ ULONG DataEntryIndex,
    _In_reads_bytes_(BufferSize) PVOID Buffer,
    _In_ SIZE_T BufferSize,
    _Out_opt_ PSIZE_T NumberOfBytesWritten
))

typedef enum _PORT_INFORMATION_CLASS
{
    PortBasicInformation,
    PortDumpInformation
} PORT_INFORMATION_CLASS;

NATIVE_API(NTSTATUS, /*Nt*/QueryInformationPort, (
    _In_ HANDLE PortHandle,
    _In_ PORT_INFORMATION_CLASS PortInformationClass,
    _Out_writes_bytes_to_(Length, *ReturnLength) PVOID PortInformation,
    _In_ ULONG Length,
    _Out_opt_ PULONG ReturnLength
))

// Asynchronous Local Inter-process Communication

// rev
typedef HANDLE ALPC_HANDLE, *PALPC_HANDLE;

#define ALPC_PORFLG_ALLOW_LPC_REQUESTS 0x20000 // rev
#define ALPC_PORFLG_WAITABLE_PORT 0x40000 // dbg
#define ALPC_PORFLG_SYSTEM_PROCESS 0x100000 // dbg

// symbols
typedef struct _ALPC_PORT_ATTRIBUTES
{
    ULONG Flags;
    SECURITY_QUALITY_OF_SERVICE SecurityQos;
    SIZE_T MaxMessageLength;
    SIZE_T MemoryBandwidth;
    SIZE_T MaxPoolUsage;
    SIZE_T MaxSectionSize;
    SIZE_T MaxViewSize;
    SIZE_T MaxTotalSectionSize;
    ULONG DupObjectTypes;
#ifdef _WIN64
    ULONG Reserved;
#endif
} ALPC_PORT_ATTRIBUTES, *PALPC_PORT_ATTRIBUTES;

// begin_rev
#define ALPC_MESSAGE_SECURITY_ATTRIBUTE 0x80000000
#define ALPC_MESSAGE_VIEW_ATTRIBUTE 0x40000000
#define ALPC_MESSAGE_CONTEXT_ATTRIBUTE 0x20000000
#define ALPC_MESSAGE_HANDLE_ATTRIBUTE 0x10000000
// end_rev

// symbols
typedef struct _ALPC_MESSAGE_ATTRIBUTES
{
    ULONG AllocatedAttributes;
    ULONG ValidAttributes;
} ALPC_MESSAGE_ATTRIBUTES, *PALPC_MESSAGE_ATTRIBUTES;

// symbols
typedef struct _ALPC_COMPLETION_LIST_STATE
{
    union
    {
        struct
        {
            ULONG64 Head : 24;
            ULONG64 Tail : 24;
            ULONG64 ActiveThreadCount : 16;
        } s1;
        ULONG64 Value;
    } u1;
} ALPC_COMPLETION_LIST_STATE, *PALPC_COMPLETION_LIST_STATE;

#define ALPC_COMPLETION_LIST_BUFFER_GRANULARITY_MASK 0x3f // dbg

// symbols
typedef struct DECLSPEC_ALIGN(128) _ALPC_COMPLETION_LIST_HEADER
{
    ULONG64 StartMagic;

    ULONG TotalSize;
    ULONG ListOffset;
    ULONG ListSize;
    ULONG BitmapOffset;
    ULONG BitmapSize;
    ULONG DataOffset;
    ULONG DataSize;
    ULONG AttributeFlags;
    ULONG AttributeSize;

    DECLSPEC_ALIGN(128) ALPC_COMPLETION_LIST_STATE State;
    ULONG LastMessageId;
    ULONG LastCallbackId;
    DECLSPEC_ALIGN(128) ULONG PostCount;
    DECLSPEC_ALIGN(128) ULONG ReturnCount;
    DECLSPEC_ALIGN(128) ULONG LogSequenceNumber;
    DECLSPEC_ALIGN(128) RTL_SRWLOCK UserLock;

    ULONG64 EndMagic;
} ALPC_COMPLETION_LIST_HEADER, *PALPC_COMPLETION_LIST_HEADER;

// private
typedef struct _ALPC_CONTEXT_ATTR
{
    PVOID PortContext;
    PVOID MessageContext;
    ULONG Sequence;
    ULONG MessageId;
    ULONG CallbackId;
} ALPC_CONTEXT_ATTR, *PALPC_CONTEXT_ATTR;

// begin_rev
#define ALPC_HANDLEFLG_DUPLICATE_SAME_ACCESS 0x10000
#define ALPC_HANDLEFLG_DUPLICATE_SAME_ATTRIBUTES 0x20000
#define ALPC_HANDLEFLG_DUPLICATE_INHERIT 0x80000
// end_rev

// private
typedef struct _ALPC_HANDLE_ATTR32
{
    ULONG Flags;
    ULONG Reserved0;
    ULONG SameAccess;
    ULONG SameAttributes;
    ULONG Indirect;
    ULONG Inherit;
    ULONG Reserved1;
    ULONG Handle;
    ULONG ObjectType; // ObjectTypeCode, not ObjectTypeIndex
    ULONG DesiredAccess;
    ULONG GrantedAccess;
} ALPC_HANDLE_ATTR32, *PALPC_HANDLE_ATTR32;

// private
typedef struct _ALPC_HANDLE_ATTR
{
    ULONG Flags;
    ULONG Reserved0;
    ULONG SameAccess;
    ULONG SameAttributes;
    ULONG Indirect;
    ULONG Inherit;
    ULONG Reserved1;
    HANDLE Handle;
    PALPC_HANDLE_ATTR32 HandleAttrArray;
    ULONG ObjectType; // ObjectTypeCode, not ObjectTypeIndex
    ULONG HandleCount;
    ACCESS_MASK DesiredAccess;
    ACCESS_MASK GrantedAccess;
} ALPC_HANDLE_ATTR, *PALPC_HANDLE_ATTR;

#define ALPC_SECFLG_CREATE_HANDLE 0x20000 // dbg
#define ALPC_SECFLG_NOSECTIONHANDLE 0x40000
// private
typedef struct _ALPC_SECURITY_ATTR
{
    ULONG Flags;
    PSECURITY_QUALITY_OF_SERVICE QoS;
    ALPC_HANDLE ContextHandle; // dbg
} ALPC_SECURITY_ATTR, *PALPC_SECURITY_ATTR;

// begin_rev
#define ALPC_VIEWFLG_NOT_SECURE 0x40000
// end_rev

// private
typedef struct _ALPC_DATA_VIEW_ATTR
{
    ULONG Flags;
    ALPC_HANDLE SectionHandle;
    PVOID ViewBase; // must be zero on input
    SIZE_T ViewSize;
} ALPC_DATA_VIEW_ATTR, *PALPC_DATA_VIEW_ATTR;

// private
typedef enum _ALPC_PORT_INFORMATION_CLASS
{
    AlpcBasicInformation, // q: out ALPC_BASIC_INFORMATION
    AlpcPortInformation, // s: in ALPC_PORT_ATTRIBUTES
    AlpcAssociateCompletionPortInformation, // s: in ALPC_PORT_ASSOCIATE_COMPLETION_PORT
    AlpcConnectedSIDInformation, // q: in SID
    AlpcServerInformation, // q: inout ALPC_SERVER_INFORMATION
    AlpcMessageZoneInformation, // s: in ALPC_PORT_MESSAGE_ZONE_INFORMATION
    AlpcRegisterCompletionListInformation, // s: in ALPC_PORT_COMPLETION_LIST_INFORMATION
    AlpcUnregisterCompletionListInformation, // s: VOID
    AlpcAdjustCompletionListConcurrencyCountInformation, // s: in ULONG
    AlpcRegisterCallbackInformation, // kernel-mode only
    AlpcCompletionListRundownInformation, // s: VOID
    AlpcWaitForPortReferences
} ALPC_PORT_INFORMATION_CLASS;

// private
typedef struct _ALPC_BASIC_INFORMATION
{
    ULONG Flags;
    ULONG SequenceNo;
    PVOID PortContext;
} ALPC_BASIC_INFORMATION, *PALPC_BASIC_INFORMATION;

// private
typedef struct _ALPC_PORT_ASSOCIATE_COMPLETION_PORT
{
    PVOID CompletionKey;
    HANDLE CompletionPort;
} ALPC_PORT_ASSOCIATE_COMPLETION_PORT, *PALPC_PORT_ASSOCIATE_COMPLETION_PORT;

// private
typedef struct _ALPC_SERVER_INFORMATION
{
    union
    {
        struct
        {
            HANDLE ThreadHandle;
        } In;
        struct
        {
            BOOLEAN ThreadBlocked;
            HANDLE ConnectedProcessId;
            UNICODE_STRING ConnectionPortName;
        } Out;
    };
} ALPC_SERVER_INFORMATION, *PALPC_SERVER_INFORMATION;

// private
typedef struct _ALPC_PORT_MESSAGE_ZONE_INFORMATION
{
    PVOID Buffer;
    ULONG Size;
} ALPC_PORT_MESSAGE_ZONE_INFORMATION, *PALPC_PORT_MESSAGE_ZONE_INFORMATION;

// private
typedef struct _ALPC_PORT_COMPLETION_LIST_INFORMATION
{
    PVOID Buffer; // PALPC_COMPLETION_LIST_HEADER
    ULONG Size;
    ULONG ConcurrencyCount;
    ULONG AttributeFlags;
} ALPC_PORT_COMPLETION_LIST_INFORMATION, *PALPC_PORT_COMPLETION_LIST_INFORMATION;

// private
typedef enum _ALPC_MESSAGE_INFORMATION_CLASS
{
    AlpcMessageSidInformation, // q: out SID
    AlpcMessageTokenModifiedIdInformation,  // q: out LUID
    AlpcMessageDirectStatusInformation,
    AlpcMessageHandleInformation, // ALPC_MESSAGE_HANDLE_INFORMATION
    MaxAlpcMessageInfoClass
} ALPC_MESSAGE_INFORMATION_CLASS, *PALPC_MESSAGE_INFORMATION_CLASS;

typedef struct _ALPC_MESSAGE_HANDLE_INFORMATION
{
    ULONG Index;
    ULONG Flags;
    ULONG Handle;
    ULONG ObjectType;
    ACCESS_MASK GrantedAccess;
} ALPC_MESSAGE_HANDLE_INFORMATION, *PALPC_MESSAGE_HANDLE_INFORMATION;

// begin_private

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_VISTA)

// System calls

NATIVE_API(NTSTATUS, /*Nt*/AlpcCreatePort, (
    _Out_ PHANDLE PortHandle,
    _In_opt_ POBJECT_ATTRIBUTES ObjectAttributes,
    _In_opt_ PALPC_PORT_ATTRIBUTES PortAttributes
))

NATIVE_API(NTSTATUS, /*Nt*/AlpcDisconnectPort, (
    _In_ HANDLE PortHandle,
    _In_ ULONG Flags
))

NATIVE_API(NTSTATUS, /*Nt*/AlpcQueryInformation, (
    _In_opt_ HANDLE PortHandle,
    _In_ ALPC_PORT_INFORMATION_CLASS PortInformationClass,
    _Inout_updates_bytes_to_(Length, *ReturnLength) PVOID PortInformation,
    _In_ ULONG Length,
    _Out_opt_ PULONG ReturnLength
))

NATIVE_API(NTSTATUS, /*Nt*/AlpcSetInformation, (
    _In_ HANDLE PortHandle,
    _In_ ALPC_PORT_INFORMATION_CLASS PortInformationClass,
    _In_reads_bytes_opt_(Length) PVOID PortInformation,
    _In_ ULONG Length
))

NATIVE_API(NTSTATUS, /*Nt*/AlpcCreatePortSection, (
    _In_ HANDLE PortHandle,
    _In_ ULONG Flags,
    _In_opt_ HANDLE SectionHandle,
    _In_ SIZE_T SectionSize,
    _Out_ PALPC_HANDLE AlpcSectionHandle,
    _Out_ PSIZE_T ActualSectionSize
))

NATIVE_API(NTSTATUS, /*Nt*/AlpcDeletePortSection, (
    _In_ HANDLE PortHandle,
    _Reserved_ ULONG Flags,
    _In_ ALPC_HANDLE SectionHandle
))

NATIVE_API(NTSTATUS, /*Nt*/AlpcCreateResourceReserve, (
    _In_ HANDLE PortHandle,
    _Reserved_ ULONG Flags,
    _In_ SIZE_T MessageSize,
    _Out_ PALPC_HANDLE ResourceId
))

NATIVE_API(NTSTATUS, /*Nt*/AlpcDeleteResourceReserve, (
    _In_ HANDLE PortHandle,
    _Reserved_ ULONG Flags,
    _In_ ALPC_HANDLE ResourceId
))

NATIVE_API(NTSTATUS, /*Nt*/AlpcCreateSectionView, (
    _In_ HANDLE PortHandle,
    _Reserved_ ULONG Flags,
    _Inout_ PALPC_DATA_VIEW_ATTR ViewAttributes
))

NATIVE_API(NTSTATUS, /*Nt*/AlpcDeleteSectionView, (
    _In_ HANDLE PortHandle,
    _Reserved_ ULONG Flags,
    _In_ PVOID ViewBase
))

NATIVE_API(NTSTATUS, /*Nt*/AlpcCreateSecurityContext, (
    _In_ HANDLE PortHandle,
    _Reserved_ ULONG Flags,
    _Inout_ PALPC_SECURITY_ATTR SecurityAttribute
))

NATIVE_API(NTSTATUS, /*Nt*/AlpcDeleteSecurityContext, (
    _In_ HANDLE PortHandle,
    _Reserved_ ULONG Flags,
    _In_ ALPC_HANDLE ContextHandle
))

NATIVE_API(NTSTATUS, /*Nt*/AlpcRevokeSecurityContext, (
    _In_ HANDLE PortHandle,
    _Reserved_ ULONG Flags,
    _In_ ALPC_HANDLE ContextHandle
))

NATIVE_API(NTSTATUS, /*Nt*/AlpcQueryInformationMessage, (
    _In_ HANDLE PortHandle,
    _In_ PPORT_MESSAGE PortMessage,
    _In_ ALPC_MESSAGE_INFORMATION_CLASS MessageInformationClass,
    _Out_writes_bytes_to_opt_(Length, *ReturnLength) PVOID MessageInformation,
    _In_ ULONG Length,
    _Out_opt_ PULONG ReturnLength
))

#define ALPC_MSGFLG_REPLY_MESSAGE 0x1
#define ALPC_MSGFLG_LPC_MODE 0x2 // ?
#define ALPC_MSGFLG_RELEASE_MESSAGE 0x10000 // dbg
#define ALPC_MSGFLG_SYNC_REQUEST 0x20000 // dbg
#define ALPC_MSGFLG_WAIT_USER_MODE 0x100000
#define ALPC_MSGFLG_WAIT_ALERTABLE 0x200000
#define ALPC_MSGFLG_WOW64_CALL 0x80000000 // dbg

NATIVE_API(NTSTATUS, /*Nt*/AlpcConnectPort, (
    _Out_ PHANDLE PortHandle,
    _In_ PUNICODE_STRING PortName,
    _In_opt_ POBJECT_ATTRIBUTES ObjectAttributes,
    _In_opt_ PALPC_PORT_ATTRIBUTES PortAttributes,
    _In_ ULONG Flags,
    _In_opt_ PSID RequiredServerSid,
    _Inout_updates_bytes_to_opt_(*BufferLength, *BufferLength) PPORT_MESSAGE ConnectionMessage,
    _Inout_opt_ PULONG BufferLength,
    _Inout_opt_ PALPC_MESSAGE_ATTRIBUTES OutMessageAttributes,
    _Inout_opt_ PALPC_MESSAGE_ATTRIBUTES InMessageAttributes,
    _In_opt_ PLARGE_INTEGER Timeout
))

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_8)
NATIVE_API(NTSTATUS, /*Nt*/AlpcConnectPortEx, (
    _Out_ PHANDLE PortHandle,
    _In_ POBJECT_ATTRIBUTES ConnectionPortObjectAttributes,
    _In_opt_ POBJECT_ATTRIBUTES ClientPortObjectAttributes,
    _In_opt_ PALPC_PORT_ATTRIBUTES PortAttributes,
    _In_ ULONG Flags,
    _In_opt_ PSECURITY_DESCRIPTOR ServerSecurityRequirements,
    _Inout_updates_bytes_to_opt_(*BufferLength, *BufferLength) PPORT_MESSAGE ConnectionMessage,
    _Inout_opt_ PSIZE_T BufferLength,
    _Inout_opt_ PALPC_MESSAGE_ATTRIBUTES OutMessageAttributes,
    _Inout_opt_ PALPC_MESSAGE_ATTRIBUTES InMessageAttributes,
    _In_opt_ PLARGE_INTEGER Timeout
))
#endif

NATIVE_API(NTSTATUS, /*Nt*/AlpcAcceptConnectPort, (
    _Out_ PHANDLE PortHandle,
    _In_ HANDLE ConnectionPortHandle,
    _In_ ULONG Flags,
    _In_opt_ POBJECT_ATTRIBUTES ObjectAttributes,
    _In_opt_ PALPC_PORT_ATTRIBUTES PortAttributes,
    _In_opt_ PVOID PortContext,
    _In_reads_bytes_(ConnectionRequest->u1.s1.TotalLength) PPORT_MESSAGE ConnectionRequest,
    _Inout_opt_ PALPC_MESSAGE_ATTRIBUTES ConnectionMessageAttributes,
    _In_ BOOLEAN AcceptConnection
))

NATIVE_API(NTSTATUS, /*Nt*/AlpcSendWaitReceivePort, (
    _In_ HANDLE PortHandle,
    _In_ ULONG Flags,
    _In_reads_bytes_opt_(SendMessage->u1.s1.TotalLength) PPORT_MESSAGE SendMessage,
    _Inout_opt_ PALPC_MESSAGE_ATTRIBUTES SendMessageAttributes,
    _Out_writes_bytes_to_opt_(*BufferLength, *BufferLength) PPORT_MESSAGE ReceiveMessage,
    _Inout_opt_ PSIZE_T BufferLength,
    _Inout_opt_ PALPC_MESSAGE_ATTRIBUTES ReceiveMessageAttributes,
    _In_opt_ PLARGE_INTEGER Timeout
))

#define ALPC_CANCELFLG_TRY_CANCEL 0x1 // dbg
#define ALPC_CANCELFLG_NO_CONTEXT_CHECK 0x8
#define ALPC_CANCELFLGP_FLUSH 0x10000 // dbg

NATIVE_API(NTSTATUS, /*Nt*/AlpcCancelMessage, (
    _In_ HANDLE PortHandle,
    _In_ ULONG Flags,
    _In_ PALPC_CONTEXT_ATTR MessageContext
))

NATIVE_API(NTSTATUS, /*Nt*/AlpcImpersonateClientOfPort, (
    _In_ HANDLE PortHandle,
    _In_ PPORT_MESSAGE Message,
    _In_ PVOID Flags
))

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_10_TH1)
NATIVE_API(NTSTATUS, /*Nt*/AlpcImpersonateClientContainerOfPort, (
    _In_ HANDLE PortHandle,
    _In_ PPORT_MESSAGE Message,
    _In_ ULONG Flags
))
#endif

NATIVE_API(NTSTATUS, /*Nt*/AlpcOpenSenderProcess, (
    _Out_ PHANDLE ProcessHandle,
    _In_ HANDLE PortHandle,
    _In_ PPORT_MESSAGE PortMessage,
    _In_ ULONG Flags,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ POBJECT_ATTRIBUTES ObjectAttributes
))

NATIVE_API(NTSTATUS, /*Nt*/AlpcOpenSenderThread, (
    _Out_ PHANDLE ThreadHandle,
    _In_ HANDLE PortHandle,
    _In_ PPORT_MESSAGE PortMessage,
    _In_ ULONG Flags,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ POBJECT_ATTRIBUTES ObjectAttributes
))

// Support functions

NTDLL_API(ULONG, AlpcMaxAllowedMessageLength, (
    VOID
))

NTDLL_API(ULONG, AlpcGetHeaderSize, (
    _In_ ULONG Flags
))

#define ALPC_ATTRFLG_ALLOCATEDATTR 0x20000000
#define ALPC_ATTRFLG_VALIDATTR 0x40000000
#define ALPC_ATTRFLG_KEEPRUNNINGATTR 0x60000000

NTDLL_API(NTSTATUS, AlpcInitializeMessageAttribute, (
    _In_ ULONG AttributeFlags,
    _Out_opt_ PALPC_MESSAGE_ATTRIBUTES Buffer,
    _In_ ULONG BufferSize,
    _Out_ PULONG RequiredBufferSize
))

NTDLL_API(PVOID, AlpcGetMessageAttribute, (
    _In_ PALPC_MESSAGE_ATTRIBUTES Buffer,
    _In_ ULONG AttributeFlag
))

NTDLL_API(NTSTATUS, AlpcRegisterCompletionList, (
    _In_ HANDLE PortHandle,
    _Out_ PALPC_COMPLETION_LIST_HEADER Buffer,
    _In_ ULONG Size,
    _In_ ULONG ConcurrencyCount,
    _In_ ULONG AttributeFlags
))

NTDLL_API(NTSTATUS, AlpcUnregisterCompletionList, (
    _In_ HANDLE PortHandle
))

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_7)
// rev
NTDLL_API(NTSTATUS, AlpcRundownCompletionList, (
    _In_ HANDLE PortHandle
))
#endif

NTDLL_API(NTSTATUS, AlpcAdjustCompletionListConcurrencyCount, (
    _In_ HANDLE PortHandle,
    _In_ ULONG ConcurrencyCount
))

NTDLL_API(BOOLEAN, AlpcRegisterCompletionListWorkerThread, (
    _Inout_ PVOID CompletionList
))

NTDLL_API(BOOLEAN, AlpcUnregisterCompletionListWorkerThread, (
    _Inout_ PVOID CompletionList
))

NTDLL_API_VOID(AlpcGetCompletionListLastMessageInformation, (
    _In_ PVOID CompletionList,
    _Out_ PULONG LastMessageId,
    _Out_ PULONG LastCallbackId
))

NTDLL_API(ULONG, AlpcGetOutstandingCompletionListMessageCount, (
    _In_ PVOID CompletionList
))

NTDLL_API(PPORT_MESSAGE, AlpcGetMessageFromCompletionList, (
    _In_ PVOID CompletionList,
    _Out_opt_ PALPC_MESSAGE_ATTRIBUTES *MessageAttributes
))

NTDLL_API_VOID(AlpcFreeCompletionListMessage, (
    _Inout_ PVOID CompletionList,
    _In_ PPORT_MESSAGE Message
))

NTDLL_API(PALPC_MESSAGE_ATTRIBUTES, AlpcGetCompletionListMessageAttributes, (
    _In_ PVOID CompletionList,
    _In_ PPORT_MESSAGE Message
))

#endif

// end_private

#endif
