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

#ifndef _NTRTL_H
#define _NTRTL_H

#define RtlOffsetToPointer(Base, Offset) ((PCHAR)(((PCHAR)(Base)) + ((ULONG_PTR)(Offset))))
#define RtlPointerToOffset(Base, Pointer) ((ULONG)(((PCHAR)(Pointer)) - ((PCHAR)(Base))))

// Linked lists

FORCEINLINE VOID InitializeListHead(
    _Out_ PLIST_ENTRY ListHead
    )
{
    ListHead->Flink = ListHead->Blink = ListHead;
}

_Check_return_ FORCEINLINE BOOLEAN IsListEmpty(
    _In_ PLIST_ENTRY ListHead
    )
{
    return ListHead->Flink == ListHead;
}

FORCEINLINE BOOLEAN RemoveEntryList(
    _In_ PLIST_ENTRY Entry
    )
{
    PLIST_ENTRY Blink;
    PLIST_ENTRY Flink;

    Flink = Entry->Flink;
    Blink = Entry->Blink;
    Blink->Flink = Flink;
    Flink->Blink = Blink;

    return Flink == Blink;
}

FORCEINLINE PLIST_ENTRY RemoveHeadList(
    _Inout_ PLIST_ENTRY ListHead
    )
{
    PLIST_ENTRY Flink;
    PLIST_ENTRY Entry;

    Entry = ListHead->Flink;
    Flink = Entry->Flink;
    ListHead->Flink = Flink;
    Flink->Blink = ListHead;

    return Entry;
}

FORCEINLINE PLIST_ENTRY RemoveTailList(
    _Inout_ PLIST_ENTRY ListHead
    )
{
    PLIST_ENTRY Blink;
    PLIST_ENTRY Entry;

    Entry = ListHead->Blink;
    Blink = Entry->Blink;
    ListHead->Blink = Blink;
    Blink->Flink = ListHead;

    return Entry;
}

FORCEINLINE VOID InsertTailList(
    _Inout_ PLIST_ENTRY ListHead,
    _Inout_ PLIST_ENTRY Entry
    )
{
    PLIST_ENTRY Blink;

    Blink = ListHead->Blink;
    Entry->Flink = ListHead;
    Entry->Blink = Blink;
    Blink->Flink = Entry;
    ListHead->Blink = Entry;
}

FORCEINLINE VOID InsertHeadList(
    _Inout_ PLIST_ENTRY ListHead,
    _Inout_ PLIST_ENTRY Entry
    )
{
    PLIST_ENTRY Flink;

    Flink = ListHead->Flink;
    Entry->Flink = Flink;
    Entry->Blink = ListHead;
    Flink->Blink = Entry;
    ListHead->Flink = Entry;
}

FORCEINLINE VOID AppendTailList(
    _Inout_ PLIST_ENTRY ListHead,
    _Inout_ PLIST_ENTRY ListToAppend
    )
{
    PLIST_ENTRY ListEnd = ListHead->Blink;

    ListHead->Blink->Flink = ListToAppend;
    ListHead->Blink = ListToAppend->Blink;
    ListToAppend->Blink->Flink = ListHead;
    ListToAppend->Blink = ListEnd;
}

FORCEINLINE PSINGLE_LIST_ENTRY PopEntryList(
    _Inout_ PSINGLE_LIST_ENTRY ListHead
    )
{
    PSINGLE_LIST_ENTRY FirstEntry;

    FirstEntry = ListHead->Next;

    if (FirstEntry)
        ListHead->Next = FirstEntry->Next;

    return FirstEntry;
}

FORCEINLINE VOID PushEntryList(
    _Inout_ PSINGLE_LIST_ENTRY ListHead,
    _Inout_ PSINGLE_LIST_ENTRY Entry
    )
{
    Entry->Next = ListHead->Next;
    ListHead->Next = Entry;
}

// AVL and splay trees

typedef enum _TABLE_SEARCH_RESULT
{
    TableEmptyTree,
    TableFoundNode,
    TableInsertAsLeft,
    TableInsertAsRight
} TABLE_SEARCH_RESULT;

typedef enum _RTL_GENERIC_COMPARE_RESULTS
{
    GenericLessThan,
    GenericGreaterThan,
    GenericEqual
} RTL_GENERIC_COMPARE_RESULTS;

typedef RTL_GENERIC_COMPARE_RESULTS (NTAPI *PRTL_AVL_COMPARE_ROUTINE)(
    _In_ struct _RTL_AVL_TABLE *Table,
    _In_ PVOID FirstStruct,
    _In_ PVOID SecondStruct
    );

typedef PVOID (NTAPI *PRTL_AVL_ALLOCATE_ROUTINE)(
    _In_ struct _RTL_AVL_TABLE *Table,
    _In_ CLONG ByteSize
    );

typedef VOID (NTAPI *PRTL_AVL_FREE_ROUTINE)(
    _In_ struct _RTL_AVL_TABLE *Table,
    _In_ _Post_invalid_ PVOID Buffer
    );

typedef NTSTATUS (NTAPI *PRTL_AVL_MATCH_FUNCTION)(
    _In_ struct _RTL_AVL_TABLE *Table,
    _In_ PVOID UserData,
    _In_ PVOID MatchData
    );

typedef struct _RTL_BALANCED_LINKS
{
    struct _RTL_BALANCED_LINKS *Parent;
    struct _RTL_BALANCED_LINKS *LeftChild;
    struct _RTL_BALANCED_LINKS *RightChild;
    CHAR Balance;
    UCHAR Reserved[3];
} RTL_BALANCED_LINKS, *PRTL_BALANCED_LINKS;

typedef struct _RTL_AVL_TABLE
{
    RTL_BALANCED_LINKS BalancedRoot;
    PVOID OrderedPointer;
    ULONG WhichOrderedElement;
    ULONG NumberGenericTableElements;
    ULONG DepthOfTree;
    PRTL_BALANCED_LINKS RestartKey;
    ULONG DeleteCount;
    PRTL_AVL_COMPARE_ROUTINE CompareRoutine;
    PRTL_AVL_ALLOCATE_ROUTINE AllocateRoutine;
    PRTL_AVL_FREE_ROUTINE FreeRoutine;
    PVOID TableContext;
} RTL_AVL_TABLE, *PRTL_AVL_TABLE;

NTDLL_API_VOID(__stdcall, RtlInitializeGenericTableAvl, (
    _Out_ PRTL_AVL_TABLE Table,
    _In_ PRTL_AVL_COMPARE_ROUTINE CompareRoutine,
    _In_ PRTL_AVL_ALLOCATE_ROUTINE AllocateRoutine,
    _In_ PRTL_AVL_FREE_ROUTINE FreeRoutine,
    _In_opt_ PVOID TableContext
))

NTDLL_API(PVOID, __stdcall, RtlInsertElementGenericTableAvl, (
    _In_ PRTL_AVL_TABLE Table,
    _In_reads_bytes_(BufferSize) PVOID Buffer,
    _In_ CLONG BufferSize,
    _Out_opt_ PBOOLEAN NewElement
))

NTDLL_API(PVOID, __stdcall, RtlInsertElementGenericTableFullAvl, (
    _In_ PRTL_AVL_TABLE Table,
    _In_reads_bytes_(BufferSize) PVOID Buffer,
    _In_ CLONG BufferSize,
    _Out_opt_ PBOOLEAN NewElement,
    _In_ PVOID NodeOrParent,
    _In_ TABLE_SEARCH_RESULT SearchResult
))

NTDLL_API(BOOLEAN, __stdcall, RtlDeleteElementGenericTableAvl, (
    _In_ PRTL_AVL_TABLE Table,
    _In_ PVOID Buffer
))

_Check_return_
NTDLL_API(PVOID, __stdcall, RtlLookupElementGenericTableAvl, (
    _In_ PRTL_AVL_TABLE Table,
    _In_ PVOID Buffer
))

NTDLL_API(PVOID, __stdcall, RtlLookupElementGenericTableFullAvl, (
    _In_ PRTL_AVL_TABLE Table,
    _In_ PVOID Buffer,
    _Out_ PVOID *NodeOrParent,
    _Out_ TABLE_SEARCH_RESULT *SearchResult
))

_Check_return_
NTDLL_API(PVOID, __stdcall, RtlEnumerateGenericTableAvl, (
    _In_ PRTL_AVL_TABLE Table,
    _In_ BOOLEAN Restart
))

_Check_return_
NTDLL_API(PVOID, __stdcall, RtlEnumerateGenericTableWithoutSplayingAvl, (
    _In_ PRTL_AVL_TABLE Table,
    _Inout_ PVOID *RestartKey
))

_Check_return_
NTDLL_API(PVOID, __stdcall, RtlLookupFirstMatchingElementGenericTableAvl, (
    _In_ PRTL_AVL_TABLE Table,
    _In_ PVOID Buffer,
    _Out_ PVOID *RestartKey
))

_Check_return_
NTDLL_API(PVOID, __stdcall, RtlEnumerateGenericTableLikeADirectory, (
    _In_ PRTL_AVL_TABLE Table,
    _In_opt_ PRTL_AVL_MATCH_FUNCTION MatchFunction,
    _In_opt_ PVOID MatchData,
    _In_ ULONG NextFlag,
    _Inout_ PVOID *RestartKey,
    _Inout_ PULONG DeleteCount,
    _In_ PVOID Buffer
))

_Check_return_
NTDLL_API(PVOID, __stdcall, RtlGetElementGenericTableAvl, (
    _In_ PRTL_AVL_TABLE Table,
    _In_ ULONG I
))

NTDLL_API(ULONG, __stdcall, RtlNumberGenericTableElementsAvl, (
    _In_ PRTL_AVL_TABLE Table
))

_Check_return_
NTDLL_API(BOOLEAN, __stdcall, RtlIsGenericTableEmptyAvl, (
    _In_ PRTL_AVL_TABLE Table
))

typedef struct _RTL_SPLAY_LINKS
{
    struct _RTL_SPLAY_LINKS *Parent;
    struct _RTL_SPLAY_LINKS *LeftChild;
    struct _RTL_SPLAY_LINKS *RightChild;
} RTL_SPLAY_LINKS, *PRTL_SPLAY_LINKS;

#define RtlInitializeSplayLinks(Links) \
{ \
    PRTL_SPLAY_LINKS _SplayLinks; \
    _SplayLinks = (PRTL_SPLAY_LINKS)(Links); \
    _SplayLinks->Parent = _SplayLinks; \
    _SplayLinks->LeftChild = NULL; \
    _SplayLinks->RightChild = NULL; \
}

#define RtlParent(Links) ((PRTL_SPLAY_LINKS)(Links)->Parent)
#define RtlLeftChild(Links) ((PRTL_SPLAY_LINKS)(Links)->LeftChild)
#define RtlRightChild(Links) ((PRTL_SPLAY_LINKS)(Links)->RightChild)
#define RtlIsRoot(Links) ((RtlParent(Links) == (PRTL_SPLAY_LINKS)(Links)))
#define RtlIsLeftChild(Links) ((RtlLeftChild(RtlParent(Links)) == (PRTL_SPLAY_LINKS)(Links)))
#define RtlIsRightChild(Links) ((RtlRightChild(RtlParent(Links)) == (PRTL_SPLAY_LINKS)(Links)))

#define RtlInsertAsLeftChild(ParentLinks, ChildLinks) \
{ \
    PRTL_SPLAY_LINKS _SplayParent; \
    PRTL_SPLAY_LINKS _SplayChild; \
    _SplayParent = (PRTL_SPLAY_LINKS)(ParentLinks); \
    _SplayChild = (PRTL_SPLAY_LINKS)(ChildLinks); \
    _SplayParent->LeftChild = _SplayChild; \
    _SplayChild->Parent = _SplayParent; \
}

#define RtlInsertAsRightChild(ParentLinks, ChildLinks) \
{ \
    PRTL_SPLAY_LINKS _SplayParent; \
    PRTL_SPLAY_LINKS _SplayChild; \
    _SplayParent = (PRTL_SPLAY_LINKS)(ParentLinks); \
    _SplayChild = (PRTL_SPLAY_LINKS)(ChildLinks); \
    _SplayParent->RightChild = _SplayChild; \
    _SplayChild->Parent = _SplayParent; \
}

NTDLL_API(PRTL_SPLAY_LINKS, __stdcall, RtlSplay, (
    _Inout_ PRTL_SPLAY_LINKS Links
))

NTDLL_API(PRTL_SPLAY_LINKS, __stdcall, RtlDelete, (
    _In_ PRTL_SPLAY_LINKS Links
))

NTDLL_API_VOID(__stdcall, RtlDeleteNoSplay, (
    _In_ PRTL_SPLAY_LINKS Links,
    _Inout_ PRTL_SPLAY_LINKS *Root
))

_Check_return_
NTDLL_API(PRTL_SPLAY_LINKS, __stdcall, RtlSubtreeSuccessor, (
    _In_ PRTL_SPLAY_LINKS Links
))

_Check_return_
NTDLL_API(PRTL_SPLAY_LINKS, __stdcall, RtlSubtreePredecessor, (
    _In_ PRTL_SPLAY_LINKS Links
))

_Check_return_
NTDLL_API(PRTL_SPLAY_LINKS, __stdcall, RtlRealSuccessor, (
    _In_ PRTL_SPLAY_LINKS Links
))

_Check_return_
NTDLL_API(PRTL_SPLAY_LINKS, __stdcall, RtlRealPredecessor, (
    _In_ PRTL_SPLAY_LINKS Links
))

struct _RTL_GENERIC_TABLE;

typedef RTL_GENERIC_COMPARE_RESULTS (NTAPI *PRTL_GENERIC_COMPARE_ROUTINE)(
    _In_ struct _RTL_GENERIC_TABLE *Table,
    _In_ PVOID FirstStruct,
    _In_ PVOID SecondStruct
    );

typedef PVOID (NTAPI *PRTL_GENERIC_ALLOCATE_ROUTINE)(
    _In_ struct _RTL_GENERIC_TABLE *Table,
    _In_ CLONG ByteSize
    );

typedef VOID (NTAPI *PRTL_GENERIC_FREE_ROUTINE)(
    _In_ struct _RTL_GENERIC_TABLE *Table,
    _In_ _Post_invalid_ PVOID Buffer
    );

typedef struct _RTL_GENERIC_TABLE
{
    PRTL_SPLAY_LINKS TableRoot;
    LIST_ENTRY InsertOrderList;
    PLIST_ENTRY OrderedPointer;
    ULONG WhichOrderedElement;
    ULONG NumberGenericTableElements;
    PRTL_GENERIC_COMPARE_ROUTINE CompareRoutine;
    PRTL_GENERIC_ALLOCATE_ROUTINE AllocateRoutine;
    PRTL_GENERIC_FREE_ROUTINE FreeRoutine;
    PVOID TableContext;
} RTL_GENERIC_TABLE, *PRTL_GENERIC_TABLE;

NTDLL_API_VOID(__stdcall, RtlInitializeGenericTable, (
    _Out_ PRTL_GENERIC_TABLE Table,
    _In_ PRTL_GENERIC_COMPARE_ROUTINE CompareRoutine,
    _In_ PRTL_GENERIC_ALLOCATE_ROUTINE AllocateRoutine,
    _In_ PRTL_GENERIC_FREE_ROUTINE FreeRoutine,
    _In_opt_ PVOID TableContext
))

NTDLL_API(PVOID, __stdcall, RtlInsertElementGenericTable, (
    _In_ PRTL_GENERIC_TABLE Table,
    _In_reads_bytes_(BufferSize) PVOID Buffer,
    _In_ CLONG BufferSize,
    _Out_opt_ PBOOLEAN NewElement
))

NTDLL_API(PVOID, __stdcall, RtlInsertElementGenericTableFull, (
    _In_ PRTL_GENERIC_TABLE Table,
    _In_reads_bytes_(BufferSize) PVOID Buffer,
    _In_ CLONG BufferSize,
    _Out_opt_ PBOOLEAN NewElement,
    _In_ PVOID NodeOrParent,
    _In_ TABLE_SEARCH_RESULT SearchResult
))

NTDLL_API(BOOLEAN, __stdcall, RtlDeleteElementGenericTable, (
    _In_ PRTL_GENERIC_TABLE Table,
    _In_ PVOID Buffer
))

_Check_return_
NTDLL_API(PVOID, __stdcall, RtlLookupElementGenericTable, (
    _In_ PRTL_GENERIC_TABLE Table,
    _In_ PVOID Buffer
))

NTDLL_API(PVOID, __stdcall, RtlLookupElementGenericTableFull, (
    _In_ PRTL_GENERIC_TABLE Table,
    _In_ PVOID Buffer,
    _Out_ PVOID *NodeOrParent,
    _Out_ TABLE_SEARCH_RESULT *SearchResult
))

_Check_return_
NTDLL_API(PVOID, __stdcall, RtlEnumerateGenericTable, (
    _In_ PRTL_GENERIC_TABLE Table,
    _In_ BOOLEAN Restart
))

_Check_return_
NTDLL_API(PVOID, __stdcall, RtlEnumerateGenericTableWithoutSplaying, (
    _In_ PRTL_GENERIC_TABLE Table,
    _Inout_ PVOID *RestartKey
))

_Check_return_
NTDLL_API(PVOID, __stdcall, RtlGetElementGenericTable, (
    _In_ PRTL_GENERIC_TABLE Table,
    _In_ ULONG I
))

NTDLL_API(ULONG, __stdcall, RtlNumberGenericTableElements, (
    _In_ PRTL_GENERIC_TABLE Table
))

_Check_return_
NTDLL_API(BOOLEAN, __stdcall, RtlIsGenericTableEmpty, (
    _In_ PRTL_GENERIC_TABLE Table
))

// RB trees

typedef struct _RTL_RB_TREE
{
    PRTL_BALANCED_NODE Root;
    PRTL_BALANCED_NODE Min;
} RTL_RB_TREE, *PRTL_RB_TREE;

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_8)

// rev
NTDLL_API_VOID(__stdcall, RtlRbInsertNodeEx, (
    _In_ PRTL_RB_TREE Tree,
    _In_opt_ PRTL_BALANCED_NODE Parent,
    _In_ BOOLEAN Right,
    _Out_ PRTL_BALANCED_NODE Node
))

// rev
NTDLL_API_VOID(__stdcall, RtlRbRemoveNode, (
    _In_ PRTL_RB_TREE Tree,
    _In_ PRTL_BALANCED_NODE Node
))

#endif

// Hash tables

// begin_ntddk

#define RTL_HASH_ALLOCATED_HEADER 0x00000001
#define RTL_HASH_RESERVED_SIGNATURE 0

typedef struct _RTL_DYNAMIC_HASH_TABLE_ENTRY
{
    LIST_ENTRY Linkage;
    ULONG_PTR Signature;
} RTL_DYNAMIC_HASH_TABLE_ENTRY, *PRTL_DYNAMIC_HASH_TABLE_ENTRY;

#define HASH_ENTRY_KEY(x) ((x)->Signature)

typedef struct _RTL_DYNAMIC_HASH_TABLE_CONTEXT
{
    PLIST_ENTRY ChainHead;
    PLIST_ENTRY PrevLinkage;
    ULONG_PTR Signature;
} RTL_DYNAMIC_HASH_TABLE_CONTEXT, *PRTL_DYNAMIC_HASH_TABLE_CONTEXT;

typedef struct _RTL_DYNAMIC_HASH_TABLE_ENUMERATOR
{
    RTL_DYNAMIC_HASH_TABLE_ENTRY HashEntry;
    PLIST_ENTRY ChainHead;
    ULONG BucketIndex;
} RTL_DYNAMIC_HASH_TABLE_ENUMERATOR, *PRTL_DYNAMIC_HASH_TABLE_ENUMERATOR;

typedef struct _RTL_DYNAMIC_HASH_TABLE
{
    // Entries initialized at creation.
    ULONG Flags;
    ULONG Shift;

    // Entries used in bucket computation.
    ULONG TableSize;
    ULONG Pivot;
    ULONG DivisorMask;

    // Counters.
    ULONG NumEntries;
    ULONG NonEmptyBuckets;
    ULONG NumEnumerators;

    // The directory. This field is for internal use only.
    PVOID Directory;
} RTL_DYNAMIC_HASH_TABLE, *PRTL_DYNAMIC_HASH_TABLE;

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_7)

FORCEINLINE
VOID
RtlInitHashTableContext(
    _Inout_ PRTL_DYNAMIC_HASH_TABLE_CONTEXT Context
    )
{
    Context->ChainHead = NULL;
    Context->PrevLinkage = NULL;
}

FORCEINLINE
VOID
RtlInitHashTableContextFromEnumerator(
    _Inout_ PRTL_DYNAMIC_HASH_TABLE_CONTEXT Context,
    _In_ PRTL_DYNAMIC_HASH_TABLE_ENUMERATOR Enumerator
    )
{
    Context->ChainHead = Enumerator->ChainHead;
    Context->PrevLinkage = Enumerator->HashEntry.Linkage.Blink;
}

FORCEINLINE
VOID
RtlReleaseHashTableContext(
    _Inout_ PRTL_DYNAMIC_HASH_TABLE_CONTEXT Context
    )
{
    UNREFERENCED_PARAMETER(Context);
    return;
}

FORCEINLINE
ULONG
RtlTotalBucketsHashTable(
    _In_ PRTL_DYNAMIC_HASH_TABLE HashTable
    )
{
    return HashTable->TableSize;
}

FORCEINLINE
ULONG
RtlNonEmptyBucketsHashTable(
    _In_ PRTL_DYNAMIC_HASH_TABLE HashTable
    )
{
    return HashTable->NonEmptyBuckets;
}

FORCEINLINE
ULONG
RtlEmptyBucketsHashTable(
    _In_ PRTL_DYNAMIC_HASH_TABLE HashTable
    )
{
    return HashTable->TableSize - HashTable->NonEmptyBuckets;
}

FORCEINLINE
ULONG
RtlTotalEntriesHashTable(
    _In_ PRTL_DYNAMIC_HASH_TABLE HashTable
    )
{
    return HashTable->NumEntries;
}

FORCEINLINE
ULONG
RtlActiveEnumeratorsHashTable(
    _In_ PRTL_DYNAMIC_HASH_TABLE HashTable
    )
{
    return HashTable->NumEnumerators;
}

_Must_inspect_result_
NTDLL_API(BOOLEAN, __stdcall, RtlCreateHashTable, (
    _Inout_ _When_(*HashTable == NULL, __drv_allocatesMem(Mem)) PRTL_DYNAMIC_HASH_TABLE *HashTable,
    _In_ ULONG Shift,
    _In_ _Reserved_ ULONG Flags
))

NTDLL_API_VOID(__stdcall, RtlDeleteHashTable, (
    _In_ PRTL_DYNAMIC_HASH_TABLE HashTable
))

NTDLL_API(BOOLEAN, __stdcall, RtlInsertEntryHashTable, (
    _In_ PRTL_DYNAMIC_HASH_TABLE HashTable,
    _In_ PRTL_DYNAMIC_HASH_TABLE_ENTRY Entry,
    _In_ ULONG_PTR Signature,
    _Inout_opt_ PRTL_DYNAMIC_HASH_TABLE_CONTEXT Context
))

NTDLL_API(BOOLEAN, __stdcall, RtlRemoveEntryHashTable, (
    _In_ PRTL_DYNAMIC_HASH_TABLE HashTable,
    _In_ PRTL_DYNAMIC_HASH_TABLE_ENTRY Entry,
    _Inout_opt_ PRTL_DYNAMIC_HASH_TABLE_CONTEXT Context
))

_Must_inspect_result_
NTDLL_API(PRTL_DYNAMIC_HASH_TABLE_ENTRY, __stdcall, RtlLookupEntryHashTable, (
    _In_ PRTL_DYNAMIC_HASH_TABLE HashTable,
    _In_ ULONG_PTR Signature,
    _Out_opt_ PRTL_DYNAMIC_HASH_TABLE_CONTEXT Context
))

_Must_inspect_result_
NTDLL_API(PRTL_DYNAMIC_HASH_TABLE_ENTRY, __stdcall, RtlGetNextEntryHashTable, (
    _In_ PRTL_DYNAMIC_HASH_TABLE HashTable,
    _In_ PRTL_DYNAMIC_HASH_TABLE_CONTEXT Context
))

NTDLL_API(BOOLEAN, __stdcall, RtlInitEnumerationHashTable, (
    _In_ PRTL_DYNAMIC_HASH_TABLE HashTable,
    _Out_ PRTL_DYNAMIC_HASH_TABLE_ENUMERATOR Enumerator
))

_Must_inspect_result_
NTDLL_API(PRTL_DYNAMIC_HASH_TABLE_ENTRY, __stdcall, RtlEnumerateEntryHashTable, (
    _In_ PRTL_DYNAMIC_HASH_TABLE HashTable,
    _Inout_ PRTL_DYNAMIC_HASH_TABLE_ENUMERATOR Enumerator
))

NTDLL_API_VOID(__stdcall, RtlEndEnumerationHashTable, (
    _In_ PRTL_DYNAMIC_HASH_TABLE HashTable,
    _Inout_ PRTL_DYNAMIC_HASH_TABLE_ENUMERATOR Enumerator
))

NTDLL_API(BOOLEAN, __stdcall, RtlInitWeakEnumerationHashTable, (
    _In_ PRTL_DYNAMIC_HASH_TABLE HashTable,
    _Out_ PRTL_DYNAMIC_HASH_TABLE_ENUMERATOR Enumerator
))

_Must_inspect_result_
NTDLL_API(PRTL_DYNAMIC_HASH_TABLE_ENTRY, __stdcall, RtlWeaklyEnumerateEntryHashTable, (
    _In_ PRTL_DYNAMIC_HASH_TABLE HashTable,
    _Inout_ PRTL_DYNAMIC_HASH_TABLE_ENUMERATOR Enumerator
))

NTDLL_API_VOID(__stdcall, RtlEndWeakEnumerationHashTable, (
    _In_ PRTL_DYNAMIC_HASH_TABLE HashTable,
    _Inout_ PRTL_DYNAMIC_HASH_TABLE_ENUMERATOR Enumerator
))

NTDLL_API(BOOLEAN, __stdcall, RtlExpandHashTable, (
    _In_ PRTL_DYNAMIC_HASH_TABLE HashTable
))

NTDLL_API(BOOLEAN, __stdcall, RtlContractHashTable, (
    _In_ PRTL_DYNAMIC_HASH_TABLE HashTable
))

#endif

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_10_TH1)

NTDLL_API(BOOLEAN, __stdcall, RtlInitStrongEnumerationHashTable, (
    _In_ PRTL_DYNAMIC_HASH_TABLE HashTable,
    _Out_ PRTL_DYNAMIC_HASH_TABLE_ENUMERATOR Enumerator
))

_Must_inspect_result_
NTDLL_API(PRTL_DYNAMIC_HASH_TABLE_ENTRY, __stdcall, RtlStronglyEnumerateEntryHashTable, (
    _In_ PRTL_DYNAMIC_HASH_TABLE HashTable,
    _Inout_ PRTL_DYNAMIC_HASH_TABLE_ENUMERATOR Enumerator
))

NTDLL_API_VOID(__stdcall, RtlEndStrongEnumerationHashTable, (
    _In_ PRTL_DYNAMIC_HASH_TABLE HashTable,
    _Inout_ PRTL_DYNAMIC_HASH_TABLE_ENUMERATOR Enumerator
))

#endif

// end_ntddk

// Critical sections

NTDLL_API(NTSTATUS, __stdcall, RtlInitializeCriticalSection, (
    _Out_ PRTL_CRITICAL_SECTION CriticalSection
))

NTDLL_API(NTSTATUS, __stdcall, RtlInitializeCriticalSectionAndSpinCount, (
    _Inout_ PRTL_CRITICAL_SECTION CriticalSection,
    _In_ ULONG SpinCount
))

NTDLL_API(NTSTATUS, __stdcall, RtlDeleteCriticalSection, (
    _Inout_ PRTL_CRITICAL_SECTION CriticalSection
))

NTDLL_API(NTSTATUS, __stdcall, RtlEnterCriticalSection, (
    _Inout_ PRTL_CRITICAL_SECTION CriticalSection
))

NTDLL_API(NTSTATUS, __stdcall, RtlLeaveCriticalSection, (
    _Inout_ PRTL_CRITICAL_SECTION CriticalSection
))

NTDLL_API(LOGICAL, __stdcall, RtlTryEnterCriticalSection, (
    _Inout_ PRTL_CRITICAL_SECTION CriticalSection
))

NTDLL_API(LOGICAL, __stdcall, RtlIsCriticalSectionLocked, (
    _In_ PRTL_CRITICAL_SECTION CriticalSection
))

NTDLL_API(LOGICAL, __stdcall, RtlIsCriticalSectionLockedByThread, (
    _In_ PRTL_CRITICAL_SECTION CriticalSection
))

NTDLL_API(ULONG, __stdcall, RtlGetCriticalSectionRecursionCount, (
    _In_ PRTL_CRITICAL_SECTION CriticalSection
))

