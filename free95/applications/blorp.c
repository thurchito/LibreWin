/*++

Free95

You may only use this code if you agree to the terms of the Free95 Source Code License agreement (GNU GPL v3) (see LICENSE).
If you do not agree to the terms, do not use the code.


	PROJECT: Free95 Userspace Components
	FILE: blorp.c
	DESCRIPTION: Displays silly alien noises in the native shell. Testing app for print lib
	AUTHOR: @realblobii

--*/
#include "appinclude/print.h"


void _start()
{
	char msg[] = "gnarp gnap! gleeb! blorp!\n";

	PUNICODE_STRING puni_msg = to_punicode(msg);

	NtDisplayString(puni_msg);
}
