/*++

LibreWin 20x/TX Kernel

You may only use this code if you agree to the terms of the LibreWin Source Code License agreement (GNU GPL v3) (see LICENSE).
If you do not agree to the terms, do not use the code.


Module Name:

    ntdll.c

Abstract:

    This module implements replicating NTDLL, an essential Dynamic Link Library for Windows Applications.
    It implements low-level functions, like invoking syscalls.

--*/

#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include "basetsd.h"

#define SYS_NtAccessCheckAndAuditAlarm 0x0002
#define SYS_NtAccessCheckByType 0x0003
#define SYS_NtAccessCheckByTypeAndAuditAlarm 0x0004
#define SYS_NtAccessCheckByTypeResultList 0x0005

typedef long NTSTATUS;
#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS ((NTSTATUS)0x00000000)
#endif
#ifndef STATUS_INFO_LENGTH_MISMATCH
#define STATUS_INFO_LENGTH_MISMATCH ((NTSTATUS)0xC0000004)
#endif
#ifndef STATUS_INVALID_INFO_CLASS
#define STATUS_INVALID_INFO_CLASS ((NTSTATUS)0xC0000003)
#endif
#ifndef STATUS_INSUFFICIENT_RESOURCES
#define STATUS_INSUFFICIENT_RESOURCES ((NTSTATUS)0xC000009A)
#endif
#ifndef STATUS_CONNECTION_REFUSED
#define STATUS_CONNECTION_REFUSED ((NTSTATUS)0xC0000232)
#endif
#ifndef STATUS_ACCESS_DENIED
#define STATUS_ACCESS_DENIED ((NTSTATUS)0xC0000022)
#endif

typedef struct _PORT_MESSAGE {
    USHORT DataLength;
    USHORT TotalLength;
    USHORT Type;
    USHORT DataInfoOffset;
    ULONG  ClientId;
    ULONG  MessageId;
} PORT_MESSAGE, *PPORT_MESSAGE;

typedef struct _SYSTEM_EXCEPTION_INFORMATION SYSTEM_EXCEPTION_INFORMATION, *PSYSTEM_EXCEPTION_INFORMATION;
typedef struct _SYSTEM_LEAP_SECOND_INFORMATION SYSTEM_LEAP_SECOND_INFORMATION, *PSYSTEM_LEAP_SECOND_INFORMATION;
typedef struct _SYSTEM_LOOKASIDE_INFORMATION SYSTEM_LOOKASIDE_INFORMATION, *PSYSTEM_LOOKASIDE_INFORMATION;
typedef struct _SYSTEM_POLICY_INFORMATION SYSTEM_POLICY_INFORMATION, *PSYSTEM_POLICY_INFORMATION;
typedef struct _SYSTEM_QUERY_PERFORMANCE_COUNTER_INFORMATION SYSTEM_QUERY_PERFORMANCE_COUNTER_INFORMATION, *PSYSTEM_QUERY_PERFORMANCE_COUNTER_INFORMATION;
typedef struct _SYSTEM_SPECULATION_CONTROL_INFORMATION SYSTEM_SPECULATION_CONTROL_INFORMATION, *PSYSTEM_SPECULATION_CONTROL_INFORMATION;

typedef struct _SYSTEM_BASIC_INFORMATION {
    ULONG Reserved;
    ULONG TimerResolution;
    ULONG PageSize;
    ULONG MinimumApplicationAddress;
    ULONG MaximumApplicationAddress;
    ULONG ActiveProcessorsAffinityMask;
    UCHAR NumberOfProcessors;
} SYSTEM_BASIC_INFORMATION;

typedef struct _SYSTEM_BASICPROCESS_INFORMATION {
    ULONG NextEntryOffset;
    HANDLE UniqueProcessId;
    HANDLE InheritedFromUniqueProcessId;
    ULONG64 SequenceNumber;
    UNICODE_STRING ImageName;
} SYSTEM_BASICPROCESS_INFORMATION, *PSYSTEM_BASICPROCESS_INFORMATION;

typedef struct _SYSTEM_CODEINTEGRITY_INFORMATION {
    ULONG Length;
    ULONG CodeIntegrityOptions;
} SYSTEM_CODEINTEGRITY_INFORMATION, *PSYSTEM_CODEINTEGRITY_INFORMATION;

typedef struct _SYSTEM_EXCEPTION_INFORMATION {
    BYTE Reserved1[16];
} SYSTEM_EXCEPTION_INFORMATION;

typedef enum _SYSTEM_INFORMATION_CLASS {
    SystemBasicInformation = 0,
    SystemPerformanceInformation = 2,
    SystemTimeOfDayInformation = 3,
    SystemProcessInformation = 5,
    SystemProcessorPerformanceInformation = 8,
    SystemInterruptInformation = 23,
    SystemExceptionInformation = 33,
    SystemRegistryQuotaInformation = 37,
    SystemLookasideInformation = 45,

    SystemLeapSecondInformation = 46,
    SystemPolicyInformation = 47,
    SystemQueryPerformanceCounterInformation = 48,
    SystemSpeculationControlInformation = 49,

} SYSTEM_INFORMATION_CLASS;

typedef struct _SYSTEM_INTERRUPT_INFORMATION {
    ULONG ContextSwitches;
    ULONG DpcCount;
    ULONG DpcRate;
    ULONG TimeIncrement;
    ULONG DpcBypassCount;
    ULONG ApcBypassCount;
} SYSTEM_INTERRUPT_INFORMATION, *PSYSTEM_INTERRUPT_INFORMATION;