NTDLL_API(ULONG, __stdcall, RtlSetCriticalSectionSpinCount, (
    _Inout_ PRTL_CRITICAL_SECTION CriticalSection,
    _In_ ULONG SpinCount
))

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_VISTA)
// private
NTDLL_API(HANDLE, __stdcall, RtlQueryCriticalSectionOwner, (
    _In_ HANDLE EventHandle
))
#endif

NTDLL_API_VOID(__stdcall, RtlCheckForOrphanedCriticalSections, (
    _In_ HANDLE ThreadHandle
))

// Resources

typedef struct _RTL_RESOURCE
{
    RTL_CRITICAL_SECTION CriticalSection;

    HANDLE SharedSemaphore;
    volatile ULONG NumberOfWaitingShared;
    HANDLE ExclusiveSemaphore;
    volatile ULONG NumberOfWaitingExclusive;

    volatile LONG NumberOfActive; // negative: exclusive acquire; zero: not acquired; positive: shared acquire(s)
    HANDLE ExclusiveOwnerThread;

    ULONG Flags; // RTL_RESOURCE_FLAG_*

    PRTL_RESOURCE_DEBUG DebugInfo;
} RTL_RESOURCE, *PRTL_RESOURCE;

#define RTL_RESOURCE_FLAG_LONG_TERM ((ULONG)0x00000001)

NTDLL_API_VOID(__stdcall, RtlInitializeResource, (
    _Out_ PRTL_RESOURCE Resource
))

NTDLL_API_VOID(__stdcall, RtlDeleteResource, (
    _Inout_ PRTL_RESOURCE Resource
))

NTDLL_API(BOOLEAN, __stdcall, RtlAcquireResourceShared, (
    _Inout_ PRTL_RESOURCE Resource,
    _In_ BOOLEAN Wait
))

NTDLL_API(BOOLEAN, __stdcall, RtlAcquireResourceExclusive, (
    _Inout_ PRTL_RESOURCE Resource,
    _In_ BOOLEAN Wait
))

NTDLL_API_VOID(__stdcall, RtlReleaseResource, (
    _Inout_ PRTL_RESOURCE Resource
))

NTDLL_API_VOID(__stdcall, RtlConvertSharedToExclusive, (
    _Inout_ PRTL_RESOURCE Resource
))

NTDLL_API_VOID(__stdcall, RtlConvertExclusiveToShared, (
    _Inout_ PRTL_RESOURCE Resource
))

// Slim reader-writer locks, condition variables, and barriers

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_VISTA)

// winbase:InitializeSRWLock
NTDLL_API_VOID(__stdcall, RtlInitializeSRWLock, (
    _Out_ PRTL_SRWLOCK SRWLock
))

// winbase:AcquireSRWLockExclusive
NTDLL_API_VOID(__stdcall, RtlAcquireSRWLockExclusive, (
    _Inout_ PRTL_SRWLOCK SRWLock
))

// winbase:AcquireSRWLockShared
NTDLL_API_VOID(__stdcall, RtlAcquireSRWLockShared, (
    _Inout_ PRTL_SRWLOCK SRWLock
))

// winbase:ReleaseSRWLockExclusive
NTDLL_API_VOID(__stdcall, RtlReleaseSRWLockExclusive, (
    _Inout_ PRTL_SRWLOCK SRWLock
))

// winbase:ReleaseSRWLockShared
NTDLL_API_VOID(__stdcall, RtlReleaseSRWLockShared, (
    _Inout_ PRTL_SRWLOCK SRWLock
))

// winbase:TryAcquireSRWLockExclusive
NTDLL_API(BOOLEAN, __stdcall, RtlTryAcquireSRWLockExclusive, (
    _Inout_ PRTL_SRWLOCK SRWLock
))

// winbase:TryAcquireSRWLockShared
NTDLL_API(BOOLEAN, __stdcall, RtlTryAcquireSRWLockShared, (
    _Inout_ PRTL_SRWLOCK SRWLock
))

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_7)
// rev
NTDLL_API_VOID(__stdcall, RtlAcquireReleaseSRWLockExclusive, (
    _Inout_ PRTL_SRWLOCK SRWLock
))
#endif

#endif

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_VISTA)

// winbase:InitializeConditionVariable
NTDLL_API_VOID(__stdcall, RtlInitializeConditionVariable, (
    _Out_ PRTL_CONDITION_VARIABLE ConditionVariable
))

// private
NTDLL_API(NTSTATUS, __stdcall, RtlSleepConditionVariableCS, (
    _Inout_ PRTL_CONDITION_VARIABLE ConditionVariable,
    _Inout_ PRTL_CRITICAL_SECTION CriticalSection,
    _In_opt_ PLARGE_INTEGER Timeout
))

// private
NTDLL_API(NTSTATUS, __stdcall, RtlSleepConditionVariableSRW, (
    _Inout_ PRTL_CONDITION_VARIABLE ConditionVariable,
    _Inout_ PRTL_SRWLOCK SRWLock,
    _In_opt_ PLARGE_INTEGER Timeout,
    _In_ ULONG Flags
))

// winbase:WakeConditionVariable
NTDLL_API_VOID(__stdcall, RtlWakeConditionVariable, (
    _Inout_ PRTL_CONDITION_VARIABLE ConditionVariable
))

// winbase:WakeAllConditionVariable
NTDLL_API_VOID(__stdcall, RtlWakeAllConditionVariable, (
    _Inout_ PRTL_CONDITION_VARIABLE ConditionVariable
))

#endif

// begin_rev
#define RTL_BARRIER_FLAGS_SPIN_ONLY 0x00000001 // never block on event - always spin
#define RTL_BARRIER_FLAGS_BLOCK_ONLY 0x00000002 // always block on event - never spin
#define RTL_BARRIER_FLAGS_NO_DELETE 0x00000004 // use if barrier will never be deleted
// end_rev

// begin_private

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_VISTA)

NTDLL_API(NTSTATUS, __stdcall, RtlInitBarrier, (
    _Out_ PRTL_BARRIER Barrier,
    _In_ ULONG TotalThreads,
    _In_ ULONG SpinCount
))

NTDLL_API(NTSTATUS, __stdcall, RtlDeleteBarrier, (
    _In_ PRTL_BARRIER Barrier
))

NTDLL_API(BOOLEAN, __stdcall, RtlBarrier, (
    _Inout_ PRTL_BARRIER Barrier,
    _In_ ULONG Flags
))

NTDLL_API(BOOLEAN, __stdcall, RtlBarrierForDelete, (
    _Inout_ PRTL_BARRIER Barrier,
    _In_ ULONG Flags
))

#endif

// end_private

// Wait on address

// begin_rev

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_8)

NTDLL_API(NTSTATUS, __stdcall, RtlWaitOnAddress, (
    _In_ volatile VOID *Address,
    _In_ PVOID CompareAddress,
    _In_ SIZE_T AddressSize,
    _In_opt_ PLARGE_INTEGER Timeout
))

NTDLL_API_VOID(__stdcall, RtlWakeAddressAll, (
    _In_ PVOID Address
))

NTDLL_API_VOID(__stdcall, RtlWakeAddressSingle, (
    _In_ PVOID Address
))

#endif

// end_rev

// Strings

#ifndef PHNT_NO_INLINE_INIT_STRING
FORCEINLINE VOID RtlInitString(
    _Out_ PSTRING DestinationString,
    _In_opt_ PSTR SourceString
    )
{
    if (SourceString)
        DestinationString->MaximumLength = (DestinationString->Length = (USHORT)strlen(SourceString)) + 1;
    else
        DestinationString->MaximumLength = DestinationString->Length = 0;

    DestinationString->Buffer = SourceString;
}
#else
NTDLL_API_VOID(__stdcall, RtlInitString, (
    _Out_ PSTRING DestinationString,
    _In_opt_ PSTR SourceString
))
#endif

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_10_TH1)
NTDLL_API(NTSTATUS, __stdcall, RtlInitStringEx, (
    _Out_ PSTRING DestinationString,
    _In_opt_z_ PCSZ SourceString
))
#endif

#ifndef PHNT_NO_INLINE_INIT_STRING
FORCEINLINE VOID RtlInitAnsiString(
    _Out_ PANSI_STRING DestinationString,
    _In_opt_ PSTR SourceString
    )
{
    if (SourceString)
        DestinationString->MaximumLength = (DestinationString->Length = (USHORT)strlen(SourceString)) + 1;
    else
        DestinationString->MaximumLength = DestinationString->Length = 0;

    DestinationString->Buffer = SourceString;
}
#else
NTDLL_API_VOID(__stdcall, RtlInitAnsiString, (
    _Out_ PANSI_STRING DestinationString,
    _In_opt_ PSTR SourceString
))
#endif

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_XP)
NTDLL_API(NTSTATUS, __stdcall, RtlInitAnsiStringEx, (
    _Out_ PANSI_STRING DestinationString,
    _In_opt_z_ PCSZ SourceString
))
#endif

NTDLL_API_VOID(__stdcall, RtlFreeAnsiString, (
    _In_ PANSI_STRING AnsiString
))

NTDLL_API_VOID(__stdcall, RtlFreeOemString, (
    _In_ POEM_STRING OemString
))

NTDLL_API_VOID(__stdcall, RtlCopyString, (
    _In_ PSTRING DestinationString,
    _In_opt_ PSTRING SourceString
))

NTDLL_API(CHAR, __stdcall, RtlUpperChar, (
    _In_ CHAR Character
))

_Must_inspect_result_
NTDLL_API(LONG, __stdcall, RtlCompareString, (
    _In_ PSTRING String1,
    _In_ PSTRING String2,
    _In_ BOOLEAN CaseInSensitive
))

_Must_inspect_result_
NTDLL_API(BOOLEAN, __stdcall, RtlEqualString, (
    _In_ PSTRING String1,
    _In_ PSTRING String2,
    _In_ BOOLEAN CaseInSensitive
))

_Must_inspect_result_
NTDLL_API(BOOLEAN, __stdcall, RtlPrefixString, (
    _In_ PSTRING String1,
    _In_ PSTRING String2,
    _In_ BOOLEAN CaseInSensitive
))

NTDLL_API(NTSTATUS, __stdcall, RtlAppendStringToString, (
    _In_ PSTRING Destination,
    _In_ PSTRING Source
))

NTDLL_API(NTSTATUS, __stdcall, RtlAppendAsciizToString, (
    _In_ PSTRING Destination,
    _In_opt_ PSTR Source
))

NTDLL_API_VOID(__stdcall, RtlUpperString, (
    _In_ PSTRING DestinationString,
    _In_ PSTRING SourceString
))

FORCEINLINE
BOOLEAN
RtlIsNullOrEmptyUnicodeString(
    _In_opt_ PUNICODE_STRING String
    )
{
    return !String || String->Length == 0;
}

FORCEINLINE
VOID
NTAPI
RtlInitEmptyUnicodeString(
    _Out_ PUNICODE_STRING DestinationString,
    _In_opt_ PWCHAR Buffer,
    _In_ USHORT MaximumLength
    )
{
    DestinationString->Buffer = Buffer;
    DestinationString->MaximumLength = MaximumLength;
    DestinationString->Length = 0;
}

#ifndef PHNT_NO_INLINE_INIT_STRING
FORCEINLINE VOID RtlInitUnicodeString(
    _Out_ PUNICODE_STRING DestinationString,
    _In_opt_ PWSTR SourceString
    )
{
    if (SourceString)
        DestinationString->MaximumLength = (DestinationString->Length = (USHORT)(wcslen(SourceString) * sizeof(WCHAR))) + sizeof(UNICODE_NULL);
    else
        DestinationString->MaximumLength = DestinationString->Length = 0;

    DestinationString->Buffer = SourceString;
}
#else
NTDLL_API_VOID(__stdcall, RtlInitUnicodeString, (
    _Out_ PUNICODE_STRING DestinationString,
    _In_opt_ PWSTR SourceString
))
#endif

NTDLL_API(NTSTATUS, __stdcall, RtlInitUnicodeStringEx, (
    _Out_ PUNICODE_STRING DestinationString,
    _In_opt_ PWSTR SourceString
))

NTDLL_API(BOOLEAN, __stdcall, RtlCreateUnicodeString, (
    _Out_ PUNICODE_STRING DestinationString,
    _In_ PWSTR SourceString
))

NTDLL_API(BOOLEAN, __stdcall, RtlCreateUnicodeStringFromAsciiz, (
    _Out_ PUNICODE_STRING DestinationString,
    _In_ PSTR SourceString
))

NTDLL_API_VOID(__stdcall, RtlFreeUnicodeString, (
    _In_ PUNICODE_STRING UnicodeString
))

#define RTL_DUPLICATE_UNICODE_STRING_NULL_TERMINATE (0x00000001)
#define RTL_DUPLICATE_UNICODE_STRING_ALLOCATE_NULL_STRING (0x00000002)

NTDLL_API(NTSTATUS, __stdcall, RtlDuplicateUnicodeString, (
    _In_ ULONG Flags,
    _In_ PUNICODE_STRING StringIn,
    _Out_ PUNICODE_STRING StringOut
))

NTDLL_API_VOID(__stdcall, RtlCopyUnicodeString, (
    _In_ PUNICODE_STRING DestinationString,
    _In_ PUNICODE_STRING SourceString
))

NTDLL_API(WCHAR, __stdcall, RtlUpcaseUnicodeChar, (
    _In_ WCHAR SourceCharacter
))

NTDLL_API(WCHAR, __stdcall, RtlDowncaseUnicodeChar, (
    _In_ WCHAR SourceCharacter
))

_Must_inspect_result_
NTDLL_API(LONG, __stdcall, RtlCompareUnicodeString, (
    _In_ PUNICODE_STRING String1,
    _In_ PUNICODE_STRING String2,
    _In_ BOOLEAN CaseInSensitive
))

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_VISTA)
_Must_inspect_result_
NTDLL_API(LONG, __stdcall, RtlCompareUnicodeStrings, (
    _In_reads_(String1Length) PWCH String1,
    _In_ SIZE_T String1Length,
    _In_reads_(String2Length) PWCH String2,
    _In_ SIZE_T String2Length,
    _In_ BOOLEAN CaseInSensitive
))
#endif

_Must_inspect_result_
NTDLL_API(BOOLEAN, __stdcall, RtlEqualUnicodeString, (
    _In_ PUNICODE_STRING String1,
    _In_ PUNICODE_STRING String2,
    _In_ BOOLEAN CaseInSensitive
))

#define HASH_STRING_ALGORITHM_DEFAULT 0
#define HASH_STRING_ALGORITHM_X65599 1
#define HASH_STRING_ALGORITHM_INVALID 0xffffffff

NTDLL_API(NTSTATUS, __stdcall, RtlHashUnicodeString, (
    _In_ PUNICODE_STRING String,
    _In_ BOOLEAN CaseInSensitive,
    _In_ ULONG HashAlgorithm,
    _Out_ PULONG HashValue
))

NTDLL_API(NTSTATUS, __stdcall, RtlValidateUnicodeString, (
    _In_ ULONG Flags,
    _In_ PUNICODE_STRING String
))

_Must_inspect_result_
NTDLL_API(BOOLEAN, __stdcall, RtlPrefixUnicodeString, (
    _In_ PUNICODE_STRING String1,
    _In_ PUNICODE_STRING String2,
    _In_ BOOLEAN CaseInSensitive
))

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_10_TH1)
_Must_inspect_result_
NTDLL_API(BOOLEAN, __stdcall, RtlSuffixUnicodeString, (
    _In_ PUNICODE_STRING String1,
    _In_ PUNICODE_STRING String2,
    _In_ BOOLEAN CaseInSensitive
))
#endif

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_10_TH1)
_Must_inspect_result_
NTDLL_API(PWCHAR, __stdcall, RtlFindUnicodeSubstring, (
    _In_ PUNICODE_STRING FullString,
    _In_ PUNICODE_STRING SearchString,
    _In_ BOOLEAN CaseInSensitive
))
#endif

#define RTL_FIND_CHAR_IN_UNICODE_STRING_START_AT_END 0x00000001
#define RTL_FIND_CHAR_IN_UNICODE_STRING_COMPLEMENT_CHAR_SET 0x00000002
#define RTL_FIND_CHAR_IN_UNICODE_STRING_CASE_INSENSITIVE 0x00000004

NTDLL_API(NTSTATUS, __stdcall, RtlFindCharInUnicodeString, (
    _In_ ULONG Flags,
    _In_ PUNICODE_STRING StringToSearch,
    _In_ PUNICODE_STRING CharSet,
    _Out_ PUSHORT NonInclusivePrefixLength
))

NTDLL_API(NTSTATUS, __stdcall, RtlAppendUnicodeStringToString, (
    _In_ PUNICODE_STRING Destination,
    _In_ PUNICODE_STRING Source
))

NTDLL_API(NTSTATUS, __stdcall, RtlAppendUnicodeToString, (
    _In_ PUNICODE_STRING Destination,
    _In_opt_ PWSTR Source
))

NTDLL_API(NTSTATUS, __stdcall, RtlUpcaseUnicodeString, (
    _Inout_ PUNICODE_STRING DestinationString,
    _In_ PUNICODE_STRING SourceString,
    _In_ BOOLEAN AllocateDestinationString
))

NTDLL_API(NTSTATUS, __stdcall, RtlDowncaseUnicodeString, (
    _Inout_ PUNICODE_STRING DestinationString,
    _In_ PUNICODE_STRING SourceString,
    _In_ BOOLEAN AllocateDestinationString
))

NTDLL_API_VOID(__stdcall, RtlEraseUnicodeString, (
    _Inout_ PUNICODE_STRING String
))

NTDLL_API(NTSTATUS, __stdcall, RtlAnsiStringToUnicodeString, (
    _Inout_ PUNICODE_STRING DestinationString,
    _In_ PANSI_STRING SourceString,
    _In_ BOOLEAN AllocateDestinationString
))

NTDLL_API(NTSTATUS, __stdcall, RtlUnicodeStringToAnsiString, (
    _Inout_ PANSI_STRING DestinationString,
    _In_ PUNICODE_STRING SourceString,
    _In_ BOOLEAN AllocateDestinationString
))

NTDLL_API(WCHAR, __stdcall, RtlAnsiCharToUnicodeChar, (
    _Inout_ PUCHAR *SourceCharacter
))

NTDLL_API(NTSTATUS, __stdcall, RtlUpcaseUnicodeStringToAnsiString, (
    _Inout_ PANSI_STRING DestinationString,
    _In_ PUNICODE_STRING SourceString,
    _In_ BOOLEAN AllocateDestinationString
))

NTDLL_API(NTSTATUS, __stdcall, RtlOemStringToUnicodeString, (
    _Inout_ PUNICODE_STRING DestinationString,
    _In_ POEM_STRING SourceString,
    _In_ BOOLEAN AllocateDestinationString
))

NTDLL_API(NTSTATUS, __stdcall, RtlUnicodeStringToOemString, (
    _Inout_ POEM_STRING DestinationString,
    _In_ PUNICODE_STRING SourceString,
    _In_ BOOLEAN AllocateDestinationString
))

NTDLL_API(NTSTATUS, __stdcall, RtlUpcaseUnicodeStringToOemString, (
    _Inout_ POEM_STRING DestinationString,
    _In_ PUNICODE_STRING SourceString,
    _In_ BOOLEAN AllocateDestinationString
))

NTDLL_API(NTSTATUS, __stdcall, RtlUnicodeStringToCountedOemString, (
    _Inout_ POEM_STRING DestinationString,
    _In_ PUNICODE_STRING SourceString,
    _In_ BOOLEAN AllocateDestinationString
))

NTDLL_API(NTSTATUS, __stdcall, RtlUpcaseUnicodeStringToCountedOemString, (
    _Inout_ POEM_STRING DestinationString,
    _In_ PUNICODE_STRING SourceString,
    _In_ BOOLEAN AllocateDestinationString
))

NTDLL_API(NTSTATUS, __stdcall, RtlMultiByteToUnicodeN, (
    _Out_writes_bytes_to_(MaxBytesInUnicodeString, *BytesInUnicodeString) PWCH UnicodeString,
    _In_ ULONG MaxBytesInUnicodeString,
    _Out_opt_ PULONG BytesInUnicodeString,
    _In_reads_bytes_(BytesInMultiByteString) PSTR MultiByteString,
    _In_ ULONG BytesInMultiByteString
))

NTDLL_API(NTSTATUS, __stdcall, RtlMultiByteToUnicodeSize, (
    _Out_ PULONG BytesInUnicodeString,
    _In_reads_bytes_(BytesInMultiByteString) PSTR MultiByteString,
    _In_ ULONG BytesInMultiByteString
))

NTDLL_API(NTSTATUS, __stdcall, RtlUnicodeToMultiByteN, (
    _Out_writes_bytes_to_(MaxBytesInMultiByteString, *BytesInMultiByteString) PCHAR MultiByteString,
    _In_ ULONG MaxBytesInMultiByteString,
    _Out_opt_ PULONG BytesInMultiByteString,
    _In_reads_bytes_(BytesInUnicodeString) PWCH UnicodeString,
    _In_ ULONG BytesInUnicodeString
))

NTDLL_API(NTSTATUS, __stdcall, RtlUnicodeToMultiByteSize, (
    _Out_ PULONG BytesInMultiByteString,
    _In_reads_bytes_(BytesInUnicodeString) PWCH UnicodeString,
    _In_ ULONG BytesInUnicodeString
))

NTDLL_API(NTSTATUS, __stdcall, RtlUpcaseUnicodeToMultiByteN, (
    _Out_writes_bytes_to_(MaxBytesInMultiByteString, *BytesInMultiByteString) PCHAR MultiByteString,
    _In_ ULONG MaxBytesInMultiByteString,
    _Out_opt_ PULONG BytesInMultiByteString,
    _In_reads_bytes_(BytesInUnicodeString) PWCH UnicodeString,
    _In_ ULONG BytesInUnicodeString
))

NTDLL_API(NTSTATUS, __stdcall, RtlOemToUnicodeN, (
    _Out_writes_bytes_to_(MaxBytesInUnicodeString, *BytesInUnicodeString) PWSTR UnicodeString,
    _In_ ULONG MaxBytesInUnicodeString,
    _Out_opt_ PULONG BytesInUnicodeString,
    _In_reads_bytes_(BytesInOemString) PCH OemString,
    _In_ ULONG BytesInOemString
))

NTDLL_API(NTSTATUS, __stdcall, RtlUnicodeToOemN, (
    _Out_writes_bytes_to_(MaxBytesInOemString, *BytesInOemString) PCHAR OemString,
    _In_ ULONG MaxBytesInOemString,
    _Out_opt_ PULONG BytesInOemString,
    _In_reads_bytes_(BytesInUnicodeString) PWCH UnicodeString,
    _In_ ULONG BytesInUnicodeString
))

NTDLL_API(NTSTATUS, __stdcall, RtlUpcaseUnicodeToOemN, (
    _Out_writes_bytes_to_(MaxBytesInOemString, *BytesInOemString) PCHAR OemString,
    _In_ ULONG MaxBytesInOemString,
    _Out_opt_ PULONG BytesInOemString,
    _In_reads_bytes_(BytesInUnicodeString) PWCH UnicodeString,
    _In_ ULONG BytesInUnicodeString
))

NTDLL_API(NTSTATUS, __stdcall, RtlConsoleMultiByteToUnicodeN, (
    _Out_writes_bytes_to_(MaxBytesInUnicodeString, *BytesInUnicodeString) PWCH UnicodeString,
    _In_ ULONG MaxBytesInUnicodeString,
    _Out_opt_ PULONG BytesInUnicodeString,
    _In_reads_bytes_(BytesInMultiByteString) PCH MultiByteString,
    _In_ ULONG BytesInMultiByteString,
    _Out_ PULONG pdwSpecialChar
))

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_7)
NTDLL_API(NTSTATUS, __stdcall, RtlUTF8ToUnicodeN, (
    _Out_writes_bytes_to_(UnicodeStringMaxByteCount, *UnicodeStringActualByteCount) PWSTR UnicodeStringDestination,
    _In_ ULONG UnicodeStringMaxByteCount,
    _Out_ PULONG UnicodeStringActualByteCount,
    _In_reads_bytes_(UTF8StringByteCount) PCH UTF8StringSource,
    _In_ ULONG UTF8StringByteCount
))
#endif

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_7)
NTDLL_API(NTSTATUS, __stdcall, RtlUnicodeToUTF8N, (
    _Out_writes_bytes_to_(UTF8StringMaxByteCount, *UTF8StringActualByteCount) PCHAR UTF8StringDestination,
    _In_ ULONG UTF8StringMaxByteCount,
    _Out_ PULONG UTF8StringActualByteCount,
    _In_reads_bytes_(UnicodeStringByteCount) PWCH UnicodeStringSource,
    _In_ ULONG UnicodeStringByteCount
))
#endif

NTDLL_API(NTSTATUS, __stdcall, RtlCustomCPToUnicodeN, (
    _In_ PCPTABLEINFO CustomCP,
    _Out_writes_bytes_to_(MaxBytesInUnicodeString, *BytesInUnicodeString) PWCH UnicodeString,
    _In_ ULONG MaxBytesInUnicodeString,
    _Out_opt_ PULONG BytesInUnicodeString,
    _In_reads_bytes_(BytesInCustomCPString) PCH CustomCPString,
    _In_ ULONG BytesInCustomCPString
))

NTDLL_API(NTSTATUS, __stdcall, RtlUnicodeToCustomCPN, (
    _In_ PCPTABLEINFO CustomCP,
    _Out_writes_bytes_to_(MaxBytesInCustomCPString, *BytesInCustomCPString) PCH CustomCPString,
    _In_ ULONG MaxBytesInCustomCPString,
    _Out_opt_ PULONG BytesInCustomCPString,
    _In_reads_bytes_(BytesInUnicodeString) PWCH UnicodeString,
    _In_ ULONG BytesInUnicodeString
))

NTDLL_API(NTSTATUS, __stdcall, RtlUpcaseUnicodeToCustomCPN, (
    _In_ PCPTABLEINFO CustomCP,
    _Out_writes_bytes_to_(MaxBytesInCustomCPString, *BytesInCustomCPString) PCH CustomCPString,
    _In_ ULONG MaxBytesInCustomCPString,
    _Out_opt_ PULONG BytesInCustomCPString,
    _In_reads_bytes_(BytesInUnicodeString) PWCH UnicodeString,
    _In_ ULONG BytesInUnicodeString
))

NTDLL_API_VOID(__stdcall, RtlInitCodePageTable, (
    _In_ PUSHORT TableBase,
    _Out_ PCPTABLEINFO CodePageTable
))

NTDLL_API_VOID(__stdcall, RtlInitNlsTables, (
    _In_ PUSHORT AnsiNlsBase,
    _In_ PUSHORT OemNlsBase,
    _In_ PUSHORT LanguageNlsBase,
    _Out_ PNLSTABLEINFO TableInfo // PCPTABLEINFO?
))

NTDLL_API_VOID(__stdcall, RtlResetRtlTranslations, (
    _In_ PNLSTABLEINFO TableInfo
))

NTDLL_API(BOOLEAN, __stdcall, RtlIsTextUnicode, (
    _In_ PVOID Buffer,
    _In_ ULONG Size,
    _Inout_opt_ PULONG Result
))

typedef enum _RTL_NORM_FORM
{
    NormOther = 0x0,
    NormC = 0x1,
    NormD = 0x2,
    NormKC = 0x5,
    NormKD = 0x6,
    NormIdna = 0xd,
    DisallowUnassigned = 0x100,
    NormCDisallowUnassigned = 0x101,
    NormDDisallowUnassigned = 0x102,
    NormKCDisallowUnassigned = 0x105,
    NormKDDisallowUnassigned = 0x106,
    NormIdnaDisallowUnassigned = 0x10d
} RTL_NORM_FORM;

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_VISTA)
NTDLL_API(NTSTATUS, __stdcall, RtlNormalizeString, (
    _In_ ULONG NormForm, // RTL_NORM_FORM
    _In_ PCWSTR SourceString,
    _In_ LONG SourceStringLength,
    _Out_writes_to_(*DestinationStringLength, *DestinationStringLength) PWSTR DestinationString,
    _Inout_ PLONG DestinationStringLength
))
#endif

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_VISTA)
NTDLL_API(NTSTATUS, __stdcall, RtlIsNormalizedString, (
    _In_ ULONG NormForm, // RTL_NORM_FORM
    _In_ PCWSTR SourceString,
    _In_ LONG SourceStringLength,
    _Out_ PBOOLEAN Normalized
))
#endif

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_7)
// ntifs:FsRtlIsNameInExpression
NTDLL_API(BOOLEAN, __stdcall, RtlIsNameInExpression, (
    _In_ PUNICODE_STRING Expression,
    _In_ PUNICODE_STRING Name,
    _In_ BOOLEAN IgnoreCase,
    _In_opt_ PWCH UpcaseTable
))
#endif

#if (NTLIB_WIN_VERSION >= NTLIB_WIN_10_RS14)
// rev
NTDLL_API(BOOLEAN, __stdcall, RtlIsNameInUnUpcasedExpression, (
    _In_ PUNICODE_STRING Expression,
    _In_ PUNICODE_STRING Name,
    _In_ BOOLEAN IgnoreCase,
    _In_opt_ PWCH UpcaseTable
))
#endif

