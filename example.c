#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "ATtiny_hardware_serial.h"

// Example file. On startup 16 bytes will be sent.
// After that everything you send is echoed back
// Baudrate is 10000.
// Buffers are USI_BUFFER_SIZE bytes large. (30)

// You need this global variable since we need it in the interrupt routines.
struct serial_handle ser;

int main() {
    // Call the initializer to setup the ports (PB0 and PB1) and registers.
    ATtiny_serial_init();
    // enable interrupts.
    sei();

    // Wait a bit
    int i;
    for (i  =0; i < 10000; ++i) {;}
    // Setup the first message 
    for (i = 0; i < USI_BUF_SIZE; ++i) {
        ser.write_buf[i] = 1 << (i % 8);
    }
    // Start sending it.
    // This function returns as soon as the sequence has started
    // Interrupts will handle the rest.
    ATtiny_serial_write_async(16);

    // You can check ser.write_index or ser.usi_mode if you want to know 
    // When it's done.

    while(1) {
        // ser.read_flag will be 1 when something has been received. 
        // ser.read_flag can be cleared by hand, it will also be cleared when a new message is being received.
        // ser.read_index will be the number of bytes received.
        // The bytes are in ser.read_buffer
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

