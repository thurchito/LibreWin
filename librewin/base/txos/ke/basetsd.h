#ifndef BASETSD_H
#define BASETSD_H

#define _MSC_VER 1200
#define WINAPI __stdcall
#define APIENTRY WINAPI
#define CALLBACK __stdcall

#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)
#endif

#ifndef STATUS_INFO_LENGTH_MISMATCH
#define STATUS_INFO_LENGTH_MISMATCH ((NTSTATUS)0xC0000004L)
#endif

#define SystemCodeIntegrityInformation 103
#define SystemKernelVaShadowInformation 196

#define CODEINTEGRITY_OPTION_ENABLED                 0x00000001
#define CODEINTEGRITY_OPTION_TESTSIGN                0x00000002
#define CODEINTEGRITY_OPTION_UMCI_ENABLED            0x00000004
#define CODEINTEGRITY_OPTION_UMCI_AUDITMODE_ENABLED  0x00000008
#define CODEINTEGRITY_OPTION_HVCI_KMCI_ENABLED       0x00000400
#define CODEINTEGRITY_OPTION_HVCI_KMCI_AUDITMODE_ENABLED 0x00000800
#define CODEINTEGRITY_OPTION_HVCI_KMCI_STRICTMODE_ENABLED 0x00001000
#define CODEINTEGRITY_OPTION_HVCI_IUM_ENABLED        0x00002000

#define KVA_SHADOW_ENABLED                    0x00000001
#define KVA_SHADOW_USER_GLOBAL                0x00000002
#define KVA_SHADOW_PCID                       0x00000004
#define KVA_SHADOW_INVPCID                    0x00000008
#define KVA_SHADOW_REQUIRED                   0x00000010
#define KVA_SHADOW_REQUIRED_AVAILABLE         0x00000020
#define KVA_INVALID_PTE_BIT_MASK              0x00000FC0
#define KVA_L1D_FLUSH_SUPPORTED               0x00001000
#define KVA_L1TF_MITIGATION_PRESENT           0x00002000
#define CONST const
#define VOID void

#ifndef _BASETSD_INTTYPES_DEFINED
#define _BASETSD_INTTYPES_DEFINED
typedef unsigned long ULONG_PTR;
typedef unsigned long DWORD_PTR;
typedef unsigned long SIZE_T;
typedef signed long   SSIZE_T;
typedef unsigned long UINT_PTR;
typedef long          INT_PTR;
#endif

#ifndef WPARAM
typedef unsigned int WPARAM;
#endif

#ifndef LPARAM
typedef long LPARAM;
#endif

#ifndef LRESULT
typedef long LRESULT;
#endif

typedef UNICODE_STRING *PUNICODE_STRING;
typedef const UNICODE_STRING *PCUNICODE_STRING;
typedef LONGLONG USN;

#ifndef _NTSTATUS_H
#define _NTSTATUS_H

typedef long NTSTATUS;

#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)

#define STATUS_UNSUCCESSFUL              ((NTSTATUS)0xC0000001L)
#define STATUS_CONNECTION_REFUSED        ((NTSTATUS)0xC0000236L)
#define STATUS_INSUFFICIENT_RESOURCES    ((NTSTATUS)0xC000009AL)

#endif // _NTSTATUS_H