NTDLL_API(BOOLEAN, __stdcall, RtlEqualDomainName, (
    _In_ PUNICODE_STRING String1,
    _In_ PUNICODE_STRING String2
))

NTDLL_API(BOOLEAN, __stdcall, RtlEqualComputerName, (
    _In_ PUNICODE_STRING String1,
    _In_ PUNICODE_STRING String2
))

NTDLL_API(NTSTATUS, __stdcall, RtlDnsHostNameToComputerName, (
    _Out_ PUNICODE_STRING ComputerNameString,
    _In_ PUNICODE_STRING DnsHostNameString,
    _In_ BOOLEAN AllocateComputerNameString
))

NTDLL_API(NTSTATUS, __stdcall, RtlStringFromGUID, (
    _In_ PGUID Guid,
    _Out_ PUNICODE_STRING GuidString
))

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_8_1)

// rev
NTDLL_API(NTSTATUS, __stdcall, RtlStringFromGUIDEx, (
    _In_ PGUID Guid,
    _Inout_ PUNICODE_STRING GuidString,
    _In_ BOOLEAN AllocateGuidString
))

#endif

NTDLL_API(NTSTATUS, __stdcall, RtlGUIDFromString, (
    _In_ PUNICODE_STRING GuidString,
    _Out_ PGUID Guid
))

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_VISTA)

NTDLL_API(LONG, __stdcall, RtlCompareAltitudes, (
    _In_ PUNICODE_STRING Altitude1,
    _In_ PUNICODE_STRING Altitude2
))

NTDLL_API(NTSTATUS, __stdcall, RtlIdnToAscii, (
    _In_ ULONG Flags,
    _In_ PWSTR SourceString,
    _In_ LONG SourceStringLength,
    _Out_writes_to_(*DestinationStringLength, *DestinationStringLength) PWSTR DestinationString,
    _Inout_ PLONG DestinationStringLength
))

NTDLL_API(NTSTATUS, __stdcall, RtlIdnToUnicode, (
    _In_ ULONG Flags,
    _In_ PWSTR SourceString,
    _In_ LONG SourceStringLength,
    _Out_writes_to_(*DestinationStringLength, *DestinationStringLength) PWSTR DestinationString,
    _Inout_ PLONG DestinationStringLength
))

NTDLL_API(NTSTATUS, __stdcall, RtlIdnToNameprepUnicode, (
    _In_ ULONG Flags,
    _In_ PWSTR SourceString,
    _In_ LONG SourceStringLength,
    _Out_writes_to_(*DestinationStringLength, *DestinationStringLength) PWSTR DestinationString,
    _Inout_ PLONG DestinationStringLength
))

#endif

// Prefix

typedef struct _PREFIX_TABLE_ENTRY
{
    CSHORT NodeTypeCode;
    CSHORT NameLength;
    struct _PREFIX_TABLE_ENTRY *NextPrefixTree;
    RTL_SPLAY_LINKS Links;
    PSTRING Prefix;
} PREFIX_TABLE_ENTRY, *PPREFIX_TABLE_ENTRY;

typedef struct _PREFIX_TABLE
{
    CSHORT NodeTypeCode;
    CSHORT NameLength;
    PPREFIX_TABLE_ENTRY NextPrefixTree;
} PREFIX_TABLE, *PPREFIX_TABLE;

NTDLL_API_VOID(__stdcall, PfxInitialize, (
    _Out_ PPREFIX_TABLE PrefixTable
))

NTDLL_API(BOOLEAN, __stdcall, PfxInsertPrefix, (
    _In_ PPREFIX_TABLE PrefixTable,
    _In_ PSTRING Prefix,
    _Out_ PPREFIX_TABLE_ENTRY PrefixTableEntry
))

NTDLL_API_VOID(__stdcall, PfxRemovePrefix, (
    _In_ PPREFIX_TABLE PrefixTable,
    _In_ PPREFIX_TABLE_ENTRY PrefixTableEntry
))

NTDLL_API(PPREFIX_TABLE_ENTRY, __stdcall, PfxFindPrefix, (
    _In_ PPREFIX_TABLE PrefixTable,
    _In_ PSTRING FullName
))

typedef struct _UNICODE_PREFIX_TABLE_ENTRY
{
    CSHORT NodeTypeCode;
    CSHORT NameLength;
    struct _UNICODE_PREFIX_TABLE_ENTRY *NextPrefixTree;
    struct _UNICODE_PREFIX_TABLE_ENTRY *CaseMatch;
    RTL_SPLAY_LINKS Links;
    PUNICODE_STRING Prefix;
} UNICODE_PREFIX_TABLE_ENTRY, *PUNICODE_PREFIX_TABLE_ENTRY;

typedef struct _UNICODE_PREFIX_TABLE
{
    CSHORT NodeTypeCode;
    CSHORT NameLength;
    PUNICODE_PREFIX_TABLE_ENTRY NextPrefixTree;
    PUNICODE_PREFIX_TABLE_ENTRY LastNextEntry;
} UNICODE_PREFIX_TABLE, *PUNICODE_PREFIX_TABLE;

NTDLL_API_VOID(__stdcall, RtlInitializeUnicodePrefix, (
    _Out_ PUNICODE_PREFIX_TABLE PrefixTable
))

NTDLL_API(BOOLEAN, __stdcall, RtlInsertUnicodePrefix, (
    _In_ PUNICODE_PREFIX_TABLE PrefixTable,
    _In_ PUNICODE_STRING Prefix,
    _Out_ PUNICODE_PREFIX_TABLE_ENTRY PrefixTableEntry
))

NTDLL_API_VOID(__stdcall, RtlRemoveUnicodePrefix, (
    _In_ PUNICODE_PREFIX_TABLE PrefixTable,
    _In_ PUNICODE_PREFIX_TABLE_ENTRY PrefixTableEntry
))

NTDLL_API(PUNICODE_PREFIX_TABLE_ENTRY, __stdcall, RtlFindUnicodePrefix, (
    _In_ PUNICODE_PREFIX_TABLE PrefixTable,
    _In_ PUNICODE_STRING FullName,
    _In_ ULONG CaseInsensitiveIndex
))

NTDLL_API(PUNICODE_PREFIX_TABLE_ENTRY, __stdcall, RtlNextUnicodePrefix, (
    _In_ PUNICODE_PREFIX_TABLE PrefixTable,
    _In_ BOOLEAN Restart
))

// Compression

typedef struct _COMPRESSED_DATA_INFO
{
    USHORT CompressionFormatAndEngine; // COMPRESSION_FORMAT_* and COMPRESSION_ENGINE_*

    UCHAR CompressionUnitShift;
    UCHAR ChunkShift;
    UCHAR ClusterShift;
    UCHAR Reserved;

    USHORT NumberOfChunks;

    ULONG CompressedChunkSizes[1];
} COMPRESSED_DATA_INFO, *PCOMPRESSED_DATA_INFO;

NTDLL_API(NTSTATUS, __stdcall, RtlGetCompressionWorkSpaceSize, (
    _In_ USHORT CompressionFormatAndEngine,
    _Out_ PULONG CompressBufferWorkSpaceSize,
    _Out_ PULONG CompressFragmentWorkSpaceSize
))

NTDLL_API(NTSTATUS, __stdcall, RtlCompressBuffer, (
    _In_ USHORT CompressionFormatAndEngine,
    _In_reads_bytes_(UncompressedBufferSize) PUCHAR UncompressedBuffer,
    _In_ ULONG UncompressedBufferSize,
    _Out_writes_bytes_to_(CompressedBufferSize, *FinalCompressedSize) PUCHAR CompressedBuffer,
    _In_ ULONG CompressedBufferSize,
    _In_ ULONG UncompressedChunkSize,
    _Out_ PULONG FinalCompressedSize,
    _In_ PVOID WorkSpace
))

NTDLL_API(NTSTATUS, __stdcall, RtlDecompressBuffer, (
    _In_ USHORT CompressionFormat,
    _Out_writes_bytes_to_(UncompressedBufferSize, *FinalUncompressedSize) PUCHAR UncompressedBuffer,
    _In_ ULONG UncompressedBufferSize,
    _In_reads_bytes_(CompressedBufferSize) PUCHAR CompressedBuffer,
    _In_ ULONG CompressedBufferSize,
    _Out_ PULONG FinalUncompressedSize
))

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_8)
NTDLL_API(NTSTATUS, __stdcall, RtlDecompressBufferEx, (
    _In_ USHORT CompressionFormat,
    _Out_writes_bytes_to_(UncompressedBufferSize, *FinalUncompressedSize) PUCHAR UncompressedBuffer,
    _In_ ULONG UncompressedBufferSize,
    _In_reads_bytes_(CompressedBufferSize) PUCHAR CompressedBuffer,
    _In_ ULONG CompressedBufferSize,
    _Out_ PULONG FinalUncompressedSize,
    _In_ PVOID WorkSpace
))
#endif

NTDLL_API(NTSTATUS, __stdcall, RtlDecompressFragment, (
    _In_ USHORT CompressionFormat,
    _Out_writes_bytes_to_(UncompressedFragmentSize, *FinalUncompressedSize) PUCHAR UncompressedFragment,
    _In_ ULONG UncompressedFragmentSize,
    _In_reads_bytes_(CompressedBufferSize) PUCHAR CompressedBuffer,
    _In_ ULONG CompressedBufferSize,
    _In_range_(<, CompressedBufferSize) ULONG FragmentOffset,
    _Out_ PULONG FinalUncompressedSize,
    _In_ PVOID WorkSpace
))

NTDLL_API(NTSTATUS, __stdcall, RtlDescribeChunk, (
    _In_ USHORT CompressionFormat,
    _Inout_ PUCHAR *CompressedBuffer,
    _In_ PUCHAR EndOfCompressedBufferPlus1,
    _Out_ PUCHAR *ChunkBuffer,
    _Out_ PULONG ChunkSize
))

NTDLL_API(NTSTATUS, __stdcall, RtlReserveChunk, (
    _In_ USHORT CompressionFormat,
    _Inout_ PUCHAR *CompressedBuffer,
    _In_ PUCHAR EndOfCompressedBufferPlus1,
    _Out_ PUCHAR *ChunkBuffer,
    _In_ ULONG ChunkSize
))

NTDLL_API(NTSTATUS, __stdcall, RtlDecompressChunks, (
    _Out_writes_bytes_(UncompressedBufferSize) PUCHAR UncompressedBuffer,
    _In_ ULONG UncompressedBufferSize,
    _In_reads_bytes_(CompressedBufferSize) PUCHAR CompressedBuffer,
    _In_ ULONG CompressedBufferSize,
    _In_reads_bytes_(CompressedTailSize) PUCHAR CompressedTail,
    _In_ ULONG CompressedTailSize,
    _In_ PCOMPRESSED_DATA_INFO CompressedDataInfo
))

NTDLL_API(NTSTATUS, __stdcall, RtlCompressChunks, (
    _In_reads_bytes_(UncompressedBufferSize) PUCHAR UncompressedBuffer,
    _In_ ULONG UncompressedBufferSize,
    _Out_writes_bytes_(CompressedBufferSize) PUCHAR CompressedBuffer,
    _In_range_(>=, (UncompressedBufferSize - (UncompressedBufferSize / 16))) ULONG CompressedBufferSize,
    _Inout_updates_bytes_(CompressedDataInfoLength) PCOMPRESSED_DATA_INFO CompressedDataInfo,
    _In_range_(>, sizeof(COMPRESSED_DATA_INFO)) ULONG CompressedDataInfoLength,
    _In_ PVOID WorkSpace
))

// Locale

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_VISTA)

// private
NTDLL_API(NTSTATUS, __stdcall, RtlConvertLCIDToString, (
    _In_ LCID LcidValue,
    _In_ ULONG Base,
    _In_ ULONG Padding, // string is padded to this width
    _Out_writes_(Size) PWSTR pResultBuf,
    _In_ ULONG Size
))

// private
NTDLL_API(BOOLEAN, __stdcall, RtlIsValidLocaleName, (
    _In_ PWSTR LocaleName,
    _In_ ULONG Flags
))

// private
NTDLL_API(NTSTATUS, __stdcall, RtlGetParentLocaleName, (
    _In_ PWSTR LocaleName,
    _Inout_ PUNICODE_STRING ParentLocaleName,
    _In_ ULONG Flags,
    _In_ BOOLEAN AllocateDestinationString
))

// private
NTDLL_API(NTSTATUS, __stdcall, RtlLcidToLocaleName, (
    _In_ LCID lcid, // sic
    _Inout_ PUNICODE_STRING LocaleName,
    _In_ ULONG Flags,
    _In_ BOOLEAN AllocateDestinationString
))

// private
NTDLL_API(NTSTATUS, __stdcall, RtlLocaleNameToLcid, (
    _In_ PWSTR LocaleName,
    _Out_ PLCID lcid,
    _In_ ULONG Flags
))

// private
NTDLL_API(BOOLEAN, __stdcall, RtlLCIDToCultureName, (
    _In_ LCID Lcid,
    _Inout_ PUNICODE_STRING String
))

// private
NTDLL_API(BOOLEAN, __stdcall, RtlCultureNameToLCID, (
    _In_ PUNICODE_STRING String,
    _Out_ PLCID Lcid
))

// private
NTDLL_API_VOID(__stdcall, RtlCleanUpTEBLangLists, (
    VOID
))

#endif

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_7)

// rev
NTDLL_API(NTSTATUS, __stdcall, RtlGetLocaleFileMappingAddress, (
    _Out_ PVOID *BaseAddress,
    _Out_ PLCID DefaultLocaleId,
    _Out_ PLARGE_INTEGER DefaultCasingTableSize
))

#endif

// PEB

NTDLL_API(PPEB, __stdcall, RtlGetCurrentPeb, (
    VOID
))

NTDLL_API_VOID(__stdcall, RtlAcquirePebLock, (
    VOID
))

NTDLL_API_VOID(__stdcall, RtlReleasePebLock, (
    VOID
))

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_VISTA)
// private
NTDLL_API(LOGICAL, __stdcall, RtlTryAcquirePebLock, (
    VOID
))
#endif

NTDLL_API(NTSTATUS, __stdcall, RtlAllocateFromPeb, (
    _In_ ULONG Size,
    _Out_ PVOID *Block
))

NTDLL_API(NTSTATUS, __stdcall, RtlFreeToPeb, (
    _In_ PVOID Block,
    _In_ ULONG Size
))

// Processes

#define DOS_MAX_COMPONENT_LENGTH 255
#define DOS_MAX_PATH_LENGTH (DOS_MAX_COMPONENT_LENGTH + 5)

typedef struct _CURDIR
{
    UNICODE_STRING DosPath;
    HANDLE Handle;
} CURDIR, *PCURDIR;

#define RTL_USER_PROC_CURDIR_CLOSE 0x00000002
#define RTL_USER_PROC_CURDIR_INHERIT 0x00000003

typedef struct _RTL_DRIVE_LETTER_CURDIR
{
    USHORT Flags;
    USHORT Length;
    ULONG TimeStamp;
    STRING DosPath;
} RTL_DRIVE_LETTER_CURDIR, *PRTL_DRIVE_LETTER_CURDIR;

#define RTL_MAX_DRIVE_LETTERS 32
#define RTL_DRIVE_LETTER_VALID (USHORT)0x0001

typedef struct _RTL_USER_PROCESS_PARAMETERS
{
    ULONG MaximumLength;
    ULONG Length;

    ULONG Flags;
    ULONG DebugFlags;

    HANDLE ConsoleHandle;
    ULONG ConsoleFlags;
    HANDLE StandardInput;
    HANDLE StandardOutput;
    HANDLE StandardError;

    CURDIR CurrentDirectory;
    UNICODE_STRING DllPath;
    UNICODE_STRING ImagePathName;
    UNICODE_STRING CommandLine;
    PVOID Environment;

    ULONG StartingX;
    ULONG StartingY;
    ULONG CountX;
    ULONG CountY;
    ULONG CountCharsX;
    ULONG CountCharsY;
    ULONG FillAttribute;

    ULONG WindowFlags;
    ULONG ShowWindowFlags;
    UNICODE_STRING WindowTitle;
    UNICODE_STRING DesktopInfo;
    UNICODE_STRING ShellInfo;
    UNICODE_STRING RuntimeData;
    RTL_DRIVE_LETTER_CURDIR CurrentDirectories[RTL_MAX_DRIVE_LETTERS];

    ULONG_PTR EnvironmentSize;
    ULONG_PTR EnvironmentVersion;
    PVOID PackageDependencyData;
    ULONG ProcessGroupId;
    ULONG LoaderThreads;

    UNICODE_STRING RedirectionDllName; // REDSTONE4
    UNICODE_STRING HeapPartitionName; // 19H1
    ULONG_PTR DefaultThreadpoolCpuSetMasks;
    ULONG DefaultThreadpoolCpuSetMaskCount;
} RTL_USER_PROCESS_PARAMETERS, *PRTL_USER_PROCESS_PARAMETERS;

#define RTL_USER_PROC_PARAMS_NORMALIZED 0x00000001
#define RTL_USER_PROC_PROFILE_USER 0x00000002
#define RTL_USER_PROC_PROFILE_KERNEL 0x00000004
#define RTL_USER_PROC_PROFILE_SERVER 0x00000008
#define RTL_USER_PROC_RESERVE_1MB 0x00000020
#define RTL_USER_PROC_RESERVE_16MB 0x00000040
#define RTL_USER_PROC_CASE_SENSITIVE 0x00000080
#define RTL_USER_PROC_DISABLE_HEAP_DECOMMIT 0x00000100
#define RTL_USER_PROC_DLL_REDIRECTION_LOCAL 0x00001000
#define RTL_USER_PROC_APP_MANIFEST_PRESENT 0x00002000
#define RTL_USER_PROC_IMAGE_KEY_MISSING 0x00004000
#define RTL_USER_PROC_OPTIN_PROCESS 0x00020000

NTDLL_API(NTSTATUS, __stdcall, RtlCreateProcessParameters, (
    _Out_ PRTL_USER_PROCESS_PARAMETERS *pProcessParameters,
    _In_ PUNICODE_STRING ImagePathName,
    _In_opt_ PUNICODE_STRING DllPath,
    _In_opt_ PUNICODE_STRING CurrentDirectory,
    _In_opt_ PUNICODE_STRING CommandLine,
    _In_opt_ PVOID Environment,
    _In_opt_ PUNICODE_STRING WindowTitle,
    _In_opt_ PUNICODE_STRING DesktopInfo,
    _In_opt_ PUNICODE_STRING ShellInfo,
    _In_opt_ PUNICODE_STRING RuntimeData
))

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_VISTA)
// private
NTDLL_API(NTSTATUS, __stdcall, RtlCreateProcessParametersEx, (
    _Out_ PRTL_USER_PROCESS_PARAMETERS *pProcessParameters,
    _In_ PUNICODE_STRING ImagePathName,
    _In_opt_ PUNICODE_STRING DllPath,
    _In_opt_ PUNICODE_STRING CurrentDirectory,
    _In_opt_ PUNICODE_STRING CommandLine,
    _In_opt_ PVOID Environment,
    _In_opt_ PUNICODE_STRING WindowTitle,
    _In_opt_ PUNICODE_STRING DesktopInfo,
    _In_opt_ PUNICODE_STRING ShellInfo,
    _In_opt_ PUNICODE_STRING RuntimeData,
    _In_ ULONG Flags // pass RTL_USER_PROC_PARAMS_NORMALIZED to keep parameters normalized
))
#endif

NTDLL_API(NTSTATUS, __stdcall, RtlDestroyProcessParameters, (
    _In_ _Post_invalid_ PRTL_USER_PROCESS_PARAMETERS ProcessParameters
))

NTDLL_API(PRTL_USER_PROCESS_PARAMETERS, __stdcall, RtlNormalizeProcessParams, (
    _Inout_ PRTL_USER_PROCESS_PARAMETERS ProcessParameters
))

NTDLL_API(PRTL_USER_PROCESS_PARAMETERS, __stdcall, RtlDeNormalizeProcessParams, (
    _Inout_ PRTL_USER_PROCESS_PARAMETERS ProcessParameters
))

typedef struct _RTL_USER_PROCESS_INFORMATION
{
    ULONG Length;
    HANDLE Process;
    HANDLE Thread;
    CLIENT_ID ClientId;
    SECTION_IMAGE_INFORMATION ImageInformation;
} RTL_USER_PROCESS_INFORMATION, *PRTL_USER_PROCESS_INFORMATION;

// private
NTDLL_API(NTSTATUS, __stdcall, RtlCreateUserProcess, (
    _In_ PUNICODE_STRING NtImagePathName,
    _In_ ULONG AttributesDeprecated,
    _In_ PRTL_USER_PROCESS_PARAMETERS ProcessParameters,
    _In_opt_ PSECURITY_DESCRIPTOR ProcessSecurityDescriptor,
    _In_opt_ PSECURITY_DESCRIPTOR ThreadSecurityDescriptor,
    _In_opt_ HANDLE ParentProcess,
    _In_ BOOLEAN InheritHandles,
    _In_opt_ HANDLE DebugPort,
    _In_opt_ HANDLE TokenHandle, // used to be ExceptionPort
    _Out_ PRTL_USER_PROCESS_INFORMATION ProcessInformation
))

NTDLL_API(NTSTATUS, __stdcall, RtlCreateUserProcessEx, (
    _In_ PUNICODE_STRING NtImagePathName,
    _In_ PRTL_USER_PROCESS_PARAMETERS ProcessParameters,
    _In_ BOOLEAN InheritHandles,
    _Reserved_ ULONG Flags,
    _Out_ PRTL_USER_PROCESS_INFORMATION ProcessInformation
))

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_VISTA)
DECLSPEC_NORETURN
NTDLL_API_VOID(__stdcall, RtlExitUserProcess, (
    _In_ NTSTATUS ExitStatus
))
#else

#define RtlExitUserProcess RtlExitUserProcess_R

DECLSPEC_NORETURN
FORCEINLINE VOID RtlExitUserProcess_R(
    _In_ NTSTATUS ExitStatus
    )
{
    ExitProcess(ExitStatus);
}

#endif

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_VISTA)

// begin_rev
#define RTL_CLONE_PROCESS_FLAGS_CREATE_SUSPENDED 0x00000001
#define RTL_CLONE_PROCESS_FLAGS_INHERIT_HANDLES 0x00000002
#define RTL_CLONE_PROCESS_FLAGS_NO_SYNCHRONIZE 0x00000004 // don't update synchronization objects
// end_rev

// private
NTDLL_API(NTSTATUS, __stdcall, RtlCloneUserProcess, (
    _In_ ULONG ProcessFlags,
    _In_opt_ PSECURITY_DESCRIPTOR ProcessSecurityDescriptor,
    _In_opt_ PSECURITY_DESCRIPTOR ThreadSecurityDescriptor,
    _In_opt_ HANDLE DebugPort,
    _Out_ PRTL_USER_PROCESS_INFORMATION ProcessInformation
))

// private
NTDLL_API_VOID(__stdcall, RtlUpdateClonedCriticalSection, (
    _Inout_ PRTL_CRITICAL_SECTION CriticalSection
))

// private
NTDLL_API_VOID(__stdcall, RtlUpdateClonedSRWLock, (
    _Inout_ PRTL_SRWLOCK SRWLock,
    _In_ LOGICAL Shared // TRUE to set to shared acquire
))

// private
typedef struct _RTLP_PROCESS_REFLECTION_REFLECTION_INFORMATION
{
    HANDLE ReflectionProcessHandle;
    HANDLE ReflectionThreadHandle;
    CLIENT_ID ReflectionClientId;
} RTLP_PROCESS_REFLECTION_REFLECTION_INFORMATION, *PRTLP_PROCESS_REFLECTION_REFLECTION_INFORMATION;

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_7)
// rev
NTDLL_API(NTSTATUS, __stdcall, RtlCreateProcessReflection, (
    _In_ HANDLE ProcessHandle,
    _In_ ULONG Flags,
    _In_opt_ PVOID StartRoutine,
    _In_opt_ PVOID StartContext,
    _In_opt_ HANDLE EventHandle,
    _Out_opt_ PRTLP_PROCESS_REFLECTION_REFLECTION_INFORMATION ReflectionInformation
))
#endif

#endif

NTDLL_API(NTSTATUS, __cdecl, RtlSetProcessIsCritical, (
    _In_ BOOLEAN NewValue,
    _Out_opt_ PBOOLEAN OldValue,
    _In_ BOOLEAN CheckFlag
))

NTDLL_API(NTSTATUS, __cdecl, RtlSetThreadIsCritical, (
    _In_ BOOLEAN NewValue,
    _Out_opt_ PBOOLEAN OldValue,
    _In_ BOOLEAN CheckFlag
))

// rev
NTDLL_API(BOOLEAN, __stdcall, RtlValidProcessProtection, (
    _In_ PS_PROTECTION ProcessProtection
))

// rev
NTDLL_API(BOOLEAN, __stdcall, RtlTestProtectedAccess, (
    _In_ PS_PROTECTION Source,
    _In_ PS_PROTECTION Target
))

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_10_RS13)
// rev
NTDLL_API(BOOLEAN, __stdcall, RtlIsCurrentProcess, (
    // NtCompareObjects(NtCurrentProcess(), ProcessHandle)
    _In_ HANDLE ProcessHandle
))

// rev
NTDLL_API(BOOLEAN, __stdcall, RtlIsCurrentThread, (
    // NtCompareObjects(NtCurrentThread(), ThreadHandle)
    _In_ HANDLE ThreadHandle
))
#endif

// Threads

typedef NTSTATUS (NTAPI *PUSER_THREAD_START_ROUTINE)(
    _In_ PVOID ThreadParameter
    );

NTDLL_API(NTSTATUS, __stdcall, RtlCreateUserThread, (
    _In_ HANDLE Process,
    _In_opt_ PSECURITY_DESCRIPTOR ThreadSecurityDescriptor,
    _In_ BOOLEAN CreateSuspended,
    _In_opt_ ULONG ZeroBits,
    _In_opt_ SIZE_T MaximumStackSize,
    _In_opt_ SIZE_T CommittedStackSize,
    _In_ PUSER_THREAD_START_ROUTINE StartAddress,
    _In_opt_ PVOID Parameter,
    _Out_opt_ PHANDLE Thread,
    _Out_opt_ PCLIENT_ID ClientId
))

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_VISTA) // should be PHNT_WINXP, but is NTLIB_WIN_VISTA for consistency with RtlExitUserProcess
DECLSPEC_NORETURN
NTDLL_API_VOID(__stdcall, RtlExitUserThread, (
    _In_ NTSTATUS ExitStatus
))
#else

#define RtlExitUserThread RtlExitUserThread_R

DECLSPEC_NORETURN
FORCEINLINE VOID RtlExitUserThread_R(
    _In_ NTSTATUS ExitStatus
    )
{
    ExitThread(ExitStatus);
}

#endif

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_VISTA)

// rev
NTDLL_API(BOOLEAN, __stdcall, RtlIsCurrentThreadAttachExempt, (
    VOID
))

#endif

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_VISTA)

// private
NTDLL_API(NTSTATUS, __stdcall, RtlCreateUserStack, (
    _In_opt_ SIZE_T CommittedStackSize,
    _In_opt_ SIZE_T MaximumStackSize,
    _In_opt_ ULONG_PTR ZeroBits,
    _In_ SIZE_T PageSize,
    _In_ ULONG_PTR ReserveAlignment,
    _Out_ PINITIAL_TEB InitialTeb
))

// private
NTDLL_API(NTSTATUS, __stdcall, RtlFreeUserStack, (
    _In_ PVOID AllocationBase
))

#endif

// Extended thread context

typedef struct _CONTEXT_CHUNK 
{
    LONG Offset; // Offset may be negative.
    ULONG Length;
} CONTEXT_CHUNK, *PCONTEXT_CHUNK;

typedef struct _CONTEXT_EX 
{
    CONTEXT_CHUNK All;
    CONTEXT_CHUNK Legacy;
    CONTEXT_CHUNK XState;
} CONTEXT_EX, *PCONTEXT_EX;

#define CONTEXT_EX_LENGTH ALIGN_UP_BY(sizeof(CONTEXT_EX), PAGE_SIZE)
#define RTL_CONTEXT_EX_OFFSET(ContextEx, Chunk) ((ContextEx)->Chunk.Offset)
#define RTL_CONTEXT_EX_LENGTH(ContextEx, Chunk) ((ContextEx)->Chunk.Length)
#define RTL_CONTEXT_EX_CHUNK(Base, Layout, Chunk) ((PVOID)((PCHAR)(Base) + RTL_CONTEXT_EX_OFFSET(Layout, Chunk)))
#define RTL_CONTEXT_OFFSET(Context, Chunk) RTL_CONTEXT_EX_OFFSET((PCONTEXT_EX)(Context + 1), Chunk)
#define RTL_CONTEXT_LENGTH(Context, Chunk) RTL_CONTEXT_EX_LENGTH((PCONTEXT_EX)(Context + 1), Chunk)
#define RTL_CONTEXT_CHUNK(Context, Chunk) RTL_CONTEXT_EX_CHUNK((PCONTEXT_EX)(Context + 1), (PCONTEXT_EX)(Context + 1), Chunk)

