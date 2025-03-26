/*++

Free95 20x/TX Kernel

You may only use this code if you agree to the terms of the Free95 Source Code License agreement (GNU GPL v3) (see LICENSE).
If you do not agree to the terms, do not use the code.


Module Name:

    idt.c

Abstract:

    This module implements the Interrupt Descriptor Table code.

--*/

#include "idt.h"
#include "../config.h"
#include "../../init/kernel.h"
#include "../memory/memory.h"
#include "../io/io.h"
#include "../bug.h"
#include "../../init/loader.h"

#define RING3 0xEE

struct idt_desc idt_descriptors[FREE95_TOTAL_INTERRUPTS];
struct idtr_desc idtr_descriptor;

extern void idt_load(struct idtr_desc* ptr);
extern void int21h();
extern void int2eh();
extern void no_interrupt();

char* strcat(char* dest, const char* src)
{
    char* dest_ptr = dest;

    // Move dest_ptr to the end of the destination string
    while (*dest_ptr != '\0')
    {
        dest_ptr++;
    }

    // Append characters from src to dest
    while (*src != '\0')
    {
        *dest_ptr = *src;
        dest_ptr++;
        src++;
    }

    // Null-terminate the result
    *dest_ptr = '\0';

    return dest;
}

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64

static char kb[] =
    {
        0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
        '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
        0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
        0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
        '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

static char input_buffer[256]; // Global input buffer
static int input_pos = 0;                   // Current position in the buffer

void int21h_handler()
{
    uint8_t scancode = insb(KEYBOARD_DATA_PORT);
    char key = kb[scancode];

    if (key == '\b' && input_pos > 0) // Handle backspace
    {
        input_pos--;
        input_buffer[input_pos] = '\0';
        terminal_writechar('\b', 15);
    }
    else if (key == '\n') // Handle Enter key
    {
        input_buffer[input_pos] = '\0'; // Null-terminate the string

        SetExecBuffer(input_buffer);

//         print("\nExecuting: ");
//         print(input_buffer);
//         print("\n");
//
//         LdrLoadPe(input_buffer); // Load the program

        input_pos = 0; // Reset the buffer position

        //print("\n> ");
    }
    else if (key != 0 && input_pos < sizeof(input_buffer) - 1) // Append valid characters
    {
        input_buffer[input_pos++] = key;
        DbgPutc(key); // Echo the character
        terminal_writechar(key, 15);
    }

    outb(0x20, 0x20); // Send End of Interrupt (EOI) signal
}

int NtGetInputBufferSyscall(char *buffer)
{
    int buffer_size = 256;

    if (buffer == NULL || buffer_size <= 0)
    {
        return 445;
    }

    int copy_size = input_pos < buffer_size ? input_pos : buffer_size - 1;
    memcpy(buffer, input_buffer, copy_size);
    buffer[copy_size] = '\0'; // Null-terminate the string

    return STATUS_SUCCESS;
}

int NtOpenFileSyscall(
    PHANDLE FileHandle,
    int DesiredAccess,
    POBJECT_ATTRIBUTES ObjectAttributes,
    PVOID IoStatusBlock,
    ULONG ShareAccess,
    ULONG OpenOptions
)
{
    //FileHandle = (PHANDLE)fopen(ObjectAttributes->ObjectName->Buffer, "r");

    DbgPrint("File Name: %s", ObjectAttributes->ObjectName->Buffer);
    return fopen(ObjectAttributes->ObjectName->Buffer, "r");

//     if (!FileHandle)
//     {
//         return STATUS_OBJECT_NAME_NOT_FOUND;
//     }
//     else
//     {
//         return STATUS_SUCCESS;
//     }
}

int NtReadFileSyscall(
    int FileHandle,
    HANDLE Event,
    PVOID ApcRoutine,
    PVOID ApcContext,
    PVOID IoStatusBlock,
    PVOID Buffer,
    ULONG Length,
    ULONG ByteOffset,
    PULONG Key
)
{
    //return fread(Buffer, Length - 1, 1, FileHandle);
    return STATUS_SUCCESS;
}

int NtDisplayStringSyscall(PUNICODE_STRING String)
{
    print((char*)String->Buffer);
    DbgPrint((char*)String->Buffer);
    return STATUS_SUCCESS;
}

void *syscall_dispatcher(uint32_t syscall_number, uint32_t arg1, uint32_t arg2, uint32_t arg3,
                         uint32_t arg4, uint32_t arg5, uint32_t arg6, uint32_t arg7, uint32_t arg8, uint32_t arg9)
{
    void* result = (void*)STATUS_INVALID_SYSTEM_SERVICE;

    switch (syscall_number)
    {
        /* NOTE: Syscalls 1 and 2 are tests, not accurate to the real NT Syscall(s) 1 and 2. */
        case 0x01:
            if (arg1 == 5)
            {
                print("Example syscall!\n");
            }
            else
            {
                print("Test syscall!\n");
            }
            //result = 1;
            break;

        case 0x02:
            //LdrLoadPe((char*)arg1);
            result = (void*)LdrLoadPe((char*)arg1);;
            break;

        case 0x03:
            result = (void*)NtGetInputBufferSyscall((char *)arg1);
            break;

        case 0x002e:
            // Only uncomment while debugging this syscall
            // print("NtDisplayString() syscall called\n");
            result = (void*)NtDisplayStringSyscall((PUNICODE_STRING)arg1);
            break;

        case 0x004f:
            // Only uncomment while debugging this syscall
            // print("NtOpenFile() syscall called\n");
            result = (void*)NtOpenFileSyscall(0, 0, (POBJECT_ATTRIBUTES)arg3, 0, 0, 0);

            DbgPrint("NtOpenFile(): result is %d\n", result);

            break;

        default:
            print("Syscall failed with NTSTATUS: 0xC000001C\n");
            DbgPrint("\nSyscall failed with NTSTATUS: 0xC000001C\n");
            break;
    }

    return (void*)result;
}

void syscall_handler()
{
    uint32_t syscall_number;
    uint32_t arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9;

    asm volatile("mov %%eax, %0" : "=r"(syscall_number));
    asm volatile("mov %%ebx, %0" : "=r"(arg1));
    asm volatile("mov %%ecx, %0" : "=r"(arg2));
    asm volatile("mov %%edx, %0" : "=r"(arg3));

    asm volatile("mov 4(%%esp), %0" : "=r"(arg4));
    asm volatile("mov 8(%%esp), %0" : "=r"(arg5));
    asm volatile("mov 12(%%esp), %0" : "=r"(arg6));
    asm volatile("mov 16(%%esp), %0" : "=r"(arg7));
    asm volatile("mov 20(%%esp), %0" : "=r"(arg8));
    asm volatile("mov 24(%%esp), %0" : "=r"(arg9));

    syscall_dispatcher(syscall_number, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
}


void no_interrupt_handler()
{
	outb(0x20, 0x20);
}

void idt_zero()
{
    KeBugCheck(KMODE_DIV_ZERO);
}

void idt_pf()
{
    KeBugCheck(KMODE_PAGE_FAULT);
}

void idt_gpf()
{
    KeBugCheck(KMODE_GPF);
}

void idt_df()
{
    KeBugCheck(KMODE_DF);
}

void idt_snp()
{
    KeBugCheck(KMODE_INVOP);
}

void idt_inv()
{
    KeBugCheck(KMODE_SNP);
}

void idt_imp()
{
	KeBugCheck(IMP_CRASH);
}

void idt_set(int interrupt_no, void* address)
{
    struct idt_desc* desc = &idt_descriptors[interrupt_no];
    desc->offset_1 = (uint32_t) address & 0x0000ffff;
    desc->selector = KERNEL_CODE_SELECTOR;
    desc->zero = 0x00;
    desc->type_attr = RING3;
    desc->offset_2 = (uint32_t) address >> 16;
}

void idt_init()
{
    memset(idt_descriptors, 0, sizeof(idt_descriptors));
    idtr_descriptor.limit = sizeof(idt_descriptors) -1;
    idtr_descriptor.base = (uint32_t) idt_descriptors;

	for (int i = 0; i < FREE95_TOTAL_INTERRUPTS; i++)
	{
		idt_set(i, no_interrupt);
	}

    idt_set(0x2E, int2eh);

    idt_set(0, idt_zero);
    idt_set(14, idt_pf);
    idt_set(13, idt_gpf);
    idt_set(6, idt_inv);
    idt_set(8, idt_df);
    idt_set(11, idt_snp);

    idt_set(0x21, int21h);

    // Load the interrupt descriptor table
    idt_load(&idtr_descriptor);
}
