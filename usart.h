#pragma once

#include <avr/io.h>
#include <stdio.h>

#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1

void USART_Init(unsigned int ubrr);
void USART_Transmit(unsigned char data);
void USART_TransmitString(unsigned char a[]);