NTDLL_API_VOID(__stdcall, RtlInitializeContext, (
    _In_ HANDLE Process,
    _Out_ PCONTEXT Context,
    _In_opt_ PVOID Parameter,
    _In_opt_ PVOID InitialPc,
    _In_opt_ PVOID InitialSp
))

NTDLL_API(ULONG, __stdcall, RtlInitializeExtendedContext, (
    _Out_ PCONTEXT Context,
    _In_ ULONG ContextFlags,
    _Out_ PCONTEXT_EX* ContextEx
))

NTDLL_API(ULONG, __stdcall, RtlCopyExtendedContext, (
    _Out_ PCONTEXT_EX Destination,
    _In_ ULONG ContextFlags,
    _In_ PCONTEXT_EX Source
))

NTDLL_API(ULONG, __stdcall, RtlGetExtendedContextLength, (
    _In_ ULONG ContextFlags,
    _Out_ PULONG ContextLength
))

NTDLL_API(ULONG64, __stdcall, RtlGetExtendedFeaturesMask, (
    _In_ PCONTEXT_EX ContextEx
))

NTDLL_API(PVOID, __stdcall, RtlLocateExtendedFeature, (
    _In_ PCONTEXT_EX ContextEx,
    _In_ ULONG FeatureId,
    _Out_opt_ PULONG Length
))

NTDLL_API(PCONTEXT, __stdcall, RtlLocateLegacyContext, (
    _In_ PCONTEXT_EX ContextEx,
    _Out_opt_ PULONG Length
))

NTDLL_API_VOID(__stdcall, RtlSetExtendedFeaturesMask, (
    __out PCONTEXT_EX ContextEx,
    _Out_ ULONG64 FeatureMask
))

#if (defined(PHNT_COMPILE) || defined(_WIN64))
// rev
NTDLL_API(NTSTATUS, __stdcall, RtlWow64GetThreadContext, (
    _In_ HANDLE ThreadHandle,
    _Inout_ PWOW64_CONTEXT ThreadContext
))

// rev
NTDLL_API(NTSTATUS, __stdcall, RtlWow64SetThreadContext, (
    _In_ HANDLE ThreadHandle,
    _In_ PWOW64_CONTEXT ThreadContext
))
#endif

NTDLL_API(NTSTATUS, __stdcall, RtlRemoteCall, (
    _In_ HANDLE Process,
    _In_ HANDLE Thread,
    _In_ PVOID CallSite,
    _In_ ULONG ArgumentCount,
    _In_opt_ PULONG_PTR Arguments,
    _In_ BOOLEAN PassContext,
    _In_ BOOLEAN AlreadySuspended
))

// Vectored exception handlers

NTDLL_API(PVOID, __stdcall, RtlAddVectoredExceptionHandler, (
    _In_ ULONG First,
    _In_ PVECTORED_EXCEPTION_HANDLER Handler
))

NTDLL_API(ULONG, __stdcall, RtlRemoveVectoredExceptionHandler, (
    _In_ PVOID Handle
))

NTDLL_API(PVOID, __stdcall, RtlAddVectoredContinueHandler, (
    _In_ ULONG First,
    _In_ PVECTORED_EXCEPTION_HANDLER Handler
))

NTDLL_API(ULONG, __stdcall, RtlRemoveVectoredContinueHandler, (
    _In_ PVOID Handle
))

// Runtime exception handling

typedef ULONG (NTAPI *PRTLP_UNHANDLED_EXCEPTION_FILTER)(
    _In_ PEXCEPTION_POINTERS ExceptionInfo
    );

NTDLL_API_VOID(__stdcall, RtlSetUnhandledExceptionFilter, (
    _In_ PRTLP_UNHANDLED_EXCEPTION_FILTER UnhandledExceptionFilter
))

// rev
NTDLL_API(LONG, __stdcall, RtlUnhandledExceptionFilter, (
    _In_ PEXCEPTION_POINTERS ExceptionPointers
))

// rev
NTDLL_API(LONG, __stdcall, RtlUnhandledExceptionFilter2, (
    _In_ PEXCEPTION_POINTERS ExceptionPointers,
    _In_ ULONG Flags
))

// rev
NTDLL_API(LONG, __stdcall, RtlKnownExceptionFilter, (
    _In_ PEXCEPTION_POINTERS ExceptionPointers
))

#if (defined(PHNT_COMPILE) || defined(_WIN64))

// private
typedef enum _FUNCTION_TABLE_TYPE
{
    RF_SORTED,
    RF_UNSORTED,
    RF_CALLBACK,
    RF_KERNEL_DYNAMIC
} FUNCTION_TABLE_TYPE;

// private
typedef struct _DYNAMIC_FUNCTION_TABLE
{
    LIST_ENTRY ListEntry;
    PRUNTIME_FUNCTION FunctionTable;
    LARGE_INTEGER TimeStamp;
    ULONG64 MinimumAddress;
    ULONG64 MaximumAddress;
    ULONG64 BaseAddress;
    PGET_RUNTIME_FUNCTION_CALLBACK Callback;
    PVOID Context;
    PWSTR OutOfProcessCallbackDll;
    FUNCTION_TABLE_TYPE Type;
    ULONG EntryCount;
    RTL_BALANCED_NODE TreeNode;
} DYNAMIC_FUNCTION_TABLE, *PDYNAMIC_FUNCTION_TABLE;

// rev
NTDLL_API(PLIST_ENTRY, __stdcall, RtlGetFunctionTableListHead, (
    VOID
))

#endif

// Images

NTDLL_API(PIMAGE_NT_HEADERS, __stdcall, RtlImageNtHeader, (
    _In_ PVOID BaseOfImage
))

#define RTL_IMAGE_NT_HEADER_EX_FLAG_NO_RANGE_CHECK 0x00000001

NTDLL_API(NTSTATUS, __stdcall, RtlImageNtHeaderEx, (
    _In_ ULONG Flags,
    _In_ PVOID BaseOfImage,
    _In_ ULONG64 Size,
    _Out_ PIMAGE_NT_HEADERS *OutHeaders
))

NTDLL_API(PVOID, __stdcall, RtlAddressInSectionTable, (
    _In_ PIMAGE_NT_HEADERS NtHeaders,
    _In_ PVOID BaseOfImage,
    _In_ ULONG VirtualAddress
))

NTDLL_API(PIMAGE_SECTION_HEADER, __stdcall, RtlSectionTableFromVirtualAddress, (
    _In_ PIMAGE_NT_HEADERS NtHeaders,
    _In_ PVOID BaseOfImage,
    _In_ ULONG VirtualAddress
))

NTDLL_API(PVOID, __stdcall, RtlImageDirectoryEntryToData, (
    _In_ PVOID BaseOfImage,
    _In_ BOOLEAN MappedAsImage,
    _In_ USHORT DirectoryEntry,
    _Out_ PULONG Size
))

NTDLL_API(PIMAGE_SECTION_HEADER, __stdcall, RtlImageRvaToSection, (
    _In_ PIMAGE_NT_HEADERS NtHeaders,
    _In_ PVOID BaseOfImage,
    _In_ ULONG Rva
))

NTDLL_API(PVOID, __stdcall, RtlImageRvaToVa, (
    _In_ PIMAGE_NT_HEADERS NtHeaders,
    _In_ PVOID BaseOfImage,
    _In_ ULONG Rva,
    _Inout_opt_ PIMAGE_SECTION_HEADER *LastRvaSection
))

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_10_TH1)

// rev
NTDLL_API(PVOID, __stdcall, RtlFindExportedRoutineByName, (
    _In_ PVOID BaseOfImage,
    _In_ PSTR RoutineName
))

#endif

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_10_RS1)

// rev
NTDLL_API(NTSTATUS, __stdcall, RtlGuardCheckLongJumpTarget, (
    _In_ PVOID PcValue, 
    _In_ BOOL IsFastFail, 
    _Out_ PBOOL IsLongJumpTarget
))

#endif

// Memory

NTDLL_API(SIZE_T, __stdcall, RtlCompareMemoryUlong, (
    _In_ PVOID Source,
    _In_ SIZE_T Length,
    _In_ ULONG Pattern
))

NTDLL_API_VOID(__stdcall, RtlFillMemoryUlong, (
    _Out_ PVOID Destination,
    _In_ SIZE_T Length,
    _In_ ULONG Pattern
))

NTDLL_API_VOID(__stdcall, RtlFillMemoryUlonglong, (
    _Out_ PVOID Destination,
    _In_ SIZE_T Length,
    _In_ ULONGLONG Pattern
))

// Environment

NTDLL_API(NTSTATUS, __stdcall, RtlCreateEnvironment, (
    _In_ BOOLEAN CloneCurrentEnvironment,
    _Out_ PVOID *Environment
))

// begin_rev
#define RTL_CREATE_ENVIRONMENT_TRANSLATE 0x1 // translate from multi-byte to Unicode
#define RTL_CREATE_ENVIRONMENT_TRANSLATE_FROM_OEM 0x2 // translate from OEM to Unicode (Translate flag must also be set)
#define RTL_CREATE_ENVIRONMENT_EMPTY 0x4 // create empty environment block
// end_rev

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_VISTA)
// private
NTDLL_API(NTSTATUS, __stdcall, RtlCreateEnvironmentEx, (
    _In_ PVOID SourceEnv,
    _Out_ PVOID *Environment,
    _In_ ULONG Flags
))
#endif

NTDLL_API(NTSTATUS, __stdcall, RtlDestroyEnvironment, (
    _In_ PVOID Environment
))

NTDLL_API(NTSTATUS, __stdcall, RtlSetCurrentEnvironment, (
    _In_ PVOID Environment,
    _Out_opt_ PVOID *PreviousEnvironment
))

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_VISTA)
// private
NTDLL_API(NTSTATUS, __stdcall, RtlSetEnvironmentVar, (
    _In_opt_ PWSTR *Environment,
    _In_reads_(NameLength) PWSTR Name,
    _In_ SIZE_T NameLength,
    _In_reads_(ValueLength) PWSTR Value,
    _In_ SIZE_T ValueLength
))
#endif

NTDLL_API(NTSTATUS, __stdcall, RtlSetEnvironmentVariable, (
    _In_opt_ PVOID *Environment,
    _In_ PUNICODE_STRING Name,
    _In_opt_ PUNICODE_STRING Value
))

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_VISTA)
// private
NTDLL_API(NTSTATUS, __stdcall, RtlQueryEnvironmentVariable, (
    _In_opt_ PVOID Environment,
    _In_reads_(NameLength) PWSTR Name,
    _In_ SIZE_T NameLength,
    _Out_writes_(ValueLength) PWSTR Value,
    _In_ SIZE_T ValueLength,
    _Out_ PSIZE_T ReturnLength
))
#endif

NTDLL_API(NTSTATUS, __stdcall, RtlQueryEnvironmentVariable_U, (
    _In_opt_ PVOID Environment,
    _In_ PUNICODE_STRING Name,
    _Out_ PUNICODE_STRING Value
))

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_VISTA)
// private
NTDLL_API(NTSTATUS, __stdcall, RtlExpandEnvironmentStrings, (
    _In_opt_ PVOID Environment,
    _In_reads_(SrcLength) PWSTR Src,
    _In_ SIZE_T SrcLength,
    _Out_writes_(DstLength) PWSTR Dst,
    _In_ SIZE_T DstLength,
    _Out_opt_ PSIZE_T ReturnLength
))
#endif

NTDLL_API(NTSTATUS, __stdcall, RtlExpandEnvironmentStrings_U, (
    _In_opt_ PVOID Environment,
    _In_ PUNICODE_STRING Source,
    _Out_ PUNICODE_STRING Destination,
    _Out_opt_ PULONG ReturnedLength
))

NTDLL_API(NTSTATUS, __stdcall, RtlSetEnvironmentStrings, (
    _In_ PWCHAR NewEnvironment,
    _In_ SIZE_T NewEnvironmentSize
))

// Directory and path support

typedef struct _RTLP_CURDIR_REF
{
    LONG ReferenceCount;
    HANDLE DirectoryHandle;
} RTLP_CURDIR_REF, *PRTLP_CURDIR_REF;

typedef struct _RTL_RELATIVE_NAME_U
{
    UNICODE_STRING RelativeName;
    HANDLE ContainingDirectory;
    PRTLP_CURDIR_REF CurDirRef;
} RTL_RELATIVE_NAME_U, *PRTL_RELATIVE_NAME_U;

typedef enum _RTL_PATH_TYPE
{
    RtlPathTypeUnknown,
    RtlPathTypeUncAbsolute,
    RtlPathTypeDriveAbsolute,
    RtlPathTypeDriveRelative,
    RtlPathTypeRooted,
    RtlPathTypeRelative,
    RtlPathTypeLocalDevice,
    RtlPathTypeRootLocalDevice
} RTL_PATH_TYPE;

// Data exports (ntdll.lib/ntdllp.lib)

NTSYSAPI PWSTR RtlNtdllName;
NTSYSAPI UNICODE_STRING RtlDosPathSeperatorsString;
NTSYSAPI UNICODE_STRING RtlAlternateDosPathSeperatorString;
NTSYSAPI UNICODE_STRING RtlNtPathSeperatorString;

#ifndef PHNT_INLINE_SEPERATOR_STRINGS
#define RtlNtdllName L"ntdll.dll"
#define RtlDosPathSeperatorsString ((UNICODE_STRING)RTL_CONSTANT_STRING(L"\\/"))
#define RtlAlternateDosPathSeperatorString ((UNICODE_STRING)RTL_CONSTANT_STRING(L"/"))
#define RtlNtPathSeperatorString ((UNICODE_STRING)RTL_CONSTANT_STRING(L"\\"))
#endif

// Path functions

NTDLL_API(RTL_PATH_TYPE, __stdcall, RtlDetermineDosPathNameType_U, (
    _In_ PWSTR DosFileName
))

NTDLL_API(RTL_PATH_TYPE, __stdcall, RtlDetermineDosPathNameType_Ustr, (
    _In_ PCUNICODE_STRING DosFileName
))

NTDLL_API(ULONG, __stdcall, RtlIsDosDeviceName_U, (
    _In_ PWSTR DosFileName
))

NTDLL_API(ULONG, __stdcall, RtlIsDosDeviceName_Ustr, (
    _In_ PUNICODE_STRING DosFileName
))

NTDLL_API(ULONG, __stdcall, RtlGetFullPathName_U, (
    _In_ PWSTR FileName,
    _In_ ULONG BufferLength,
    _Out_writes_bytes_(BufferLength) PWSTR Buffer,
    _Out_opt_ PWSTR *FilePart
))

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_7)
// rev
NTDLL_API(NTSTATUS, __stdcall, RtlGetFullPathName_UEx, (
    _In_ PWSTR FileName,
    _In_ ULONG BufferLength,
    _Out_writes_bytes_(BufferLength) PWSTR Buffer,
    _Out_opt_ PWSTR *FilePart,
    _Out_opt_ ULONG *BytesRequired
))
#endif

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_XP)
NTDLL_API(NTSTATUS, __stdcall, RtlGetFullPathName_UstrEx, (
    _In_ PUNICODE_STRING FileName,
    _Inout_ PUNICODE_STRING StaticString,
    _Out_opt_ PUNICODE_STRING DynamicString,
    _Out_opt_ PUNICODE_STRING *StringUsed,
    _Out_opt_ SIZE_T *FilePartPrefixCch,
    _Out_opt_ PBOOLEAN NameInvalid,
    _Out_ RTL_PATH_TYPE *InputPathType,
    _Out_opt_ SIZE_T *BytesRequired
))
#endif

NTDLL_API(ULONG, __stdcall, RtlGetCurrentDirectory_U, (
    _In_ ULONG BufferLength,
    _Out_writes_bytes_(BufferLength) PWSTR Buffer
))

NTDLL_API(NTSTATUS, __stdcall, RtlSetCurrentDirectory_U, (
    _In_ PUNICODE_STRING PathName
))

NTDLL_API(ULONG, __stdcall, RtlGetLongestNtPathLength, (
    VOID
))

NTDLL_API(BOOLEAN, __stdcall, RtlDosPathNameToNtPathName_U, (
    _In_ PWSTR DosFileName,
    _Out_ PUNICODE_STRING NtFileName,
    _Out_opt_ PWSTR *FilePart,
    _Out_opt_ PRTL_RELATIVE_NAME_U RelativeName
))

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_XP)
NTDLL_API(NTSTATUS, __stdcall, RtlDosPathNameToNtPathName_U_WithStatus, (
    _In_ PWSTR DosFileName,
    _Out_ PUNICODE_STRING NtFileName,
    _Out_opt_ PWSTR *FilePart,
    _Out_opt_ PRTL_RELATIVE_NAME_U RelativeName
))
#endif

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_10_RS13)
// rev
NTDLL_API(NTSTATUS, __stdcall, RtlDosLongPathNameToNtPathName_U_WithStatus, (
    _In_ PWSTR DosFileName,
    _Out_ PUNICODE_STRING NtFileName,
    _Out_opt_ PWSTR *FilePart,
    _Out_opt_ PRTL_RELATIVE_NAME_U RelativeName
))
#endif

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_XP)
NTDLL_API(BOOLEAN, __stdcall, RtlDosPathNameToRelativeNtPathName_U, (
    _In_ PWSTR DosFileName,
    _Out_ PUNICODE_STRING NtFileName,
    _Out_opt_ PWSTR *FilePart,
    _Out_opt_ PRTL_RELATIVE_NAME_U RelativeName
))
#endif

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_XP)
NTDLL_API(NTSTATUS, __stdcall, RtlDosPathNameToRelativeNtPathName_U_WithStatus, (
    _In_ PWSTR DosFileName,
    _Out_ PUNICODE_STRING NtFileName,
    _Out_opt_ PWSTR *FilePart,
    _Out_opt_ PRTL_RELATIVE_NAME_U RelativeName
))
#endif

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_10_RS13)
// rev
NTDLL_API(NTSTATUS, __stdcall, RtlDosLongPathNameToRelativeNtPathName_U_WithStatus, (
    _In_ PWSTR DosFileName,
    _Out_ PUNICODE_STRING NtFileName,
    _Out_opt_ PWSTR *FilePart,
    _Out_opt_ PRTL_RELATIVE_NAME_U RelativeName
))
#endif

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_XP)
NTDLL_API_VOID(__stdcall, RtlReleaseRelativeName, (
    _Inout_ PRTL_RELATIVE_NAME_U RelativeName
))
#endif

NTDLL_API(ULONG, __stdcall, RtlDosSearchPath_U, (
    _In_ PWSTR Path,
    _In_ PWSTR FileName,
    _In_opt_ PWSTR Extension,
    _In_ ULONG BufferLength,
    _Out_writes_bytes_(BufferLength) PWSTR Buffer,
    _Out_opt_ PWSTR *FilePart
))

#define RTL_DOS_SEARCH_PATH_FLAG_APPLY_ISOLATION_REDIRECTION 0x00000001
#define RTL_DOS_SEARCH_PATH_FLAG_DISALLOW_DOT_RELATIVE_PATH_SEARCH 0x00000002
#define RTL_DOS_SEARCH_PATH_FLAG_APPLY_DEFAULT_EXTENSION_WHEN_NOT_RELATIVE_PATH_EVEN_IF_FILE_HAS_EXTENSION 0x00000004

NTDLL_API(NTSTATUS, __stdcall, RtlDosSearchPath_Ustr, (
    _In_ ULONG Flags,
    _In_ PUNICODE_STRING Path,
    _In_ PUNICODE_STRING FileName,
    _In_opt_ PUNICODE_STRING DefaultExtension,
    _Out_opt_ PUNICODE_STRING StaticString,
    _Out_opt_ PUNICODE_STRING DynamicString,
    _Out_opt_ PCUNICODE_STRING *FullFileNameOut,
    _Out_opt_ SIZE_T *FilePartPrefixCch,
    _Out_opt_ SIZE_T *BytesRequired
))

NTDLL_API(BOOLEAN, __stdcall, RtlDoesFileExists_U, (
    _In_ PWSTR FileName
))

NTDLL_API(NTSTATUS, __stdcall, RtlGetLengthWithoutLastFullDosOrNtPathElement, (
    _Reserved_ ULONG Flags,
    _In_ PUNICODE_STRING PathString,
    _Out_ PULONG Length
))

NTDLL_API(NTSTATUS, __stdcall, RtlGetLengthWithoutTrailingPathSeperators, (
    _Reserved_ ULONG Flags,
    _In_ PUNICODE_STRING PathString,
    _Out_ PULONG Length
))

typedef struct _GENERATE_NAME_CONTEXT
{
    USHORT Checksum;
    BOOLEAN CheckSumInserted;
    UCHAR NameLength;
    WCHAR NameBuffer[8];
    ULONG ExtensionLength;
    WCHAR ExtensionBuffer[4];
    ULONG LastIndexValue;
} GENERATE_NAME_CONTEXT, *PGENERATE_NAME_CONTEXT;

// private
NTDLL_API(NTSTATUS, __stdcall, RtlGenerate8dot3Name, (
    _In_ PUNICODE_STRING Name,
    _In_ BOOLEAN AllowExtendedCharacters,
    _In_ PGENERATE_NAME_CONTEXT Context,
    _Out_ PUNICODE_STRING Name8dot3
))

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_8)

// private
NTDLL_API(NTSTATUS, __stdcall, RtlComputePrivatizedDllName_U, (
    _In_ PUNICODE_STRING DllName,
    _Out_ PUNICODE_STRING RealName,
    _Out_ PUNICODE_STRING LocalName
))

// rev
NTDLL_API(BOOLEAN, __stdcall, RtlGetSearchPath, (
    _Out_ PWSTR *SearchPath
))

// rev
NTDLL_API(NTSTATUS, __stdcall, RtlSetSearchPathMode, (
    _In_ ULONG Flags
))

// rev
NTDLL_API(PWSTR, __stdcall, RtlGetExePath, (
    VOID
))

#endif

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_10_RS12)

// private
NTDLL_API(PWSTR, __stdcall, RtlGetNtSystemRoot, (
    VOID
))

// rev
NTDLL_API(BOOLEAN, __stdcall, RtlAreLongPathsEnabled, (
    VOID
))

#endif

NTDLL_API(BOOLEAN, __stdcall, RtlIsThreadWithinLoaderCallout, (
    VOID
))

NTDLL_API(BOOLEAN, __stdcall, RtlDllShutdownInProgress, (
    VOID
))

// Heaps

typedef struct _RTL_HEAP_ENTRY
{
    SIZE_T Size;
    USHORT Flags;
    USHORT AllocatorBackTraceIndex;
    union
    {
        struct
        {
            SIZE_T Settable;
            ULONG Tag;
        } s1;
        struct
        {
            SIZE_T CommittedSize;
            PVOID FirstBlock;
        } s2;
    } u;
} RTL_HEAP_ENTRY, *PRTL_HEAP_ENTRY;

#define RTL_HEAP_BUSY (USHORT)0x0001
#define RTL_HEAP_SEGMENT (USHORT)0x0002
#define RTL_HEAP_SETTABLE_VALUE (USHORT)0x0010
#define RTL_HEAP_SETTABLE_FLAG1 (USHORT)0x0020
#define RTL_HEAP_SETTABLE_FLAG2 (USHORT)0x0040
#define RTL_HEAP_SETTABLE_FLAG3 (USHORT)0x0080
#define RTL_HEAP_SETTABLE_FLAGS (USHORT)0x00e0
#define RTL_HEAP_UNCOMMITTED_RANGE (USHORT)0x0100
#define RTL_HEAP_PROTECTED_ENTRY (USHORT)0x0200

typedef struct _RTL_HEAP_TAG
{
    ULONG NumberOfAllocations;
    ULONG NumberOfFrees;
    SIZE_T BytesAllocated;
    USHORT TagIndex;
    USHORT CreatorBackTraceIndex;
    WCHAR TagName[24];
} RTL_HEAP_TAG, *PRTL_HEAP_TAG;

typedef struct _RTL_HEAP_INFORMATION
{
    PVOID BaseAddress;
    ULONG Flags;
    USHORT EntryOverhead;
    USHORT CreatorBackTraceIndex;
    SIZE_T BytesAllocated;
    SIZE_T BytesCommitted;
    ULONG NumberOfTags;
    ULONG NumberOfEntries;
    ULONG NumberOfPseudoTags;
    ULONG PseudoTagGranularity;
    ULONG Reserved[5];
    PRTL_HEAP_TAG Tags;
    PRTL_HEAP_ENTRY Entries;
} RTL_HEAP_INFORMATION, *PRTL_HEAP_INFORMATION;

typedef struct _RTL_PROCESS_HEAPS
{
    ULONG NumberOfHeaps;
    RTL_HEAP_INFORMATION Heaps[1];
} RTL_PROCESS_HEAPS, *PRTL_PROCESS_HEAPS;

typedef NTSTATUS (NTAPI *PRTL_HEAP_COMMIT_ROUTINE)(
    _In_ PVOID Base,
    _Inout_ PVOID *CommitAddress,
    _Inout_ PSIZE_T CommitSize
    );

typedef struct _RTL_HEAP_PARAMETERS
{
    ULONG Length;
    SIZE_T SegmentReserve;
    SIZE_T SegmentCommit;
    SIZE_T DeCommitFreeBlockThreshold;
    SIZE_T DeCommitTotalFreeThreshold;
    SIZE_T MaximumAllocationSize;
    SIZE_T VirtualMemoryThreshold;
    SIZE_T InitialCommit;
    SIZE_T InitialReserve;
    PRTL_HEAP_COMMIT_ROUTINE CommitRoutine;
    SIZE_T Reserved[2];
} RTL_HEAP_PARAMETERS, *PRTL_HEAP_PARAMETERS;

#define HEAP_SETTABLE_USER_VALUE 0x00000100
#define HEAP_SETTABLE_USER_FLAG1 0x00000200
#define HEAP_SETTABLE_USER_FLAG2 0x00000400
#define HEAP_SETTABLE_USER_FLAG3 0x00000800
#define HEAP_SETTABLE_USER_FLAGS 0x00000e00

#define HEAP_CLASS_0 0x00000000 // Process heap
#define HEAP_CLASS_1 0x00001000 // Private heap
#define HEAP_CLASS_2 0x00002000 // Kernel heap
#define HEAP_CLASS_3 0x00003000 // GDI heap
#define HEAP_CLASS_4 0x00004000 // User heap
#define HEAP_CLASS_5 0x00005000 // Console heap
#define HEAP_CLASS_6 0x00006000 // User desktop heap
#define HEAP_CLASS_7 0x00007000 // CSR shared heap
#define HEAP_CLASS_8 0x00008000 // CSR port heap
#define HEAP_CLASS_MASK 0x0000f000

NTDLL_API(PVOID, __stdcall, RtlCreateHeap, (
    _In_ ULONG Flags,
    _In_opt_ PVOID HeapBase,
    _In_opt_ SIZE_T ReserveSize,
    _In_opt_ SIZE_T CommitSize,
    _In_opt_ PVOID Lock,
    _In_opt_ PRTL_HEAP_PARAMETERS Parameters
))

NTDLL_API(PVOID, __stdcall, RtlDestroyHeap, (
    _Frees_ptr_ PVOID HeapHandle
))

NTDLL_API(PVOID, __stdcall, RtlAllocateHeap, (
    _In_ PVOID HeapHandle,
    _In_opt_ ULONG Flags,
    _In_ SIZE_T Size
))

NTDLL_API(BOOLEAN, __stdcall, RtlFreeHeap, (
    _In_ PVOID HeapHandle,
    _In_opt_ ULONG Flags,
    _Frees_ptr_opt_ PVOID BaseAddress
))

NTDLL_API(SIZE_T, __stdcall, RtlSizeHeap, (
    _In_ PVOID HeapHandle,
    _In_ ULONG Flags,
    _In_ PVOID BaseAddress
))

NTDLL_API(NTSTATUS, __stdcall, RtlZeroHeap, (
    _In_ PVOID HeapHandle,
    _In_ ULONG Flags
))

NTDLL_API_VOID(__stdcall, RtlProtectHeap, (
    _In_ PVOID HeapHandle,
    _In_ BOOLEAN MakeReadOnly
))

#define RtlProcessHeap() (NtCurrentPeb()->ProcessHeap)

NTDLL_API(BOOLEAN, __stdcall, RtlLockHeap, (
    _In_ PVOID HeapHandle
))

NTDLL_API(BOOLEAN, __stdcall, RtlUnlockHeap, (
    _In_ PVOID HeapHandle
))

NTDLL_API(PVOID, __stdcall, RtlReAllocateHeap, (
    _In_ PVOID HeapHandle,
    _In_ ULONG Flags,
    _Frees_ptr_opt_ PVOID BaseAddress,
    _In_ SIZE_T Size
))

NTDLL_API(BOOLEAN, __stdcall, RtlGetUserInfoHeap, (
    _In_ PVOID HeapHandle,
    _In_ ULONG Flags,
    _In_ PVOID BaseAddress,
    _Out_opt_ PVOID *UserValue,
    _Out_opt_ PULONG UserFlags
))

NTDLL_API(BOOLEAN, __stdcall, RtlSetUserValueHeap, (
    _In_ PVOID HeapHandle,
    _In_ ULONG Flags,
    _In_ PVOID BaseAddress,
    _In_ PVOID UserValue
))

