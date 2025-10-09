/* ke/port/accept.c
   Kernel-side implementation of NtAcceptConnectPort (LPC accept).
*/

#include <stddef.h>
#include "port.h"
#include "../base.h"
#include "../basetsd.h"
#include "../task/process.h"
#include "../ntdll.h"

static void* default_port = (void*)0x1000;

PPORT_OBJECT PortObjectFromHandle(PHANDLE h)
{
    (void)h;
    return (PPORT_OBJECT)default_port;
}

int IS_VALID_HANDLE(PHANDLE h)
{
    return (h != (PHANDLE)0 && h != NULL);
}

void LOCK_PORT(PPORT_OBJECT Port)   { (void)Port; }
void UNLOCK_PORT(PPORT_OBJECT Port) { (void)Port; }

static CONNECTION_REQUEST _stub_pending_req = {
    .ConnectionId = 1,
    .RequestedSection = NULL,
    .state = 0
};

PCONNECTION_REQUEST LookupPendingRequestLocked(PPORT_OBJECT Port, ULONG ConnectionId)
{
    (void)Port;
    (void)ConnectionId;
    return &_stub_pending_req;
}

void RemovePendingRequestLocked(PPORT_OBJECT Port, PCONNECTION_REQUEST Req)
{
    (void)Port; (void)Req;
}

void WakeClientOfRequest(PCONNECTION_REQUEST Req, NTSTATUS Status)
{
    (void)Req; (void)Status;
}

void DerefAndFreePendingRequest(PCONNECTION_REQUEST Req)
{
    (void)Req;
}

NTSTATUS PerformSecurityAcceptChecks(PPORT_OBJECT Port, PCONNECTION_REQUEST Req)
{
    (void)Port; (void)Req;
    return STATUS_SUCCESS;
}

static CONNECTION _stub_connection = {
    .ServerContext = NULL,
    .ClientContext = NULL
};

PCONNECTION CreateConnectionLocked(PPORT_OBJECT Port, PCONNECTION_REQUEST Req)
{
    (void)Port; (void)Req;
    return &_stub_connection;
}

void DestroyConnection(PCONNECTION Conn)
{
    (void)Conn;
}

NTSTATUS SetupSectionMappings(PCONNECTION Conn, PCONNECTION_REQUEST Req, PVOID *WriteOut, PVOID *ReadOut)
{
    (void)Conn; (void)Req;
    if (WriteOut) *WriteOut = NULL;
    if (ReadOut)  *ReadOut  = NULL;
    return STATUS_SUCCESS;
}

void LinkConnectionIntoPortLocked(PPORT_OBJECT Port, PCONNECTION Conn)
{
    (void)Port; (void)Conn;
}

void UnlinkConnection(PPORT_OBJECT Port, PCONNECTION Conn)
{
    (void)Port; (void)Conn;
}

PHANDLE CreateHandleForConnection(void *Process, PCONNECTION Conn, ULONG DesiredAccess)
{
    (void)Process; (void)Conn; (void)DesiredAccess;
    return (PHANDLE)(uintptr_t)0x1001;
}

void *GetCurrentProcess(void)
{
    return NULL;
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
