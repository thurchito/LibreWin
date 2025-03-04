SOURCES= boot.o kernel.o vga.o descriptorTables.o isr.o dt.o interrupt.o user.o string.o kb.o pmm.o kheap.o win32.o hello.o freever.o reshell.o tutorial.o
CFLAGS= -m32 -std=gnu99 -ffreestanding -O2 -Wall -Wextra -fno-pie
LDFLAGS= -m elf_i386 -T ../linker.ld -nostdlib
ASFLAGS=-felf32

all: $(SOURCES) link run

link:
	ld $(LDFLAGS) -o nxoskrnl.nef $(SOURCES)

.s.o:
	nasm $(ASFLAGS) $< 

.c.o:
	gcc -c $(CFLAGS) $< -o $@

run:
	qemu-system-i386 -kernel nxoskrnl.nef

clean:
	rm *.o
	rm *.nef