NTDLL_API(BOOLEAN, __stdcall, RtlSetUserFlagsHeap, (
    _In_ PVOID HeapHandle,
    _In_ ULONG Flags,
    _In_ PVOID BaseAddress,
    _In_ ULONG UserFlagsReset,
    _In_ ULONG UserFlagsSet
))

typedef struct _RTL_HEAP_TAG_INFO
{
    ULONG NumberOfAllocations;
    ULONG NumberOfFrees;
    SIZE_T BytesAllocated;
} RTL_HEAP_TAG_INFO, *PRTL_HEAP_TAG_INFO;

#define RTL_HEAP_MAKE_TAG HEAP_MAKE_TAG_FLAGS

NTDLL_API(ULONG, __stdcall, RtlCreateTagHeap, (
    _In_ PVOID HeapHandle,
    _In_ ULONG Flags,
    _In_opt_ PWSTR TagPrefix,
    _In_ PWSTR TagNames
))

NTDLL_API(PWSTR, __stdcall, RtlQueryTagHeap, (
    _In_ PVOID HeapHandle,
    _In_ ULONG Flags,
    _In_ USHORT TagIndex,
    _In_ BOOLEAN ResetCounters,
    _Out_opt_ PRTL_HEAP_TAG_INFO TagInfo
))

NTDLL_API(NTSTATUS, __stdcall, RtlExtendHeap, (
    _In_ PVOID HeapHandle,
    _In_ ULONG Flags,
    _In_ PVOID Base,
    _In_ SIZE_T Size
))

NTDLL_API(SIZE_T, __stdcall, RtlCompactHeap, (
    _In_ PVOID HeapHandle,
    _In_ ULONG Flags
))

NTDLL_API(BOOLEAN, __stdcall, RtlValidateHeap, (
    _In_ PVOID HeapHandle,
    _In_ ULONG Flags,
    _In_ PVOID BaseAddress
))

NTDLL_API(BOOLEAN, __stdcall, RtlValidateProcessHeaps, (
    VOID
))

NTDLL_API(ULONG, __stdcall, RtlGetProcessHeaps, (
    _In_ ULONG NumberOfHeaps,
    _Out_ PVOID *ProcessHeaps
))

typedef NTSTATUS (NTAPI *PRTL_ENUM_HEAPS_ROUTINE)(
    _In_ PVOID HeapHandle,
    _In_ PVOID Parameter
    );

NTDLL_API(NTSTATUS, __stdcall, RtlEnumProcessHeaps, (
    _In_ PRTL_ENUM_HEAPS_ROUTINE EnumRoutine,
    _In_ PVOID Parameter
))

typedef struct _RTL_HEAP_USAGE_ENTRY
{
    struct _RTL_HEAP_USAGE_ENTRY *Next;
    PVOID Address;
    SIZE_T Size;
    USHORT AllocatorBackTraceIndex;
    USHORT TagIndex;
} RTL_HEAP_USAGE_ENTRY, *PRTL_HEAP_USAGE_ENTRY;

typedef struct _RTL_HEAP_USAGE
{
    ULONG Length;
    SIZE_T BytesAllocated;
    SIZE_T BytesCommitted;
    SIZE_T BytesReserved;
    SIZE_T BytesReservedMaximum;
    PRTL_HEAP_USAGE_ENTRY Entries;
    PRTL_HEAP_USAGE_ENTRY AddedEntries;
    PRTL_HEAP_USAGE_ENTRY RemovedEntries;
    ULONG_PTR Reserved[8];
} RTL_HEAP_USAGE, *PRTL_HEAP_USAGE;

#define HEAP_USAGE_ALLOCATED_BLOCKS HEAP_REALLOC_IN_PLACE_ONLY
#define HEAP_USAGE_FREE_BUFFER HEAP_ZERO_MEMORY

NTDLL_API(NTSTATUS, __stdcall, RtlUsageHeap, (
    _In_ PVOID HeapHandle,
    _In_ ULONG Flags,
    _Inout_ PRTL_HEAP_USAGE Usage
))

typedef struct _RTL_HEAP_WALK_ENTRY
{
    PVOID DataAddress;
    SIZE_T DataSize;
    UCHAR OverheadBytes;
    UCHAR SegmentIndex;
    USHORT Flags;
    union
    {
        struct
        {
            SIZE_T Settable;
            USHORT TagIndex;
            USHORT AllocatorBackTraceIndex;
            ULONG Reserved[2];
        } Block;
        struct
        {
            ULONG CommittedSize;
            ULONG UnCommittedSize;
            PVOID FirstEntry;
            PVOID LastEntry;
        } Segment;
    };
} RTL_HEAP_WALK_ENTRY, *PRTL_HEAP_WALK_ENTRY;

NTDLL_API(NTSTATUS, __stdcall, RtlWalkHeap, (
    _In_ PVOID HeapHandle,
    _Inout_ PRTL_HEAP_WALK_ENTRY Entry
))

// HEAP_INFORMATION_CLASS
#define HeapCompatibilityInformation 0x0 // q; s: ULONG
#define HeapEnableTerminationOnCorruption 0x1 // q; s: NULL
#define HeapExtendedInformation 0x2 // q; s: HEAP_EXTENDED_INFORMATION
#define HeapOptimizeResources 0x3 // q; s: HEAP_OPTIMIZE_RESOURCES_INFORMATION 
#define HeapTaggingInformation 0x4
#define HeapStackDatabase 0x5
#define HeapDetailedFailureInformation 0x80000001
#define HeapSetDebuggingInformation 0x80000002 // q; s: HEAP_DEBUGGING_INFORMATION

typedef enum _HEAP_COMPATIBILITY_MODE
{
    HEAP_COMPATIBILITY_STANDARD = 0UL,
    HEAP_COMPATIBILITY_LAL = 1UL,
    HEAP_COMPATIBILITY_LFH = 2UL,
} HEAP_COMPATIBILITY_MODE;

typedef struct _PROCESS_HEAP_INFORMATION
{
    ULONG_PTR ReserveSize;
    ULONG_PTR CommitSize;
    ULONG NumberOfHeaps;
    ULONG_PTR FirstHeapInformationOffset;
} PROCESS_HEAP_INFORMATION, *PPROCESS_HEAP_INFORMATION;

typedef struct _HEAP_INFORMATION
{
    ULONG_PTR Address;
    ULONG Mode;
    ULONG_PTR ReserveSize;
    ULONG_PTR CommitSize;
    ULONG_PTR FirstRegionInformationOffset;
    ULONG_PTR NextHeapInformationOffset;
} HEAP_INFORMATION, *PHEAP_INFORMATION;

typedef struct _HEAP_EXTENDED_INFORMATION
{
    HANDLE Process;
    ULONG_PTR Heap;
    ULONG Level;
    PVOID CallbackRoutine;
    PVOID CallbackContext;
    union
    {
        PROCESS_HEAP_INFORMATION ProcessHeapInformation;
        HEAP_INFORMATION HeapInformation;
    };
} HEAP_EXTENDED_INFORMATION, *PHEAP_EXTENDED_INFORMATION;

// rev
typedef NTSTATUS (NTAPI *PRTL_HEAP_LEAK_ENUMERATION_ROUTINE)(
    _In_ LONG Reserved,
    _In_ PVOID HeapHandle,
    _In_ PVOID BaseAddress,
    _In_ SIZE_T BlockSize,
    _In_ ULONG StackTraceDepth,
    _In_ PVOID *StackTrace
    );

// symbols
typedef struct _HEAP_DEBUGGING_INFORMATION
{
    PVOID InterceptorFunction;
    USHORT InterceptorValue;
    ULONG ExtendedOptions;
    ULONG StackTraceDepth;
    SIZE_T MinTotalBlockSize;
    SIZE_T MaxTotalBlockSize;
    PRTL_HEAP_LEAK_ENUMERATION_ROUTINE HeapLeakEnumerationRoutine;
} HEAP_DEBUGGING_INFORMATION, *PHEAP_DEBUGGING_INFORMATION;

NTDLL_API(NTSTATUS, __stdcall, RtlQueryHeapInformation, (
    _In_ PVOID HeapHandle,
    _In_ HEAP_INFORMATION_CLASS HeapInformationClass,
    _Out_opt_ PVOID HeapInformation,
    _In_opt_ SIZE_T HeapInformationLength,
    _Out_opt_ PSIZE_T ReturnLength
))

NTDLL_API(NTSTATUS, __stdcall, RtlSetHeapInformation, (
    _In_ PVOID HeapHandle,
    _In_ HEAP_INFORMATION_CLASS HeapInformationClass,
    _In_opt_ PVOID HeapInformation,
    _In_opt_ SIZE_T HeapInformationLength
))

NTDLL_API(ULONG, __stdcall, RtlMultipleAllocateHeap, (
    _In_ PVOID HeapHandle,
    _In_ ULONG Flags,
    _In_ SIZE_T Size,
    _In_ ULONG Count,
    _Out_ PVOID *Array
))

NTDLL_API(ULONG, __stdcall, RtlMultipleFreeHeap, (
    _In_ PVOID HeapHandle,
    _In_ ULONG Flags,
    _In_ ULONG Count,
    _In_ PVOID *Array
))

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_7)
NTDLL_API_VOID(__stdcall, RtlDetectHeapLeaks, (
    VOID
))
#endif

NTDLL_API_VOID(__stdcall, RtlFlushHeaps, (
    VOID
))

// Memory zones

// begin_private

typedef struct _RTL_MEMORY_ZONE_SEGMENT
{
    struct _RTL_MEMORY_ZONE_SEGMENT *NextSegment;
    SIZE_T Size;
    PVOID Next;
    PVOID Limit;
} RTL_MEMORY_ZONE_SEGMENT, *PRTL_MEMORY_ZONE_SEGMENT;

typedef struct _RTL_MEMORY_ZONE
{
    RTL_MEMORY_ZONE_SEGMENT Segment;
    RTL_SRWLOCK Lock;
    ULONG LockCount;
    PRTL_MEMORY_ZONE_SEGMENT FirstSegment;
} RTL_MEMORY_ZONE, *PRTL_MEMORY_ZONE;

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_VISTA)

NTDLL_API(NTSTATUS, __stdcall, RtlCreateMemoryZone, (
    _Out_ PVOID *MemoryZone,
    _In_ SIZE_T InitialSize,
    _Reserved_ ULONG Flags
))

NTDLL_API(NTSTATUS, __stdcall, RtlDestroyMemoryZone, (
    _In_ _Post_invalid_ PVOID MemoryZone
))

NTDLL_API(NTSTATUS, __stdcall, RtlAllocateMemoryZone, (
    _In_ PVOID MemoryZone,
    _In_ SIZE_T BlockSize,
    _Out_ PVOID *Block
))

NTDLL_API(NTSTATUS, __stdcall, RtlResetMemoryZone, (
    _In_ PVOID MemoryZone
))

NTDLL_API(NTSTATUS, __stdcall, RtlLockMemoryZone, (
    _In_ PVOID MemoryZone
))

NTDLL_API(NTSTATUS, __stdcall, RtlUnlockMemoryZone, (
    _In_ PVOID MemoryZone
))

#endif

// end_private

// Memory block lookaside lists

// begin_private

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_VISTA)

NTDLL_API(NTSTATUS, __stdcall, RtlCreateMemoryBlockLookaside, (
    _Out_ PVOID *MemoryBlockLookaside,
    _Reserved_ ULONG Flags,
    _In_ ULONG InitialSize,
    _In_ ULONG MinimumBlockSize,
    _In_ ULONG MaximumBlockSize
))

NTDLL_API(NTSTATUS, __stdcall, RtlDestroyMemoryBlockLookaside, (
    _In_ PVOID MemoryBlockLookaside
))

NTDLL_API(NTSTATUS, __stdcall, RtlAllocateMemoryBlockLookaside, (
    _In_ PVOID MemoryBlockLookaside,
    _In_ ULONG BlockSize,
    _Out_ PVOID *Block
))

NTDLL_API(NTSTATUS, __stdcall, RtlFreeMemoryBlockLookaside, (
    _In_ PVOID MemoryBlockLookaside,
    _In_ PVOID Block
))

NTDLL_API(NTSTATUS, __stdcall, RtlExtendMemoryBlockLookaside, (
    _In_ PVOID MemoryBlockLookaside,
    _In_ ULONG Increment
))

NTDLL_API(NTSTATUS, __stdcall, RtlResetMemoryBlockLookaside, (
    _In_ PVOID MemoryBlockLookaside
))

NTDLL_API(NTSTATUS, __stdcall, RtlLockMemoryBlockLookaside, (
    _In_ PVOID MemoryBlockLookaside
))

NTDLL_API(NTSTATUS, __stdcall, RtlUnlockMemoryBlockLookaside, (
    _In_ PVOID MemoryBlockLookaside
))

#endif

// end_private

// Transactions

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_VISTA)
// private
NTDLL_API(HANDLE, __stdcall, RtlGetCurrentTransaction, (
    VOID
))
#endif

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_VISTA)
// private
NTDLL_API(LOGICAL, __stdcall, RtlSetCurrentTransaction, (
    _In_ HANDLE TransactionHandle
))
#endif

// LUIDs

FORCEINLINE BOOLEAN RtlIsEqualLuid( // RtlEqualLuid
    _In_ PLUID L1,
    _In_ PLUID L2
    )
{
    return L1->LowPart == L2->LowPart &&
        L1->HighPart == L2->HighPart;
}

FORCEINLINE BOOLEAN RtlIsZeroLuid(
    _In_ PLUID L1
    )
{
    return (L1->LowPart | L1->HighPart) == 0;
}

FORCEINLINE LUID RtlConvertLongToLuid(
    _In_ LONG Long
    )
{
    LUID tempLuid;
    LARGE_INTEGER tempLi;

    tempLi.QuadPart = Long;
    tempLuid.LowPart = tempLi.LowPart;
    tempLuid.HighPart = tempLi.HighPart;

    return tempLuid;
}

FORCEINLINE LUID RtlConvertUlongToLuid(
    _In_ ULONG Ulong
    )
{
    LUID tempLuid;

    tempLuid.LowPart = Ulong;
    tempLuid.HighPart = 0;

    return tempLuid;
}

NTDLL_API_VOID(__stdcall, RtlCopyLuid, (
    _Out_ PLUID DestinationLuid,
    _In_ PLUID SourceLuid
))

// ros
NTDLL_API_VOID(__stdcall, RtlCopyLuidAndAttributesArray, (
    _In_ ULONG Count,
    _In_ PLUID_AND_ATTRIBUTES Src,
    _In_ PLUID_AND_ATTRIBUTES Dest
))

// Byte swap routines.

#ifndef PHNT_RTL_BYTESWAP
#define RtlUshortByteSwap(_x) _byteswap_ushort((USHORT)(_x))
#define RtlUlongByteSwap(_x) _byteswap_ulong((_x))
#define RtlUlonglongByteSwap(_x) _byteswap_uint64((_x))
#else
NTDLL_API(USHORT, __fastcall, RtlUshortByteSwap, (
    _In_ USHORT Source
))

NTDLL_API(ULONG, __fastcall, RtlUlongByteSwap, (
    _In_ ULONG Source
))

NTDLL_API(ULONGLONG, __fastcall, RtlUlonglongByteSwap, (
    _In_ ULONGLONG Source
))
#endif

// Debugging

// private
typedef struct _RTL_PROCESS_VERIFIER_OPTIONS
{
    ULONG SizeStruct;
    ULONG Option;
    UCHAR OptionData[1];
} RTL_PROCESS_VERIFIER_OPTIONS, *PRTL_PROCESS_VERIFIER_OPTIONS;

// private
typedef struct _RTL_DEBUG_INFORMATION
{
    HANDLE SectionHandleClient;
    PVOID ViewBaseClient;
    PVOID ViewBaseTarget;
    ULONG_PTR ViewBaseDelta;
    HANDLE EventPairClient;
    HANDLE EventPairTarget;
    HANDLE TargetProcessId;
    HANDLE TargetThreadHandle;
    ULONG Flags;
    SIZE_T OffsetFree;
    SIZE_T CommitSize;
    SIZE_T ViewSize;
    union
    {
        struct _RTL_PROCESS_MODULES *Modules;
        struct _RTL_PROCESS_MODULE_INFORMATION_EX *ModulesEx;
    };
    struct _RTL_PROCESS_BACKTRACES *BackTraces;
    struct _RTL_PROCESS_HEAPS *Heaps;
    struct _RTL_PROCESS_LOCKS *Locks;
    PVOID SpecificHeap;
    HANDLE TargetProcessHandle;
    PRTL_PROCESS_VERIFIER_OPTIONS VerifierOptions;
    PVOID ProcessHeap;
    HANDLE CriticalSectionHandle;
    HANDLE CriticalSectionOwnerThread;
    PVOID Reserved[4];
} RTL_DEBUG_INFORMATION, *PRTL_DEBUG_INFORMATION;

NTDLL_API(PRTL_DEBUG_INFORMATION, __stdcall, RtlCreateQueryDebugBuffer, (
    _In_opt_ ULONG MaximumCommit,
    _In_ BOOLEAN UseEventPair
))

NTDLL_API(NTSTATUS, __stdcall, RtlDestroyQueryDebugBuffer, (
    _In_ PRTL_DEBUG_INFORMATION Buffer
))

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_VISTA)

// private
NTDLL_API(PVOID, __stdcall, RtlCommitDebugInfo, (
    _Inout_ PRTL_DEBUG_INFORMATION Buffer,
    _In_ SIZE_T Size
))

// private
NTDLL_API_VOID(__stdcall, RtlDeCommitDebugInfo, (
    _Inout_ PRTL_DEBUG_INFORMATION Buffer,
    _In_ PVOID p,
    _In_ SIZE_T Size
))

#endif

#define RTL_QUERY_PROCESS_MODULES 0x00000001
#define RTL_QUERY_PROCESS_BACKTRACES 0x00000002
#define RTL_QUERY_PROCESS_HEAP_SUMMARY 0x00000004
#define RTL_QUERY_PROCESS_HEAP_TAGS 0x00000008
#define RTL_QUERY_PROCESS_HEAP_ENTRIES 0x00000010
#define RTL_QUERY_PROCESS_LOCKS 0x00000020
#define RTL_QUERY_PROCESS_MODULES32 0x00000040
#define RTL_QUERY_PROCESS_VERIFIER_OPTIONS 0x00000080 // rev
#define RTL_QUERY_PROCESS_MODULESEX 0x00000100 // rev
#define RTL_QUERY_PROCESS_HEAP_ENTRIES_EX 0x00000200 // ?
#define RTL_QUERY_PROCESS_CS_OWNER 0x00000400 // rev
#define RTL_QUERY_PROCESS_NONINVASIVE 0x80000000

NTDLL_API(NTSTATUS, __stdcall, RtlQueryProcessDebugInformation, (
    _In_ HANDLE UniqueProcessId,
    _In_ ULONG Flags,
    _Inout_ PRTL_DEBUG_INFORMATION Buffer
))

// Messages

NTDLL_API(NTSTATUS, __stdcall, RtlFindMessage, (
    _In_ PVOID DllHandle,
    _In_ ULONG MessageTableId,
    _In_ ULONG MessageLanguageId,
    _In_ ULONG MessageId,
    _Out_ PMESSAGE_RESOURCE_ENTRY *MessageEntry
))

NTDLL_API(NTSTATUS, __stdcall, RtlFormatMessage, (
    _In_ PWSTR MessageFormat,
    _In_ ULONG MaximumWidth,
    _In_ BOOLEAN IgnoreInserts,
    _In_ BOOLEAN ArgumentsAreAnsi,
    _In_ BOOLEAN ArgumentsAreAnArray,
    _In_ va_list *Arguments,
    _Out_writes_bytes_to_(Length, *ReturnLength) PWSTR Buffer,
    _In_ ULONG Length,
    _Out_opt_ PULONG ReturnLength
))

typedef struct _PARSE_MESSAGE_CONTEXT
{
    ULONG fFlags;
    ULONG cwSavColumn;
    SIZE_T iwSrc;
    SIZE_T iwDst;
    SIZE_T iwDstSpace;
    va_list lpvArgStart;
} PARSE_MESSAGE_CONTEXT, *PPARSE_MESSAGE_CONTEXT;

#define INIT_PARSE_MESSAGE_CONTEXT(ctx) { (ctx)->fFlags = 0; }
#define TEST_PARSE_MESSAGE_CONTEXT_FLAG(ctx, flag) ((ctx)->fFlags & (flag))
#define SET_PARSE_MESSAGE_CONTEXT_FLAG(ctx, flag) ((ctx)->fFlags |= (flag))
#define CLEAR_PARSE_MESSAGE_CONTEXT_FLAG(ctx, flag) ((ctx)->fFlags &= ~(flag))

NTDLL_API(NTSTATUS, __stdcall, RtlFormatMessageEx, (
    _In_ PWSTR MessageFormat,
    _In_ ULONG MaximumWidth,
    _In_ BOOLEAN IgnoreInserts,
    _In_ BOOLEAN ArgumentsAreAnsi,
    _In_ BOOLEAN ArgumentsAreAnArray,
    _In_ va_list *Arguments,
    _Out_writes_bytes_to_(Length, *ReturnLength) PWSTR Buffer,
    _In_ ULONG Length,
    _Out_opt_ PULONG ReturnLength,
    _Out_opt_ PPARSE_MESSAGE_CONTEXT ParseContext
))

// Errors

NTDLL_API(ULONG, __stdcall, RtlNtStatusToDosError, (
    _In_ NTSTATUS Status
))

NTDLL_API(ULONG, __stdcall, RtlNtStatusToDosErrorNoTeb, (
    _In_ NTSTATUS Status
))

NTDLL_API(NTSTATUS, __stdcall, RtlGetLastNtStatus, (
    VOID
))

NTDLL_API(LONG, __stdcall, RtlGetLastWin32Error, (
    VOID
))

NTDLL_API_VOID(__stdcall, RtlSetLastWin32ErrorAndNtStatusFromNtStatus, (
    _In_ NTSTATUS Status
))

NTDLL_API_VOID(__stdcall, RtlSetLastWin32Error, (
    _In_ LONG Win32Error
))

NTDLL_API_VOID(__stdcall, RtlRestoreLastWin32Error, (
    _In_ LONG Win32Error
))

#define RTL_ERRORMODE_FAILCRITICALERRORS 0x0010
#define RTL_ERRORMODE_NOGPFAULTERRORBOX 0x0020
#define RTL_ERRORMODE_NOOPENFILEERRORBOX 0x0040

NTDLL_API(ULONG, __stdcall, RtlGetThreadErrorMode, (
    VOID
))

NTDLL_API(NTSTATUS, __stdcall, RtlSetThreadErrorMode, (
    _In_ ULONG NewMode,
    _Out_opt_ PULONG OldMode
))

// Windows Error Reporting

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_VISTA)
// private
NTDLL_API(NTSTATUS, __stdcall, RtlReportException, (
    _In_ PEXCEPTION_RECORD ExceptionRecord,
    _In_ PCONTEXT ContextRecord,
    _In_ ULONG Flags
))
#endif

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_10_RS1)
// rev
NTDLL_API(NTSTATUS, __stdcall, RtlReportExceptionEx, (
    _In_ PEXCEPTION_RECORD ExceptionRecord,
    _In_ PCONTEXT ContextRecord,
    _In_ ULONG Flags,
    _In_ PLARGE_INTEGER Timeout
))
#endif

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_VISTA)
// private
NTDLL_API(NTSTATUS, __stdcall, RtlWerpReportException, (
    _In_ ULONG ProcessId,
    _In_ HANDLE CrashReportSharedMem,
    _In_ ULONG Flags,
    _Out_ PHANDLE CrashVerticalProcessHandle
))
#endif

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_7)
// rev
NTDLL_API(NTSTATUS, __stdcall, RtlReportSilentProcessExit, (
    _In_ HANDLE ProcessHandle,
    _In_ NTSTATUS ExitStatus
))
#endif

// Vectored Exception Handlers

/*
NTDLL_API(PVOID, __stdcall, RtlAddVectoredExceptionHandler, (
    _In_ ULONG First,
    _In_ PVECTORED_EXCEPTION_HANDLER Handler
))

NTDLL_API(ULONG, __stdcall, RtlRemoveVectoredExceptionHandler, (
    _In_ PVOID Handle
))

NTDLL_API(PVOID, __stdcall, RtlAddVectoredContinueHandler, (
    _In_ ULONG First,
    _In_ PVECTORED_EXCEPTION_HANDLER Handler
))

NTDLL_API(ULONG, __stdcall, RtlRemoveVectoredContinueHandler, (
    _In_ PVOID Handle
))
*/

// Random

NTDLL_API(ULONG, __stdcall, RtlUniform, (
    _Inout_ PULONG Seed
))

NTDLL_API(ULONG, __stdcall, RtlRandom, (
    _Inout_ PULONG Seed
))

NTDLL_API(ULONG, __stdcall, RtlRandomEx, (
    _Inout_ PULONG Seed
))

NTDLL_API(NTSTATUS, __stdcall, RtlComputeImportTableHash, (
    _In_ HANDLE FileHandle,
    _Out_writes_bytes_(16) PCHAR Hash,
    _In_ ULONG ImportTableHashRevision // must be 1
))

// Integer conversion

NTDLL_API(NTSTATUS, __stdcall, RtlIntegerToChar, (
    _In_ ULONG Value,
    _In_opt_ ULONG Base,
    _In_ LONG OutputLength, // negative to pad to width
    _Out_ PSTR String
))

NTDLL_API(NTSTATUS, __stdcall, RtlCharToInteger, (
    _In_ PSTR String,
    _In_opt_ ULONG Base,
    _Out_ PULONG Value
))

NTDLL_API(NTSTATUS, __stdcall, RtlLargeIntegerToChar, (
    _In_ PLARGE_INTEGER Value,
    _In_opt_ ULONG Base,
    _In_ LONG OutputLength,
    _Out_ PSTR String
))

NTDLL_API(NTSTATUS, __stdcall, RtlIntegerToUnicodeString, (
    _In_ ULONG Value,
    _In_opt_ ULONG Base,
    _Inout_ PUNICODE_STRING String
))

NTDLL_API(NTSTATUS, __stdcall, RtlInt64ToUnicodeString, (
    _In_ ULONGLONG Value,
    _In_opt_ ULONG Base,
    _Inout_ PUNICODE_STRING String
))

#ifdef _WIN64
#define RtlIntPtrToUnicodeString(Value, Base, String) RtlInt64ToUnicodeString(Value, Base, String)
#else
#define RtlIntPtrToUnicodeString(Value, Base, String) RtlIntegerToUnicodeString(Value, Base, String)
#endif

NTDLL_API(NTSTATUS, __stdcall, RtlUnicodeStringToInteger, (
    _In_ PUNICODE_STRING String,
    _In_opt_ ULONG Base,
    _Out_ PULONG Value
))

// IPv4/6 conversion

struct in_addr;
struct in6_addr;

NTDLL_API(PWSTR, __stdcall, RtlIpv4AddressToStringW, (
    _In_ const struct in_addr *Address,
    _Out_writes_(16) PWSTR AddressString
))

NTDLL_API(NTSTATUS, __stdcall, RtlIpv4AddressToStringExW, (
    _In_ const struct in_addr *Address,
    _In_ USHORT Port,
    _Out_writes_to_(*AddressStringLength, *AddressStringLength) PWSTR AddressString,
    _Inout_ PULONG AddressStringLength
))

NTDLL_API(PWSTR, __stdcall, RtlIpv6AddressToStringW, (
    _In_ const struct in6_addr *Address,
    _Out_writes_(46) PWSTR AddressString
))

NTDLL_API(NTSTATUS, __stdcall, RtlIpv6AddressToStringExW, (
    _In_ const struct in6_addr *Address,
    _In_ ULONG ScopeId,
    _In_ USHORT Port,
    _Out_writes_to_(*AddressStringLength, *AddressStringLength) PWSTR AddressString,
    _Inout_ PULONG AddressStringLength
))

NTDLL_API(NTSTATUS, __stdcall, RtlIpv4StringToAddressW, (
    _In_ PCWSTR AddressString,
    _In_ BOOLEAN Strict,
    _Out_ LPCWSTR *Terminator,
    _Out_ struct in_addr *Address
))

NTDLL_API(NTSTATUS, __stdcall, RtlIpv4StringToAddressExW, (
    _In_ PCWSTR AddressString,
    _In_ BOOLEAN Strict,
    _Out_ struct in_addr *Address,
    _Out_ PUSHORT Port
))

NTDLL_API(NTSTATUS, __stdcall, RtlIpv6StringToAddressW, (
    _In_ PCWSTR AddressString,
    _Out_ PCWSTR *Terminator,
    _Out_ struct in6_addr *Address
))

NTDLL_API(NTSTATUS, __stdcall, RtlIpv6StringToAddressExW, (
    _In_ PCWSTR AddressString,
    _Out_ struct in6_addr *Address,
    _Out_ PULONG ScopeId,
    _Out_ PUSHORT Port
))

