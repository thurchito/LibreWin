# Free95 Syscall Table

A table of all current system calls for NT 4.0 implemented in Free95.

*SYSCALL.md template by blobii*

## How to use

Look up the syscall in the table you want to use and follow it like this:

``` 
    mov eax, {eax} : 1st parameter (eax)
    mov ebx, {ebx} ; 2nd parameter (ebx)
    mov ecx, {ecx} ; 3rd parameter (ecx)
    mov edx, {edx} ; 4th parameter (edx)
    mov esi, {esi} ; 5th parameter (esi)
    mov edi, {edi} ; 6th parameter (edi)
    int 0x2e       ; execute syscall
```
## Syscall Table
|Name           |Description                               |eax       |ebx                    |ecx       |edx       |esi       |edi       |ebp|
|---------------|------------------------------------------|----------|-----------------------|----------|----------|----------|----------|-|
|NtDisplayString|Displays string {ebx} in text mode. (Typically crash screen)       |0x2e      |PUNICODE_STRING        |null      |null      |null      |null      |null|
|NtOpenFile     |Opens {ebx} file with {ecx} access, {edx} object attributes, {esi} I/O Status Block, {edi} sharing access, and {ebp} Open Options|0x4f|PHANDLE|INT|POBJECT_ATTRIBUTES|PVOID|ULONG|ULONG|
|NtShutdownSystem|Shuts down system with SHUTDOWN_ACTION {ebx}       |0x00b4      |SHUTDOWN_ACTION        |null      |null      |null      |null      |null|
