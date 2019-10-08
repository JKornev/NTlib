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

#ifndef _NTTMAPI_H
#define _NTTMAPI_H

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_VISTA)

NATIVE_API(NTSTATUS, /*Nt*/CreateTransactionManager, (
    _Out_ PHANDLE TmHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_opt_ POBJECT_ATTRIBUTES ObjectAttributes,
    _In_opt_ PUNICODE_STRING LogFileName,
    _In_opt_ ULONG CreateOptions,
    _In_opt_ ULONG CommitStrength
))

NATIVE_API(NTSTATUS, /*Nt*/OpenTransactionManager, (
    _Out_ PHANDLE TmHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_opt_ POBJECT_ATTRIBUTES ObjectAttributes,
    _In_opt_ PUNICODE_STRING LogFileName,
    _In_opt_ LPGUID TmIdentity,
    _In_opt_ ULONG OpenOptions
))

NATIVE_API(NTSTATUS, /*Nt*/RenameTransactionManager, (
    _In_ PUNICODE_STRING LogFileName,
    _In_ LPGUID ExistingTransactionManagerGuid
))

NATIVE_API(NTSTATUS, /*Nt*/RollforwardTransactionManager, (
    _In_ HANDLE TransactionManagerHandle,
    _In_opt_ PLARGE_INTEGER TmVirtualClock
))

NATIVE_API(NTSTATUS, /*Nt*/RecoverTransactionManager, (
    _In_ HANDLE TransactionManagerHandle
))

NATIVE_API(NTSTATUS, /*Nt*/QueryInformationTransactionManager, (
    _In_ HANDLE TransactionManagerHandle,
    _In_ TRANSACTIONMANAGER_INFORMATION_CLASS TransactionManagerInformationClass,
    _Out_writes_bytes_(TransactionManagerInformationLength) PVOID TransactionManagerInformation,
    _In_ ULONG TransactionManagerInformationLength,
    _Out_opt_ PULONG ReturnLength
))

NATIVE_API(NTSTATUS, /*Nt*/SetInformationTransactionManager, (
    _In_opt_ HANDLE TmHandle,
    _In_ TRANSACTIONMANAGER_INFORMATION_CLASS TransactionManagerInformationClass,
    _In_reads_bytes_(TransactionManagerInformationLength) PVOID TransactionManagerInformation,
    _In_ ULONG TransactionManagerInformationLength
))

NATIVE_API(NTSTATUS, /*Nt*/EnumerateTransactionObject, (
    _In_opt_ HANDLE RootObjectHandle,
    _In_ KTMOBJECT_TYPE QueryType,
    _Inout_updates_bytes_(ObjectCursorLength) PKTMOBJECT_CURSOR ObjectCursor,
    _In_ ULONG ObjectCursorLength,
    _Out_ PULONG ReturnLength
))

NATIVE_API(NTSTATUS, /*Nt*/CreateTransaction, (
    _Out_ PHANDLE TransactionHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_opt_ POBJECT_ATTRIBUTES ObjectAttributes,
    _In_opt_ LPGUID Uow,
    _In_opt_ HANDLE TmHandle,
    _In_opt_ ULONG CreateOptions,
    _In_opt_ ULONG IsolationLevel,
    _In_opt_ ULONG IsolationFlags,
    _In_opt_ PLARGE_INTEGER Timeout,
    _In_opt_ PUNICODE_STRING Description
))

NATIVE_API(NTSTATUS, /*Nt*/OpenTransaction, (
    _Out_ PHANDLE TransactionHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ POBJECT_ATTRIBUTES ObjectAttributes,
    _In_ LPGUID Uow,
    _In_opt_ HANDLE TmHandle
))

NATIVE_API(NTSTATUS, /*Nt*/QueryInformationTransaction, (
    _In_ HANDLE TransactionHandle,
    _In_ TRANSACTION_INFORMATION_CLASS TransactionInformationClass,
    _Out_writes_bytes_(TransactionInformationLength) PVOID TransactionInformation,
    _In_ ULONG TransactionInformationLength,
    _Out_opt_ PULONG ReturnLength
))