typedef struct _SYSTEM_KERNEL_VA_SHADOW_INFORMATION {
    ULONG Flags;
} SYSTEM_KERNEL_VA_SHADOW_INFORMATION, *PSYSTEM_KERNEL_VA_SHADOW_INFORMATION;

typedef struct _SYSTEM_LEAP_SECOND_INFORMATION {
    BYTE Reserved1[16];
} SYSTEM_LEAP_SECOND_INFORMATION, *PSYSTEM_LEAP_SECOND_INFORMATION;

typedef struct _SYSTEM_LOOKASIDE_INFORMATION {
    BYTE Reserved1[16];
} SYSTEM_LOOKASIDE_INFORMATION, *PSYSTEM_LOOKASIDE_INFORMATION;

typedef struct _SYSTEM_PERFORMANCE_INFORMATION {
    LARGE_INTEGER IdleTime;
    LARGE_INTEGER ReadTransferCount;
    LARGE_INTEGER WriteTransferCount;
    LARGE_INTEGER OtherTransferCount;
    ULONG ReadOperationCount;
    ULONG WriteOperationCount;
    ULONG OtherOperationCount;
    ULONG AvailablePages;
    ULONG CommittedPages;
    ULONG CommitLimit;
    ULONG PeakCommitment;
    ULONG PageFaultCount;
    ULONG CopyOnWriteCount;
    ULONG TransitionCount;
    ULONG CacheTransitionCount;
    ULONG DemandZeroCount;
    ULONG PageReadCount;
    ULONG PageReadIoCount;
    ULONG CacheReadCount;
    ULONG CacheIoCount;
    ULONG DirtyPagesWriteCount;
    ULONG DirtyWriteIoCount;
    ULONG MappedPagesWriteCount;
    ULONG MappedWriteIoCount;
    ULONG PagedPoolPages;
    ULONG NonPagedPoolPages;
    ULONG PagedPoolAllocs;
    ULONG PagedPoolFrees;
    ULONG NonPagedPoolAllocs;
    ULONG NonPagedPoolFrees;
    ULONG FreeSystemPtes;
    ULONG ResidentSystemCodePage;
    ULONG TotalSystemDriverPages;
    ULONG TotalSystemCodePages;
    ULONG NonPagedPoolLookasideHits;
    ULONG PagedPoolLookasideHits;
    ULONG AvailablePagedPoolPages;
    ULONG ResidentSystemCachePage;
    ULONG ResidentPagedPoolPage;
    ULONG ResidentSystemDriverPage;
    ULONG CcFastReadNoWait;
    ULONG CcFastReadWait;
    ULONG CcFastReadResourceMiss;
    ULONG CcFastReadNotPossible;
    ULONG CcFastMdlReadNoWait;
    ULONG CcFastMdlReadWait;
    ULONG CcFastMdlReadResourceMiss;
    ULONG CcFastMdlReadNotPossible;
    ULONG CcMapDataNoWait;
    ULONG CcMapDataWait;
    ULONG CcMapDataNoWaitMiss;
    ULONG CcMapDataWaitMiss;
    ULONG CcPinMappedDataCount;
    ULONG CcPinReadNoWait;
    ULONG CcPinReadWait;
    ULONG CcPinReadNoWaitMiss;
    ULONG CcPinReadWaitMiss;
    ULONG CcCopyReadNoWait;
    ULONG CcCopyReadWait;
    ULONG CcCopyReadNoWaitMiss;
    ULONG CcCopyReadWaitMiss;
    ULONG CcMdlReadNoWait;
    ULONG CcMdlReadWait;
    ULONG CcMdlReadNoWaitMiss;
    ULONG CcMdlReadWaitMiss;
    ULONG CcReadAheadIos;
    ULONG CcLazyWriteIos;
    ULONG CcLazyWritePages;
    ULONG CcDataFlushes;
    ULONG CcDataPages;
} SYSTEM_PERFORMANCE_INFORMATION, *PSYSTEM_PERFORMANCE_INFORMATION;

typedef struct _SYSTEM_POLICY_INFORMATION {
    BYTE Reserved[16];
} SYSTEM_POLICY_INFORMATION, *PSYSTEM_POLICY_INFORMATION;

typedef struct _SYSTEM_QUERY_PERFORMANCE_COUNTER_INFORMATION {
    LARGE_INTEGER PerformanceCounter;
    LARGE_INTEGER PerformanceFrequency;
} SYSTEM_QUERY_PERFORMANCE_COUNTER_INFORMATION, *PSYSTEM_QUERY_PERFORMANCE_COUNTER_INFORMATION;

typedef struct _SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION {
    LARGE_INTEGER IdleTime;
    LARGE_INTEGER KernelTime;
    LARGE_INTEGER UserTime;
    LARGE_INTEGER DpcTime;
    LARGE_INTEGER InterruptTime;
    ULONG InterruptCount;
} SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION, *PSYSTEM_PROCESSOR_PERFORMANCE_INFORMATION;

typedef struct _SYSTEM_REGISTRY_QUOTA_INFORMATION {
    BYTE Reserved[16];
} SYSTEM_REGISTRY_QUOTA_INFORMATION, *PSYSTEM_REGISTRY_QUOTA_INFORMATION;

