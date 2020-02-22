#!/bin/bash

program="test"

avr-gcc -std=c99 -g -Os -mmcu=atmega328p -o ${program}.o -DF_CPU=8000000 \
	main.c \
	crc.c \
	pindef.c \
	onewire.c \
	ds18b20.c \
	usart.c \
	radio.c \
	defines.h \
 || exit 1

avr-objcopy -j .text -j .data -O ihex ${program}.o ${program}.hex || exit 1
avr-size -d --mcu=atmega328p ${program}.o

# E:07, H:D9, L:E2
# sudo avrdude -c usbasp -p m328p -U lfuse:w:0xe2:m -U hfuse:w:0xd9:m -U efuse:w:0x07:m

sudo avrdude -c usbasp -p m328p -U flash:w:${program}.hex
