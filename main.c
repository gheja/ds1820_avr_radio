#ifndef F_CPU
#define F_CPU 8000000UL // 8 MHz clock speed
#endif

#define DDR DDRC
#define PORT PORTC
#define PIN_RADIO 0
#define PIN_LED 1

#define BIT_SET(a, b) a |= 1 << b
#define BIT_CLEAR(a, b) a &= !(1 << b)

#define RADIO_ON BIT_SET(PORT, PIN_RADIO)
#define RADIO_OFF BIT_CLEAR(PORT, PIN_RADIO)

#define LED_ON BIT_SET(PORT, PIN_LED)
#define LED_OFF BIT_CLEAR(PORT, PIN_LED)

#define RADIO_ON BIT_SET(PORT, PIN_RADIO)
#define RADIO_OFF BIT_CLEAR(PORT, PIN_RADIO)

#include <avr/io.h>
#include <util/delay.h>

#include "pindef.h"
#include "onewire.h"
#include "ds18b20.h"
#include "usart.h"
#include "radio.h"

int main()
{
	char s[50];
	unsigned int i;
	uint16_t a1;
	uint8_t a2, a3;
	
	onewire_search_state search;
	
	DDR = 0x00;
	DDR |= 1 << PIN_RADIO;
	DDR |= 1 << PIN_LED;
	
	USART_Init(MYUBRR);
	
	USART_TransmitString("Hello!\r\n");
	
	// pin definition format needed by the ds18b20 library
	const gpin_t sensorPin = { &PORTC, &PINC, &DDRC, PC2 };
	
	while (1)
	{
		LED_ON;
		_delay_ms(50);
		LED_OFF;
		
		_delay_ms(50);
		
		if (onewire_reset(&sensorPin))
		{
			LED_ON;
			_delay_ms(50);
			LED_OFF;
			
			onewire_search_init(&search);
			
			i = 0;
			
			// loop through all devices
			while (onewire_search(&sensorPin, &search))
			{
				if (!onewire_check_rom_crc(&search))
				{
					USART_TransmitString("check_rom: crc error");
					continue;
				}
				
				// read the temperature
				ds18b20_convert_slave(&sensorPin, search.address);
				
				// wait for conversion to finish
				_delay_ms(750);
				
				// read the temperature from device
				int16_t reading = ds18b20_read_slave(&sensorPin, search.address);
				
				// create a 2 byte hash from the device's address - used for radio device id
				a1 = (search.address[0] << 8 | search.address[1]) ^
					(search.address[2] << 8 | search.address[3]) ^
					(search.address[4] << 8 | search.address[5]) ^
					(search.address[6] << 8 | search.address[7]);
				
				// calculate a radio device id and channel
				// that are valid for the emulated thermometer type
				a2 = (a1 >> 4) & 0x0F;
				a3 = a1 & 0x03;
				
				// send the device index, generated device id and channel, address on serial
				sprintf(s, "%d %04x %02x %02x %02x%02x%02x%02x%02x%02x%02x%02x: ", i, a1, a2, a3, search.address[0], search.address[1], search.address[2], search.address[3], search.address[4], search.address[5], search.address[6], search.address[7]);
				USART_TransmitString(s);
				
				// if reading failed skip this device
				if (reading == kDS18B20_CrcCheckFailed)
				{
					USART_TransmitString("read_slave: crc error\r\n");
					continue;
				}
				
				// Convert to floating point (or keep as a Q12.4 fixed point value)
				float temperature = ((float) reading) / 16;
				
				sprintf(s, "%d %04x", (int) (temperature * 10), reading);
				USART_TransmitString(s);
				
				// void prologue_send(uint8_t id, uint8_t channel, float temperature, uint8_t humidity, uint8_t battery_status, uint8_t button_pressed)
				prologue_send(a2, a3, temperature, 11, 1, 0);
				
				// wait before going to the next device
				_delay_ms(10000);
				
				USART_TransmitString("\r\n");
				
				i++;
			}
			
			USART_TransmitString("\r\n");
		}
	}
	
	return 0;
}