typedef struct _SYSTEM_SPECULATION_CONTROL_INFORMATION {
    struct {
        ULONG BpbEnabled : 1;
        ULONG BpbDisabledSystemPolicy : 1;
        ULONG BpbDisabledNoHardwareSupport : 1;
        ULONG SpecCtrlEnumerated : 1;
        ULONG SpecCmdEnumerated : 1;
        ULONG IbrsPresent : 1;
        ULONG StibpPresent : 1;
        ULONG SmepPresent : 1;
        ULONG SpeculativeStoreBypassDisableAvailable : 1;
        ULONG SpeculativeStoreBypassDisableSupported : 1;
        ULONG SpeculativeStoreBypassDisabledSystemWide : 1;
        ULONG SpeculativeStoreBypassDisabledKernel : 1;
        ULONG SpeculativeStoreBypassDisableRequired : 1;
        ULONG BpbDisabledKernelToUser : 1;
        ULONG SpecCtrlRetpolineEnabled : 1;
        ULONG SpecCtrlImportOptimizationEnabled : 1;
        ULONG Reserved : 16;
    } SpeculationControlFlags;
} SYSTEM_SPECULATION_CONTROL_INFORMATION, * PSYSTEM_SPECULATION_CONTROL_INFORMATION;

typedef struct _SYSTEM_TIMEOFDAY_INFORMATION {
    LARGE_INTEGER CurrentTime;
    LARGE_INTEGER BootTime;
    LARGE_INTEGER TimeZoneBias;
    ULONG TimeZoneId;
    ULONG Reserved[3]; // padding/opaque data
} SYSTEM_TIMEOFDAY_INFORMATION, *PSYSTEM_TIMEOFDAY_INFORMATION;

typedef LONG NTSTATUS;
SYSTEM_INFORMATION_CLASS SystemInformationClass;
PVOID SystemInformation;
ULONG SystemInformationLength;
PULONG ReturnLength;
SYSTEM_BASIC_INFORMATION sbi;
ULONG len;
ULONG sbisize = sizeof(sbi);

#if defined(__i386__)
NTSTATUS NTAPI NtAcceptConnectPort(
    PHANDLE PortHandle, PVOID PortContext, PPORT_MESSAGE ConnectionRequest,
    BOOLEAN AcceptConnection, PVOID* WriteSection, PVOID* ReadSection)
{
    NTSTATUS status;
    unsigned int accept_val = (unsigned int)AcceptConnection;

    asm volatile (
        "push %[rsec]\n\t"
        "push %[wsec]\n\t"
        "push %[accept]\n\t"
        "push %[creq]\n\t"
        "push %[pctx]\n\t"
        "push %[ph]\n\t"
        "mov $0x0000, %%eax\n\t"
        "int $0x002e\n\t"
        "add $24, %%esp\n\t"
        : "=a" (status)
        : [ph] "r" (PortHandle),
          [pctx] "r" (PortContext),
          [creq] "r" (ConnectionRequest),
          [accept] "m" (accept_val),
          [wsec] "r" (WriteSection),
          [rsec] "r" (ReadSection)
        : "memory", "cc"
    );
    return status;
}
#endif

#define SYS_NtAccessCheck 0x0001

NTSTATUS NTAPI NtAccessCheckAndAuditAlarm(
    PUNICODE_STRING SubsystemName,
    PVOID HandleId,
    PUNICODE_STRING ObjectTypeName,
    PUNICODE_STRING ObjectName,
    PSECURITY_DESCRIPTOR SecurityDescriptor,
    ACCESS_MASK DesiredAccess,
    PGENERIC_MAPPING GenericMapping,
    BOOLEAN ObjectCreation,
    PULONG GrantedAccess,
    PBOOLEAN AccessStatus,
    PBOOLEAN GenerateOnClose
) {
    NTSTATUS status;

#if defined(__i386__)
    // Locals to hold casted values (for safe 32-bit pushing)
    uint32_t subName = (uint32_t)(uintptr_t)SubsystemName;
    uint32_t hId = (uint32_t)(uintptr_t)HandleId;
    uint32_t objType = (uint32_t)(uintptr_t)ObjectTypeName;
    uint32_t objName = (uint32_t)(uintptr_t)ObjectName;
    uint32_t secDesc = (uint32_t)(uintptr_t)SecurityDescriptor;
    uint32_t desAccess = (uint32_t)DesiredAccess;
    uint32_t genMap = (uint32_t)(uintptr_t)GenericMapping;
    uint32_t objCreate = (uint32_t)ObjectCreation;
    uint32_t grantAccess = (uint32_t)(uintptr_t)GrantedAccess;
    uint32_t accStatus = (uint32_t)(uintptr_t)AccessStatus;
    uint32_t genClose = (uint32_t)(uintptr_t)GenerateOnClose;

    __asm__ volatile (
        "push %10\n\t"  // GenerateOnClose
        "push %9\n\t"   // AccessStatus
        "push %8\n\t"   // GrantedAccess
        "push %7\n\t"   // ObjectCreation
        "push %6\n\t"   // GenericMapping
        "push %5\n\t"   // DesiredAccess
        "push %4\n\t"   // SecurityDescriptor
        "push %3\n\t"   // ObjectName
        "push %2\n\t"   // ObjectTypeName
        "push %1\n\t"   // HandleId
        "push %0\n\t"   // SubsystemName
        "mov %11, %%eax\n\t"  // Syscall number
        "int $0x002e\n\t"
        "add $44, %%esp\n\t"  // 11 * 4 = 44
        : "=a"(status)
        : "m"(subName), "m"(hId), "m"(objType), "m"(objName), "m"(secDesc),
          "m"(desAccess), "m"(genMap), "m"(objCreate), "m"(grantAccess),
          "m"(accStatus), "m"(genClose), "i"(SYS_NtAccessCheckAndAuditAlarm)
        : "memory", "cc"
    );
#else
    // x64 placeholder; implement properly if needed
    status = STATUS_ACCESS_DENIED;
#endif

    return status;
}

