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

#define SYS_NtAccessCheckAndAuditAlarm 0x0001
#define SYS_NtAccessCheckByType 0x0002
#define SYS_NtAccessCheckByTypeAndAuditAlarm 0x0003

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
        "mov $0x60, %%eax\n\t"
        "int $0x2e\n\t"
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

#define SYS_NtAccessCheck 0x123

NTSTATUS NTAPI NtAccessCheck(
    PSECURITY_DESCRIPTOR SecurityDescriptor,
    HANDLE ClientToken,
    ACCESS_MASK DesiredAccess,
    PGENERIC_MAPPING GenericMapping,
    PRIVILEGE_SET* Privileges,
    ULONG* PrivilegeSetLength,
    PULONG GrantedAccess,
    BOOLEAN* AccessStatus
) {
    NTSTATUS status;

    uint32_t sd = (uint32_t)(uintptr_t)SecurityDescriptor;
    uint32_t token = (uint32_t)(uintptr_t)ClientToken;
    uint32_t access = (uint32_t)DesiredAccess;
    uint32_t mapping = (uint32_t)(uintptr_t)GenericMapping;
    uint32_t priv = (uint32_t)(uintptr_t)Privileges;
    uint32_t privLen = (uint32_t)(uintptr_t)PrivilegeSetLength;
    uint32_t granted = (uint32_t)(uintptr_t)GrantedAccess;
    uint32_t statusPtr = (uint32_t)(uintptr_t)AccessStatus;

    asm volatile(
        "mov $0x123, %%eax\n\t"
        "push %8\n\t"
        "push %7\n\t"
        "push %6\n\t"
        "push %5\n\t"
        "push %4\n\t"
        "push %3\n\t"
        "push %2\n\t"
        "push %1\n\t"
        "int $0x2e\n\t"
        "add $32, %%esp\n\t"
        : "=a"(status)
        : "m"(sd),
          "m"(token),
          "m"(access),
          "m"(mapping),
          "m"(priv),
          "m"(privLen),
          "m"(granted),
          "m"(statusPtr)
        : "memory", "cc"
    );

    return status;
}

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

    __asm__ volatile (
        "push %[GenerateOnClose]\n\t"
        "push %[AccessStatus]\n\t"
        "push %[GrantedAccess]\n\t"
        "push %[ObjectCreation]\n\t"
        "push %[GenericMapping]\n\t"
        "push %[DesiredAccess]\n\t"
        "push %[SecurityDescriptor]\n\t"
        "push %[ObjectName]\n\t"
        "push %[ObjectTypeName]\n\t"
        "push %[HandleId]\n\t"
        "push %[SubsystemName]\n\t"
        "mov eax, %[Syscall]\n\t"
        "int $0x2e\n\t"
        "add esp, 44\n\t"  // 11 args * 4 bytes each
        "mov %[Status], eax\n\t"
        : [Status] "=r"(status)
        : [Syscall] "i"(SYS_NtAccessCheckAndAuditAlarm),
          [SubsystemName] "r"(SubsystemName),
          [HandleId] "r"(HandleId),
          [ObjectTypeName] "r"(ObjectTypeName),
          [ObjectName] "r"(ObjectName),
          [SecurityDescriptor] "r"(SecurityDescriptor),
          [DesiredAccess] "r"(DesiredAccess),
          [GenericMapping] "r"(GenericMapping),
          [ObjectCreation] "r"(ObjectCreation),
          [GrantedAccess] "r"(GrantedAccess),
          [AccessStatus] "r"(AccessStatus),
          [GenerateOnClose] "r"(GenerateOnClose)
        : "eax", "memory"
    );

    return status;
}

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

    /* Push args (reverse) and int 0x2e. Keep operands as inputs ("r"). */
    asm volatile (
        "mov eax, %1\n\t"     /* syscall number */
        "push %11\n\t"        /* AccessStatus */
        "push %10\n\t"        /* GrantedAccess */
        "push %9\n\t"         /* PrivilegeSetLength */
        "push %8\n\t"         /* Privileges */
        "push %7\n\t"         /* GenericMapping */
        "push %6\n\t"         /* TypeListLength */
        "push %5\n\t"         /* TypeList */
        "push %4\n\t"         /* DesiredAccess */
        "push %3\n\t"         /* ClientToken */
        "push %2\n\t"         /* PrincipalSelfSid */
        "push %0\n\t"         /* SecurityDescriptor */
        "int $0x2e\n\t"
        "mov %0, eax\n\t"
        "add esp, 44\n\t"     /* 11 args * 4 bytes = 44 */
        : "=r"(status)
        : "r"(SYS_NtAccessCheckByType),
          "r"(PrincipalSelfSid),
          "r"(ClientToken),
          "r"(DesiredAccess),
          "r"(TypeList),
          "r"(TypeListLength),
          "r"(GenericMapping),
          "r"(Privileges),
          "r"(PrivilegeSetLength),
          "r"(GrantedAccess),
          "r"(AccessStatus),
          "0"(status)
        : "eax", "memory"
    );

    return status;
}
#endif

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
    uint64_t status;
    const uint64_t syscall_number = 0x1234; /* replace */

    /* Bind registers: RCX, RDX, R8, R9 are first four args on Windows x64
       Put next arguments on the stack in order. */
    register uint64_t rcx_reg asm("rcx") = (uint64_t)SecurityDescriptor;      // arg1
    register uint64_t rdx_reg asm("rdx") = (uint64_t)PrincipalSelfSid;       // arg2
    register uint64_t r8_reg  asm("r8")  = (uint64_t)ClientToken;            // arg3
    register uint64_t r9_reg  asm("r9")  = (uint64_t)DesiredAccess;          // arg4

    /* For remaining args, place into stack prior to syscall. Some kernels expect
       user mode syscall wrappers that place them appropriately. */
    register uint64_t r10_reg asm("r10") = (uint64_t)TypeList; /* r10 commonly used by syscall */
    register uint64_t rax_reg asm("rax") = syscall_number;    /* syscall number in rax */

    asm volatile (
        "push %10\n\t"    /* TypeListLength */
        "push %9\n\t"     /* GenericMapping */
        "push %8\n\t"     /* Privileges */
        "push %7\n\t"     /* PrivilegeSetLength */
        "push %6\n\t"     /* GrantedAccess */
        "push %5\n\t"     /* AccessStatus */
        "syscall\n\t"
        "mov %0, rax\n\t"
        "add $48, %%rsp\n\t" /* cleanup: 6 * 8 = 48 bytes pushed */
        : "=r"(status)
        : "r"(rax_reg), "r"(rcx_reg), "r"(rdx_reg), "r"(r8_reg), "r"(r9_reg),
          "r"(AccessStatus), "r"(GrantedAccess), "r"(PrivilegeSetLength),
          "r"(Privileges), "r"(GenericMapping), "r"(TypeListLength)
        : "rcx", "rdx", "r8", "r9", "r11", "memory"
    );

    return (NTSTATUS)status;
}
#endif

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

    asm volatile (
        "mov eax, %1\n\t"
        "push %16\n\t"  // GenerateOnClose
        "push %15\n\t"  // AccessStatus
        "push %14\n\t"  // GrantedAccess
        "push %13\n\t"  // ObjectCreation
        "push %12\n\t"  // GenericMapping
        "push %11\n\t"  // ObjectTypeListLength
        "push %10\n\t"  // ObjectTypeList
        "push %9\n\t"   // Flags
        "push %8\n\t"   // AuditType
        "push %7\n\t"   // DesiredAccess
        "push %6\n\t"   // PrincipalSelfSid
        "push %5\n\t"   // SecurityDescriptor
        "push %4\n\t"   // ObjectName
        "push %3\n\t"   // ObjectTypeName
        "push %2\n\t"   // HandleId
        "push %0\n\t"   // SubsystemName
        "int $0x2e\n\t"
        "mov %0, eax\n\t"
        "add esp, 64\n\t"  // 16 * 4 bytes = 64
        : "=r"(status)
        : "r"(SYS_NtAccessCheckByTypeAndAuditAlarm),
          "r"(HandleId),
          "r"(ObjectTypeName),
          "r"(ObjectName),
          "r"(SecurityDescriptor),
          "r"(PrincipalSelfSid),
          "r"(DesiredAccess),
          "r"(AuditType),
          "r"(Flags),
          "r"(ObjectTypeList),
          "r"(ObjectTypeListLength),
          "r"(GenericMapping),
          "r"(ObjectCreation),
          "r"(GrantedAccess),
          "r"(AccessStatus),
          "r"(GenerateOnClose),
          "0"(status)
        : "eax", "memory"
    );

    return status;
}

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
					"int $0x2e\n\t"
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





