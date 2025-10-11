;++
;
; LibreWin 20x/TX Loader
;
; You may only use this code if you agree to the terms of the LibreWin Source Code License agreement (GNU GPL v3) (see LICENSE).
; If you do not agree to the terms, do not use the code.
;
;
; Module Name:
;
;    x86fboot.asm
;
; Abstract:
;
;    This module implements the assembly code necessary to load
;    the 20x/TX Kernel for LibreWin.
;
;--

BITS 16
ORG 0x7c00

; --- Configuration Constants ---
KERNEL_LBA_START   equ 1      ; Start reading from sector 1 (after the bootloader)
KERNEL_SECTORS_NUM equ 100    ; Number of sectors to load for the kernel
KERNEL_LOAD_ADDR   equ 0x100000 ; Destination address in memory (1MB)

; --- ATA PIO Port Definitions ---
ATA_PORT_DATA       equ 0x1F0
ATA_PORT_ERROR      equ 0x1F1
ATA_PORT_SECT_COUNT equ 0x1F2
ATA_PORT_LBA_LOW    equ 0x1F3
ATA_PORT_LBA_MID    equ 0x1F4
ATA_PORT_LBA_HIGH   equ 0x1F5
ATA_PORT_DRIVE_HEAD equ 0x1F6
ATA_PORT_STATUS_CMD equ 0x1F7

; --- ATA Commands & Status Bits ---
ATA_CMD_READ_SECTORS equ 0x20
ATA_STATUS_BUSY      equ 0x80
ATA_STATUS_DRQ       equ 0x08
ATA_STATUS_ERROR     equ 0x01

; --- GDT Selectors (calculated at assembly time) ---
CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

jmp short start
nop

; --- FAT16 BPB for a 1.44MB Floppy ---
OEMIdentifier       db 'LIBREWIN'
BytesPerSector      dw 512
SectorsPerCluster   db 1
ReservedSectors     dw 1
NumberOfFATs        db 2
RootEntries         dw 224
TotalSectors16      dw 2880
MediaDescriptor     db 0xF0
SectorsPerFAT       dw 9
SectorsPerTrack     dw 18
HeadsPerCylinder    dw 2
HiddenSectors       dd 0
TotalSectors32      dd 0
DriveNumber         db 0
Reserved1           db 0
Signature           db 0x29
VolumeID            dd 0xD105
VolumeIDString      db 'LIBREWIN BO'
SystemIDString      db 'FAT16   '

start:
    ; Step 1: Set up segment registers and a reliable stack
    xor ax, ax      ; ax = 0
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00  ; Stack grows downwards from our load address
    cld             ; Clear direction flag for string operations

    ; Step 2: Enable the A20 line to access memory beyond the first megabyte
    call enable_a20

    ; Step 3: Load the Global Descriptor Table (GDT)
    lgdt [gdt_descriptor]

    ; Step 4: Disable interrupts and enter protected mode by setting the PE bit in CR0
    cli
    mov eax, cr0
    or al, 1
    mov cr0, eax

    ; Step 5: Far jump to our 32-bit code to flush the CPU pipeline and load CS
    jmp CODE_SEG:protected_mode_start


; --- Real Mode Functions ---

; Enables the A20 line using the fast A20 gate method (port 0x92)
enable_a20:
    in al, 0x92
    test al, 0b10   ; Is bit 1 already set?
    jnz .done
    or al, 0b10     ; Set bit 1 to enable
    out 0x92, al
.done:
    ret

gdt_start:
    ; Null Descriptor (required)
    dq 0x0

gdt_code:  ; Code Segment Descriptor (Base=0, Limit=4GB, Ring 0, Execute/Read)
    dw 0xFFFF       ; Limit (0-15)
    dw 0x0000       ; Base (0-15)
    db 0x00         ; Base (16-23)
    db 0b10011010   ; Access Byte: Present, Ring 0, Code, Exec/Read
    db 0b11001111   ; Flags(4) & Limit(16-19): 4K Granularity, 32-bit
    db 0x00         ; Base (24-31)