// Fixed NtAccessCheckByType (32-bit version)
#if defined(__i386__)
NTSTATUS NTAPI NtAccessCheckByType(
    PSECURITY_DESCRIPTOR SecurityDescriptor,
    PSID PrincipalSelfSid,
    HANDLE ClientToken,
    ACCESS_MASK DesiredAccess,
    POBJECT_TYPE_LIST TypeList,
    ULONG TypeListLength,
    PGENERIC_MAPPING GenericMapping,
    PPRIVILEGE_SET Privileges,
    PULONG PrivilegeSetLength,
    PULONG GrantedAccess,
    PBOOLEAN AccessStatus
) {
    NTSTATUS status;

    uint32_t secDesc = (uint32_t)(uintptr_t)SecurityDescriptor;
    uint32_t prinSid = (uint32_t)(uintptr_t)PrincipalSelfSid;
    uint32_t clToken = (uint32_t)(uintptr_t)ClientToken;
    uint32_t desAccess = (uint32_t)DesiredAccess;
    uint32_t typList = (uint32_t)(uintptr_t)TypeList;
    uint32_t typLen = (uint32_t)TypeListLength;
    uint32_t genMap = (uint32_t)(uintptr_t)GenericMapping;
    uint32_t privs = (uint32_t)(uintptr_t)Privileges;
    uint32_t privLen = (uint32_t)(uintptr_t)PrivilegeSetLength;
    uint32_t grantAcc = (uint32_t)(uintptr_t)GrantedAccess;
    uint32_t accStat = (uint32_t)(uintptr_t)AccessStatus;

    __asm__ volatile (
        "push %10\n\t"  // AccessStatus
        "push %9\n\t"   // GrantedAccess
        "push %8\n\t"   // PrivilegeSetLength
        "push %7\n\t"   // Privileges
        "push %6\n\t"   // GenericMapping
        "push %5\n\t"   // TypeListLength
        "push %4\n\t"   // TypeList
        "push %3\n\t"   // DesiredAccess
        "push %2\n\t"   // ClientToken
        "push %1\n\t"   // PrincipalSelfSid
        "push %0\n\t"   // SecurityDescriptor
        "mov %11, %%eax\n\t"
        "int $0x002e\n\t"
        "add $44, %%esp\n\t"
        : "=a"(status)
        : "m"(secDesc), "m"(prinSid), "m"(clToken), "m"(desAccess), "m"(typList),
          "m"(typLen), "m"(genMap), "m"(privs), "m"(privLen), "m"(grantAcc),
          "m"(accStat), "i"(SYS_NtAccessCheckByType)
        : "memory", "cc"
    );

    return status;
}
#endif

// Fixed NtAccessCheckByType (x64 version)
#if defined(__x86_64__)
NTSTATUS NTAPI NtAccessCheckByType(
    PSECURITY_DESCRIPTOR SecurityDescriptor,
    PSID PrincipalSelfSid,
    HANDLE ClientToken,
    ACCESS_MASK DesiredAccess,
    POBJECT_TYPE_LIST TypeList,
    ULONG TypeListLength,
    PGENERIC_MAPPING GenericMapping,
    PPRIVILEGE_SET Privileges,
    PULONG PrivilegeSetLength,
    PULONG GrantedAccess,
    PBOOLEAN AccessStatus
) {
    NTSTATUS status;
    const uint64_t syscall_number = SYS_NtAccessCheckByType;  // Use actual number

    // Set registers for args 1-4
    register uint64_t rax_reg asm("rax") = syscall_number;
    register uint64_t rcx_reg asm("rcx") = (uint64_t)SecurityDescriptor;  // arg1 (will move to r10)
    register uint64_t rdx_reg asm("rdx") = (uint64_t)PrincipalSelfSid;   // arg2
    register uint64_t r8_reg asm("r8") = (uint64_t)ClientToken;          // arg3
    register uint64_t r9_reg asm("r9") = (uint64_t)DesiredAccess;        // arg4

    // Args 5+ go on stack (push in reverse); reserve 32-byte shadow space
    __asm__ volatile (
        "sub $0x20, %%rsp\n\t"  // Shadow space
        "push %5\n\t"           // AccessStatus (arg11)
        "push %6\n\t"           // GrantedAccess (arg10)
        "push %7\n\t"           // PrivilegeSetLength (arg9)
        "push %8\n\t"           // Privileges (arg8)
        "push %9\n\t"           // GenericMapping (arg7)
        "push %10\n\t"          // TypeListLength (arg6)
        "mov %%rcx, %%r10\n\t"  // Move arg1 to r10 (kernel expectation)
        "syscall\n\t"
        "add $0x50, %%rsp\n\t"  // Cleanup: 32 shadow + 6*8=48 = 80 (0x50)
        : "=a"(status)
        : "r"(rax_reg), "r"(rcx_reg), "r"(rdx_reg), "r"(r8_reg), "r"(r9_reg),
          "r"(AccessStatus), "r"(GrantedAccess), "r"(PrivilegeSetLength),
          "r"(Privileges), "r"(GenericMapping), "r"(TypeListLength),
          "r"((uint64_t)TypeList)  // arg5 on stack? No, for >4, stack starts at arg5
        : "r10", "r11", "memory", "cc"
    );

    return status;
}
#endif

