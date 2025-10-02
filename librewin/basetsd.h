#define APIENTRY WINAPI
#define CALLBACK __stdcall
#define CONST const
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

typedef unsigned int ULONG32;
typedef unsigned __int64 ULONG64;

typedef struct _UNICODE_STRING
{
    unsigned short Length;
    unsigned short MaximumLength;
    char* Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

typedef unsigned short USHORT;
typedef LONGLONG USN;
