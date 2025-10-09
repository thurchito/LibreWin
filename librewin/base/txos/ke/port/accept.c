/* ke/port/accept.c
   Kernel-side implementation of NtAcceptConnectPort (LPC accept).
*/

#include "../ke/port/port.h"
#include "../ke/base.h"
#include "../ke/basetsd.h"
#include "../ke/task/process.h"
#include <stddef.h>

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
    PPORT_OBJECT serverPort = NULL;
    PCONNECTION_REQUEST pendingReq = NULL;
    PCONNECTION connection = NULL;

    if (PortHandle != NULL && IS_VALID_HANDLE(*PortHandle)) {
        serverPort = PortObjectFromHandle(*PortHandle);
    } else {

#ifdef GetCurrentProcessListeningPort
        serverPort = GetCurrentProcessListeningPort();
#else
        return STATUS_INVALID_PARAMETER;
#endif
    }

    if (!serverPort) {
        return STATUS_INVALID_PARAMETER;
    }

    LOCK_PORT(serverPort);

    if (!ConnectionRequest) {
        UNLOCK_PORT(serverPort);
        return STATUS_INVALID_PARAMETER;
    }

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
        status = SetupSectionMappings(connection, pendingReq, (PVOID*)WriteSectionOut, (PVOID*)ReadSectionOut);
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
        PHANDLE h = CreateHandleForConnection(GetCurrentProcess(), connection, 0);
        if (!IS_VALID_HANDLE(h)) {
            UnlinkConnection(serverPort, connection);
            DestroyConnection(connection);
            UNLOCK_PORT(serverPort);
            return STATUS_INSUFFICIENT_RESOURCES;
        }
        *PortHandle = h;
    }

    UNLOCK_PORT(serverPort);
    return STATUS_SUCCESS;
}
