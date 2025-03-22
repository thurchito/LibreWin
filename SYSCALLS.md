# Free95 Syscall Table

A table of all current system calls for free95.

## How to use

Look up the syscall in the table you want to use and follow it like this:

``` 
    mov eax, {eax} : 1st paramater (eax)
    mov ebx, {ebx} ; 2nd paramater (ebx)
    mov ecx, {ecx} ; 3rd paramater (ecx)
    mov edx, {edx} ; 4th paramater (edx)
    mov esx, {esx} ; 5th paramater (esx)
    mov edi, {edi} ; 6th paramater (edi)
    int 0x2e       ; execute syscall
```
## Syscall Table
- NtDisplayString: Displays string {ebx} in text mode. EBX must be a PUNICODE_STRING
- NtOpenFile: Opens a file.
