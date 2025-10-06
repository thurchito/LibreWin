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

#include <windows.h>
#include "base.h"
#include "basetsd.h"

void NtDisplayString(PUNICODE_STRING string)
{
    __asm__ __volatile__(
        "movl %0, %%eax\n\t"    // Load syscall number for NtDisplayString (0x25) into EAX
        "movl %1, %%ebx\n\t"    // Load the pointer to the string into EBX
        "int $0x2E\n\t"         // Trigger the syscall interrupt
        :
        : "r"(0x002e), "r"(string)
        : "eax", "ebx"
    );
}

int NtOpenFile(
    PHANDLE FileHandle,
    int DesiredAccess,
    POBJECT_ATTRIBUTES ObjectAttributes,
    PVOID IoStatusBlock,
    ULONG ShareAccess,
    ULONG OpenOptions
)
{
    int syscallResult = 0;

    asm volatile(
        "mov $0x004f, %%eax\n"
        "mov %1, %%edx\n"
        "int $0x2e\n"
        "mov %%eax, %0\n"
        : "=r" (syscallResult)
        : "r" (ObjectAttributes)
        : "eax", "edx"
    );

    return syscallResult;
}

NTSTATUS NtAcceptConnectPort(
    PHANDLE PortHandle,
    PVOID PortContext,
    PPORT_MESSAGE ConnectionRequest,
    BOOLEAN AcceptConnection,
    PPORT_SECTION_WRITE* WriteSectionOut,
    PPORT_SECTION_READ* ReadSectionOut
)
{
    NTSTATUS status = STATUS_UNSUCCESSFUL;
    PPORT_OBJECT serverPort;
    PCONNECTION_REQUEST pendingReq;
    LOCK_PORT(serverPort);

    pendingReq = LookupPendingRequestLocked(serverPort, ConnectionRequest->ConnectionId);
    if (!pendingReq) {
        UNLOCK_PORT(serverPort);
        return STATUS_INVALID_PARAMETER;
    }

    if (!AcceptConnection) {
        RemovePendingRequestLocked(serverPort, pendingReq);
        pendingReq->state = CONNECTION_REFUSED;
        WakeClientOfRequest(pendingReq, STATUS_CONNECTION_REFUSED);
        DerefAndFreePendingRequest(pendingReq);
        UNLOCK_PORT(serverPort);
        return STATUS_CONNECTION_REFUSED;
    }

    status = PerformSecurityAcceptChecks(serverPort, pendingReq);
    if (!NT_SUCCESS(status)) {
        RemovePendingRequestLocked(serverPort, pendingReq);
        WakeClientOfRequest(pendingReq, status);
        DerefAndFreePendingRequest(pendingReq);
        UNLOCK_PORT(serverPort);
        return status;
    }

    connection = CreateConnectionLocked(serverPort, pendingReq);
    if (!connection) {
        RemovePendingRequestLocked(serverPort, pendingReq);
        WakeClientOfRequest(pendingReq, STATUS_INSUFFICIENT_RESOURCES);
        UNLOCK_PORT(serverPort);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    if (pendingReq->RequestedSection) {
        status = SetupSectionMappings(connection, pendingReq, WriteSectionOut, ReadSectionOut);
        if (!NT_SUCCESS(status)) {
            DestroyConnection(connection);
            RemovePendingRequestLocked(serverPort, pendingReq);
            WakeClientOfRequest(pendingReq, status);
            UNLOCK_PORT(serverPort);
            return status;
        }
    }

    connection->ServerContext = PortContext;

    LinkConnectionIntoPortLocked(serverPort, connection);
    RemovePendingRequestLocked(serverPort, pendingReq);
    pendingReq->state = CONNECTION_ACCEPTED;
    WakeClientOfRequest(pendingReq, STATUS_SUCCESS);

    if (PortHandle) {
        *PortHandle = CreateHandleForConnection(currentProcess, connection, DESIRED_ACCESS);
        if (!IS_VALID_HANDLE(*PortHandle)) {
            UnlinkConnection(serverPort, connection);
            DestroyConnection(connection);
            UNLOCK_PORT(serverPort);
            return STATUS_INSUFFICIENT_RESOURCES;
        }
    }

    UNLOCK_PORT(serverPort);
    return STATUS_SUCCESS;
}

#endif
