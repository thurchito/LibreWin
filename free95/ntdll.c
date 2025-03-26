/*++

Free95 20x/TX Kernel

You may only use this code if you agree to the terms of the Free95 Source Code License agreement (GNU GPL v3) (see LICENSE).
If you do not agree to the terms, do not use the code.


Module Name:

    ntdll.c

Abstract:

    This module implements replicating NTDLL, an essential Dynamic Link Library for Windows Applications.
    It implements low-level functions, like invoking syscalls.

--*/

#include <windows.h>

// Exported function
__declspec(dllexport) int AddNumbers(int a, int b)
//int AddNumbers(int a, int b)
{
    return a + b;
}

typedef struct _UNICODE_STRING
{
    unsigned short Length;
    unsigned short MaximumLength;
    char* Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	char text[] = "Hello from NTDLL!\n";
	char loaded[] = "Attached to Thread!\n";
	char loaded2[] = "Attached to Process!\n";
	
	PUNICODE_STRING str;

	str->Length = 30;
	str->MaximumLength = 50;
	str->Buffer = text;

	asm volatile (
		        "movl $0x002e, %%eax\n\t"
		        "movl %0, %%ebx\n\t"
		        "int $0x2e\n\t"
		        :
		        : "r"(str)
		        : "%eax", "%ebx"
		);

    switch (fdwReason) {
        case DLL_PROCESS_ATTACH:
				str->Length = 30;
            	str->MaximumLength = 50;
            	str->Buffer = loaded2;
            
            	asm volatile (
            		        "movl $0x002e, %%eax\n\t"
            		        "movl %0, %%ebx\n\t"
            		        "int $0x2e\n\t"
            		        :
            		        : "r"(str)
            		        : "%eax", "%ebx"
            		);
            break;
        case DLL_THREAD_ATTACH:
				str->Length = 30;
            	str->MaximumLength = 50;
            	str->Buffer = loaded;
            
            	asm volatile (
            		        "movl $0x002e, %%eax\n\t"
            		        "movl %0, %%ebx\n\t"
            		        "int $0x2e\n\t"
            		        :
            		        : "r"(str)
            		        : "%eax", "%ebx"
            		);
            break;
        case DLL_THREAD_DETACH:
            // Code to run when a thread ends
            break;
        case DLL_PROCESS_DETACH:
            // Code to run when the DLL is unloaded
            break;
    }

    return TRUE; // Successful load
}
