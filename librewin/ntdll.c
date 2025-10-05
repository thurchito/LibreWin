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
#include <stdio.h>
#include "basetsd.h"

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
    SystemLookasideInformation = 45
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

typedef struct _SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION {
    LARGE_INTEGER IdleTime;
    LARGE_INTEGER KernelTime;
    LARGE_INTEGER UserTime;
    LARGE_INTEGER DpcTime;
    LARGE_INTEGER InterruptTime;
    ULONG InterruptCount;
} SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION, *PSYSTEM_PROCESSOR_PERFORMANCE_INFORMATION;

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

SYSTEM_INFORMATION_CLASS SystemInformationClass;
PVOID SystemInformation;
ULONG SystemInformationLength;
PULONG ReturnLength;
SYSTEM_BASIC_INFORMATION sbi;
ULONG len;
ULONG sbisize = sizeof(sbi);

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

__declspec(dllexport) int AddNumbers(int a, int b) {
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

UNICODE_STRING fname_struct;
wchar_t fname_buffer[512];
fname_struct.Buffer = fname_buffer;
fname_struct.Length = 0;
fname_struct.MaximumLength = sizeof(fname_buffer);

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
    wchar_t fname_buffer[512];
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

    		DisplayMessage(&fname_struct, L"[SystemPolicyInformation] Queried (dummy policy data)");

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

    		DisplayMessage(&fname_struct, L"[SystemProcessInformation] Single dummy process entry filled");

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

    		DisplayMessage(&fname_struct, L"[SystemProcessorPerformanceInformation] Dummy CPU stats filled");

    		if (ReturnLength)
				*ReturnLength = nProcs * sizeof(SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION);
    		
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

