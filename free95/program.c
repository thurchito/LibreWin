/*++

Free95

You may only use this code if you agree to the terms of the Free95 Source Code License agreement (GNU GPL v3) (see LICENSE).
If you do not agree to the terms, do not use the code.


	PROJECT: Free95 Userspace Components
	FILE: program.c
	DESCRIPTION: A simple Native NT Application to determine if a file exists or not.
	AUTHOR: @KapTheGuy

--*/

typedef struct _UNICODE_STRING
{
    unsigned short Length;
    unsigned short MaximumLength;
    char* Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

typedef struct _OBJECT_ATTRIBUTES
{
    unsigned long Length;
    void* RootDirectory;
    PUNICODE_STRING ObjectName;
    unsigned long Attributes;
    void* SecurityDescriptor;
    void* SecurityQualityOfService;
} OBJECT_ATTRIBUTES, *POBJECT_ATTRIBUTES;

int NtDisplayString(PUNICODE_STRING String);

int NtOpenFile(
    void** FileHandle,
    int DesiredAccess,
    POBJECT_ATTRIBUTES ObjectAttributes,
    void* IoStatusBlock,
    unsigned long ShareAccess,
    unsigned long OpenOptions
);

void _start()
{
	char fnamebuf[] = "0:/test.txt";

	PUNICODE_STRING fname = 0;
	fname->Length = sizeof(fnamebuf) - sizeof(char);
	fname->MaximumLength = sizeof(fnamebuf);
	fname->Buffer = fnamebuf;

	POBJECT_ATTRIBUTES objAttrs;
    //InitializeObjectAttributes(&objAttrs, &fname, 0, 0, 0);
	objAttrs->ObjectName = fname;

	int syscallResult = 0;

	asm volatile(
        "mov $0x004f, %%eax\n"
        "mov %1, %%edx\n"
        "int $0x2e\n"
        "mov %%eax, %0\n"
        : "=r" (syscallResult)
        : "r" (objAttrs)
        : "eax", "edx"
    );

	NtOpenFile(0, 0, objAttrs, 0, 0, 0);

	NtDisplayString(fname);

	if (syscallResult)
	{
		char text[] = " File exists\n";

		PUNICODE_STRING t = 0;
		t->Length = sizeof(text) - sizeof(char);
		t->MaximumLength = sizeof(text);
		t->Buffer = text;

		NtDisplayString(t);
	}
	else
	{
		char text1[] = " File does not exist\n";

		PUNICODE_STRING t1 = 0;
		t1->Length = sizeof(text1) - sizeof(char);
		t1->MaximumLength = sizeof(text1);
		t1->Buffer = text1;

		NtDisplayString(t1);
	}
}

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

int NtOpenFile(
    void** FileHandle,
    int DesiredAccess,
    POBJECT_ATTRIBUTES ObjectAttributes,
    void* IoStatusBlock,
    unsigned long ShareAccess,
    unsigned long OpenOptions
)
{
	int r = 0;

	asm volatile(
        "mov $0x004f, %%eax\n"
        "mov %1, %%edx\n"
        "int $0x2e\n"
        "mov %%eax, %0\n"
        : "=r" (r)
        : "r" (ObjectAttributes)
        : "eax", "edx"
    );

	return r;
}
