/*++

LibreWin 20x/TX Kernel

You may only use this code if you agree to the terms of the LibreWin Source Code License agreement (GNU GPL v3) (see LICENSE).
If you do not agree to the terms, do not use the code.


Module Name:

    ntdll.h

Abstract:

    This module implements the functions to call the syscalls.

--*/

#ifndef NTDLL_H
#define NTDLL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "base.h"
#include "basetsd.h"

#if defined(__i386__)

#define SYSCALL_NtDisplayString      0x002e
#define SYSCALL_NtOpenFile           0x004f
#define SYSCALL_NtAcceptConnectPort  0x60

static inline void NtDisplayString(PUNICODE_STRING String)
{
    asm volatile(
        "int $0x002e"
        :
        : "a"(SYSCALL_NtDisplayString),
          "b"(String)
        : "memory"
    );
}

static inline NTSTATUS NtOpenFile(
    PHANDLE FileHandle,
    int DesiredAccess,
    POBJECT_ATTRIBUTES ObjectAttributes,
    PVOID IoStatusBlock,
    ULONG ShareAccess,
    ULONG OpenOptions)
{
    NTSTATUS status;
    asm volatile(
        "int $0x002e"
        : "=a"(status)
        : "a"(SYSCALL_NtOpenFile),
          "b"(FileHandle),
          "c"(DesiredAccess),
          "d"(ObjectAttributes),
          "S"(IoStatusBlock),
          "D"(ShareAccess),
          "b"(FileHandle),
          "B"(FileHandle)
        : "ebp", "memory"
    );
    return status;
}

#else
#error "LibreWin syscall wrappers only implemented for x86 (int 0x002e interface)"
#endif /* __i386__ */

#ifdef __cplusplus
}
#endif

#endif /* NTDLL_H */

