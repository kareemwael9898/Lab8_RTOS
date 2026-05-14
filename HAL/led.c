#include <avr/io.h>
#include "led.h"


void LED_init(void)
{
	DDRB |= (1 << LED_PIN);   
}

void LED_on(void)
{
	PORTB |= (1 << LED_PIN);    
}

void LED_off(void)
{
	PORTB &= ~(1 << LED_PIN);   
}

void LED_toggle(void)
{
	PORTB ^= (1 << LED_PIN);    
}