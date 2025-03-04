#include "descriptorTables.h"
#include "isr.h"
#include "string.h"
#include "vga.h"
#include "mouse.h"
#include <stdint.h>
#include <stdbool.h>
#include "string.h"

int g_mouse_x_pos = 0, g_mouse_y_pos = 0;
MOUSE_STATUS g_status;

int mouse_getlc()
{
    return g_status.left_button;
}

int mouse_getrc()
{
    return g_status.right_button;
}

int mGetX() {
    return g_mouse_x_pos;
}

int mGetY() {
    return g_mouse_y_pos;
}

void mouse_wait(bool type) {
    uint32 time_out = 100000;
    if (type == false) {
        // suspend until status is 1
        while (time_out--) {
            if ((inb(PS2_CMD_PORT) & 1) == 1) {
                return;
            }
        }
        return;
    } else {
        while (time_out--) {
            if ((inb(PS2_CMD_PORT) & 2) == 0) {
                return;
            }
        }
    }
}

void mouse_write(uint8 data) {
    // sending write command
    mouse_wait(true);
    outb(PS2_CMD_PORT, 0xD4);
    mouse_wait(true);
    // finally write data to port
    outb(MOUSE_DATA_PORT, data);
}

uint8 mouse_read() {
    mouse_wait(false);
    return inb(MOUSE_DATA_PORT);
}

void get_mouse_status(char status_byte, MOUSE_STATUS *status) {
    memset(status, 0, sizeof(MOUSE_STATUS));
    if (status_byte & 0x01)
        status->left_button = 1;
    if (status_byte & 0x02)
        status->right_button = 1;
    if (status_byte & 0x04)
        status->middle_button = 1;
    if (status_byte & 0x08)
        status->always_1 = 1;
    if (status_byte & 0x10)
        status->x_sign = 1;
    if (status_byte & 0x20)
        status->y_sign = 1;
    if (status_byte & 0x40)
        status->x_overflow = 1;
    if (status_byte & 0x80)
        status->y_overflow = 1;
}

void print_mouse_info()
{
    /* shit? */
}

void mouse_handler() {
    static uint8 mouse_cycle = 0;
    static char mouse_byte[3];

    switch (mouse_cycle) {
        case 0:
            mouse_byte[0] = mouse_read();
            get_mouse_status(mouse_byte[0], &g_status);
            mouse_cycle++;
            break;
        case 1:
            mouse_byte[1] = mouse_read();
            mouse_cycle++;
            break;
        case 2:
            mouse_byte[2] = mouse_read();
            g_mouse_x_pos = g_mouse_x_pos + mouse_byte[1];
            g_mouse_y_pos = g_mouse_y_pos - mouse_byte[2];

            if (g_mouse_x_pos < 0)
                g_mouse_x_pos = 0;
            if (g_mouse_y_pos < 0)
                g_mouse_y_pos = 0;
            if (g_mouse_x_pos > 640)
                g_mouse_x_pos = 640 - 1;
            if (g_mouse_y_pos > 480)
                g_mouse_y_pos = 480 - 1;

            mouse_cycle = 0;
            break;
    }
}

/**
 * available rates 10, 20, 40, 60, 80, 100, 200
 */
void set_mouse_rate(uint8 rate) {
    uint8 status;

    outb(MOUSE_DATA_PORT, MOUSE_CMD_SAMPLE_RATE);
    status = mouse_read();
    if(status != MOUSE_ACKNOWLEDGE) {
        return;
    }
    outb(MOUSE_DATA_PORT, rate);
    status = mouse_read();
    if(status != MOUSE_ACKNOWLEDGE) {
        return;
    }
}

void mouse_init() {
    uint8 status;

    g_mouse_x_pos = 5;
    g_mouse_y_pos = 2;

    // enable mouse device
    mouse_wait(true);
    outb(PS2_CMD_PORT, 0xA8);

    // print mouse id
    outb(MOUSE_DATA_PORT, MOUSE_CMD_MOUSE_ID);
    status = mouse_read();

    set_mouse_rate(10);

    //outb(MOUSE_DATA_PORT, MOUSE_CMD_RESOLUTION);
    //outb(MOUSE_DATA_PORT, 0);

    // enable the interrupt
    mouse_wait(true);
    outb(PS2_CMD_PORT, 0x20);
    mouse_wait(false);
    // get and set second bit
    status = (inb(MOUSE_DATA_PORT) | 2);
    // write status to port
    mouse_wait(true);
    outb(PS2_CMD_PORT, MOUSE_DATA_PORT);
    mouse_wait(true);
    outb(MOUSE_DATA_PORT, status);

    // set mouse to use default settings
    mouse_write(MOUSE_CMD_SET_DEFAULTS);
    status = mouse_read();
    if(status != MOUSE_ACKNOWLEDGE) {
        return;
    }

    // enable packet streaming to receive
    mouse_write(MOUSE_CMD_ENABLE_PACKET_STREAMING);
    status = mouse_read();
    if(status != MOUSE_ACKNOWLEDGE) {
        return;
    }

    // set mouse handler
    register_interrupt_handler(IRQ12, mouse_handler);
}
