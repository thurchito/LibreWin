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
#include <memory.h>
#include <io.h>

struct idt_desc idt_descriptors[FREE95_TOTAL_INTERRUPTS];
struct idtr_desc idtr_descriptor;

extern void idt_load(struct idtr_desc* ptr);
extern void int21h();
extern void no_interrupt();

void int21h_handler()
{
	print("\nKey pressed!\n");
	outb(0x20, 0x20);
}

void no_interrupt_handler()
{
	outb(0x20, 0x20);
}

void KeBugCheck(unsigned long BugCheckCode)
{
	print("\n\n!?\n     Something went wrong.\n     ERROR CODE:\n");

	if (BugCheckCode == DIVISION_BY_ZERO)
	{
		print("     DIV_BY_ZERO\n");
	}
	else
	{
		print("     UNKNOWN_SYSERR\n");
	}
	
	while(1);
}

void idt_zero()
{
    //print("Divide by zero error\n");
    KeBugCheck(DIVISION_BY_ZERO);
}

void idt_set(int interrupt_no, void* address, uint8_t permission_level)
{
    struct idt_desc* desc = &idt_descriptors[interrupt_no];
    desc->offset_1 = (uint32_t) address & 0x0000ffff;
    desc->selector = KERNEL_CODE_SELECTOR;
    desc->zero = 0x00;
    desc->type_attr = permission_level;
    desc->offset_2 = (uint32_t) address >> 16;
}

#define RING0 0x8E
#define RING3 0xEE

void idt_init()
{
    memset(idt_descriptors, 0, sizeof(idt_descriptors));
    idtr_descriptor.limit = sizeof(idt_descriptors) -1;
    idtr_descriptor.base = (uint32_t) idt_descriptors;

	for (int i = 0; i < FREE95_TOTAL_INTERRUPTS; i++)
	{
		idt_set(i, no_interrupt, RING0);
	}

    idt_set(0, idt_zero, RING0);
	idt_set(0x21, int21h, RING0); // IRQ 1, Keyboard

    // Load the interrupt descriptor table
    idt_load(&idtr_descriptor);
}