#define RtlIpv4AddressToString RtlIpv4AddressToStringW
#define RtlIpv4AddressToStringEx RtlIpv4AddressToStringExW
#define RtlIpv6AddressToString RtlIpv6AddressToStringW
#define RtlIpv6AddressToStringEx RtlIpv6AddressToStringExW
#define RtlIpv4StringToAddress RtlIpv4StringToAddressW
#define RtlIpv4StringToAddressEx RtlIpv4StringToAddressExW
#define RtlIpv6StringToAddress RtlIpv6StringToAddressW
#define RtlIpv6StringToAddressEx RtlIpv6StringToAddressExW

// Time

typedef struct _TIME_FIELDS
{
    CSHORT Year; // 1601...
    CSHORT Month; // 1..12
    CSHORT Day; // 1..31
    CSHORT Hour; // 0..23
    CSHORT Minute; // 0..59
    CSHORT Second; // 0..59
    CSHORT Milliseconds; // 0..999
    CSHORT Weekday; // 0..6 = Sunday..Saturday
} TIME_FIELDS, *PTIME_FIELDS;

NTDLL_API(BOOLEAN, __stdcall, RtlCutoverTimeToSystemTime, (
    _In_ PTIME_FIELDS CutoverTime,
    _Out_ PLARGE_INTEGER SystemTime,
    _In_ PLARGE_INTEGER CurrentSystemTime,
    _In_ BOOLEAN ThisYear
))

NTDLL_API(NTSTATUS, __stdcall, RtlSystemTimeToLocalTime, (
    _In_ PLARGE_INTEGER SystemTime,
    _Out_ PLARGE_INTEGER LocalTime
))

NTDLL_API(NTSTATUS, __stdcall, RtlLocalTimeToSystemTime, (
    _In_ PLARGE_INTEGER LocalTime,
    _Out_ PLARGE_INTEGER SystemTime
))

NTDLL_API_VOID(__stdcall, RtlTimeToElapsedTimeFields, (
    _In_ PLARGE_INTEGER Time,
    _Out_ PTIME_FIELDS TimeFields
))

NTDLL_API_VOID(__stdcall, RtlTimeToTimeFields, (
    _In_ PLARGE_INTEGER Time,
    _Out_ PTIME_FIELDS TimeFields
))

NTDLL_API(BOOLEAN, __stdcall, RtlTimeFieldsToTime, (
    _In_ PTIME_FIELDS TimeFields, // Weekday is ignored
    _Out_ PLARGE_INTEGER Time
))

NTDLL_API(BOOLEAN, __stdcall, RtlTimeToSecondsSince1980, (
    _In_ PLARGE_INTEGER Time,
    _Out_ PULONG ElapsedSeconds
))

NTDLL_API_VOID(__stdcall, RtlSecondsSince1980ToTime, (
    _In_ ULONG ElapsedSeconds,
    _Out_ PLARGE_INTEGER Time
))

NTDLL_API(BOOLEAN, __stdcall, RtlTimeToSecondsSince1970, (
    _In_ PLARGE_INTEGER Time,
    _Out_ PULONG ElapsedSeconds
))

NTDLL_API_VOID(__stdcall, RtlSecondsSince1970ToTime, (
    _In_ ULONG ElapsedSeconds,
    _Out_ PLARGE_INTEGER Time
))

// Time zones

typedef struct _RTL_TIME_ZONE_INFORMATION
{
    LONG Bias;
    WCHAR StandardName[32];
    TIME_FIELDS StandardStart;
    LONG StandardBias;
    WCHAR DaylightName[32];
    TIME_FIELDS DaylightStart;
    LONG DaylightBias;
} RTL_TIME_ZONE_INFORMATION, *PRTL_TIME_ZONE_INFORMATION;

NTDLL_API(NTSTATUS, __stdcall, RtlQueryTimeZoneInformation, (
    _Out_ PRTL_TIME_ZONE_INFORMATION TimeZoneInformation
))

NTDLL_API(NTSTATUS, __stdcall, RtlSetTimeZoneInformation, (
    _In_ PRTL_TIME_ZONE_INFORMATION TimeZoneInformation
))

// Bitmaps

typedef struct _RTL_BITMAP
{
    ULONG SizeOfBitMap;
    PULONG Buffer;
} RTL_BITMAP, *PRTL_BITMAP;

NTDLL_API_VOID(__stdcall, RtlInitializeBitMap, (
    _Out_ PRTL_BITMAP BitMapHeader,
    _In_ PULONG BitMapBuffer,
    _In_ ULONG SizeOfBitMap
))

#if (defined(PHNT_COMPILE) || (NTLIB_CPU_MODE == NTLIB_KERNEL_MODE || NTLIB_WIN_VERSION >= NTLIB_WIN_8))
NTDLL_API_VOID(__stdcall, RtlClearBit, (
    _In_ PRTL_BITMAP BitMapHeader,
    _In_range_(<, BitMapHeader->SizeOfBitMap) ULONG BitNumber
))
#endif

#if (defined(PHNT_COMPILE) || (NTLIB_CPU_MODE == NTLIB_KERNEL_MODE || NTLIB_WIN_VERSION >= NTLIB_WIN_8))
NTDLL_API_VOID(__stdcall, RtlSetBit, (
    _In_ PRTL_BITMAP BitMapHeader,
    _In_range_(<, BitMapHeader->SizeOfBitMap) ULONG BitNumber
))
#endif

_Check_return_
NTDLL_API(BOOLEAN, __stdcall, RtlTestBit, (
    _In_ PRTL_BITMAP BitMapHeader,
    _In_range_(<, BitMapHeader->SizeOfBitMap) ULONG BitNumber
))

NTDLL_API_VOID(__stdcall, RtlClearAllBits, (
    _In_ PRTL_BITMAP BitMapHeader
))

NTDLL_API_VOID(__stdcall, RtlSetAllBits, (
    _In_ PRTL_BITMAP BitMapHeader
))

_Success_(return != -1)
_Check_return_
NTDLL_API(ULONG, __stdcall, RtlFindClearBits, (
    _In_ PRTL_BITMAP BitMapHeader,
    _In_ ULONG NumberToFind,
    _In_ ULONG HintIndex
))

_Success_(return != -1)
_Check_return_
NTDLL_API(ULONG, __stdcall, RtlFindSetBits, (
    _In_ PRTL_BITMAP BitMapHeader,
    _In_ ULONG NumberToFind,
    _In_ ULONG HintIndex
))

_Success_(return != -1)
NTDLL_API(ULONG, __stdcall, RtlFindClearBitsAndSet, (
    _In_ PRTL_BITMAP BitMapHeader,
    _In_ ULONG NumberToFind,
    _In_ ULONG HintIndex
))

_Success_(return != -1)
NTDLL_API(ULONG, __stdcall, RtlFindSetBitsAndClear, (
    _In_ PRTL_BITMAP BitMapHeader,
    _In_ ULONG NumberToFind,
    _In_ ULONG HintIndex
))

NTDLL_API_VOID(__stdcall, RtlClearBits, (
    _In_ PRTL_BITMAP BitMapHeader,
    _In_range_(0, BitMapHeader->SizeOfBitMap - NumberToClear) ULONG StartingIndex,
    _In_range_(0, BitMapHeader->SizeOfBitMap - StartingIndex) ULONG NumberToClear
))

NTDLL_API_VOID(__stdcall, RtlSetBits, (
    _In_ PRTL_BITMAP BitMapHeader,
    _In_range_(0, BitMapHeader->SizeOfBitMap - NumberToSet) ULONG StartingIndex,
    _In_range_(0, BitMapHeader->SizeOfBitMap - StartingIndex) ULONG NumberToSet
))

NTDLL_API(CCHAR, __stdcall, RtlFindMostSignificantBit, (
    _In_ ULONGLONG Set
))

NTDLL_API(CCHAR, __stdcall, RtlFindLeastSignificantBit, (
    _In_ ULONGLONG Set
))

typedef struct _RTL_BITMAP_RUN
{
    ULONG StartingIndex;
    ULONG NumberOfBits;
} RTL_BITMAP_RUN, *PRTL_BITMAP_RUN;

NTDLL_API(ULONG, __stdcall, RtlFindClearRuns, (
    _In_ PRTL_BITMAP BitMapHeader,
    _Out_writes_to_(SizeOfRunArray, return) PRTL_BITMAP_RUN RunArray,
    _In_range_(>, 0) ULONG SizeOfRunArray,
    _In_ BOOLEAN LocateLongestRuns
))

NTDLL_API(ULONG, __stdcall, RtlFindLongestRunClear, (
    _In_ PRTL_BITMAP BitMapHeader,
    _Out_ PULONG StartingIndex
))

NTDLL_API(ULONG, __stdcall, RtlFindFirstRunClear, (
    _In_ PRTL_BITMAP BitMapHeader,
    _Out_ PULONG StartingIndex
))

_Check_return_
FORCEINLINE
BOOLEAN
RtlCheckBit(
    _In_ PRTL_BITMAP BitMapHeader,
    _In_range_(<, BitMapHeader->SizeOfBitMap) ULONG BitPosition
    )
{
#ifdef _WIN64
    return BitTest64((LONG64 const *)BitMapHeader->Buffer, (LONG64)BitPosition);
#else
    return (((PLONG)BitMapHeader->Buffer)[BitPosition / 32] >> (BitPosition % 32)) & 0x1;
#endif
}

NTDLL_API(ULONG, __stdcall, RtlNumberOfClearBits, (
    _In_ PRTL_BITMAP BitMapHeader
))

NTDLL_API(ULONG, __stdcall, RtlNumberOfSetBits, (
    _In_ PRTL_BITMAP BitMapHeader
))

_Check_return_
NTDLL_API(BOOLEAN, __stdcall, RtlAreBitsClear, (
    _In_ PRTL_BITMAP BitMapHeader,
    _In_ ULONG StartingIndex,
    _In_ ULONG Length
))

_Check_return_
NTDLL_API(BOOLEAN, __stdcall, RtlAreBitsSet, (
    _In_ PRTL_BITMAP BitMapHeader,
    _In_ ULONG StartingIndex,
    _In_ ULONG Length
))

NTDLL_API(ULONG, __stdcall, RtlFindNextForwardRunClear, (
    _In_ PRTL_BITMAP BitMapHeader,
    _In_ ULONG FromIndex,
    _Out_ PULONG StartingRunIndex
))

NTDLL_API(ULONG, __stdcall, RtlFindLastBackwardRunClear, (
    _In_ PRTL_BITMAP BitMapHeader,
    _In_ ULONG FromIndex,
    _Out_ PULONG StartingRunIndex
))

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_VISTA)

NTDLL_API(ULONG, __stdcall, RtlNumberOfSetBitsUlongPtr, (
    _In_ ULONG_PTR Target
))

#endif

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_7)

// rev
NTDLL_API_VOID(__stdcall, RtlInterlockedClearBitRun, (
    _In_ PRTL_BITMAP BitMapHeader,
    _In_range_(0, BitMapHeader->SizeOfBitMap - NumberToClear) ULONG StartingIndex,
    _In_range_(0, BitMapHeader->SizeOfBitMap - StartingIndex) ULONG NumberToClear
))

// rev
NTDLL_API_VOID(__stdcall, RtlInterlockedSetBitRun, (
    _In_ PRTL_BITMAP BitMapHeader,
    _In_range_(0, BitMapHeader->SizeOfBitMap - NumberToSet) ULONG StartingIndex,
    _In_range_(0, BitMapHeader->SizeOfBitMap - StartingIndex) ULONG NumberToSet
))

#endif

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_8)

NTDLL_API_VOID(__stdcall, RtlCopyBitMap, (
    _In_ PRTL_BITMAP Source,
    _In_ PRTL_BITMAP Destination,
    _In_range_(0, Destination->SizeOfBitMap - 1) ULONG TargetBit
))

NTDLL_API_VOID(__stdcall, RtlExtractBitMap, (
    _In_ PRTL_BITMAP Source,
    _In_ PRTL_BITMAP Destination,
    _In_range_(0, Source->SizeOfBitMap - 1) ULONG TargetBit,
    _In_range_(0, Source->SizeOfBitMap) ULONG NumberOfBits
))

NTDLL_API(ULONG, __stdcall, RtlNumberOfClearBitsInRange, (
    _In_ PRTL_BITMAP BitMapHeader,
    _In_ ULONG StartingIndex,
    _In_ ULONG Length
))

NTDLL_API(ULONG, __stdcall, RtlNumberOfSetBitsInRange, (
    _In_ PRTL_BITMAP BitMapHeader,
    _In_ ULONG StartingIndex,
    _In_ ULONG Length
))

#endif


#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_10_TH1)

// private
typedef struct _RTL_BITMAP_EX
{
    ULONG64 SizeOfBitMap;
    PULONG64 Buffer;
} RTL_BITMAP_EX, *PRTL_BITMAP_EX;

// rev
NTDLL_API_VOID(__stdcall, RtlInitializeBitMapEx, (
    _Out_ PRTL_BITMAP_EX BitMapHeader,
    _In_ PULONG64 BitMapBuffer,
    _In_ ULONG64 SizeOfBitMap
))

// rev
_Check_return_
NTDLL_API(BOOLEAN, __stdcall, RtlTestBitEx, (
    _In_ PRTL_BITMAP_EX BitMapHeader,
    _In_range_(<, BitMapHeader->SizeOfBitMap) ULONG64 BitNumber
))

#if (defined(PHNT_COMPILE) || NTLIB_CPU_MODE == NTLIB_KERNEL_MODE)
// rev
NTDLL_API_VOID(__stdcall, RtlClearAllBitsEx, (
    _In_ PRTL_BITMAP_EX BitMapHeader
))

// rev
NTDLL_API_VOID(__stdcall, RtlClearBitEx, (
    _In_ PRTL_BITMAP_EX BitMapHeader,
    _In_range_(<, BitMapHeader->SizeOfBitMap) ULONG64 BitNumber
))

// rev
NTDLL_API_VOID(__stdcall, RtlSetBitEx, (
    _In_ PRTL_BITMAP_EX BitMapHeader,
    _In_range_(<, BitMapHeader->SizeOfBitMap) ULONG64 BitNumber
))

// rev
NTDLL_API(ULONG64, __stdcall, RtlFindSetBitsEx, (
    _In_ PRTL_BITMAP_EX BitMapHeader,
    _In_ ULONG64 NumberToFind,
    _In_ ULONG64 HintIndex
))

NTDLL_API(ULONG64, __stdcall, RtlFindSetBitsAndClearEx, (
    _In_ PRTL_BITMAP_EX BitMapHeader,
    _In_ ULONG64 NumberToFind,
    _In_ ULONG64 HintIndex
))
#endif

#endif

// Handle tables

typedef struct _RTL_HANDLE_TABLE_ENTRY
{
    union
    {
        ULONG Flags; // allocated entries have the low bit set
        struct _RTL_HANDLE_TABLE_ENTRY *NextFree;
    };
} RTL_HANDLE_TABLE_ENTRY, *PRTL_HANDLE_TABLE_ENTRY;

#define RTL_HANDLE_ALLOCATED (USHORT)0x0001

typedef struct _RTL_HANDLE_TABLE
{
    ULONG MaximumNumberOfHandles;
    ULONG SizeOfHandleTableEntry;
    ULONG Reserved[2];
    PRTL_HANDLE_TABLE_ENTRY FreeHandles;
    PRTL_HANDLE_TABLE_ENTRY CommittedHandles;
    PRTL_HANDLE_TABLE_ENTRY UnCommittedHandles;
    PRTL_HANDLE_TABLE_ENTRY MaxReservedHandles;
} RTL_HANDLE_TABLE, *PRTL_HANDLE_TABLE;

NTDLL_API_VOID(__stdcall, RtlInitializeHandleTable, (
    _In_ ULONG MaximumNumberOfHandles,
    _In_ ULONG SizeOfHandleTableEntry,
    _Out_ PRTL_HANDLE_TABLE HandleTable
))

NTDLL_API(NTSTATUS, __stdcall, RtlDestroyHandleTable, (
    _Inout_ PRTL_HANDLE_TABLE HandleTable
))

NTDLL_API(PRTL_HANDLE_TABLE_ENTRY, __stdcall, RtlAllocateHandle, (
    _In_ PRTL_HANDLE_TABLE HandleTable,
    _Out_opt_ PULONG HandleIndex
))

NTDLL_API(BOOLEAN, __stdcall, RtlFreeHandle, (
    _In_ PRTL_HANDLE_TABLE HandleTable,
    _In_ PRTL_HANDLE_TABLE_ENTRY Handle
))

NTDLL_API(BOOLEAN, __stdcall, RtlIsValidHandle, (
    _In_ PRTL_HANDLE_TABLE HandleTable,
    _In_ PRTL_HANDLE_TABLE_ENTRY Handle
))

NTDLL_API(BOOLEAN, __stdcall, RtlIsValidIndexHandle, (
    _In_ PRTL_HANDLE_TABLE HandleTable,
    _In_ ULONG HandleIndex,
    _Out_ PRTL_HANDLE_TABLE_ENTRY *Handle
))

// Atom tables

#define RTL_ATOM_MAXIMUM_INTEGER_ATOM (RTL_ATOM)0xc000
#define RTL_ATOM_INVALID_ATOM (RTL_ATOM)0x0000
#define RTL_ATOM_TABLE_DEFAULT_NUMBER_OF_BUCKETS 37
#define RTL_ATOM_MAXIMUM_NAME_LENGTH 255
#define RTL_ATOM_PINNED 0x01

NTDLL_API(NTSTATUS, __stdcall, RtlCreateAtomTable, (
    _In_ ULONG NumberOfBuckets,
    _Out_ PVOID *AtomTableHandle
))

NTDLL_API(NTSTATUS, __stdcall, RtlDestroyAtomTable, (
    _In_ _Post_invalid_ PVOID AtomTableHandle
))

NTDLL_API(NTSTATUS, __stdcall, RtlEmptyAtomTable, (
    _In_ PVOID AtomTableHandle,
    _In_ BOOLEAN IncludePinnedAtoms
))

NTDLL_API(NTSTATUS, __stdcall, RtlAddAtomToAtomTable, (
    _In_ PVOID AtomTableHandle,
    _In_ PWSTR AtomName,
    _Inout_opt_ PRTL_ATOM Atom
))

NTDLL_API(NTSTATUS, __stdcall, RtlLookupAtomInAtomTable, (
    _In_ PVOID AtomTableHandle,
    _In_ PWSTR AtomName,
    _Out_opt_ PRTL_ATOM Atom
))

NTDLL_API(NTSTATUS, __stdcall, RtlDeleteAtomFromAtomTable, (
    _In_ PVOID AtomTableHandle,
    _In_ RTL_ATOM Atom
))

NTDLL_API(NTSTATUS, __stdcall, RtlPinAtomInAtomTable, (
    _In_ PVOID AtomTableHandle,
    _In_ RTL_ATOM Atom
))

NTDLL_API(NTSTATUS, __stdcall, RtlQueryAtomInAtomTable, (
    _In_ PVOID AtomTableHandle,
    _In_ RTL_ATOM Atom,
    _Out_opt_ PULONG AtomUsage,
    _Out_opt_ PULONG AtomFlags,
    _Inout_updates_bytes_to_opt_(*AtomNameLength, *AtomNameLength) PWSTR AtomName,
    _Inout_opt_ PULONG AtomNameLength
))

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_VISTA)
// rev
NTDLL_API(BOOLEAN, __stdcall, RtlGetIntegerAtom, (
    _In_ PWSTR AtomName,
    _Out_opt_ PUSHORT IntegerAtom
))
#endif

// SIDs

_Check_return_
NTDLL_API(BOOLEAN, __stdcall, RtlValidSid, (
    _In_ PSID Sid
))

_Check_return_
NTDLL_API(BOOLEAN, __stdcall, RtlEqualSid, (
    _In_ PSID Sid1,
    _In_ PSID Sid2
))

_Check_return_
NTDLL_API(BOOLEAN, __stdcall, RtlEqualPrefixSid, (
    _In_ PSID Sid1,
    _In_ PSID Sid2
))

NTDLL_API(ULONG, __stdcall, RtlLengthRequiredSid, (
    _In_ ULONG SubAuthorityCount
))

NTDLL_API(PVOID, __stdcall, RtlFreeSid, (
    _In_ _Post_invalid_ PSID Sid
))

_Check_return_
NTDLL_API(NTSTATUS, __stdcall, RtlAllocateAndInitializeSid, (
    _In_ PSID_IDENTIFIER_AUTHORITY IdentifierAuthority,
    _In_ UCHAR SubAuthorityCount,
    _In_ ULONG SubAuthority0,
    _In_ ULONG SubAuthority1,
    _In_ ULONG SubAuthority2,
    _In_ ULONG SubAuthority3,
    _In_ ULONG SubAuthority4,
    _In_ ULONG SubAuthority5,
    _In_ ULONG SubAuthority6,
    _In_ ULONG SubAuthority7,
    _Outptr_ PSID *Sid
))

NTDLL_API(NTSTATUS, __stdcall, RtlInitializeSid, (
    _Out_ PSID Sid,
    _In_ PSID_IDENTIFIER_AUTHORITY IdentifierAuthority,
    _In_ UCHAR SubAuthorityCount
))

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_10_TH1)
NTDLL_API(NTSTATUS, __stdcall, RtlInitializeSidEx, (
    _Out_writes_bytes_(SECURITY_SID_SIZE(SubAuthorityCount)) PSID Sid,
    _In_ PSID_IDENTIFIER_AUTHORITY IdentifierAuthority,
    _In_ UCHAR SubAuthorityCount,
    ...
))
#endif

NTDLL_API(PSID_IDENTIFIER_AUTHORITY, __stdcall, RtlIdentifierAuthoritySid, (
    _In_ PSID Sid
))

NTDLL_API(PULONG, __stdcall, RtlSubAuthoritySid, (
    _In_ PSID Sid,
    _In_ ULONG SubAuthority
))

NTDLL_API(PUCHAR, __stdcall, RtlSubAuthorityCountSid, (
    _In_ PSID Sid
))

NTDLL_API(ULONG, __stdcall, RtlLengthSid, (
    _In_ PSID Sid
))

NTDLL_API(NTSTATUS, __stdcall, RtlCopySid, (
    _In_ ULONG DestinationSidLength,
    _In_reads_bytes_(DestinationSidLength) PSID DestinationSid,
    _In_ PSID SourceSid
))

// ros
NTDLL_API(NTSTATUS, __stdcall, RtlCopySidAndAttributesArray, (
    _In_ ULONG Count,
    _In_ PSID_AND_ATTRIBUTES Src,
    _In_ ULONG SidAreaSize,
    _In_ PSID_AND_ATTRIBUTES Dest,
    _In_ PSID SidArea,
    _Out_ PSID *RemainingSidArea,
    _Out_ PULONG RemainingSidAreaSize
))

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_VISTA)

NTDLL_API(NTSTATUS, __stdcall, RtlCreateServiceSid, (
    _In_ PUNICODE_STRING ServiceName,
    _Out_writes_bytes_opt_(*ServiceSidLength) PSID ServiceSid,
    _Inout_ PULONG ServiceSidLength
))

#endif

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_VISTA)

// private
NTDLL_API(NTSTATUS, __stdcall, RtlSidDominates, (
    _In_ PSID Sid1,
    _In_ PSID Sid2,
    _Out_ PBOOLEAN Dominates
))

#endif

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_8_1)

// rev
NTDLL_API(NTSTATUS, __stdcall, RtlSidDominatesForTrust, (
    _In_ PSID Sid1,
    _In_ PSID Sid2,
    _Out_ PBOOLEAN DominatesTrust // TokenProcessTrustLevel
))

#endif

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_VISTA)
// private
NTDLL_API(NTSTATUS, __stdcall, RtlSidEqualLevel, (
    _In_ PSID Sid1,
    _In_ PSID Sid2,
    _Out_ PBOOLEAN EqualLevel
))

// private
NTDLL_API(NTSTATUS, __stdcall, RtlSidIsHigherLevel, (
    _In_ PSID Sid1,
    _In_ PSID Sid2,
    _Out_ PBOOLEAN HigherLevel
))
#endif

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_7)
NTDLL_API(NTSTATUS, __stdcall, RtlCreateVirtualAccountSid, (
    _In_ PUNICODE_STRING Name,
    _In_ ULONG BaseSubAuthority,
    _Out_writes_bytes_(*SidLength) PSID Sid,
    _Inout_ PULONG SidLength
))
#endif

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_7)
NTDLL_API(NTSTATUS, __stdcall, RtlReplaceSidInSd, (
    _Inout_ PSECURITY_DESCRIPTOR SecurityDescriptor,
    _In_ PSID OldSid,
    _In_ PSID NewSid,
    _Out_ ULONG *NumChanges
))
#endif

#define MAX_UNICODE_STACK_BUFFER_LENGTH 256

NTDLL_API(NTSTATUS, __stdcall, RtlConvertSidToUnicodeString, (
    _Inout_ PUNICODE_STRING UnicodeString,
    _In_ PSID Sid,
    _In_ BOOLEAN AllocateDestinationString
))

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_VISTA)
// private
NTDLL_API(NTSTATUS, __stdcall, RtlSidHashInitialize, (
    _In_reads_(SidCount) PSID_AND_ATTRIBUTES SidAttr,
    _In_ ULONG SidCount,
    _Out_ PSID_AND_ATTRIBUTES_HASH SidAttrHash
))
#endif

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_VISTA)
// private
NTDLL_API(PSID_AND_ATTRIBUTES, __stdcall, RtlSidHashLookup, (
    _In_ PSID_AND_ATTRIBUTES_HASH SidAttrHash,
    _In_ PSID Sid
))
#endif

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_VISTA)
// rev
NTDLL_API(BOOLEAN, __stdcall, RtlIsElevatedRid, (
    _In_ PSID_AND_ATTRIBUTES SidAttr
))
#endif

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_10_RS12)
// rev
NTDLL_API(NTSTATUS, __stdcall, RtlDeriveCapabilitySidsFromName, (
    _Inout_ PUNICODE_STRING UnicodeString,
    _Out_ PSID CapabilityGroupSid,
    _Out_ PSID CapabilitySid
))
#endif

// Security Descriptors

NTDLL_API(NTSTATUS, __stdcall, RtlCreateSecurityDescriptor, (
    _Out_ PSECURITY_DESCRIPTOR SecurityDescriptor,
    _In_ ULONG Revision
))

_Check_return_
NTDLL_API(BOOLEAN, __stdcall, RtlValidSecurityDescriptor, (
    _In_ PSECURITY_DESCRIPTOR SecurityDescriptor
))

NTDLL_API(ULONG, __stdcall, RtlLengthSecurityDescriptor, (
    _In_ PSECURITY_DESCRIPTOR SecurityDescriptor
))

_Check_return_
NTDLL_API(BOOLEAN, __stdcall, RtlValidRelativeSecurityDescriptor, (
    _In_reads_bytes_(SecurityDescriptorLength) PSECURITY_DESCRIPTOR SecurityDescriptorInput,
    _In_ ULONG SecurityDescriptorLength,
    _In_ SECURITY_INFORMATION RequiredInformation
))

NTDLL_API(NTSTATUS, __stdcall, RtlGetControlSecurityDescriptor, (
    _In_ PSECURITY_DESCRIPTOR SecurityDescriptor,
    _Out_ PSECURITY_DESCRIPTOR_CONTROL Control,
    _Out_ PULONG Revision
))

NTDLL_API(NTSTATUS, __stdcall, RtlSetControlSecurityDescriptor, (
    _Inout_ PSECURITY_DESCRIPTOR SecurityDescriptor,
     _In_ SECURITY_DESCRIPTOR_CONTROL ControlBitsOfInterest,
     _In_ SECURITY_DESCRIPTOR_CONTROL ControlBitsToSet
))

NTDLL_API(NTSTATUS, __stdcall, RtlSetAttributesSecurityDescriptor, (
    _Inout_ PSECURITY_DESCRIPTOR SecurityDescriptor,
    _In_ SECURITY_DESCRIPTOR_CONTROL Control,
    _Out_ PULONG Revision
))

NTDLL_API(BOOLEAN, __stdcall, RtlGetSecurityDescriptorRMControl, (
    _In_ PSECURITY_DESCRIPTOR SecurityDescriptor,
    _Out_ PUCHAR RMControl
))

NTDLL_API_VOID(__stdcall, RtlSetSecurityDescriptorRMControl, (
    _Inout_ PSECURITY_DESCRIPTOR SecurityDescriptor,
    _In_opt_ PUCHAR RMControl
))

NTDLL_API(NTSTATUS, __stdcall, RtlSetDaclSecurityDescriptor, (
    _Inout_ PSECURITY_DESCRIPTOR SecurityDescriptor,
    _In_ BOOLEAN DaclPresent,
    _In_opt_ PACL Dacl,
    _In_opt_ BOOLEAN DaclDefaulted
))

NTDLL_API(NTSTATUS, __stdcall, RtlGetDaclSecurityDescriptor, (
    _In_ PSECURITY_DESCRIPTOR SecurityDescriptor,
    _Out_ PBOOLEAN DaclPresent,
    _Out_ PACL *Dacl,
    _Out_ PBOOLEAN DaclDefaulted
))

