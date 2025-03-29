/*++

Free95

You may only use this code if you agree to the terms of the Free95 Source Code License agreement (GNU GPL v3) (see LICENSE).
If you do not agree to the terms, do not use the code.


	PROJECT: Free95 Userspace Components
	FILE: print.h
	DESCRIPTION: Simple NtDisplayString Library, adapted from hello.c
	AUTHORS: @KapTheGuy, @realblobii

--*/

#ifndef PRINT_H
#define PRINT_H

typedef struct _UNICODE_STRING
{
    unsigned short Length;
    unsigned short MaximumLength;
    char* Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

int NtDisplayString(PUNICODE_STRING String);
PUNICODE_STRING to_punicode(char fnamebuf[]);

#endif