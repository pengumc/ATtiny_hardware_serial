#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "ATtiny_hardware_serial.h"

struct serial_handle ser;

uint8_t reverse_byte(uint8_t n) {
    int i, temp;
    temp = 0;
    for (i = 0; i < 8; ++i) {
        temp <<= 1;
        temp |= 0x01 & n;
        n >>= 1;
    }
    return temp;
}


int main() {
    DDRB |= (1<<PB4);
    ATtiny_serial_init();
    sei();

    int i;
    for (i  =0; i < 10000; ++i) {;}
    for (i = 0; i < USI_BUF_SIZE; ++i) {
        ser.write_buf[i] = 1 << (i % 8);
    }

    ATtiny_serial_write_async(16);
    while(1) {
        if (ser.read_flag != 0) {
            for (i = 0; i < ser.read_index; ++i) {
                ser.write_buf[i] = ser.read_buf[i];
            }
            ATtiny_serial_write_async(ser.read_index);
            ser.read_flag = 0;
        }
        asm("nop");  // something has to be here or -Os break things.
    }
    return 0;
}