NTDLL_API(NTSTATUS, __stdcall, RtlSetSaclSecurityDescriptor, (
    _Inout_ PSECURITY_DESCRIPTOR SecurityDescriptor,
    _In_ BOOLEAN SaclPresent,
    _In_opt_ PACL Sacl,
    _In_opt_ BOOLEAN SaclDefaulted
))

/*
NTDLL_API(NTSTATUS, __stdcall, RtlGetSaclSecurityDescriptor, (
    _In_ PSECURITY_DESCRIPTOR SecurityDescriptor,
    _Out_ PBOOLEAN SaclPresent,
    _Out_ PACL *Sacl,
    _Out_ PBOOLEAN SaclDefaulted
))
*/

NTDLL_API(NTSTATUS, __stdcall, RtlGetSaclSecurityDescriptor, (
    _In_ PSECURITY_DESCRIPTOR SecurityDescriptor,
    _Out_ PBOOLEAN SaclPresent,
    _Out_ PACL *Sacl,
    _Out_ PBOOLEAN SaclDefaulted
))

NTDLL_API(NTSTATUS, __stdcall, RtlSetOwnerSecurityDescriptor, (
    _Inout_ PSECURITY_DESCRIPTOR SecurityDescriptor,
    _In_opt_ PSID Owner,
    _In_opt_ BOOLEAN OwnerDefaulted
))

NTDLL_API(NTSTATUS, __stdcall, RtlGetOwnerSecurityDescriptor, (
    _In_ PSECURITY_DESCRIPTOR SecurityDescriptor,
    _Out_ PSID *Owner,
    _Out_ PBOOLEAN OwnerDefaulted
))

NTDLL_API(NTSTATUS, __stdcall, RtlSetGroupSecurityDescriptor, (
    _Inout_ PSECURITY_DESCRIPTOR SecurityDescriptor,
    _In_opt_ PSID Group,
    _In_opt_ BOOLEAN GroupDefaulted
))

NTDLL_API(NTSTATUS, __stdcall, RtlGetGroupSecurityDescriptor, (
    _In_ PSECURITY_DESCRIPTOR SecurityDescriptor,
    _Out_ PSID *Group,
    _Out_ PBOOLEAN GroupDefaulted
))

NTDLL_API(NTSTATUS, __stdcall, RtlMakeSelfRelativeSD, (
    _In_ PSECURITY_DESCRIPTOR AbsoluteSecurityDescriptor,
    _Out_writes_bytes_(*BufferLength) PSECURITY_DESCRIPTOR SelfRelativeSecurityDescriptor,
    _Inout_ PULONG BufferLength
))

NTDLL_API(NTSTATUS, __stdcall, RtlAbsoluteToSelfRelativeSD, (
    _In_ PSECURITY_DESCRIPTOR AbsoluteSecurityDescriptor,
    _Out_writes_bytes_to_opt_(*BufferLength, *BufferLength) PSECURITY_DESCRIPTOR SelfRelativeSecurityDescriptor,
    _Inout_ PULONG BufferLength
))

NTDLL_API(NTSTATUS, __stdcall, RtlSelfRelativeToAbsoluteSD, (
    _In_ PSECURITY_DESCRIPTOR SelfRelativeSecurityDescriptor,
    _Out_writes_bytes_to_opt_(*AbsoluteSecurityDescriptorSize, *AbsoluteSecurityDescriptorSize) PSECURITY_DESCRIPTOR AbsoluteSecurityDescriptor,
    _Inout_ PULONG AbsoluteSecurityDescriptorSize,
    _Out_writes_bytes_to_opt_(*DaclSize, *DaclSize) PACL Dacl,
    _Inout_ PULONG DaclSize,
    _Out_writes_bytes_to_opt_(*SaclSize, *SaclSize) PACL Sacl,
    _Inout_ PULONG SaclSize,
    _Out_writes_bytes_to_opt_(*OwnerSize, *OwnerSize) PSID Owner,
    _Inout_ PULONG OwnerSize,
    _Out_writes_bytes_to_opt_(*PrimaryGroupSize, *PrimaryGroupSize) PSID PrimaryGroup,
    _Inout_ PULONG PrimaryGroupSize
))

// private
NTDLL_API(NTSTATUS, __stdcall, RtlSelfRelativeToAbsoluteSD2, (
    _Inout_ PSECURITY_DESCRIPTOR pSelfRelativeSecurityDescriptor,
    _Inout_ PULONG pBufferSize
))

// Access masks

NTDLL_API(BOOLEAN, __stdcall, RtlAreAllAccessesGranted, (
    _In_ ACCESS_MASK GrantedAccess,
    _In_ ACCESS_MASK DesiredAccess
))

NTDLL_API(BOOLEAN, __stdcall, RtlAreAnyAccessesGranted, (
    _In_ ACCESS_MASK GrantedAccess,
    _In_ ACCESS_MASK DesiredAccess
))

NTDLL_API_VOID(__stdcall, RtlMapGenericMask, (
    _Inout_ PACCESS_MASK AccessMask,
    _In_ PGENERIC_MAPPING GenericMapping
))

// ACLs

NTDLL_API(NTSTATUS, __stdcall, RtlCreateAcl, (
    _Out_writes_bytes_(AclLength) PACL Acl,
    _In_ ULONG AclLength,
    _In_ ULONG AclRevision
))

NTDLL_API(BOOLEAN, __stdcall, RtlValidAcl, (
    _In_ PACL Acl
))

NTDLL_API(NTSTATUS, __stdcall, RtlQueryInformationAcl, (
    _In_ PACL Acl,
    _Out_writes_bytes_(AclInformationLength) PVOID AclInformation,
    _In_ ULONG AclInformationLength,
    _In_ ACL_INFORMATION_CLASS AclInformationClass
))

NTDLL_API(NTSTATUS, __stdcall, RtlSetInformationAcl, (
    _Inout_ PACL Acl,
    _In_reads_bytes_(AclInformationLength) PVOID AclInformation,
    _In_ ULONG AclInformationLength,
    _In_ ACL_INFORMATION_CLASS AclInformationClass
))

NTDLL_API(NTSTATUS, __stdcall, RtlAddAce, (
    _Inout_ PACL Acl,
    _In_ ULONG AceRevision,
    _In_ ULONG StartingAceIndex,
    _In_reads_bytes_(AceListLength) PVOID AceList,
    _In_ ULONG AceListLength
))

NTDLL_API(NTSTATUS, __stdcall, RtlDeleteAce, (
    _Inout_ PACL Acl,
    _In_ ULONG AceIndex
))

NTDLL_API(NTSTATUS, __stdcall, RtlGetAce, (
    _In_ PACL Acl,
    _In_ ULONG AceIndex,
    _Outptr_ PVOID *Ace
))

NTDLL_API(BOOLEAN, __stdcall, RtlFirstFreeAce, (
    _In_ PACL Acl,
    _Out_ PVOID *FirstFree
))

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_VISTA)
// private
NTDLL_API(PVOID, __stdcall, RtlFindAceByType, (
    _In_ PACL pAcl,
    _In_ UCHAR AceType,
    _Out_opt_ PULONG pIndex
))
#endif

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_VISTA)
// private
NTDLL_API(BOOLEAN, __stdcall, RtlOwnerAcesPresent, (
    _In_ PACL pAcl
))
#endif

NTDLL_API(NTSTATUS, __stdcall, RtlAddAccessAllowedAce, (
    _Inout_ PACL Acl,
    _In_ ULONG AceRevision,
    _In_ ACCESS_MASK AccessMask,
    _In_ PSID Sid
))

NTDLL_API(NTSTATUS, __stdcall, RtlAddAccessAllowedAceEx, (
    _Inout_ PACL Acl,
    _In_ ULONG AceRevision,
    _In_ ULONG AceFlags,
    _In_ ACCESS_MASK AccessMask,
    _In_ PSID Sid
))

NTDLL_API(NTSTATUS, __stdcall, RtlAddAccessDeniedAce, (
    _Inout_ PACL Acl,
    _In_ ULONG AceRevision,
    _In_ ACCESS_MASK AccessMask,
    _In_ PSID Sid
))

NTDLL_API(NTSTATUS, __stdcall, RtlAddAccessDeniedAceEx, (
    _Inout_ PACL Acl,
    _In_ ULONG AceRevision,
    _In_ ULONG AceFlags,
    _In_ ACCESS_MASK AccessMask,
    _In_ PSID Sid
))

NTDLL_API(NTSTATUS, __stdcall, RtlAddAuditAccessAce, (
    _Inout_ PACL Acl,
    _In_ ULONG AceRevision,
    _In_ ACCESS_MASK AccessMask,
    _In_ PSID Sid,
    _In_ BOOLEAN AuditSuccess,
    _In_ BOOLEAN AuditFailure
))

NTDLL_API(NTSTATUS, __stdcall, RtlAddAuditAccessAceEx, (
    _Inout_ PACL Acl,
    _In_ ULONG AceRevision,
    _In_ ULONG AceFlags,
    _In_ ACCESS_MASK AccessMask,
    _In_ PSID Sid,
    _In_ BOOLEAN AuditSuccess,
    _In_ BOOLEAN AuditFailure
))

NTDLL_API(NTSTATUS, __stdcall, RtlAddAccessAllowedObjectAce, (
    _Inout_ PACL Acl,
    _In_ ULONG AceRevision,
    _In_ ULONG AceFlags,
    _In_ ACCESS_MASK AccessMask,
    _In_opt_ PGUID ObjectTypeGuid,
    _In_opt_ PGUID InheritedObjectTypeGuid,
    _In_ PSID Sid
))

NTDLL_API(NTSTATUS, __stdcall, RtlAddAccessDeniedObjectAce, (
    _Inout_ PACL Acl,
    _In_ ULONG AceRevision,
    _In_ ULONG AceFlags,
    _In_ ACCESS_MASK AccessMask,
    _In_opt_ PGUID ObjectTypeGuid,
    _In_opt_ PGUID InheritedObjectTypeGuid,
    _In_ PSID Sid
))

NTDLL_API(NTSTATUS, __stdcall, RtlAddAuditAccessObjectAce, (
    _Inout_ PACL Acl,
    _In_ ULONG AceRevision,
    _In_ ULONG AceFlags,
    _In_ ACCESS_MASK AccessMask,
    _In_opt_ PGUID ObjectTypeGuid,
    _In_opt_ PGUID InheritedObjectTypeGuid,
    _In_ PSID Sid,
    _In_ BOOLEAN AuditSuccess,
    _In_ BOOLEAN AuditFailure
))

NTDLL_API(NTSTATUS, __stdcall, RtlAddCompoundAce, (
    _Inout_ PACL Acl,
    _In_ ULONG AceRevision,
    _In_ UCHAR AceType,
    _In_ ACCESS_MASK AccessMask,
    _In_ PSID ServerSid,
    _In_ PSID ClientSid
))

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_VISTA)
// private
NTDLL_API(NTSTATUS, __stdcall, RtlAddMandatoryAce, (
    _Inout_ PACL Acl,
    _In_ ULONG AceRevision,
    _In_ ULONG AceFlags,
    _In_ PSID Sid,
    _In_ UCHAR AceType,
    _In_ ACCESS_MASK AccessMask
))
#endif

// Named pipes

NTDLL_API(NTSTATUS, __stdcall, RtlDefaultNpAcl, (
    _Out_ PACL *Acl
))

// Security objects

NTDLL_API(NTSTATUS, __stdcall, RtlNewSecurityObject, (
    _In_opt_ PSECURITY_DESCRIPTOR ParentDescriptor,
    _In_opt_ PSECURITY_DESCRIPTOR CreatorDescriptor,
    _Out_ PSECURITY_DESCRIPTOR *NewDescriptor,
    _In_ BOOLEAN IsDirectoryObject,
    _In_opt_ HANDLE Token,
    _In_ PGENERIC_MAPPING GenericMapping
))

NTDLL_API(NTSTATUS, __stdcall, RtlNewSecurityObjectEx, (
    _In_opt_ PSECURITY_DESCRIPTOR ParentDescriptor,
    _In_opt_ PSECURITY_DESCRIPTOR CreatorDescriptor,
    _Out_ PSECURITY_DESCRIPTOR *NewDescriptor,
    _In_opt_ GUID *ObjectType,
    _In_ BOOLEAN IsDirectoryObject,
    _In_ ULONG AutoInheritFlags, // SEF_*
    _In_opt_ HANDLE Token,
    _In_ PGENERIC_MAPPING GenericMapping
))

NTDLL_API(NTSTATUS, __stdcall, RtlNewSecurityObjectWithMultipleInheritance, (
    _In_opt_ PSECURITY_DESCRIPTOR ParentDescriptor,
    _In_opt_ PSECURITY_DESCRIPTOR CreatorDescriptor,
    _Out_ PSECURITY_DESCRIPTOR *NewDescriptor,
    _In_opt_ GUID **ObjectType,
    _In_ ULONG GuidCount,
    _In_ BOOLEAN IsDirectoryObject,
    _In_ ULONG AutoInheritFlags, // SEF_*
    _In_opt_ HANDLE Token,
    _In_ PGENERIC_MAPPING GenericMapping
))

NTDLL_API(NTSTATUS, __stdcall, RtlDeleteSecurityObject, (
    _Inout_ PSECURITY_DESCRIPTOR *ObjectDescriptor
))

NTDLL_API(NTSTATUS, __stdcall, RtlQuerySecurityObject, (
    _In_ PSECURITY_DESCRIPTOR ObjectDescriptor,
     _In_ SECURITY_INFORMATION SecurityInformation,
     _Out_opt_ PSECURITY_DESCRIPTOR ResultantDescriptor,
     _In_ ULONG DescriptorLength,
     _Out_ PULONG ReturnLength
))

NTDLL_API(NTSTATUS, __stdcall, RtlSetSecurityObject, (
    _In_ SECURITY_INFORMATION SecurityInformation,
    _In_ PSECURITY_DESCRIPTOR ModificationDescriptor,
    _Inout_ PSECURITY_DESCRIPTOR *ObjectsSecurityDescriptor,
    _In_ PGENERIC_MAPPING GenericMapping,
    _In_opt_ HANDLE Token
))

NTDLL_API(NTSTATUS, __stdcall, RtlSetSecurityObjectEx, (
    _In_ SECURITY_INFORMATION SecurityInformation,
    _In_ PSECURITY_DESCRIPTOR ModificationDescriptor,
    _Inout_ PSECURITY_DESCRIPTOR *ObjectsSecurityDescriptor,
    _In_ ULONG AutoInheritFlags, // SEF_*
    _In_ PGENERIC_MAPPING GenericMapping,
    _In_opt_ HANDLE Token
))

NTDLL_API(NTSTATUS, __stdcall, RtlConvertToAutoInheritSecurityObject, (
    _In_opt_ PSECURITY_DESCRIPTOR ParentDescriptor,
    _In_ PSECURITY_DESCRIPTOR CurrentSecurityDescriptor,
    _Out_ PSECURITY_DESCRIPTOR *NewSecurityDescriptor,
    _In_opt_ GUID *ObjectType,
    _In_ BOOLEAN IsDirectoryObject,
    _In_ PGENERIC_MAPPING GenericMapping
))

NTDLL_API(NTSTATUS, __stdcall, RtlNewInstanceSecurityObject, (
    _In_ BOOLEAN ParentDescriptorChanged,
    _In_ BOOLEAN CreatorDescriptorChanged,
    _In_ PLUID OldClientTokenModifiedId,
    _Out_ PLUID NewClientTokenModifiedId,
    _In_opt_ PSECURITY_DESCRIPTOR ParentDescriptor,
    _In_opt_ PSECURITY_DESCRIPTOR CreatorDescriptor,
    _Out_ PSECURITY_DESCRIPTOR *NewDescriptor,
    _In_ BOOLEAN IsDirectoryObject,
    _In_ HANDLE Token,
    _In_ PGENERIC_MAPPING GenericMapping
))

NTDLL_API(NTSTATUS, __stdcall, RtlCopySecurityDescriptor, (
    _In_ PSECURITY_DESCRIPTOR InputSecurityDescriptor,
    _Out_ PSECURITY_DESCRIPTOR *OutputSecurityDescriptor
))

// Misc. security

NTDLL_API_VOID(__stdcall, RtlRunEncodeUnicodeString, (
    _Inout_ PUCHAR Seed,
    _In_ PUNICODE_STRING String
))

NTDLL_API_VOID(__stdcall, RtlRunDecodeUnicodeString, (
    _In_ UCHAR Seed,
    _In_ PUNICODE_STRING String
))

NTDLL_API(NTSTATUS, __stdcall, RtlImpersonateSelf, (
    _In_ SECURITY_IMPERSONATION_LEVEL ImpersonationLevel
))

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_VISTA)
// private
NTDLL_API(NTSTATUS, __stdcall, RtlImpersonateSelfEx, (
    _In_ SECURITY_IMPERSONATION_LEVEL ImpersonationLevel,
    _In_opt_ ACCESS_MASK AdditionalAccess,
    _Out_opt_ PHANDLE ThreadToken
))
#endif

NTDLL_API(NTSTATUS, __stdcall, RtlAdjustPrivilege, (
    _In_ ULONG Privilege,
    _In_ BOOLEAN Enable,
    _In_ BOOLEAN Client,
    _Out_ PBOOLEAN WasEnabled
))

#define RTL_ACQUIRE_PRIVILEGE_REVERT 0x00000001
#define RTL_ACQUIRE_PRIVILEGE_PROCESS 0x00000002

NTDLL_API(NTSTATUS, __stdcall, RtlAcquirePrivilege, (
    _In_ PULONG Privilege,
    _In_ ULONG NumPriv,
    _In_ ULONG Flags,
    _Out_ PVOID *ReturnedState
))

NTDLL_API_VOID(__stdcall, RtlReleasePrivilege, (
    _In_ PVOID StatePointer
))

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_VISTA)
// private
NTDLL_API(NTSTATUS, __stdcall, RtlRemovePrivileges, (
    _In_ HANDLE TokenHandle,
    _In_ PULONG PrivilegesToKeep,
    _In_ ULONG PrivilegeCount
))
#endif

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_8)

// rev
NTDLL_API(NTSTATUS, __stdcall, RtlIsUntrustedObject, (
    _In_opt_ HANDLE Handle,
    _In_opt_ PVOID Object,
    _Out_ PBOOLEAN IsUntrustedObject
))

NTDLL_API(ULONG, __stdcall, RtlQueryValidationRunlevel, (
    _In_opt_ PUNICODE_STRING ComponentName
))

#endif

// Private namespaces

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_VISTA)

// begin_private

NTDLL_API(PVOID, __stdcall, RtlCreateBoundaryDescriptor, (
    _In_ PUNICODE_STRING Name,
    _In_ ULONG Flags
))

NTDLL_API_VOID(__stdcall, RtlDeleteBoundaryDescriptor, (
    _In_ PVOID BoundaryDescriptor
))

NTDLL_API(NTSTATUS, __stdcall, RtlAddSIDToBoundaryDescriptor, (
    _Inout_ PVOID *BoundaryDescriptor,
    _In_ PSID RequiredSid
))

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_7)
// rev
NTDLL_API(NTSTATUS, __stdcall, RtlAddIntegrityLabelToBoundaryDescriptor, (
    _Inout_ PVOID *BoundaryDescriptor,
    _In_ PSID IntegrityLabel
))
#endif

// end_private

#endif

// Version

NTDLL_API(NTSTATUS, __stdcall, RtlGetVersion, (
    _Out_ PRTL_OSVERSIONINFOW lpVersionInformation
))

NTDLL_API(NTSTATUS, __stdcall, RtlVerifyVersionInfo, (
    _In_ PRTL_OSVERSIONINFOEXW VersionInfo,
    _In_ ULONG TypeMask,
    _In_ ULONGLONG ConditionMask
))

// rev
NTDLL_API_VOID(__stdcall, RtlGetNtVersionNumbers, (
    _Out_opt_ PULONG NtMajorVersion,
    _Out_opt_ PULONG NtMinorVersion,
    _Out_opt_ PULONG NtBuildNumber
))

// System information

// rev
NTDLL_API(ULONG, __stdcall, RtlGetNtGlobalFlags, (
    VOID
))

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_10_RS1)
// rev
NTDLL_API(BOOLEAN, __stdcall, RtlGetNtProductType, (
    _Out_ PNT_PRODUCT_TYPE NtProductType
))
#endif

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_10_RS12)
// private
NTDLL_API(ULONG, __stdcall, RtlGetSuiteMask, (
    VOID
))
#endif

// Thread pool (old)

NTDLL_API(NTSTATUS, __stdcall, RtlRegisterWait, (
    _Out_ PHANDLE WaitHandle,
    _In_ HANDLE Handle,
    _In_ WAITORTIMERCALLBACKFUNC Function,
    _In_ PVOID Context,
    _In_ ULONG Milliseconds,
    _In_ ULONG Flags
))

NTDLL_API(NTSTATUS, __stdcall, RtlDeregisterWait, (
    _In_ HANDLE WaitHandle
))

NTDLL_API(NTSTATUS, __stdcall, RtlDeregisterWaitEx, (
    _In_ HANDLE WaitHandle,
    _In_ HANDLE Event
))

NTDLL_API(NTSTATUS, __stdcall, RtlQueueWorkItem, (
    _In_ WORKERCALLBACKFUNC Function,
    _In_ PVOID Context,
    _In_ ULONG Flags
))

NTDLL_API(NTSTATUS, __stdcall, RtlSetIoCompletionCallback, (
    _In_ HANDLE FileHandle,
    _In_ APC_CALLBACK_FUNCTION CompletionProc,
    _In_ ULONG Flags
))

typedef NTSTATUS (NTAPI *PRTL_START_POOL_THREAD)(
    _In_ PTHREAD_START_ROUTINE Function,
    _In_ PVOID Parameter,
    _Out_ PHANDLE ThreadHandle
    );

typedef NTSTATUS (NTAPI *PRTL_EXIT_POOL_THREAD)(
    _In_ NTSTATUS ExitStatus
    );

NTDLL_API(NTSTATUS, __stdcall, RtlSetThreadPoolStartFunc, (
    _In_ PRTL_START_POOL_THREAD StartPoolThread,
    _In_ PRTL_EXIT_POOL_THREAD ExitPoolThread
))

NTDLL_API_VOID(__stdcall, RtlUserThreadStart, (
    _In_ PTHREAD_START_ROUTINE Function,
    _In_ PVOID Parameter
))

NTDLL_API_VOID(__stdcall, LdrInitializeThunk, (
    _In_ PCONTEXT ContextRecord,
    _In_ PVOID Parameter
))

// Timer support

NTDLL_API(NTSTATUS, __stdcall, RtlCreateTimerQueue, (
    _Out_ PHANDLE TimerQueueHandle
))

NTDLL_API(NTSTATUS, __stdcall, RtlCreateTimer, (
    _In_ HANDLE TimerQueueHandle,
    _Out_ PHANDLE Handle,
    _In_ WAITORTIMERCALLBACKFUNC Function,
    _In_opt_ PVOID Context,
    _In_ ULONG DueTime,
    _In_ ULONG Period,
    _In_ ULONG Flags
))

NTDLL_API(NTSTATUS, __stdcall, RtlUpdateTimer, (
    _In_ HANDLE TimerQueueHandle,
    _In_ HANDLE TimerHandle,
    _In_ ULONG DueTime,
    _In_ ULONG Period
))

NTDLL_API(NTSTATUS, __stdcall, RtlDeleteTimer, (
    _In_ HANDLE TimerQueueHandle,
    _In_ HANDLE TimerToCancel,
    _In_ HANDLE Event
))

NTDLL_API(NTSTATUS, __stdcall, RtlDeleteTimerQueue, (
    _In_ HANDLE TimerQueueHandle
))

NTDLL_API(NTSTATUS, __stdcall, RtlDeleteTimerQueueEx, (
    _In_ HANDLE TimerQueueHandle,
    _In_ HANDLE Event
))

// Registry access

NTDLL_API(NTSTATUS, __stdcall, RtlFormatCurrentUserKeyPath, (
    _Out_ PUNICODE_STRING CurrentUserKeyPath
))

NTDLL_API(NTSTATUS, __stdcall, RtlOpenCurrentUser, (
    _In_ ACCESS_MASK DesiredAccess,
    _Out_ PHANDLE CurrentUserKey
))

#define RTL_REGISTRY_ABSOLUTE 0
#define RTL_REGISTRY_SERVICES 1 // \Registry\Machine\System\CurrentControlSet\Services
#define RTL_REGISTRY_CONTROL 2 // \Registry\Machine\System\CurrentControlSet\Control
#define RTL_REGISTRY_WINDOWS_NT 3 // \Registry\Machine\Software\Microsoft\Windows NT\CurrentVersion
#define RTL_REGISTRY_DEVICEMAP 4 // \Registry\Machine\Hardware\DeviceMap
#define RTL_REGISTRY_USER 5 // \Registry\User\CurrentUser
#define RTL_REGISTRY_MAXIMUM 6
#define RTL_REGISTRY_HANDLE 0x40000000
#define RTL_REGISTRY_OPTIONAL 0x80000000

NTDLL_API(NTSTATUS, __stdcall, RtlCreateRegistryKey, (
    _In_ ULONG RelativeTo,
    _In_ PWSTR Path
))

NTDLL_API(NTSTATUS, __stdcall, RtlCheckRegistryKey, (
    _In_ ULONG RelativeTo,
    _In_ PWSTR Path
))

typedef NTSTATUS (NTAPI *PRTL_QUERY_REGISTRY_ROUTINE)(
    _In_ PWSTR ValueName,
    _In_ ULONG ValueType,
    _In_ PVOID ValueData,
    _In_ ULONG ValueLength,
    _In_ PVOID Context,
    _In_ PVOID EntryContext
    );

typedef struct _RTL_QUERY_REGISTRY_TABLE
{
    PRTL_QUERY_REGISTRY_ROUTINE QueryRoutine;
    ULONG Flags;
    PWSTR Name;
    PVOID EntryContext;
    ULONG DefaultType;
    PVOID DefaultData;
    ULONG DefaultLength;
} RTL_QUERY_REGISTRY_TABLE, *PRTL_QUERY_REGISTRY_TABLE;

#define RTL_QUERY_REGISTRY_SUBKEY 0x00000001
#define RTL_QUERY_REGISTRY_TOPKEY 0x00000002
#define RTL_QUERY_REGISTRY_REQUIRED 0x00000004
#define RTL_QUERY_REGISTRY_NOVALUE 0x00000008
#define RTL_QUERY_REGISTRY_NOEXPAND 0x00000010
#define RTL_QUERY_REGISTRY_DIRECT 0x00000020
#define RTL_QUERY_REGISTRY_DELETE 0x00000040

NTDLL_API(NTSTATUS, __stdcall, RtlQueryRegistryValues, (
    _In_ ULONG RelativeTo,
    _In_ PWSTR Path,
    _In_ PRTL_QUERY_REGISTRY_TABLE QueryTable,
    _In_ PVOID Context,
    _In_opt_ PVOID Environment
))

// rev
NTDLL_API(NTSTATUS, __stdcall, RtlQueryRegistryValuesEx, (
    _In_ ULONG RelativeTo,
    _In_ PWSTR Path,
    _In_ PRTL_QUERY_REGISTRY_TABLE QueryTable,
    _In_ PVOID Context,
    _In_opt_ PVOID Environment
))

NTDLL_API(NTSTATUS, __stdcall, RtlWriteRegistryValue, (
    _In_ ULONG RelativeTo,
    _In_ PWSTR Path,
    _In_ PWSTR ValueName,
    _In_ ULONG ValueType,
    _In_ PVOID ValueData,
    _In_ ULONG ValueLength
))

NTDLL_API(NTSTATUS, __stdcall, RtlDeleteRegistryValue, (
    _In_ ULONG RelativeTo,
    _In_ PWSTR Path,
    _In_ PWSTR ValueName
))

// Thread profiling

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_7)

// rev
NTDLL_API(NTSTATUS, __stdcall, RtlEnableThreadProfiling, (
    _In_ HANDLE ThreadHandle,
    _In_ ULONG Flags,
    _In_ ULONG64 HardwareCounters,
    _Out_ PVOID *PerformanceDataHandle
))

// rev
NTDLL_API(NTSTATUS, __stdcall, RtlDisableThreadProfiling, (
    _In_ PVOID PerformanceDataHandle
))

// rev
NTDLL_API(NTSTATUS, __stdcall, RtlQueryThreadProfiling, (
    _In_ HANDLE ThreadHandle,
    _Out_ PBOOLEAN Enabled
))

// rev
NTDLL_API(NTSTATUS, __stdcall, RtlReadThreadProfilingData, (
    _In_ HANDLE PerformanceDataHandle,
    _In_ ULONG Flags,
    _Out_ PPERFORMANCE_DATA PerformanceData
))

#endif

// WOW64

NTDLL_API(NTSTATUS, __stdcall, RtlGetNativeSystemInformation, (
    _In_ ULONG SystemInformationClass,
    _In_ PVOID NativeSystemInformation,
    _In_ ULONG InformationLength,
    _Out_opt_ PULONG ReturnLength
))

NTDLL_API(NTSTATUS, __stdcall, RtlQueueApcWow64Thread, (
    _In_ HANDLE ThreadHandle,
    _In_ PPS_APC_ROUTINE ApcRoutine,
    _In_opt_ PVOID ApcArgument1,
    _In_opt_ PVOID ApcArgument2,
    _In_opt_ PVOID ApcArgument3
))

