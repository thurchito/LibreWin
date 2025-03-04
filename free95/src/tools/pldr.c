#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// Define PE structures
typedef struct _IMAGE_DOS_HEADER {
    uint16_t e_magic;      // Magic number ("MZ")
    uint16_t e_cblp;
    uint16_t e_cp;
    uint16_t e_crlc;
    uint16_t e_cparhdr;
    uint16_t e_minalloc;
    uint16_t e_maxalloc;
    uint16_t e_ss;
    uint16_t e_sp;
    uint16_t e_csum;
    uint16_t e_ip;
    uint16_t e_cs;
    uint16_t e_lfarlc;
    uint16_t e_ovno;
    uint16_t e_res[4];
    uint16_t e_oemid;
    uint16_t e_oeminfo;
    uint16_t e_res2[10];
    int32_t  e_lfanew;     // Offset to PE header
} IMAGE_DOS_HEADER;

typedef struct _IMAGE_FILE_HEADER {
    uint16_t Machine;
    uint16_t NumberOfSections;
    uint32_t TimeDateStamp;
    uint32_t PointerToSymbolTable;
    uint32_t NumberOfSymbols;
    uint16_t SizeOfOptionalHeader;
    uint16_t Characteristics;
} IMAGE_FILE_HEADER;

typedef struct _IMAGE_OPTIONAL_HEADER {
    uint16_t Magic;
    uint8_t  MajorLinkerVersion;
    uint8_t  MinorLinkerVersion;
    uint32_t SizeOfCode;
    uint32_t SizeOfInitializedData;
    uint32_t SizeOfUninitializedData;
    uint32_t AddressOfEntryPoint;
    uint32_t BaseOfCode;
    uint64_t ImageBase;
    uint32_t SectionAlignment;
    uint32_t FileAlignment;
    uint16_t MajorOperatingSystemVersion;
    uint16_t MinorOperatingSystemVersion;
    uint32_t SizeOfImage;
    uint32_t SizeOfHeaders;
    uint32_t CheckSum;
    uint16_t Subsystem;
    uint16_t DllCharacteristics;
} IMAGE_OPTIONAL_HEADER;

typedef struct _IMAGE_NT_HEADERS {
    uint32_t Signature;
    IMAGE_FILE_HEADER FileHeader;
    IMAGE_OPTIONAL_HEADER OptionalHeader;
} IMAGE_NT_HEADERS;

typedef struct _IMAGE_SECTION_HEADER {
    char Name[8];
    uint32_t VirtualSize;
    uint32_t VirtualAddress;
    uint32_t SizeOfRawData;
    uint32_t PointerToRawData;
    uint32_t PointerToRelocations;
    uint32_t PointerToLinenumbers;
    uint16_t NumberOfRelocations;
    uint16_t NumberOfLinenumbers;
    uint32_t Characteristics;
} IMAGE_SECTION_HEADER;

// Function to read and parse the PE file
void parsePE(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Failed to open file");
        return;
    }

    // Read DOS header
    IMAGE_DOS_HEADER dosHeader;
    fread(&dosHeader, sizeof(IMAGE_DOS_HEADER), 1, file);

    if (dosHeader.e_magic != 0x5A4D) {  // Check for 'MZ'
        printf("Not a valid PE file.\n");
        fclose(file);
        return;
    }

    // Seek to PE header
    fseek(file, dosHeader.e_lfanew, SEEK_SET);

    // Read PE header
    IMAGE_NT_HEADERS ntHeaders;
    fread(&ntHeaders, sizeof(IMAGE_NT_HEADERS), 1, file);

    if (ntHeaders.Signature != 0x4550) {  // Check for 'PE\0\0'
        printf("Invalid PE header.\n");
        fclose(file);
        return;
    }

    // Print PE header information
    printf("PE Header Information:\n");
    printf("  Machine: 0x%X\n", ntHeaders.FileHeader.Machine);
    printf("  Number of Sections: %d\n", ntHeaders.FileHeader.NumberOfSections);
    printf("  Size of Optional Header: %d\n", ntHeaders.FileHeader.SizeOfOptionalHeader);
    printf("  Entry Point: 0x%X\n", ntHeaders.OptionalHeader.AddressOfEntryPoint);
    printf("  Image Base: 0x%llX\n", ntHeaders.OptionalHeader.ImageBase);
    printf("  Section Alignment: 0x%X\n", ntHeaders.OptionalHeader.SectionAlignment);
    printf("  File Alignment: 0x%X\n", ntHeaders.OptionalHeader.FileAlignment);

    // Read section headers
    IMAGE_SECTION_HEADER sectionHeader;
    printf("\nSection Information:\n");
    for (int i = 0; i < ntHeaders.FileHeader.NumberOfSections; i++) {
        fread(&sectionHeader, sizeof(IMAGE_SECTION_HEADER), 1, file);
        printf("  Section Name: %s\n", sectionHeader.Name);
        printf("    Virtual Address: 0x%X\n", sectionHeader.VirtualAddress);
        printf("    Size of Raw Data: 0x%X\n", sectionHeader.SizeOfRawData);
        printf("    Pointer to Raw Data: 0x%X\n", sectionHeader.PointerToRawData);
    }

    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <PE file>\n", argv[0]);
        return 1;
    }

    parsePE(argv[1]);
    return 0;
}
