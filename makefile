#default makefile for avr stuff

# Name: Makefile
# Author: Michiel van der Coelen
# date: 2011-03-31
# tabsize: 2
 
MMCU = attiny45
AVRDUDE_MCU = t45
NAME = ATtiny45_rs232


OBJECTS = ATtiny_hardware_serial.o example.o
INCLUDES = -I.
LIBS =
CFLAGS = -std=gnu99 -Wall -Os -mmcu=$(MMCU) $(INCLUDES)

CC = avr-gcc
SIZE = avr-size
OBJCOPY = avr-objcopy
AVRDUDE_PROGRAMMERID = usbasp

.PHONY: all clean test
all: bin/$(NAME).hex
	$(SIZE) $(NAME).hex

#rebuild everything!
force: clean all

bin/$(NAME).hex: $(NAME).elf
	$(OBJCOPY) -O ihex $(NAME).elf $(NAME).hex
	rm $(OBJECTS) $(NAME).elf
	
$(NAME).elf: $(OBJECTS)
	$(CC) $(CFLAGS) $(LIBS) -o $(NAME).elf $(OBJECTS)

#compile src files
%.o: %.c
		$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S
	$(CC) $(CFLAGS) $(INCLUDES) -x assembler-with-cpp -c $< -o $@


clean:
	rm -f $(OBJECTS) $(NAME).elf

program: bin/$(NAME).hex
	avrdude -c $(AVRDUDE_PROGRAMMERID) -p $(AVRDUDE_MCU) -U flash:w:$(NAME).hex

programf:
	avrdude -c $(AVRDUDE_PROGRAMMERID) -p $(AVRDUDE_MCU) -U flash:w:$(NAME).hex

test:
	avrdude -c $(AVRDUDE_PROGRAMMERID) -p $(AVRDUDE_MCU) -v
