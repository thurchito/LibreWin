/*++

Free95

You may only use this code if you agree to the terms of the Free95 Source Code License agreement (GNU GPL v3) (see LICENSE).
If you do not agree to the terms, do not use the code.


Module Name:

    program.c

Abstract:

    A sample program.

--*/

void _start()
{
	asm volatile (
	        "movl $0x01, %%eax\n\t"
	        "movl $5, %%ebx\n\t"
	        "int $0x2e\n\t"
	        :
	        :
	        : "%eax", "%ebx"
	);

	asm volatile (
	        "movl $0x01, %%eax\n\t"
	        "movl $1, %%ebx\n\t"
	        "int $0x2e\n\t"
	        :
	        :
	        : "%eax", "%ebx"
	);
}
