/*
 * UART.c
 *
 * Created: 17/04/2026 5:19:46 PM
 *  Author: Alaa
 */ 


#include <avr/io.h>
#include "uart.h"

#define BAUD_PRESCALE(baud) ((F_CPU / (16UL * baud)) - 1)

void UART_init(uint32_t baud)
{
    uint16_t ubrr = BAUD_PRESCALE(baud);

    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)(ubrr);

    UCSR0B = (1 << TXEN0); // TX enable

    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8-bit data
}

void UART_sendChar(char c)
{
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = c;
}

void UART_sendString(const char *str)
{
    while (*str)
        UART_sendChar(*str++);
}

 void uart_print_number(uint8_t num)
{
	if (num >= 100)
	UART_sendChar('0' + (num / 100));
	if (num >= 10)
	UART_sendChar('0' + ((num / 10) % 10));
	UART_sendChar('0' + (num % 10));
}