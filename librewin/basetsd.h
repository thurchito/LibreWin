#define WINVER 0x0A00
#define _WIN32_WINNT 0x0500
#define _WIN32_WINNT_WIN2K 0x0500
#define _MSC_VER 1200
#define WINAPI __stdcall
#define APIENTRY WINAPI
#define CALLBACK __stdcall
#define CONST const
#define VOID void

#if !defined(_M_IX86)
 typedef unsigned __int64 ULONGLONG;
#else
 typedef double ULONGLONG;
#endif

#if defined(_WIN64)
 typedef unsigned __int64 ULONG_PTR;
#else
 typedef unsigned long ULONG_PTR;
#endif

#ifdef _WIN64
 typedef int HALF_PTR;
#else
 typedef short HALF_PTR;
#endif

#if defined(_WIN64) 
 typedef __int64 INT_PTR; 
#else 
 typedef int INT_PTR;
#endif

#if !defined(_M_IX86)
 typedef __int64 LONGLONG; 
#else
 typedef double LONGLONG;
#endif

#if defined(_WIN64)
 typedef __int64 LONG_PTR; 
#else
 typedef long LONG_PTR;
#endif

typedef __nullterminated CONST CHAR *LPCSTR;
typedef CONST WCHAR *LPCWSTR;

#ifdef UNICODE
 typedef LPCWSTR LPCTSTR; 
#else
 typedef LPCSTR LPCTSTR;
#endif
typedef WCHAR *LPWSTR;
#ifdef UNICODE
 typedef LPWSTR LPTSTR;
#else
 typedef LPSTR LPTSTR;
#endif

#ifdef UNICODE
 typedef LPCWSTR PCTSTR;
#else
 typedef LPCSTR PCTSTR;
#endif

#ifdef _WIN64
 typedef HALF_PTR *PHALF_PTR;
#else
 typedef HALF_PTR *PHALF_PTR;
#endif

#if defined(_WIN64)
#define POINTER_32 __ptr32
#else
#define POINTER_32
#endif

#if (_MSC_VER >= 1300)
#define POINTER_64 __ptr64
#else
#define POINTER_64
#endif

#define POINTER_SIGNED __sptr
#define POINTER_UNSIGNED __uptr
typedef wchar_t WCHAR;

#ifdef UNICODE
 typedef WCHAR TBYTE;
#else
 typedef unsigned char TBYTE;
#endif

#ifdef UNICODE
 typedef WCHAR TCHAR;
#else
 typedef char TCHAR;
#endif

#ifdef _WIN64
 typedef unsigned int UHALF_PTR;
#else
 typedef unsigned short UHALF_PTR;
#endif

#ifdef _WIN64
 typedef UHALF_PTR *PUHALF_PTR;
#else
 typedef UHALF_PTR *PUHALF_PTR;
#endif

#if defined(_WIN64)
 typedef unsigned __int64 UINT_PTR;
#else
 typedef unsigned int UINT_PTR;
#endif

#if !defined(_M_IX86)
 typedef unsigned __int64 ULONGLONG;
#else
 typedef double ULONGLONG;
#endif

#if defined(_WIN64)
 typedef unsigned __int64 ULONG_PTR;
#else
 typedef unsigned long ULONG_PTR;
#endif

#ifdef UNICODE
 typedef WCHAR TBYTE;
#else
 typedef unsigned char TBYTE;
#endif

#ifdef UNICODE
 typedef WCHAR TCHAR;
#else
 typedef char TCHAR;
#endif

typedef unsigned short WORD;
typedef WORD ATOM;
typedef int BOOL;
typedef unsigned char BYTE;
typedef BYTE BOOLEAN;

typedef char CCHAR;
typedef char CHAR;
typedef unsigned long DWORD;
typedef DWORD COLORREF;
typedef unsigned __int64 DWORDLONG;

typedef unsigned long ULONG;
typedef ULONG_PTR DWORD_PTR;
typedef unsigned int DWORD32;
typedef unsigned __int64 DWORD64;
typedef float FLOAT;

typedef void *PVOID;
typedef PVOID HANDLE;
typedef HANDLE HACCEL;
typedef HANDLE HBITMAP;
typedef HANDLE HBRUSH;

typedef HANDLE HCOLORSPACE;
typedef HANDLE HCONV;
typedef HANDLE HCONVLIST;
typedef HANDLE HICON;
typedef HICON HCURSOR;

typedef HANDLE HDC;
typedef HANDLE HDDEDATA;
typedef HANDLE HDESK;
typedef HANDLE HDROP;
typedef HANDLE HDWP;

typedef HANDLE HENHMETAFILE;
typedef int HFILE;
typedef HANDLE HFONT;
typedef HANDLE HGDIOBJ;
typedef HANDLE HGLOBAL;

