/// @date 2018-08
/// @author Michiel van der Coelen
/// @copyright see LICENSE.txt

#ifndef ATTINY_HARDWARE_SERIAL_H_
#define ATTINY_HARDWARE_SERIAL_H_
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#define USI_MODE_IDLE (0)
#define USI_MODE_WRITE_START (1)
#define USI_MODE_WRITE_END (2)
#define USI_MODE_READING (3)
#define USI_MODE_WAITING (4)
#define USI_BUF_SIZE (30)

struct serial_handle {
    int usi_mode;
    uint8_t write_buf[USI_BUF_SIZE];
    int write_index;
    int write_len;
    uint8_t read_buf[USI_BUF_SIZE];
    int read_index;
    int idle_count;
    int read_flag;
};

extern struct serial_handle ser;

void ATtiny_serial_init();
void ATtiny_serial_write_async(uint8_t len);

#endif  // ATTINY_HARDWARE_SERIAL_H_
