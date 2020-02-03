#pragma once

#include <avr/io.h>
#include <util/delay.h>

#define PWM_TIME_SHORT 500
#define PWM_TIME_LONG 1150
#define PWM_TIME_GAP 6000

#define PPM_TIME_PULSE 583
#define PPM_TIME_OFF_0 2100
#define PPM_TIME_OFF_1 4020
#define PPM_TIME_SYNC 8650

void send_ppm(uint8_t bytes[], uint8_t length, uint8_t repeats);
void send_pwm(uint8_t bytes[], uint8_t length, uint8_t repeats);
void prologue_send(uint8_t id, uint8_t channel, float temperature, uint8_t humidity, uint8_t battery_status, uint8_t button_pressed);