// Fixed NtAccessCheckByTypeAndAuditAlarm (similar pattern; 16 params)
NTSTATUS NTAPI NtAccessCheckByTypeAndAuditAlarm(
    PUNICODE_STRING SubsystemName,
    PVOID HandleId,
    PUNICODE_STRING ObjectTypeName,
    PUNICODE_STRING ObjectName,
    PSECURITY_DESCRIPTOR SecurityDescriptor,
    PSID PrincipalSelfSid,
    ACCESS_MASK DesiredAccess,
    AUDIT_EVENT_TYPE AuditType,
    ULONG Flags,
    POBJECT_TYPE_LIST ObjectTypeList,
    ULONG ObjectTypeListLength,
    PGENERIC_MAPPING GenericMapping,
    BOOLEAN ObjectCreation,
    PULONG GrantedAccess,
    PBOOLEAN AccessStatus,
    PBOOLEAN GenerateOnClose
) {
    NTSTATUS status;

#if defined(__i386__)
    uint32_t subName = (uint32_t)(uintptr_t)SubsystemName;
    uint32_t hId = (uint32_t)(uintptr_t)HandleId;
    uint32_t objType = (uint32_t)(uintptr_t)ObjectTypeName;
    uint32_t objName = (uint32_t)(uintptr_t)ObjectName;
    uint32_t secDesc = (uint32_t)(uintptr_t)SecurityDescriptor;
    uint32_t prinSid = (uint32_t)(uintptr_t)PrincipalSelfSid;
    uint32_t desAccess = (uint32_t)DesiredAccess;
    uint32_t audType = (uint32_t)AuditType;
    uint32_t flgs = (uint32_t)Flags;
    uint32_t objTList = (uint32_t)(uintptr_t)ObjectTypeList;
    uint32_t objTLen = (uint32_t)ObjectTypeListLength;
    uint32_t genMap = (uint32_t)(uintptr_t)GenericMapping;
    uint32_t objCreate = (uint32_t)ObjectCreation;
    uint32_t grantAcc = (uint32_t)(uintptr_t)GrantedAccess;
    uint32_t accStat = (uint32_t)(uintptr_t)AccessStatus;
    uint32_t genClose = (uint32_t)(uintptr_t)GenerateOnClose;

    __asm__ volatile (
        "push %15\n\t"  // GenerateOnClose
        "push %14\n\t"  // AccessStatus
        "push %13\n\t"  // GrantedAccess
        "push %12\n\t"  // ObjectCreation
        "push %11\n\t"  // GenericMapping
        "push %10\n\t"  // ObjectTypeListLength
        "push %9\n\t"   // ObjectTypeList
        "push %8\n\t"   // Flags
        "push %7\n\t"   // AuditType
        "push %6\n\t"   // DesiredAccess
        "push %5\n\t"   // PrincipalSelfSid
        "push %4\n\t"   // SecurityDescriptor
        "push %3\n\t"   // ObjectName
        "push %2\n\t"   // ObjectTypeName
        "push %1\n\t"   // HandleId
        "push %0\n\t"   // SubsystemName
        "mov %16, %%eax\n\t"
        "int $0x002e\n\t"
        "add $64, %%esp\n\t"  // 16 * 4 = 64
        : "=a"(status)
        : "m"(subName), "m"(hId), "m"(objType), "m"(objName), "m"(secDesc),
          "m"(prinSid), "m"(desAccess), "m"(audType), "m"(flgs), "m"(objTList),
          "m"(objTLen), "m"(genMap), "m"(objCreate), "m"(grantAcc), "m"(accStat),
          "m"(genClose), "i"(SYS_NtAccessCheckByTypeAndAuditAlarm)
        : "memory", "cc"
    );
#else
	status = STATUS_ACCESS_DENIED;
#endif

    return status;
}

