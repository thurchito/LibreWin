/* ke/port/port.h
   Port API declarations
*/

#ifndef KE_PORT_PORT_H
#define KE_PORT_PORT_H

#include "../base.h"
#include "../basetsd.h"

typedef struct _PORT_OBJECT PORT_OBJECT, *PPORT_OBJECT;
typedef struct _CONNECTION_REQUEST CONNECTION_REQUEST, *PCONNECTION_REQUEST;
typedef struct _PORT_MESSAGE PORT_MESSAGE, *PPORT_MESSAGE;
typedef struct _CONNECTION CONNECTION, *PCONNECTION;
typedef struct _PORT_SECTION_WRITE PORT_SECTION_WRITE, *PPORT_SECTION_WRITE;
typedef struct _PORT_SECTION_READ  PORT_SECTION_READ,  *PPORT_SECTION_READ;

#define CONNECTION_PENDING   0
#define CONNECTION_ACCEPTED  1
#define CONNECTION_REFUSED   2

void LOCK_PORT(PPORT_OBJECT Port);
void UNLOCK_PORT(PPORT_OBJECT Port);

PCONNECTION_REQUEST LookupPendingRequestLocked(PPORT_OBJECT Port, ULONG ConnectionId);
void RemovePendingRequestLocked(PPORT_OBJECT Port, PCONNECTION_REQUEST Req);
void WakeClientOfRequest(PCONNECTION_REQUEST Req, NTSTATUS Status);
void DerefAndFreePendingRequest(PCONNECTION_REQUEST Req);

NTSTATUS PerformSecurityAcceptChecks(PPORT_OBJECT Port, PCONNECTION_REQUEST Req);
PCONNECTION CreateConnectionLocked(PPORT_OBJECT Port, PCONNECTION_REQUEST Req);
void DestroyConnection(PCONNECTION Conn);
NTSTATUS SetupSectionMappings(PCONNECTION Conn, PCONNECTION_REQUEST Req, PVOID *WriteOut, PVOID *ReadOut);
void LinkConnectionIntoPortLocked(PPORT_OBJECT Port, PCONNECTION Conn);
void UnlinkConnection(PPORT_OBJECT Port, PCONNECTION Conn);

PHANDLE CreateHandleForConnection(void *Process, PCONNECTION Conn, ULONG DesiredAccess);
int IS_VALID_HANDLE(PHANDLE h);
void *GetCurrentProcess(void);

PPORT_OBJECT PortObjectFromHandle(PHANDLE h);

#endif /* KE_PORT_PORT_H */
