/*
 * Button.c
 *
 * Created: 17/04/2026 5:20:54 PM
 *  Author: Alaa
 */ 

#include <avr/io.h>
#include "button.h"


void BUTTON_init(void)
{
    DDRB &= ~(1 << BTN_PIN);   // input
    PORTB |= (1 << BTN_PIN);   // pull-up
}

uint8_t BUTTON_read(void)
{
    return !(PINB & (1 << BTN_PIN)); // active LOW
}