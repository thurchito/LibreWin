null      null      # Free95 Syscall Table

A table of all current system calls for NT 4.0 implemented in Free95.

## How to use

Look up the syscall in the table you want to use and follow it like this:

``` 
    mov eax, {eax} : 1st parameter (eax)
    mov ebx, {ebx} ; 2nd parameter (ebx)
    mov ecx, {ecx} ; 3rd parameter (ecx)
    mov edx, {edx} ; 4th parameter (edx)
    mov esx, {esx} ; 5th parameter (esx)
    mov edi, {edi} ; 6th parameter (edi)
    int 0x2e       ; execute syscall
```
## Syscall Table
- NtDisplayString: Displays string {ebx} in text mode. EBX must be a PUNICODE_STRING
- NtOpenFile: Opens a file.

|Name           |Description                               |eax       |ebx                    |ecx       |edx       |esi       |edi       |ebp|
|---------------|------------------------------------------|----------|-----------------------|----------|----------|----------|----------|-|
|NtDisplayString|Displays string {ebx} in text mode.       |0x2e      |PUNICODE_STRING        |null      |null      |null      |null      |null|
|NtOpenFile     |Opens {ebx} file with {ecx} access, {edx} object attributes, {esi} I/O Status Block, {edi} sharing access, and {ebp} Open Options|0x4f|PHANDLE|INT|POBJECT_ATTRIBUTES|PVOID|ULONG|ULONG|


>fix the table and change the values if i messed something up. this comment is meant for maintainers, please delete once you finish fixing.