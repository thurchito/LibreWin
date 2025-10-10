# LibreWin Syscall Table

A table of all current system calls for Windows 2000 implemented in LibreWin.

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
    int 0x002e       ; execute syscall
```
## Syscall Table
|Name           |Description                               |eax       |ebx                    |ecx       |edx       |esi       |edi       |ebp|
|---------------|------------------------------------------|----------|-----------------------|----------|----------|----------|----------|-|
|NtAcceptConnectPort    |Base for establishing LPC communication. Should be used after NtListenPort.    |0x0000    |null    |null    |null    |null    |null    |null
|NtAccessCheck    |Used by server applications to check connections to object for client token.    |0x0001    |null    |null    |null    |null    |null    |null
|NtAccessCheckAndAuditAlarm|Determines whether a security descriptor grants a specified set of access rights to the client being impersonated by the calling thread.|0x0002|null|null|null|null|null|null
|NtAccessCheckByType    |Determines whether a security descriptor grants a specified set of access rights to the client identified by an access token.|0x0003|null|null|null|null|null|null
|NtAccessCheckByTypeAndAuditAlarm|Refer to NtAccessCheckAndAuditAlarm.|0x0004|null|null|null|null|null|null
|NtDisplayString|Displays string {ebx} in text mode. (Typically crash screen)       |0x002e      |PUNICODE_STRING        |null      |null      |null      |null      |null|
|NtOpenFile     |Opens {ebx} file with {ecx} access, {edx} object attributes, {esi} I/O Status Block, {edi} sharing access, and {ebp} Open Options|0x004f|PHANDLE|INT|POBJECT_ATTRIBUTES|PVOID|ULONG|ULONG|
|NtQuerySystemInformation|Returns requested system information. ⚠️*Warning:* It does not probe for BIOS/general system info. The function returns default safe values.    |0x007c    |PUNICODE_STRING    |0x0    |0x10000000    |null    |null    |null|
|NtShutdownSystem|Shuts down system with SHUTDOWN_ACTION {ebx}       |0x00b4      |SHUTDOWN_ACTION        |null      |null      |null      |null      |null|
