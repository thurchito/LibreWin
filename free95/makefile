# Licensed under GNU GPL 3.0
CC  := clang
CXX := clang++

# x86/32 = x86
# x86/64 = x64
# arm/32 = arm
# arm/64 = aarch64
PLATFORM ?= x86/32

SRC := ./src

LDFLAGS := -T ./res/linker.ld -nostdlib

ifeq ($(word 1,$(subst /, ,$(PLATFORM))),x86)
ifeq ($(word 2,$(subst /, ,$(PLATFORM))),32)
	LDFLAGS += -m elf_i386
endif
endif

SOURCES = 
CFLAGS= -m32 -std=gnu99 -ffreestanding -O2 -Wall -Wextra -fno-exceptions
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
