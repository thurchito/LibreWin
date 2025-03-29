/*++

Free95

You may only use this code if you agree to the terms of the Free95 Source Code License agreement (GNU GPL v3) (see LICENSE).
If you do not agree to the terms, do not use the code.


	PROJECT: Free95 Userspace Components
	FILE: buildno.c
	DESCRIPTION: A Version Displayer Native NT Application.
	AUTHOR: @KapTheGuy

--*/

typedef struct _UNICODE_STRING
{
    unsigned short Length;
    unsigned short MaximumLength;
    char* Buffer;
} UNICODE_STRING, *PUNICODE_STRING;


int NtDisplayString(PUNICODE_STRING String);
void RtlCreateUnicodeStringFromAsciiz(PUNICODE_STRING UnicodeString, const char* SourceString);
void RtlCliDisplayString(const char *msg);
void RtlCreateStringFromUint(unsigned int value, char *str);

void _start()
{
    unsigned int nBuild = 5400;
    unsigned int nBeta = 2;
    unsigned int nMajorVersion = 0;
    unsigned int nMiddleVersion = 3;
    unsigned int nMinorVersion = 0;

    char szMajorVersion[256];
    char szMiddleVersion[256];
    char szMinorVersion[256];
    char szBeta[256];
    char szBuild[256];

    RtlCreateStringFromUint(nMajorVersion, szMajorVersion);
    RtlCreateStringFromUint(nMiddleVersion, szMiddleVersion);
    RtlCreateStringFromUint(nMinorVersion, szMinorVersion);
    RtlCreateStringFromUint(nBeta, szBeta);
    RtlCreateStringFromUint(nBuild, szBuild);

	RtlCliDisplayString("Versoft Free95\nVersion ");
    RtlCliDisplayString(szMajorVersion);
    RtlCliDisplayString(".");
    RtlCliDisplayString(szMiddleVersion);
    RtlCliDisplayString(".");
    RtlCliDisplayString(szMinorVersion);
    RtlCliDisplayString(" Beta ");
    RtlCliDisplayString(szBeta);
    RtlCliDisplayString(" (Build ");
    RtlCliDisplayString(szBuild);
    RtlCliDisplayString(")\n");
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

void RtlCreateUnicodeStringFromAsciiz(PUNICODE_STRING UnicodeString, const char* SourceString)
{
    UnicodeString->Buffer = (char*)SourceString;
    UnicodeString->Length = (unsigned char)(strlen(SourceString) * sizeof(char));
    UnicodeString->MaximumLength = UnicodeString->Length + sizeof(char);
}

void RtlCliDisplayString(const char *msg)
{
    UNICODE_STRING stri;
    RtlCreateUnicodeStringFromAsciiz(&stri, msg);
    NtDisplayString(&stri);
}

void RtlCreateStringFromUint(unsigned int value, char *str)
{
    int index = 0;

    // Handle 0 explicitly, since the loop below won't run for value = 0
    if (value == 0) {
        str[index++] = '0';
        str[index] = '\0'; // Null-terminate the string
        return;
    }

    // Process each digit and store it in reverse order
    while (value > 0) {
        str[index++] = '0' + (value % 10); // Get the last digit and convert to character
        value /= 10; // Remove the last digit
    }

    // Null-terminate the string
    str[index] = '\0';

    // Reverse the string to get the correct order
    for (int i = 0, j = index - 1; i < j; i++, j--) {
        char temp = str[i];
        str[i] = str[j];
        str[j] = temp;
    }
}
