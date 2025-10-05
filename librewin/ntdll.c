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
            // Code to run when a thread is destroyed
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

NTSTATUS NTAPI NtQuerySystemInformation(
    SYSTEM_INFORMATION_CLASS SystemInformationClass,
    PVOID                    SystemInformation,
    ULONG                    SystemInformationLength,
    PULONG                   ReturnLength
) {
    if (SystemInformationClass == SystemBasicInformation) {
		unsigned int eax, ebx, ecx, edx;
        unsigned int ProcessorCount = 0;
        asm volatile (
            "cpuid"
            : "=a" (eax), "=b" (ebx), "=c" (ecx), "=d" (edx)
            : "a" (0xB), "c" (0)

		);
    	ProcessorCount = ebx & 0xFF;
    	SystemInformation = (PVOID)(uintptr_t)ProcessorCount;
    }
	if (SystemInformationClass == SystemCodeIntegrityInformation) {
		SYSTEM_CODEINTEGRITY_INFORMATION* sci = (SYSTEM_CODEINTEGRITY_INFORMATION*)SystemInformation;

   		if (sci->Length < sizeof(SYSTEM_CODEINTEGRITY_INFORMATION))
       		return STATUS_INFO_LENGTH_MISMATCH;

   		if (sci->CodeIntegrityOptions & CODEINTEGRITY_OPTION_ENABLED)
   		{
			return STATUS_SUCCESS;
  		}
    }
	if (SystemInformationClass == SystemExceptionInformation) {
    	PSYSTEM_EXCEPTION_INFORMATION sei = 
        	(PSYSTEM_EXCEPTION_INFORMATION)SystemInformation;

    	if (sei == NULL || SystemInformationLength < sizeof(*sei))
        	return STATUS_INFO_LENGTH_MISMATCH;

    	for (int i = 0; i < 16; i++) {
        	printf("%02X ", sei->Reserved1[i]);
    	}
    	printf("\n");

    	return STATUS_SUCCESS;
	}
	if (SystemInformationClass == SystemInterruptInformation) {
        PSYSTEM_INTERRUPT_INFORMATION sii =
            (PSYSTEM_INTERRUPT_INFORMATION)SystemInformation;

        printf("[SystemInterruptInformation]\n");
        printf("ContextSwitches: %lu\n", sii->ContextSwitches);
        printf("DpcCount:        %lu\n", sii->DpcCount);
        printf("DpcRate:         %lu\n", sii->DpcRate);
        printf("TimeIncrement:   %lu\n", sii->TimeIncrement);
        printf("DpcBypassCount:  %lu\n", sii->DpcBypassCount);
        printf("ApcBypassCount:  %lu\n", sii->ApcBypassCount);

        return STATUS_SUCCESS;
    }
	if (SystemInformationClass == SystemKernelVaShadowInformation) {
		SYSTEM_KERNEL_VA_SHADOW_INFORMATION* skvsi = (SYSTEM_KERNEL_VA_SHADOW_INFORMATION*)SystemInformation;
		if (skvsi->Flags & KVA_SHADOW_ENABLED)
        return KVA_SHADOW_ENABLED;
		
    	if (skvsi->Flags & KVA_SHADOW_REQUIRED)
        return KVA_SHADOW_REQUIRED;
		
    	if (skvsi->Flags & KVA_SHADOW_REQUIRED_AVAILABLE)
        return KVA_SHADOW_REQUIRED_AVAILABLE;
		
    	if (skvsi->Flags & KVA_SHADOW_PCID)
        return KVA_SHADOW_PCID;
		
    	if (skvsi->Flags & KVA_SHADOW_INVPCID)
        return KVA_SHADOW_INVPCID;
		
   		if (skvsi->Flags & KVA_L1TF_MITIGATION_PRESENT)
        return KVA_L1TF_MITIGATION_PRESENT;
		
    	if (skvsi->Flags & KVA_L1D_FLUSH_SUPPORTED)
        return KVA_L1D_FLUSH_SUPPORTED;
	}
}
