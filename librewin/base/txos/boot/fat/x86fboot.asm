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
VolumeID           dd 0xD105
VolumeIDString     db 'LIBREWIN BOO'  ; 11 bytes
SystemIDString     db 'FAT16   '      ; 8 bytes

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00
    sti

    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    jmp CODE_SEG:load32

gdt_start:
gdt_null: dd 0,0
gdt_code: dw 0xffff,0,0,0x9a,0xcf,0
gdt_data: dw 0xffff,0,0,0x92,0xcf,0
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

[BITS 32]
CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

load32:
    mov eax, 1         ; sectors to read
    mov ecx, 100       ; LBA
    mov edi, 0x0100000 ; memory destination
    call ata_lba_read
    jmp CODE_SEG:0x0100000

ata_lba_read:
    push ebx
    mov ebx, eax
    shr eax, 24
    or eax, 0xE0
    out 0x1F6, al

    mov al, cl
    out 0x1F2, al

    mov eax, ebx
    out 0x1F3, al
    mov eax, ebx
    shr eax, 8
    out 0x1F4, al
    mov eax, ebx
    shr eax, 16
    out 0x1F5, al

    mov al, 0x20
    out 0x1F7, al

.next_sector:
    push ecx
.wait:
    in al, 0x1F7
    test al, 8
    jz .wait
    mov ecx, 256
    rep insw
    pop ecx
    loop .next_sector
    pop ebx
    ret

times 510-($-$$) db 0
dw 0xAA55
