; Compact LibreWin bootloader (FAT BPB fields sized correctly)
ORG 0x7c00
BITS 16

jmp start
nop

OEMIdentifier      db 'LIBREWIN'      ; 8 bytes
BytesPerSector     dw 512
SectorsPerCluster  db 1
ReservedSectors    dw 1
NumberOfFATs       db 2
RootEntries        dw 224
TotalSectors16     dw 2880
MediaDescriptor    db 0xF0
SectorsPerFAT      dw 9
SectorsPerTrack    dw 18
HeadsPerCylinder   dw 2
HiddenSectors      dd 0
TotalSectors32     dd 0
DriveNumber        db 0
Reserved1          db 0
WinNTBit           db 0x00
Signature          db 0x29
VolumeID           dd 0x00D10500
VolumeIDString     db 'LIBREWIN BO'   ; 11 bytes (exact)
SystemIDString     db 'FAT16   '      ; 8 bytes (exact)

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00

    ; load GDT and enter Protected Mode
    lgdt [gdt_descriptor]
    mov eax, cr0
    or  eax, 1
    mov cr0, eax

    ; Far jump to flush prefetch and load new CS
    jmp 0x08:protected_entry   ; 0x08 = code selector

gdt_start:
gdt_null:    dd 0,0

gdt_code:    dw 0xffff
             dw 0
             db 0
             db 0x9A
             db 0xCF
             db 0

gdt_data:    dw 0xffff
             dw 0
             db 0
             db 0x92
             db 0xCF
             db 0

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

[BITS 32]
protected_entry:
    ; set up flat data segments and stack
    mov ax, 0x10      ; data selector (0x10)
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov esp, 0x90000

    ; Example: read 1 sector from LBA 100 into 0x00100000
    ; (caller may change eax/ecx/edi before calling)
    mov eax, 100      ; LBA (example)
    mov ecx, 1        ; sectors to read
    mov edi, 0x00100000
    call ata_lba_read

    ; Jump to loaded code (code selector = 0x08)
    jmp 0x08:0x00100000

ata_lba_read:
    push ebx
    push esi

    mov ebx, eax      ; save LBA
    mov esi, ecx      ; sectors remaining

    ; send sector count
    mov al, cl
    out 0x1F2, al

    ; send LBA low, mid, high bytes
    mov eax, ebx
    out 0x1F3, al         ; LBA 0..7
    mov eax, ebx
    shr eax, 8
    out 0x1F4, al         ; LBA 8..15
    mov eax, ebx
    shr eax, 16
    out 0x1F5, al         ; LBA 16..23

    ; send top 8 bits with 0xE0 (LBA mode + master)
    mov eax, ebx
    shr eax, 24
    or al, 0xE0
    out 0x1F6, al

    ; send READ SECTORS command (0x20)
    mov al, 0x20
    out 0x1F7, al

.read_loop:
    ; wait for DRQ
    in al, 0x1F7
    test al, 8
    jz .read_loop

    ; read 256 words (512 bytes)
    mov ecx, 256
    rep insw

    dec esi
    jnz .read_loop

    pop esi
    pop ebx
    ret

times 510-($-$$) db 0
dw 0xAA55
