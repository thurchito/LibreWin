#ifndef BASETSD_H
#define BASETSD_H

#define _MSC_VER 1200
#define WINAPI __stdcall
#define APIENTRY WINAPI
#define CALLBACK __stdcall
#define STATUS_SUCCESS              ((NTSTATUS)0x00000000L)
#define STATUS_INFO_LENGTH_MISMATCH ((NTSTATUS)0xC0000004L)

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

typedef struct _UNICODE_STRING {
  USHORT  Length;
  USHORT  MaximumLength;
PWSTR  Buffer;
} UNICODE_STRING;

typedef UNICODE_STRING *PUNICODE_STRING;
typedef const UNICODE_STRING *PCUNICODE_STRING;
typedef LONGLONG USN;
typedef UINT_PTR WPARAM;

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

#endif /* BASETSD_H */