#if defined(__i386__)
NTSTATUS NTAPI NtAccessCheckByTypeResultList(
    PSECURITY_DESCRIPTOR SecurityDescriptor,
    PSID PrincipalSelfSid,
    HANDLE ClientToken,
    ACCESS_MASK DesiredAccess,
    POBJECT_TYPE_LIST TypeList,
    ULONG TypeListLength,
    PGENERIC_MAPPING GenericMapping,
    PPRIVILEGE_SET Privileges,
    PULONG PrivilegeSetLength,
    PULONG GrantedAccessList,
    PBOOLEAN AccessStatusList
) {
    NTSTATUS status;

    /* Push arguments (reverse order) onto the stack and invoke int 0x2e.
       Count them carefully. Here we have 11 args -> 11 * 4 = 44 bytes to clean. */

    asm volatile (
        /* push in reverse order */
        "push %[asl]\n\t"        /* AccessStatusList */
        "push %[gal]\n\t"        /* GrantedAccessList */
        "push %[psl]\n\t"        /* PrivilegeSetLength */
        "push %[priv]\n\t"       /* Privileges */
        "push %[gmap]\n\t"       /* GenericMapping */
        "push %[tlen]\n\t"       /* TypeListLength */
        "push %[tlist]\n\t"      /* TypeList */
        "push %[daccess]\n\t"    /* DesiredAccess */
        "push %[ctoken]\n\t"     /* ClientToken */
        "push %[psid]\n\t"       /* PrincipalSelfSid */
        "push %[sd]\n\t"         /* SecurityDescriptor */
        "mov eax, %[scnum]\n\t"  /* syscall number */
        "int $0x2e\n\t"
        "add $44, %%esp\n\t"     /* cleanup 11 * 4 */
        "mov %[out], eax\n\t"
        : [out] "=r"(status)
        : /* inputs */
          [scnum] "i"(SYS_NtAccessCheckByTypeResultList),
          [sd] "r"(SecurityDescriptor),
          [psid] "r"(PrincipalSelfSid),
          [ctoken] "r"(ClientToken),
          [daccess] "r"(DesiredAccess),
          [tlist] "r"(TypeList),
          [tlen] "r"(TypeListLength),
          [gmap] "r"(GenericMapping),
          [priv] "r"(Privileges),
          [psl] "r"(PrivilegeSetLength),
          [gal] "r"(GrantedAccessList),
          [asl] "r"(AccessStatusList)
        : "eax", "memory"
    );

    return status;
}
#endif

int NtDisplayString(PUNICODE_STRING String);

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	PUNICODE_STRING fname = 0;
	fname->Length = 30;
	fname->MaximumLength = 50;
	fname->Buffer = L"Hello, DLL!\n";

	NtDisplayString(fname);

    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH:
        	fname->Buffer = L"Process attached!\n";
        	NtDisplayString(fname);
            break;
        case DLL_THREAD_ATTACH:
        	fname->Buffer = L"Thread attached!\n";
        	NtDisplayString(fname);
            break;
        case DLL_THREAD_DETACH:

            break;
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}

int AddNumbers(int a, int b) {
    return a + b;
}

int NtDisplayString(PUNICODE_STRING String)
{
	asm volatile (
					"movl $0x002e, %%eax\n\t"
					"movl %0, %%ebx\n\t"
					"int $0x002e\n\t"
					:
					: "r"(String)
					: "%eax", "%ebx"
			);

    return 0;
}

wchar_t fname_buffer[512];
UNICODE_STRING fname_struct = {
    .Length = 0,
    .MaximumLength = sizeof(fname_buffer),
    .Buffer = fname_buffer
};

void SystemPrint(PUNICODE_STRING fname, const wchar_t* msg)
{
    size_t len = wcslen(msg);
    if (len * sizeof(wchar_t) >= fname->MaximumLength)
        len = (fname->MaximumLength / sizeof(wchar_t)) - 1;

    wcsncpy(fname->Buffer, msg, len);
    fname->Buffer[len] = L'\0';
    fname->Length = (USHORT)(len * sizeof(wchar_t));

    NtDisplayString(fname);
}

