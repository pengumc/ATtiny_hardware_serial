
/// @date 2018-08
/// @author Michiel van der Coelen
/// @copyright see LICENSE.txt
#include "ATtiny_hardware_serial.h"

/// Wait for USI_MODE_IDLE, then start the write sequence for len bytes from ser.write_buffer
// Returns as soon as the sequence has started.
void ATtiny_serial_write_async(uint8_t len) {
    // wait for mode to become idle
    while (ser.usi_mode != USI_MODE_IDLE) {
        ;
    }
    // just enable the clock and let the OVF interrupts take over.
    USIDR = 0xFF;  // just to be sure.
    TCNT0 = 0;
    TCCR0B = 1;  // enable baudrate clock
    USISR = (1<<USIOIF) | 15;
    ser.usi_mode = USI_MODE_WRITE_START;
    ser.write_len = len;
    ser.write_index = 0;
}

void ATtiny_serial_init() {
    ser.usi_mode = USI_MODE_IDLE;
    ser.read_flag = 0;
    // USICR control reg
    USICR = (0 << USISIE) | (1 << USIOIE) | // 
            (0 << USIWM1) | (1 << USIWM0) | // three wire mode
            (0 << USICS1) | (1 << USICS0) | // clock source = timer0 compare match
            (0 << USICLK) | (0 << USITC); // don't care
    PCMSK |= (1 << PCINT0);  // aka the DI pin
    GIMSK |= (1 << PCIE);  // enable pin change interrupt
    OCR0A = 100;
    TCCR0A = (0 << COM0B1) | (0 << COM0A0) | (1 <<WGM01) | (0 << WGM00);  // COM0B 1 to toggle
    USISR = (1<<USIOIF) | 7;  // clear ovf and 4 bit counter?
    USIDR = 0xFF;
    DDRB |=  (1<<PB1);  // DO
    DDRB &= ~(1<<PB0);  // DI
}

ISR(PCINT0_vect) {
    TCCR0B = 1;  // start baudrate clock
    TCNT0 = 0;
    PCMSK = 0;  // Slear the PCINT mask so no further interrupts for the pins happen
    USISR = (1<<USIOIF) | 8;
    // clear read flag and reset index if this is a new read
    if (ser.usi_mode == USI_MODE_IDLE) {
        ser.read_index = 0;
        ser.read_flag = 0;
    }
    else {
        ser.read_flag = 0;
        if (ser.read_index >= USI_BUF_SIZE) {
            ser.read_index = 0;
        }
    }
    ser.usi_mode = USI_MODE_READING;
    DDRB &= ~(1<<PB1);
}

ISR(USI_OVF_vect) {
    switch (ser.usi_mode) {
        case USI_MODE_WRITE_START: {
            if (ser.write_index >= ser.write_len) {
                USIDR = 0xFF;
                USISR = (1 << USIOIF) | 15;
                ser.usi_mode = USI_MODE_IDLE;
            }
            else
            {
                DDRB |= (1<<PB1);
                USIDR = 0x80 | (ser.write_buf[ser.write_index] >> 2);  // 1, 0, data[7..2]
                USISR = (1 << USIOIF) | 9;
                ser.usi_mode = USI_MODE_WRITE_END;
            }
            break;
        }   
        case USI_MODE_WRITE_END: {
            USIDR = (ser.write_buf[ser.write_index] << 5) | 0x1F;  // data[2..0], 1's
            USISR = (1 << USIOIF) | 13;
            ser.usi_mode = USI_MODE_WRITE_START;
            ++ser.write_index;
            break;
        }
        case USI_MODE_READING: {
            ser.read_buf[ser.read_index++] = USIDR;
            USISR = (1<<USIOIF) | 15;
            USIDR = 0xFF;
            ser.usi_mode = USI_MODE_WAITING;
            ser.idle_count = 0;
            break;
        }
        case USI_MODE_WAITING: {
            PCMSK = (1<<PB0);
            USISR = (1<<USIOIF) | 15;
            USIDR = 0xFF;
            if (ser.idle_count++ > 3) {
                ser.usi_mode = USI_MODE_IDLE;
                ser.read_flag = 1;
                TCCR0B = 0;
            }
            break;
        }
        case USI_MODE_IDLE: {
            PCMSK = (1<<PB0);
            TCCR0B = 0;
            USISR = (1<<USIOIF);
            break;
        }
   }
}