typedef HANDLE HHOOK;
typedef HANDLE HINSTANCE;
typedef HANDLE HKEY;
typedef HANDLE HKL;
typedef HANDLE HLOCAL;

typedef HANDLE HMENU;
typedef HANDLE HMETAFILE;
typedef HINSTANCE HMODULE;
if(WINVER >= 0x0500) typedef HANDLE HMONITOR;
typedef HANDLE HPALETTE;

typedef HANDLE HPEN;
typedef long LONG;
typedef LONG HRESULT;
typedef HANDLE HRGN;
typedef HANDLE HRSRC;

typedef HANDLE HSZ;
typedef HANDLE WINSTA;
typedef HANDLE HWND;
typedef int INT;
typedef signed char INT8;

typedef signed short INT16;
typedef signed int INT32;
typedef signed __int64 INT64;
typedef unsigned short WORD;
typedef WORD LANGID;

typedef DWORD LCID;
typedef DWORD LCTYPE;
typedef DWORD LGRPID;
typedef signed int LONG32;
typedef __int64 LONG64;

typedef LONG_PTR LPARAM;
typedef BOOL far *LPBOOL;
typedef BYTE far *LPBYTE;
typedef DWORD *LPCOLORREF;
typedef CONST void *LPCVOID;

typedef DWORD *LPDWORD;
typedef HANDLE *LPHANDLE;
typedef int *LPINT;
typedef long *LPLONG;
typedef CHAR *LPSTR;

typedef void *LPVOID;
typedef WORD *LPWORD;
typedef WCHAR *LPWSTR;
typedef LONG_PTR LRESULT;
typedef BOOL *PBOOL;

typedef BOOLEAN *PBOOLEAN;
typedef BYTE *PBYTE;
typedef CHAR *PCHAR;
typedef CONST CHAR *PCSTR;
typedef CONST WCHAR *PCWSTR;

typedef DWORD *PDWORD;
typedef DWORDLONG *PDWORDLONG;
typedef DWORD_PTR *PDWORD_PTR;
typedef DWORD32 *PDWORD32;
typedef DWORD64 *PDWORD64;

typedef FLOAT *PFLOAT;
typedef HANDLE *PHANDLE;
typedef HKEY *PHKEY;
typedef int *PINT;
typedef INT_PTR *PINT_PTR;

typedef INT8 *PINT8;
typedef INT16 *PINT16;
typedef INT32 *PINT32;
typedef INT64 *PINT64;
typedef PDWORD PLCID;

typedef LONG *PLONG;
typedef LONGLONG *PLONGLONG;
typedef LONG_PTR *PLONG_PTR;
typedef LONG32 *PLONG32;
typedef LONG64 *PLONG64;

typedef short SHORT;
typedef SHORT *PSHORT;
typedef ULONG_PTR SIZE_T;
typedef SIZE_T *PSIZE_T;
typedef LONG_PTR SSIZE_T;

typedef SSIZE_T *PSSIZE_T;
typedef CHAR *PSTR;
typedef TBYTE *PTBYTE;
typedef unsigned char UCHAR;
typedef UCHAR *PUCHAR;

typedef unsigned int UINT;
typedef UINT *PUINT;
typedef UINT_PTR *PUINT_PTR;
typedef unsigned char UINT8;
typedef UINT8 *PUINT8;

typedef unsigned short UINT16;
typedef UINT16 *PUINT16;
typedef unsigned int UINT32;
typedef UINT32 *PUINT32;
typedef unsigned __int64 UINT64;

typedef UINT64 *PUINT64;
typedef unsigned long ULONG;
typedef ULONG *PULONG;
typedef ULONGLONG *PULONGLONG;
typedef ULONG_PTR *PULONG_PTR;

typedef unsigned int ULONG32;
typedef ULONG32 *PULONG32;
typedef unsigned __int64 ULONG64;
typedef ULONG64 *PULONG64;
typedef unsigned short USHORT;

typedef USHORT *PUSHORT;
typedef void *PVOID;
typedef WCHAR *PWCHAR;
typedef WORD *PWORD;
typedef WCHAR *PWSTR;

typedef unsigned __int64 QWORD;
typedef HANDLE SC_HANDLE;
typedef LPVOID SC_LOCK;
typedef HANDLE SERVICE_STATUS_HANDLE;
typedef short SHORT;
typedef ULONG_PTR SIZE_T;
typedef LONG_PTR SSIZE_T;

typedef struct _UNICODE_STRING {
  USHORT  Length;
  USHORT  MaximumLength;
PWSTR  Buffer;
} UNICODE_STRING;

typedef UNICODE_STRING *PUNICODE_STRING;
typedef const UNICODE_STRING *PCUNICODE_STRING;
typedef LONGLONG USN;
typedef UINT_PTR WPARAM;