NATIVE_API(NTSTATUS, /*Nt*/SetInformationTransaction, (
    _In_ HANDLE TransactionHandle,
    _In_ TRANSACTION_INFORMATION_CLASS TransactionInformationClass,
    _In_reads_bytes_(TransactionInformationLength) PVOID TransactionInformation,
    _In_ ULONG TransactionInformationLength
))

NATIVE_API(NTSTATUS, /*Nt*/CommitTransaction, (
    _In_ HANDLE TransactionHandle,
    _In_ BOOLEAN Wait
))

NATIVE_API(NTSTATUS, /*Nt*/RollbackTransaction, (
    _In_ HANDLE TransactionHandle,
    _In_ BOOLEAN Wait
))

NATIVE_API(NTSTATUS, /*Nt*/CreateEnlistment, (
    _Out_ PHANDLE EnlistmentHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ HANDLE ResourceManagerHandle,
    _In_ HANDLE TransactionHandle,
    _In_opt_ POBJECT_ATTRIBUTES ObjectAttributes,
    _In_opt_ ULONG CreateOptions,
    _In_ NOTIFICATION_MASK NotificationMask,
    _In_opt_ PVOID EnlistmentKey
))

NATIVE_API(NTSTATUS, /*Nt*/OpenEnlistment, (
    _Out_ PHANDLE EnlistmentHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ HANDLE ResourceManagerHandle,
    _In_ LPGUID EnlistmentGuid,
    _In_opt_ POBJECT_ATTRIBUTES ObjectAttributes
))

NATIVE_API(NTSTATUS, /*Nt*/QueryInformationEnlistment, (
    _In_ HANDLE EnlistmentHandle,
    _In_ ENLISTMENT_INFORMATION_CLASS EnlistmentInformationClass,
    _Out_writes_bytes_(EnlistmentInformationLength) PVOID EnlistmentInformation,
    _In_ ULONG EnlistmentInformationLength,
    _Out_opt_ PULONG ReturnLength
))

NATIVE_API(NTSTATUS, /*Nt*/SetInformationEnlistment, (
    _In_opt_ HANDLE EnlistmentHandle,
    _In_ ENLISTMENT_INFORMATION_CLASS EnlistmentInformationClass,
    _In_reads_bytes_(EnlistmentInformationLength) PVOID EnlistmentInformation,
    _In_ ULONG EnlistmentInformationLength
))

NATIVE_API(NTSTATUS, /*Nt*/RecoverEnlistment, (
    _In_ HANDLE EnlistmentHandle,
    _In_opt_ PVOID EnlistmentKey
))

NATIVE_API(NTSTATUS, /*Nt*/PrePrepareEnlistment, (
    _In_ HANDLE EnlistmentHandle,
    _In_opt_ PLARGE_INTEGER TmVirtualClock
))

NATIVE_API(NTSTATUS, /*Nt*/PrepareEnlistment, (
    _In_ HANDLE EnlistmentHandle,
    _In_opt_ PLARGE_INTEGER TmVirtualClock
))

NATIVE_API(NTSTATUS, /*Nt*/CommitEnlistment, (
    _In_ HANDLE EnlistmentHandle,
    _In_opt_ PLARGE_INTEGER TmVirtualClock
))

NATIVE_API(NTSTATUS, /*Nt*/RollbackEnlistment, (
    _In_ HANDLE EnlistmentHandle,
    _In_opt_ PLARGE_INTEGER TmVirtualClock
))

NATIVE_API(NTSTATUS, /*Nt*/PrePrepareComplete, (
    _In_ HANDLE EnlistmentHandle,
    _In_opt_ PLARGE_INTEGER TmVirtualClock
))

NATIVE_API(NTSTATUS, /*Nt*/PrepareComplete, (
    _In_ HANDLE EnlistmentHandle,
    _In_opt_ PLARGE_INTEGER TmVirtualClock
))

NATIVE_API(NTSTATUS, /*Nt*/CommitComplete, (
    _In_ HANDLE EnlistmentHandle,
    _In_opt_ PLARGE_INTEGER TmVirtualClock
))

