#ifndef KERNEL_H
#define KERNEL_H

#define PORT 0x3f8          // COM1 serial port
#define VGA_WIDTH 80
#define VGA_HEIGHT 20

#define FREE95_MAX_PATH 108

void kernel_main();
void print(const char* str);

#endif