NTDLL_API(NTSTATUS, __stdcall, RtlWow64EnableFsRedirection, (
    _In_ BOOLEAN Wow64FsEnableRedirection
))

NTDLL_API(NTSTATUS, __stdcall, RtlWow64EnableFsRedirectionEx, (
    _In_ PVOID Wow64FsEnableRedirection,
    _Out_ PVOID *OldFsRedirectionLevel
))

// Misc.

NTDLL_API(ULONG32, __stdcall, RtlComputeCrc32, (
    _In_ ULONG32 PartialCrc,
    _In_ PVOID Buffer,
    _In_ ULONG Length
))

NTDLL_API(PVOID, __stdcall, RtlEncodePointer, (
    _In_ PVOID Ptr
))

NTDLL_API(PVOID, __stdcall, RtlDecodePointer, (
    _In_ PVOID Ptr
))

NTDLL_API(PVOID, __stdcall, RtlEncodeSystemPointer, (
    _In_ PVOID Ptr
))

NTDLL_API(PVOID, __stdcall, RtlDecodeSystemPointer, (
    _In_ PVOID Ptr
))

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_10_TH1)
// rev
NTDLL_API(NTSTATUS, __stdcall, RtlEncodeRemotePointer, (
    _In_ HANDLE ProcessHandle,
    _In_ PVOID Pointer,
    _Out_ PVOID *EncodedPointer
))

// rev
NTDLL_API(NTSTATUS, __stdcall, RtlDecodeRemotePointer, (
    _In_ HANDLE ProcessHandle,
    _In_ PVOID Pointer,
    _Out_ PVOID *DecodedPointer
))
#endif

// rev
NTDLL_API(BOOLEAN, __stdcall, RtlIsProcessorFeaturePresent, (
    _In_ ULONG ProcessorFeature
))

// rev
NTDLL_API(ULONG, __stdcall, RtlGetCurrentProcessorNumber, (
    VOID
))

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_10_TH1)

// rev
NTDLL_API_VOID(__stdcall, RtlGetCurrentProcessorNumberEx, (
    _Out_ PPROCESSOR_NUMBER ProcessorNumber
))

#endif

// Stack support

NTDLL_API_VOID(__stdcall, RtlPushFrame, (
    _In_ PTEB_ACTIVE_FRAME Frame
))

NTDLL_API_VOID(__stdcall, RtlPopFrame, (
    _In_ PTEB_ACTIVE_FRAME Frame
))

NTDLL_API(PTEB_ACTIVE_FRAME, __stdcall, RtlGetFrame, (
    VOID
))

#define RTL_WALK_USER_MODE_STACK 0x00000001
#define RTL_WALK_VALID_FLAGS 0x00000001
#define RTL_STACK_WALKING_MODE_FRAMES_TO_SKIP_SHIFT 0x00000008

// private
NTDLL_API(ULONG, __stdcall, RtlWalkFrameChain, (
    _Out_writes_(Count - (Flags >> RTL_STACK_WALKING_MODE_FRAMES_TO_SKIP_SHIFT)) PVOID *Callers,
    _In_ ULONG Count,
    _In_ ULONG Flags
))

// rev
NTDLL_API_VOID(__stdcall, RtlGetCallersAddress, (
    // Use the intrinsic _ReturnAddress instead.
    _Out_ PVOID *CallersAddress,
    _Out_ PVOID *CallersCaller
))

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_7)

NTDLL_API(ULONG64, __stdcall, RtlGetEnabledExtendedFeatures, (
    _In_ ULONG64 FeatureMask
))

#endif

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_10_RS14)

// msdn
NTDLL_API(ULONG64, __stdcall, RtlGetEnabledExtendedAndSupervisorFeatures, (
    _In_ ULONG64 FeatureMask
))

// msdn
_Ret_maybenull_
_Success_(return != NULL)
NTDLL_API(PVOID, __stdcall, RtlLocateSupervisorFeature, (
    _In_ PXSAVE_AREA_HEADER XStateHeader,
    _In_range_(XSTATE_AVX, MAXIMUM_XSTATE_FEATURES - 1) ULONG FeatureId,
    _Out_opt_ PULONG Length
))

#endif

// private
typedef union _RTL_ELEVATION_FLAGS
{
    ULONG Flags;
    struct
    {
        ULONG ElevationEnabled : 1;
        ULONG VirtualizationEnabled : 1;
        ULONG InstallerDetectEnabled : 1;
        ULONG ReservedBits : 29;
    };
} RTL_ELEVATION_FLAGS, *PRTL_ELEVATION_FLAGS;

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_VISTA)

// private
NTDLL_API(NTSTATUS, __stdcall, RtlQueryElevationFlags, (
    _Out_ PRTL_ELEVATION_FLAGS Flags
))

#endif

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_VISTA)

// private
NTDLL_API(NTSTATUS, __stdcall, RtlRegisterThreadWithCsrss, (
    VOID
))

#endif

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_VISTA)

// private
NTDLL_API(NTSTATUS, __stdcall, RtlLockCurrentThread, (
    VOID
))

#endif

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_VISTA)

// private
NTDLL_API(NTSTATUS, __stdcall, RtlUnlockCurrentThread, (
    VOID
))

#endif

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_VISTA)

// private
NTDLL_API(NTSTATUS, __stdcall, RtlLockModuleSection, (
    _In_ PVOID Address
))

#endif

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_VISTA)

// private
NTDLL_API(NTSTATUS, __stdcall, RtlUnlockModuleSection, (
    _In_ PVOID Address
))

#endif

// begin_msdn:"Winternl"

#define RTL_UNLOAD_EVENT_TRACE_NUMBER 64

// private
typedef struct _RTL_UNLOAD_EVENT_TRACE
{
    PVOID BaseAddress;
    SIZE_T SizeOfImage;
    ULONG Sequence;
    ULONG TimeDateStamp;
    ULONG CheckSum;
    WCHAR ImageName[32];
    ULONG Version[2];
} RTL_UNLOAD_EVENT_TRACE, *PRTL_UNLOAD_EVENT_TRACE;

typedef struct _RTL_UNLOAD_EVENT_TRACE32 
{
    ULONG BaseAddress;
    ULONG SizeOfImage;
    ULONG Sequence;
    ULONG TimeDateStamp;
    ULONG CheckSum;
    WCHAR ImageName[32];
    ULONG Version[2];
} RTL_UNLOAD_EVENT_TRACE32, *PRTL_UNLOAD_EVENT_TRACE32;

NTDLL_API(PRTL_UNLOAD_EVENT_TRACE, __stdcall, RtlGetUnloadEventTrace, (
    VOID
))

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_VISTA)
NTDLL_API_VOID(__stdcall, RtlGetUnloadEventTraceEx, (
    _Out_ PULONG *ElementSize,
    _Out_ PULONG *ElementCount,
    _Out_ PVOID *EventTrace // works across all processes
))
#endif

// end_msdn

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_7)
// rev
NTDLL_API(LOGICAL, __stdcall, RtlQueryPerformanceCounter, (
    _Out_ PLARGE_INTEGER PerformanceCounter
))
#endif

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_7)
// rev
NTDLL_API(LOGICAL, __stdcall, RtlQueryPerformanceFrequency, (
    _Out_ PLARGE_INTEGER PerformanceFrequency
))
#endif

// Image Mitigation

// rev
typedef enum _IMAGE_MITIGATION_POLICY
{
    ImageDepPolicy, // RTL_IMAGE_MITIGATION_DEP_POLICY
    ImageAslrPolicy, // RTL_IMAGE_MITIGATION_ASLR_POLICY
    ImageDynamicCodePolicy, // RTL_IMAGE_MITIGATION_DYNAMIC_CODE_POLICY
    ImageStrictHandleCheckPolicy, // RTL_IMAGE_MITIGATION_STRICT_HANDLE_CHECK_POLICY
    ImageSystemCallDisablePolicy, // RTL_IMAGE_MITIGATION_SYSTEM_CALL_DISABLE_POLICY
    ImageMitigationOptionsMask,
    ImageExtensionPointDisablePolicy, // RTL_IMAGE_MITIGATION_EXTENSION_POINT_DISABLE_POLICY
    ImageControlFlowGuardPolicy, // RTL_IMAGE_MITIGATION_CONTROL_FLOW_GUARD_POLICY
    ImageSignaturePolicy, // RTL_IMAGE_MITIGATION_BINARY_SIGNATURE_POLICY
    ImageFontDisablePolicy, // RTL_IMAGE_MITIGATION_FONT_DISABLE_POLICY
    ImageImageLoadPolicy, // RTL_IMAGE_MITIGATION_IMAGE_LOAD_POLICY
    ImagePayloadRestrictionPolicy, // RTL_IMAGE_MITIGATION_PAYLOAD_RESTRICTION_POLICY
    ImageChildProcessPolicy, // RTL_IMAGE_MITIGATION_CHILD_PROCESS_POLICY
    ImageSehopPolicy, // RTL_IMAGE_MITIGATION_SEHOP_POLICY
    ImageHeapPolicy, // RTL_IMAGE_MITIGATION_HEAP_POLICY
    MaxImageMitigationPolicy
} IMAGE_MITIGATION_POLICY;

// rev
typedef union _RTL_IMAGE_MITIGATION_POLICY
{
    struct
    {
        ULONG64 AuditState : 2;
        ULONG64 AuditFlag : 1;
        ULONG64 EnableAdditionalAuditingOption : 1;
        ULONG64 Reserved : 60;
    };
    struct
    {
        ULONG64 PolicyState : 2;
        ULONG64 AlwaysInherit : 1;
        ULONG64 EnableAdditionalPolicyOption : 1;
        ULONG64 AuditReserved : 60;
    };
} RTL_IMAGE_MITIGATION_POLICY, *PRTL_IMAGE_MITIGATION_POLICY;

// rev
typedef struct _RTL_IMAGE_MITIGATION_DEP_POLICY
{
    RTL_IMAGE_MITIGATION_POLICY Dep;
} RTL_IMAGE_MITIGATION_DEP_POLICY, *PRTL_IMAGE_MITIGATION_DEP_POLICY;

// rev
typedef struct _RTL_IMAGE_MITIGATION_ASLR_POLICY
{
    RTL_IMAGE_MITIGATION_POLICY ForceRelocateImages;
    RTL_IMAGE_MITIGATION_POLICY BottomUpRandomization;
    RTL_IMAGE_MITIGATION_POLICY HighEntropyRandomization;
} RTL_IMAGE_MITIGATION_ASLR_POLICY, *PRTL_IMAGE_MITIGATION_ASLR_POLICY;

// rev
typedef struct _RTL_IMAGE_MITIGATION_DYNAMIC_CODE_POLICY
{
    RTL_IMAGE_MITIGATION_POLICY BlockDynamicCode;
} RTL_IMAGE_MITIGATION_DYNAMIC_CODE_POLICY, *PRTL_IMAGE_MITIGATION_DYNAMIC_CODE_POLICY;

// rev
typedef struct _RTL_IMAGE_MITIGATION_STRICT_HANDLE_CHECK_POLICY
{
    RTL_IMAGE_MITIGATION_POLICY StrictHandleChecks;
} RTL_IMAGE_MITIGATION_STRICT_HANDLE_CHECK_POLICY, *PRTL_IMAGE_MITIGATION_STRICT_HANDLE_CHECK_POLICY;

// rev
typedef struct _RTL_IMAGE_MITIGATION_SYSTEM_CALL_DISABLE_POLICY
{
    RTL_IMAGE_MITIGATION_POLICY BlockWin32kSystemCalls;
} RTL_IMAGE_MITIGATION_SYSTEM_CALL_DISABLE_POLICY, *PRTL_IMAGE_MITIGATION_SYSTEM_CALL_DISABLE_POLICY;

// rev
typedef struct _RTL_IMAGE_MITIGATION_EXTENSION_POINT_DISABLE_POLICY
{
    RTL_IMAGE_MITIGATION_POLICY DisableExtensionPoints;
} RTL_IMAGE_MITIGATION_EXTENSION_POINT_DISABLE_POLICY, *PRTL_IMAGE_MITIGATION_EXTENSION_POINT_DISABLE_POLICY;

// rev
typedef struct _RTL_IMAGE_MITIGATION_CONTROL_FLOW_GUARD_POLICY
{
    RTL_IMAGE_MITIGATION_POLICY ControlFlowGuard;
    RTL_IMAGE_MITIGATION_POLICY StrictControlFlowGuard;
} RTL_IMAGE_MITIGATION_CONTROL_FLOW_GUARD_POLICY, *PRTL_IMAGE_MITIGATION_CONTROL_FLOW_GUARD_POLICY;

// rev
typedef struct _RTL_IMAGE_MITIGATION_BINARY_SIGNATURE_POLICY
{
    RTL_IMAGE_MITIGATION_POLICY BlockNonMicrosoftSignedBinaries;
    RTL_IMAGE_MITIGATION_POLICY EnforceSigningOnModuleDependencies;
} RTL_IMAGE_MITIGATION_BINARY_SIGNATURE_POLICY, *PRTL_IMAGE_MITIGATION_BINARY_SIGNATURE_POLICY;

// rev
typedef struct _RTL_IMAGE_MITIGATION_FONT_DISABLE_POLICY
{
    RTL_IMAGE_MITIGATION_POLICY DisableNonSystemFonts;
} RTL_IMAGE_MITIGATION_FONT_DISABLE_POLICY, *PRTL_IMAGE_MITIGATION_FONT_DISABLE_POLICY;

// rev
typedef struct _RTL_IMAGE_MITIGATION_IMAGE_LOAD_POLICY
{
    RTL_IMAGE_MITIGATION_POLICY BlockRemoteImageLoads;
    RTL_IMAGE_MITIGATION_POLICY BlockLowLabelImageLoads;
    RTL_IMAGE_MITIGATION_POLICY PreferSystem32;
} RTL_IMAGE_MITIGATION_IMAGE_LOAD_POLICY, *PRTL_IMAGE_MITIGATION_IMAGE_LOAD_POLICY;

// rev
typedef struct _RTL_IMAGE_MITIGATION_PAYLOAD_RESTRICTION_POLICY
{
    RTL_IMAGE_MITIGATION_POLICY EnableExportAddressFilter;
    RTL_IMAGE_MITIGATION_POLICY EnableExportAddressFilterPlus;
    RTL_IMAGE_MITIGATION_POLICY EnableImportAddressFilter;
    RTL_IMAGE_MITIGATION_POLICY EnableRopStackPivot;
    RTL_IMAGE_MITIGATION_POLICY EnableRopCallerCheck;
    RTL_IMAGE_MITIGATION_POLICY EnableRopSimExec;
    WCHAR EafPlusModuleList[512]; // 19H1
} RTL_IMAGE_MITIGATION_PAYLOAD_RESTRICTION_POLICY, *PRTL_IMAGE_MITIGATION_PAYLOAD_RESTRICTION_POLICY;

// rev
typedef struct _RTL_IMAGE_MITIGATION_CHILD_PROCESS_POLICY
{
    RTL_IMAGE_MITIGATION_POLICY DisallowChildProcessCreation;
} RTL_IMAGE_MITIGATION_CHILD_PROCESS_POLICY, *PRTL_IMAGE_MITIGATION_CHILD_PROCESS_POLICY;

// rev
typedef struct _RTL_IMAGE_MITIGATION_SEHOP_POLICY
{
    RTL_IMAGE_MITIGATION_POLICY Sehop;
} RTL_IMAGE_MITIGATION_SEHOP_POLICY, *PRTL_IMAGE_MITIGATION_SEHOP_POLICY;

// rev
typedef struct _RTL_IMAGE_MITIGATION_HEAP_POLICY
{
    RTL_IMAGE_MITIGATION_POLICY TerminateOnHeapErrors;
} RTL_IMAGE_MITIGATION_HEAP_POLICY, *PRTL_IMAGE_MITIGATION_HEAP_POLICY;

typedef enum _RTL_IMAGE_MITIGATION_OPTION_STATE
{
    RtlMitigationOptionStateNotConfigured,
    RtlMitigationOptionStateOn,
    RtlMitigationOptionStateOff
} RTL_IMAGE_MITIGATION_OPTION_STATE;

// rev from PROCESS_MITIGATION_FLAGS
#define RTL_IMAGE_MITIGATION_FLAG_RESET 0x1
#define RTL_IMAGE_MITIGATION_FLAG_REMOVE 0x2
#define RTL_IMAGE_MITIGATION_FLAG_OSDEFAULT 0x4
#define RTL_IMAGE_MITIGATION_FLAG_AUDIT 0x8

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_10_RS13)

// rev
NTDLL_API(NTSTATUS, __stdcall, RtlQueryImageMitigationPolicy, (
    _In_opt_ PWSTR ImagePath, // NULL for system-wide defaults
    _In_ IMAGE_MITIGATION_POLICY Policy,
    _In_ ULONG Flags,
    _Inout_ PVOID Buffer,
    _In_ ULONG BufferSize
))

// rev
NTDLL_API(NTSTATUS, __stdcall, RtlSetImageMitigationPolicy, (
    _In_opt_ PWSTR ImagePath, // NULL for system-wide defaults
    _In_ IMAGE_MITIGATION_POLICY Policy,
    _In_ ULONG Flags,
    _Inout_ PVOID Buffer,
    _In_ ULONG BufferSize
))

#endif

// session 

// rev
NTDLL_API(ULONG, __stdcall, RtlGetCurrentServiceSessionId, (
    VOID
))

// private
NTDLL_API(ULONG, __stdcall, RtlGetActiveConsoleId, (
    VOID
))

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_10_RS1)
// private
NTDLL_API(ULONGLONG, __stdcall, RtlGetConsoleSessionForegroundProcessId, (
    VOID
))
#endif

// Appcontainer

// rev
NTDLL_API(NTSTATUS, __stdcall, RtlGetTokenNamedObjectPath, (
    _In_ HANDLE Token, 
    _In_opt_ PSID Sid, 
    _Out_ PUNICODE_STRING ObjectPath // RtlFreeUnicodeString
))

// rev
NTDLL_API(NTSTATUS, __stdcall, RtlGetAppContainerNamedObjectPath, (
    _In_opt_ HANDLE Token,
    _In_opt_ PSID AppContainerSid,
    _In_ BOOLEAN RelativePath,
    _Out_ PUNICODE_STRING ObjectPath // RtlFreeUnicodeString
))

// rev
NTDLL_API(NTSTATUS, __stdcall, RtlGetAppContainerParent, (
    _In_ PSID AppContainerSid, 
    _Out_ PSID* AppContainerSidParent // RtlFreeSid
))

// rev
NTDLL_API(NTSTATUS, __stdcall, RtlCheckSandboxedToken, (
    _In_opt_ HANDLE TokenHandle,
    _Out_ PBOOLEAN IsSandboxed
))

// rev
NTDLL_API(NTSTATUS, __stdcall, RtlCheckTokenCapability, (
    _In_opt_ HANDLE TokenHandle,
    _In_ PSID CapabilitySidToCheck,
    _Out_ PBOOLEAN HasCapability
))

// rev
NTDLL_API(NTSTATUS, __stdcall, RtlCapabilityCheck, (
    _In_opt_ HANDLE TokenHandle,
    _In_ PUNICODE_STRING CapabilityName,
    _Out_ PBOOLEAN HasCapability
))

// rev
NTDLL_API(NTSTATUS, __stdcall, RtlCheckTokenMembership, (
    _In_opt_ HANDLE TokenHandle,
    _In_ PSID SidToCheck,
    _Out_ PBOOLEAN IsMember
))

// rev
NTDLL_API(NTSTATUS, __stdcall, RtlCheckTokenMembershipEx, (
    _In_opt_ HANDLE TokenHandle,
    _In_ PSID SidToCheck,
    _In_ ULONG Flags, // CTMF_VALID_FLAGS
    _Out_ PBOOLEAN IsMember
))

// rev
NTDLL_API(NTSTATUS, __stdcall, RtlQueryTokenHostIdAsUlong64, (
    _In_ HANDLE TokenHandle,
    _Out_ PULONG64 HostId // (WIN://PKGHOSTID)
))

// rev
NTDLL_API(NTSTATUS, __stdcall, RtlIsParentOfChildAppContainer, (
    _In_ PSID ParentAppContainerSid,
    _In_ PSID ChildAppContainerSid
))

// rev
NTDLL_API(BOOLEAN, __stdcall, RtlIsCapabilitySid, (
    _In_ PSID Sid
))

// rev
NTDLL_API(BOOLEAN, __stdcall, RtlIsPackageSid, (
    _In_ PSID Sid
))

// rev
NTDLL_API(BOOLEAN, __stdcall, RtlIsValidProcessTrustLabelSid, (
    _In_ PSID Sid
))

NTDLL_API(BOOLEAN, __stdcall, RtlIsStateSeparationEnabled, (
    VOID
))

typedef enum _APPCONTAINER_SID_TYPE
{
    NotAppContainerSidType,
    ChildAppContainerSidType,
    ParentAppContainerSidType,
    InvalidAppContainerSidType,
    MaxAppContainerSidType
} APPCONTAINER_SID_TYPE, *PAPPCONTAINER_SID_TYPE;

// rev
NTDLL_API(NTSTATUS, __stdcall, RtlGetAppContainerSidType, (
    _In_ PSID AppContainerSid,
    _Out_ PAPPCONTAINER_SID_TYPE AppContainerSidType
))

NTDLL_API(NTSTATUS, __stdcall, RtlFlsAlloc, (
    _In_ PFLS_CALLBACK_FUNCTION Callback,
    _Out_ PULONG FlsIndex
))

NTDLL_API(NTSTATUS, __stdcall, RtlFlsFree, (
    _In_ ULONG FlsIndex
))

typedef enum _STATE_LOCATION_TYPE 
{
    LocationTypeRegistry,
    LocationTypeFileSystem,
    LocationTypeMaximum
} STATE_LOCATION_TYPE;

// private
NTDLL_API(NTSTATUS, __stdcall, RtlGetPersistedStateLocation, (
    _In_ PCWSTR SourceID,
    _In_opt_ PCWSTR CustomValue,
    _In_opt_ PCWSTR DefaultPath,
    _In_ STATE_LOCATION_TYPE StateLocationType,
    _Out_writes_bytes_to_opt_(BufferLengthIn, *BufferLengthOut) PWCHAR TargetPath,
    _In_ ULONG BufferLengthIn,
    _Out_opt_ PULONG BufferLengthOut
))

// msdn
NTDLL_API(BOOLEAN, __stdcall, RtlIsCloudFilesPlaceholder, (
    _In_ ULONG FileAttributes,
    _In_ ULONG ReparseTag
))

// msdn
NTDLL_API(BOOLEAN, __stdcall, RtlIsPartialPlaceholder, (
    _In_ ULONG FileAttributes,
    _In_ ULONG ReparseTag
))

// msdn
NTDLL_API(NTSTATUS, __stdcall, RtlIsPartialPlaceholderFileHandle, (
    _In_ HANDLE FileHandle,
    _Out_ PBOOLEAN IsPartialPlaceholder
))

// msdn
NTDLL_API(NTSTATUS, __stdcall, RtlIsPartialPlaceholderFileInfo, (
    _In_ PVOID InfoBuffer,
    _In_ FILE_INFORMATION_CLASS InfoClass,
    _Out_ PBOOLEAN IsPartialPlaceholder
))

// rev
NTDLL_API(BOOLEAN, __stdcall, RtlIsNonEmptyDirectoryReparsePointAllowed, (
    _In_ ULONG ReparseTag
))

// rev
NTDLL_API(NTSTATUS, __stdcall, RtlAppxIsFileOwnedByTrustedInstaller, (
    _In_ HANDLE FileHandle, 
    _Out_ PBOOLEAN IsFileOwnedByTrustedInstaller
))

// rev
typedef struct _PS_PKG_CLAIM
{
    ULONGLONG Flags;
    ULONGLONG Origin;
} PS_PKG_CLAIM, *PPS_PKG_CLAIM;

NTDLL_API(NTSTATUS, __stdcall, RtlQueryPackageClaims, (
    _In_ HANDLE TokenHandle,
    _Out_writes_bytes_to_opt_(*PackageSize, *PackageSize) PWSTR PackageFullName,
    _Inout_opt_ PSIZE_T PackageSize,
    _Out_writes_bytes_to_opt_(*AppIdSize, *AppIdSize) PWSTR AppId,
    _Inout_opt_ PSIZE_T AppIdSize,
    _Out_opt_ PGUID DynamicId,
    _Out_opt_ PPS_PKG_CLAIM PkgClaim,
    _Out_opt_ PULONG64 AttributesPresent
))

// Protected policies

// rev
NTDLL_API(NTSTATUS, __stdcall, RtlQueryProtectedPolicy, (
    _In_ PGUID PolicyGuid,
    _Out_ PULONG_PTR PolicyValue
))

// rev
NTDLL_API(NTSTATUS, __stdcall, RtlSetProtectedPolicy, (
    _In_ PGUID PolicyGuid,
    _In_ ULONG_PTR PolicyValue,
    _Out_ PULONG_PTR OldPolicyValue
))

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_10_RS1)
// private
NTDLL_API(BOOLEAN, __stdcall, RtlIsMultiSessionSku, (
    VOID
))
#endif

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_10_RS1)
// private
NTDLL_API(BOOLEAN, __stdcall, RtlIsMultiUsersInSessionSku, (
    VOID
))
#endif

// private
typedef enum _RTL_BSD_ITEM_TYPE
{
    RtlBsdItemVersionNumber,
    RtlBsdItemProductType,
    RtlBsdItemAabEnabled,
    RtlBsdItemAabTimeout,
    RtlBsdItemBootGood,
    RtlBsdItemBootShutdown,
    RtlBsdSleepInProgress,
    RtlBsdPowerTransition,
    RtlBsdItemBootAttemptCount,
    RtlBsdItemBootCheckpoint,
    RtlBsdItemBootId,
    RtlBsdItemShutdownBootId,
    RtlBsdItemReportedAbnormalShutdownBootId,
    RtlBsdItemErrorInfo,
    RtlBsdItemPowerButtonPressInfo,
    RtlBsdItemChecksum,
    RtlBsdItemMax
} RTL_BSD_ITEM_TYPE;

// private
typedef struct _RTL_BSD_ITEM
{
    RTL_BSD_ITEM_TYPE Type;
    PVOID DataBuffer;
    ULONG DataLength;
} RTL_BSD_ITEM, *PRTL_BSD_ITEM;

// ros
NTDLL_API(NTSTATUS, __stdcall, RtlCreateBootStatusDataFile, (
    VOID
))

// ros
NTDLL_API(NTSTATUS, __stdcall, RtlLockBootStatusData, (
    _Out_ PHANDLE FileHandle
))

// ros
NTDLL_API(NTSTATUS, __stdcall, RtlUnlockBootStatusData, (
    _In_ HANDLE FileHandle
))

// ros
NTDLL_API(NTSTATUS, __stdcall, RtlGetSetBootStatusData, (
    _In_ HANDLE FileHandle,
    _In_ BOOLEAN Read,
    _In_ RTL_BSD_ITEM_TYPE DataClass,
    _In_ PVOID Buffer,
    _In_ ULONG BufferSize,
    _Out_opt_ PULONG ReturnLength
))

// rev
NTDLL_API(NTSTATUS, __stdcall, RtlCheckBootStatusIntegrity, (
    _In_ HANDLE FileHandle, 
    _Out_ PBOOLEAN Verified
))

// rev
NTDLL_API(NTSTATUS, __stdcall, RtlCheckPortableOperatingSystem, (
    _Out_ PBOOLEAN IsPortable // VOID
))

// rev
NTDLL_API(NTSTATUS, __stdcall, RtlSetPortableOperatingSystem, (
    _In_ BOOLEAN IsPortable
))

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_10_TH1)

NTDLL_API(OS_DEPLOYEMENT_STATE_VALUES, __stdcall, RtlOsDeploymentState, (
    _Reserved_ _In_ ULONG Flags
))

#endif

#if (defined(PHNT_COMPILE) || NTLIB_WIN_VERSION >= NTLIB_WIN_VISTA)

NTDLL_API(NTSTATUS, __stdcall, RtlFindClosestEncodableLength, (
    _In_ ULONGLONG SourceLength,
    _Out_ PULONGLONG TargetLength
))

#endif

// Memory cache

typedef NTSTATUS (NTAPI *PRTL_SECURE_MEMORY_CACHE_CALLBACK)(
    _In_ PVOID Address,
    _In_ SIZE_T Length
    );

// ros
NTDLL_API(NTSTATUS, __stdcall, RtlRegisterSecureMemoryCacheCallback, (
    _In_ PRTL_SECURE_MEMORY_CACHE_CALLBACK Callback
))

NTDLL_API(NTSTATUS, __stdcall, RtlDeregisterSecureMemoryCacheCallback, (
    _In_ PRTL_SECURE_MEMORY_CACHE_CALLBACK Callback
))

// ros
NTDLL_API(BOOLEAN, __stdcall, RtlFlushSecureMemoryCache, (
    _In_ PVOID MemoryCache,
    _In_opt_ SIZE_T MemoryLength
))

#endif