NATIVE_API(NTSTATUS, /*Nt*/ReadOnlyEnlistment, (
    _In_ HANDLE EnlistmentHandle,
    _In_opt_ PLARGE_INTEGER TmVirtualClock
))

NATIVE_API(NTSTATUS, /*Nt*/RollbackComplete, (
    _In_ HANDLE EnlistmentHandle,
    _In_opt_ PLARGE_INTEGER TmVirtualClock
))

NATIVE_API(NTSTATUS, /*Nt*/SinglePhaseReject, (
    _In_ HANDLE EnlistmentHandle,
    _In_opt_ PLARGE_INTEGER TmVirtualClock
))

NATIVE_API(NTSTATUS, /*Nt*/CreateResourceManager, (
    _Out_ PHANDLE ResourceManagerHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ HANDLE TmHandle,
    _In_ LPGUID RmGuid,
    _In_opt_ POBJECT_ATTRIBUTES ObjectAttributes,
    _In_opt_ ULONG CreateOptions,
    _In_opt_ PUNICODE_STRING Description
))

NATIVE_API(NTSTATUS, /*Nt*/OpenResourceManager, (
    _Out_ PHANDLE ResourceManagerHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ HANDLE TmHandle,
    _In_opt_ LPGUID ResourceManagerGuid,
    _In_opt_ POBJECT_ATTRIBUTES ObjectAttributes
))

NATIVE_API(NTSTATUS, /*Nt*/RecoverResourceManager, (
    _In_ HANDLE ResourceManagerHandle
))

NATIVE_API(NTSTATUS, /*Nt*/GetNotificationResourceManager, (
    _In_ HANDLE ResourceManagerHandle,
    _Out_ PTRANSACTION_NOTIFICATION TransactionNotification,
    _In_ ULONG NotificationLength,
    _In_opt_ PLARGE_INTEGER Timeout,
    _Out_opt_ PULONG ReturnLength,
    _In_ ULONG Asynchronous,
    _In_opt_ ULONG_PTR AsynchronousContext
))

NATIVE_API(NTSTATUS, /*Nt*/QueryInformationResourceManager, (
    _In_ HANDLE ResourceManagerHandle,
    _In_ RESOURCEMANAGER_INFORMATION_CLASS ResourceManagerInformationClass,
    _Out_writes_bytes_(ResourceManagerInformationLength) PVOID ResourceManagerInformation,
    _In_ ULONG ResourceManagerInformationLength,
    _Out_opt_ PULONG ReturnLength
))

NATIVE_API(NTSTATUS, /*Nt*/SetInformationResourceManager, (
    _In_ HANDLE ResourceManagerHandle,
    _In_ RESOURCEMANAGER_INFORMATION_CLASS ResourceManagerInformationClass,
    _In_reads_bytes_(ResourceManagerInformationLength) PVOID ResourceManagerInformation,
    _In_ ULONG ResourceManagerInformationLength
))

NATIVE_API(NTSTATUS, /*Nt*/RegisterProtocolAddressInformation, (
    _In_ HANDLE ResourceManager,
    _In_ PCRM_PROTOCOL_ID ProtocolId,
    _In_ ULONG ProtocolInformationSize,
    _In_ PVOID ProtocolInformation,
    _In_opt_ ULONG CreateOptions
))

NATIVE_API(NTSTATUS, /*Nt*/PropagationComplete, (
    _In_ HANDLE ResourceManagerHandle,
    _In_ ULONG RequestCookie,
    _In_ ULONG BufferLength,
    _In_ PVOID Buffer
))

NATIVE_API(NTSTATUS, /*Nt*/PropagationFailed, (
    _In_ HANDLE ResourceManagerHandle,
    _In_ ULONG RequestCookie,
    _In_ NTSTATUS PropStatus
))

// private
NATIVE_API(NTSTATUS, /*Nt*/FreezeTransactions, (
    _In_ PLARGE_INTEGER FreezeTimeout,
    _In_ PLARGE_INTEGER ThawTimeout
))

// private
NATIVE_API(NTSTATUS, /*Nt*/ThawTransactions, (
    VOID
))
#endif

#endif
