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
Signature          db 0x29
VolumeID           dd 0x00D10500
VolumeIDString     db 'LIBREWIN BO'   ; 11 bytes
SystemIDString     db 'FAT16   '      ; 8 bytes

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00

    ; Read kernel sectors in real mode
    mov eax, 284
    mov ecx, 105
    mov edi, 0x00100000
    call ata_lba_read

    lgdt [gdt_descriptor]

[BITS 32]
    mov eax, cr0
    or  eax, 1
    mov cr0, eax

    ; Far jump to protected_entry (CS selector 0x08)
    jmp 0x08:protected_entry

[BITS 16]
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
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov esp, 0x90000

    ; Jump to kernel (already loaded at 0x00100000)
    jmp 0x08:0x00100000

[BITS 16]
ata_lba_read:
    push bx
    push si

    mov bx, ax

[BITS 32]
    push ebx
    push esi

    mov ebx, eax       ; full LBA
    mov esi, ecx       ; sectors remaining

.next_sector:
    ; sector count
    mov al, sil
    out 0x1F2, al

    ; LBA low
    mov al, bl
    out 0x1F3, al

    ; LBA mid
    mov al, bh
    out 0x1F4, al

    ; LBA high
    mov al, byte [ebx + 2]
    out 0x1F5, al

    ; top + head + drive
    mov al, byte [ebx + 3]
    or al, 0xE0
    out 0x1F6, al

    ; READ SECTORS
    mov al, 0x20
    out 0x1F7, al

.wait_drq:
    in al, 0x1F7
    test al, 8
    jz .wait_drq

    mov dx, 0x1F0
    mov ecx, 256
    rep insw
    add edi, 512

    dec esi
    jnz .next_sector

    pop esi
    pop ebx

[BITS 16]
    pop si
    pop bx
    ret

times 510-($-$$) db 0
dw 0xAA55
