#include "radio.h"
#include "defines.h"

void send_ppm(uint8_t bytes[], uint8_t length, uint8_t repeats)
{
	uint8_t i, j, k, n;
	
	for (k=0; k<repeats; k++)
	{
		// sync pulse between the repeats
		if (k != 0)
		{
			RADIO_OFF;
			_delay_us(PPM_TIME_SYNC);
		}
		
		for (i=0; i<length; i++)
		{
			j = i % 8;
			n = bytes[i / 8] & (1 << (7 - j));
			
			RADIO_ON;
			_delay_us(PPM_TIME_PULSE);
			
			RADIO_OFF;
			
			if (n != 0)
			{
				_delay_us(PPM_TIME_OFF_1);
			}
			else
			{
				_delay_us(PPM_TIME_OFF_0);
			}
		}
		
		// final bit
		RADIO_ON;
		_delay_us(PPM_TIME_PULSE);
		
		RADIO_OFF;
		_delay_us(PPM_TIME_PULSE);
	}
}

void send_pwm(uint8_t bytes[], uint8_t length, uint8_t repeats)
{
	uint8_t i, j, k, b;
	
	for (k=0; k<3; k++)
	{
		for (i=0; i<length; i++)
		{
			j = i % 8;
			b = bytes[i / 8] & (1 << (7 - j));
			
			if (b != 0)
			{
				RADIO_ON;
				_delay_us(PWM_TIME_SHORT);
				
				RADIO_OFF;
				_delay_us(PWM_TIME_LONG);
			}
			else
			{
				RADIO_ON;
				_delay_us(PWM_TIME_LONG);
				
				RADIO_OFF;
				_delay_us(PWM_TIME_SHORT);
			}
		}
		
		_delay_us(PWM_TIME_GAP);
	}
}

void prologue_send(uint8_t id, uint8_t channel, float temperature, uint8_t humidity, uint8_t battery_status, uint8_t button_pressed)
{
	uint8_t bytes[5];
	uint8_t length;
	int16_t t1;
	
	// RTL-433 recognises this device as "Prologue sensor", with id 9.
	// NOTE: RTL-433 will ignore the device if all these conditions meet:
	//   - battery_status = 0 
	//   - button_pressed = 0
	//   - channel = 1
	
	// Domoticz recognises this device as the following:
	//   ID: 9xxy, where xx is "id" in hex, y is "channel" - 1 in hex
	//   Name: Prologue
	//   Type: Temp + Humidity
	//   SubType: WTGR800
	
	// identifier (0-255)
	// NOTE: Domoticz ignores ids outside 1-127
	
	// channel (1-4)
	// battery status (0: LOW, 1: OK)
	// measurements sent by pressing button (not automatic)
	// temperature (signed integer (two's complement), celsius x 10)
	// humidity (percent, 0-100)
	
	bytes[0] = 0b10010000;
	bytes[1] = 0b00000000;
	bytes[2] = 0b00000000;
	bytes[3] = 0b00000000;
	bytes[4] = 0b00000000;
	
	length = 37;
	
	
	t1 = (int16_t) (temperature * 10);
	
	bytes[0] |= (id & 0xF0) >> 4;
	bytes[1] |= (id & 0x0F) << 4;
	bytes[1] |= (battery_status & 0x01) << 3;
	bytes[1] |= (button_pressed & 0x04) << 2;
	bytes[1] |= ((channel - 1) & 0x03);
	bytes[2] |= (t1 & 0x0FF0) >> 4;
	bytes[3] |= (t1 & 0x000F) << 4;
	bytes[3] |= (humidity & 0xF0) >> 4;
	bytes[4] |= (humidity & 0x0F) << 4;
	
	send_ppm(bytes, length, 7);
}