NTSTATUS NTAPI NtQuerySystemInformation(
    SYSTEM_INFORMATION_CLASS SystemInformationClass,
    PVOID                    SystemInformation,
    ULONG                    SystemInformationLength,
    PULONG                   ReturnLength
) {

    if (!SystemInformation)
        return STATUS_INFO_LENGTH_MISMATCH;

    UNICODE_STRING fname_struct;
    fname_struct.Buffer = fname_buffer;
    fname_struct.Length = 0;
    fname_struct.MaximumLength = sizeof(fname_buffer);

    wchar_t buf[512];

    switch (SystemInformationClass)
    {
        case SystemExceptionInformation:
        {
            PSYSTEM_EXCEPTION_INFORMATION sei = (PSYSTEM_EXCEPTION_INFORMATION)SystemInformation;

            if (SystemInformationLength < sizeof(*sei))
                return STATUS_INFO_LENGTH_MISMATCH;

            wchar_t hex[8];
            wcscpy(buf, L"[SystemExceptionInformation] Raw data: ");
            for (int i = 0; i < sizeof(sei->Reserved1); i++) {
                swprintf(hex, 8, L"%02X ", sei->Reserved1[i]);
                wcsncat(buf, hex, 512 - wcslen(buf) - 1);
            }
            SystemPrint(&fname_struct, buf);
            if (ReturnLength) *ReturnLength = sizeof(*sei);
            return STATUS_SUCCESS;
        }

        case SystemLeapSecondInformation:
        {
            PSYSTEM_LEAP_SECOND_INFORMATION lsi = (PSYSTEM_LEAP_SECOND_INFORMATION)SystemInformation;

            if (SystemInformationLength < sizeof(*lsi))
                return STATUS_INFO_LENGTH_MISMATCH;

            wcscpy(buf, L"[SystemLeapSecondInformation] Raw data: ");
            wchar_t hex[8];
            for (int i = 0; i < sizeof(lsi->Reserved1); i++) {
                swprintf(hex, 8, L"%02X ", lsi->Reserved1[i]);
                wcsncat(buf, hex, 512 - wcslen(buf) - 1);
            }
            SystemPrint(&fname_struct, buf);
            if (ReturnLength) *ReturnLength = sizeof(*lsi);
            return STATUS_SUCCESS;
        }

        case SystemLookasideInformation:
        {
            SystemPrint(&fname_struct, L"[SystemLookasideInformation] Queried (opaque data)");
            if (ReturnLength) *ReturnLength = 0;
            return STATUS_SUCCESS;
        }

        case SystemCodeIntegrityInformation:
        {
            PSYSTEM_CODEINTEGRITY_INFORMATION sci = (PSYSTEM_CODEINTEGRITY_INFORMATION)SystemInformation;

            if (SystemInformationLength < sizeof(*sci))
                return STATUS_INFO_LENGTH_MISMATCH;

            sci->Length = sizeof(*sci);
            sci->CodeIntegrityOptions = CODEINTEGRITY_OPTION_ENABLED |
                                        CODEINTEGRITY_OPTION_HVCI_KMCI_ENABLED;

            swprintf(buf, 512, L"[SystemCodeIntegrityInformation] Flags: 0x%08lX", sci->CodeIntegrityOptions);
            SystemPrint(&fname_struct, buf);

            if (ReturnLength) *ReturnLength = sizeof(*sci);
            return STATUS_SUCCESS;
        }

        case SystemKernelVaShadowInformation:
        {
            PSYSTEM_KERNEL_VA_SHADOW_INFORMATION skvsi = (PSYSTEM_KERNEL_VA_SHADOW_INFORMATION)SystemInformation;

            if (SystemInformationLength < sizeof(*skvsi))
                return STATUS_INFO_LENGTH_MISMATCH;

            skvsi->Flags = KVA_SHADOW_ENABLED | KVA_SHADOW_PCID | KVA_L1TF_MITIGATION_PRESENT;

            swprintf(buf, 512, L"[SystemKernelVaShadowInformation] Flags: 0x%08lX", skvsi->Flags);
            SystemPrint(&fname_struct, buf);

            if (ReturnLength) *ReturnLength = sizeof(*skvsi);
            return STATUS_SUCCESS;
        }

        case SystemPerformanceInformation:
        {
            PSYSTEM_PERFORMANCE_INFORMATION spi = (PSYSTEM_PERFORMANCE_INFORMATION)SystemInformation;

            if (SystemInformationLength < sizeof(*spi))
                return STATUS_INFO_LENGTH_MISMATCH;

            ZeroMemory(spi, sizeof(*spi));
            spi->IdleTime.QuadPart = 12345678;
            spi->PageFaultCount = 42;
            spi->AvailablePages = 8192;

            swprintf(buf, 512, L"[SystemPerformanceInformation] IdleTime: %lld, PageFaults: %lu, AvailablePages: %lu",
                     spi->IdleTime.QuadPart, spi->PageFaultCount, spi->AvailablePages);
            SystemPrint(&fname_struct, buf);

            if (ReturnLength) *ReturnLength = sizeof(*spi);
            return STATUS_SUCCESS;
        }

        case SystemInterruptInformation:
        {
            PSYSTEM_INTERRUPT_INFORMATION sii = (PSYSTEM_INTERRUPT_INFORMATION)SystemInformation;

            swprintf(buf, 512, L"[SystemInterruptInformation] ContextSwitches=%lu, DpcCount=%lu, DpcRate=%lu, TimeIncrement=%lu, DpcBypassCount=%lu, ApcBypassCount=%lu",
                     sii->ContextSwitches, sii->DpcCount, sii->DpcRate, sii->TimeIncrement, sii->DpcBypassCount, sii->ApcBypassCount);
            SystemPrint(&fname_struct, buf);

            if (ReturnLength) *ReturnLength = sizeof(*sii);
            return STATUS_SUCCESS;
        }

		case SystemPolicyInformation:
		{
   			PSYSTEM_POLICY_INFORMATION spi = (PSYSTEM_POLICY_INFORMATION)SystemInformation;

    		if (SystemInformationLength < sizeof(*spi))
        	return STATUS_INFO_LENGTH_MISMATCH;

    		ZeroMemory(spi, sizeof(*spi));

    		SystemPrint(&fname_struct, L"[SystemPolicyInformation] Queried (dummy policy data)");

    		if (ReturnLength) *ReturnLength = sizeof(*spi);
    		return STATUS_SUCCESS;
		}

		case SystemProcessInformation:
		{
    		if (SystemInformationLength < sizeof(SYSTEM_BASICPROCESS_INFORMATION))
     		   return STATUS_INFO_LENGTH_MISMATCH;

  			PSYSTEM_BASICPROCESS_INFORMATION procInfo = (PSYSTEM_BASICPROCESS_INFORMATION)SystemInformation;

   			procInfo->NextEntryOffset = 0;
    		procInfo->UniqueProcessId = (HANDLE)1234;
   			procInfo->InheritedFromUniqueProcessId = (HANDLE)1;
    		procInfo->SequenceNumber = 1;

    		static wchar_t dummyName[] = L"DummyProcess.exe";
    		procInfo->ImageName.Buffer = dummyName;
    		procInfo->ImageName.Length = (USHORT)(wcslen(dummyName) * sizeof(wchar_t));
    		procInfo->ImageName.MaximumLength = sizeof(dummyName);

    		SystemPrint(&fname_struct, L"[SystemProcessInformation] Single dummy process entry filled");

    		if (ReturnLength) *ReturnLength = sizeof(SYSTEM_BASICPROCESS_INFORMATION);
    		return STATUS_SUCCESS;
		}

		case SystemProcessorPerformanceInformation:
		{
  			PSYSTEM_PROCESSOR_PERFORMANCE_INFORMATION ppi =
        		(PSYSTEM_PROCESSOR_PERFORMANCE_INFORMATION)SystemInformation;

    		SYSTEM_INFO sysInfo;
    		GetSystemInfo(&sysInfo);
    		DWORD nProcs = sysInfo.dwNumberOfProcessors;

   			if (SystemInformationLength < nProcs * sizeof(SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION))
        		return STATUS_INFO_LENGTH_MISMATCH;

    		for (DWORD i = 0; i < nProcs; i++) {
        		ppi[i].IdleTime.QuadPart = 1000 * i;
        		ppi[i].KernelTime.QuadPart = 2000 * i;
        		ppi[i].UserTime.QuadPart = 1500 * i;
        		ppi[i].DpcTime.QuadPart = 50 * i;
        		ppi[i].InterruptTime.QuadPart = 25 * i;
        		ppi[i].InterruptCount = 100 + i;
    		}

    		SystemPrint(&fname_struct, L"[SystemProcessorPerformanceInformation] Dummy CPU stats filled");

    		if (ReturnLength)
				*ReturnLength = nProcs * sizeof(SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION);

			return STATUS_SUCCESS;
		}

		case SystemQueryPerformanceCounterInformation:
		{
    		if (SystemInformationLength < sizeof(SYSTEM_QUERY_PERFORMANCE_COUNTER_INFORMATION))
        		return STATUS_INFO_LENGTH_MISMATCH;

    		PSYSTEM_QUERY_PERFORMANCE_COUNTER_INFORMATION qpi =
        		(PSYSTEM_QUERY_PERFORMANCE_COUNTER_INFORMATION)SystemInformation;

    		qpi->PerformanceCounter.QuadPart = 1234567890;
    		qpi->PerformanceFrequency.QuadPart = 10000000;

    		SystemPrint(&fname_struct, L"[SystemQueryPerformanceCounterInformation] Dummy performance counter filled");

    		if (ReturnLength)
        		*ReturnLength = sizeof(*qpi);

    		return STATUS_SUCCESS;
		}

		case SystemRegistryQuotaInformation:
		{
    		if (SystemInformationLength < sizeof(SYSTEM_REGISTRY_QUOTA_INFORMATION))
        		return STATUS_INFO_LENGTH_MISMATCH;

    		PSYSTEM_REGISTRY_QUOTA_INFORMATION rqi =
        	(PSYSTEM_REGISTRY_QUOTA_INFORMATION)SystemInformation;

    		ZeroMemory(rqi, sizeof(*rqi));

    		SystemPrint(&fname_struct, L"[SystemRegistryQuotaInformation] Queried (dummy data)");

    		if (ReturnLength)
        		*ReturnLength = sizeof(*rqi);

    		return STATUS_SUCCESS;
		}

		case SystemSpeculationControlInformation:
		{
    		if (SystemInformationLength < sizeof(SYSTEM_SPECULATION_CONTROL_INFORMATION))
        		return STATUS_INFO_LENGTH_MISMATCH;

   			PSYSTEM_SPECULATION_CONTROL_INFORMATION sci =
        		(PSYSTEM_SPECULATION_CONTROL_INFORMATION)SystemInformation;

    		sci->SpeculationControlFlags.BpbEnabled = 1;
    		sci->SpeculationControlFlags.IbrsPresent = 1;
    		sci->SpeculationControlFlags.StibpPresent = 1;
    		sci->SpeculationControlFlags.SpeculativeStoreBypassDisableAvailable = 1;

    		sci->SpeculationControlFlags.Reserved = 0;

    		SystemPrint(&fname_struct, L"[SystemSpeculationControlInformation] Dummy speculative execution flags set");

    		if (ReturnLength)
        		*ReturnLength = sizeof(*sci);

    		return STATUS_SUCCESS;
		}

		case SystemTimeOfDayInformation:
		{
    		if (SystemInformationLength < sizeof(SYSTEM_TIMEOFDAY_INFORMATION))
        		return STATUS_INFO_LENGTH_MISMATCH;

    		PSYSTEM_TIMEOFDAY_INFORMATION tdi =
       			(PSYSTEM_TIMEOFDAY_INFORMATION)SystemInformation;

    		tdi->CurrentTime.QuadPart = 1324567890123456;
    		tdi->BootTime.QuadPart = 1324560000000000;
    		tdi->TimeZoneBias.QuadPart = 0;
    		tdi->TimeZoneId = 0;
    		for (int i = 0; i < 3; i++)
    		tdi->Reserved[i] = 0;

    		SystemPrint(&fname_struct, L"[SystemTimeOfDayInformation] Dummy system time filled");

    		if (ReturnLength)
        		*ReturnLength = sizeof(*tdi);

    		return STATUS_SUCCESS;
		}

        default:
        {
            swprintf(buf, 512, L"[NtQuerySystemInformation] Unsupported class: %lu", SystemInformationClass);
            SystemPrint(&fname_struct, buf);
            return STATUS_INVALID_INFO_CLASS;
        }
    }
}