gdt_data:  ; Data Segment Descriptor (Base=0, Limit=4GB, Ring 0, Read/Write)
    dw 0xFFFF       ; Limit (0-15)
    dw 0x0000       ; Base (0-15)
    db 0x00         ; Base (16-23)
    db 0b10010010   ; Access Byte: Present, Ring 0, Data, Read/Write
    db 0b11001111   ; Flags(4) & Limit(16-19): 4K Granularity, 32-bit
    db 0x00         ; Base (24-31)
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1 ; GDT Size
    dd gdt_start               ; GDT Linear Address

[BITS 32]
protected_mode_start:
    ; CRITICAL FIX: Initialize data segments to point to our GDT data descriptor
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax

    ; Set up a stack in a known safe memory area
    mov esp, 0x90000

    ; Load kernel from disk
    mov eax, KERNEL_LBA_START   ; LBA address
    mov ecx, KERNEL_SECTORS_NUM ; Sector count
    mov edi, KERNEL_LOAD_ADDR   ; Destination buffer
    call ata_lba_read

    ; If ata_lba_read returns, it was successful. Jump to the kernel.
    jmp CODE_SEG:KERNEL_LOAD_ADDR

; --- 32-bit Protected Mode ATA PIO Driver ---

ata_lba_read:
    mov ebx, eax ; Backup LBA address

    ; Step 1: Select drive and send LBA bits 24-27
    call ata_wait
    mov dx, ATA_PORT_DRIVE_HEAD
    mov al, 0xE0            ; Master drive, LBA mode
    or al, bl >> 24         ; Add highest 4 bits of LBA
    out dx, al

    ; Step 2: Send sector count
    mov dx, ATA_PORT_SECT_COUNT
    mov al, cl
    out dx, al

    ; Step 3: Send LBA bits 0-23
    mov dx, ATA_PORT_LBA_LOW
    mov al, bl
    out dx, al
    mov dx, ATA_PORT_LBA_MID
    mov al, bh
    out dx, al
    mov dx, ATA_PORT_LBA_HIGH
    shr ebx, 16
    mov al, bl
    out dx, al

    ; Step 4: Send the read command
    mov dx, ATA_PORT_STATUS_CMD
    mov al, ATA_CMD_READ_SECTORS
    out dx, al

    ; Step 5: Read sectors one by one
.next_sector:
    push ecx            ; Save outer loop counter (total sectors)
    call ata_poll       ; Wait for drive to be ready, with error check
    
    ; Read 256 words (512 bytes) from the data port into ES:EDI
    mov ecx, 256
    mov dx, ATA_PORT_DATA
    rep insw
    
    pop ecx             ; Restore outer loop counter
    loop .next_sector
    ret

; ata_wait: Waits for BSY bit to clear.
ata_wait:
    mov dx, ATA_PORT_STATUS_CMD
.loop:
    in al, dx
    test al, ATA_STATUS_BUSY
    jnz .loop
    ret

; ata_poll: Waits for drive to be ready for data transfer (DRQ set)
; and checks for errors. Halts on error.
ata_poll:
    call ata_wait ; First, wait until not busy

.loop:
    in al, dx ; Read status from ATA_PORT_STATUS_CMD (already in dx)
    
    ; Check for error bit
    test al, ATA_STATUS_ERROR
    jnz .error

    ; Check for data request bit
    test al, ATA_STATUS_DRQ
    jz .loop
    ret

.error:
    ; Simple error handler: Infinite loop to halt the system
    mov al, 'E'
    mov ebx, 0xb8000 ; Top-left of VGA text buffer
    mov word [ebx], 0x0445 ; Red 'E'
    cli
    hlt

times 510-($ - $$) db 0
dw 0xAA55
