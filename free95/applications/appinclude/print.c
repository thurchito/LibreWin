/*++

Free95

You may only use this code if you agree to the terms of the Free95 Source Code License agreement (GNU GPL v3) (see LICENSE).
If you do not agree to the terms, do not use the code.


	PROJECT: Free95 Userspace Components
	FILE: print.c
	DESCRIPTION: Simple NtDisplayString Library, adapted from hello.c
	AUTHORS: @KapTheGuy, @realblobii

--*/


#include "print.h"

int NtDisplayString(PUNICODE_STRING String)
{
	asm volatile (
					"movl $0x002e, %%eax\n\t"
					"movl %0, %%ebx\n\t"
					"int $0x2e\n\t"
					:
					: "r"(String)
					: "%eax", "%ebx"
			);

    return 0;

}
PUNICODE_STRING to_punicode(char fnamebuf[]){
    PUNICODE_STRING fname = 0;
	fname->Length = sizeof(fnamebuf) - sizeof(char);
	fname->MaximumLength = sizeof(fnamebuf);
	fname->Buffer = fnamebuf;
    return fname;
}