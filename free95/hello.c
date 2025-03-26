/*++

Free95

You may only use this code if you agree to the terms of the Free95 Source Code License agreement (GNU GPL v3) (see LICENSE).
If you do not agree to the terms, do not use the code.


Module Name:

    hello.c

Abstract:

    A sample hello world program.

--*/

#include "base/txos/ke/base.h"
#include "base/txos/ke/ntdll.h" // TODO: Load the ntdll.dll library instead of including this.

int strlen(const char* ptr)
{
    int i = 0;
    while(*ptr != 0)
    {
        i++;
        ptr += 1;
    }

    return i;
}

void InitializeUnicodeString(PUNICODE_STRING UnicodeString, const CHAR* SourceString)
{
    UnicodeString->Buffer = (PCHAR)SourceString;
    UnicodeString->Length = (UCHAR)(strlen(SourceString) * sizeof(CHAR));
    UnicodeString->MaximumLength = UnicodeString->Length + sizeof(CHAR);
}

void PrintString(const char *msg)
{
    UNICODE_STRING stri;
    InitializeUnicodeString(&stri, msg);
    NtDisplayString(&stri);
}

void _start()
{
	CHAR hello[] = "Hello, NT!\n";

   	PrintString(hello);
}
