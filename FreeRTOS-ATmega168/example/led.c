/*
File: led.c
Author: Adam Rudzinski

A simple example documenting the use of FreeRTOS port for ATmega168.
(led handling - code file)

Use it as you wish, at your own risk.
*/


#include <avr/interrupt.h>
#include <avr/io.h>
#include <led.h>

void init_LEDs()
{
//LED1 - PD6
    DDRD |= _BV(PD6);
//LED2 - PD5
    DDRD |= _BV(PD5);
}

void init_tmr0()
{
    TIMSK0 = _BV(OCIE0A);
    TCCR0A = _BV(WGM01);
    TCCR0B = 0;
    OCR0A = 0xf0;
    TCCR0B |= _BV(CS02) | _BV(CS00);
}

void LED(const unsigned char n, const LED_st st)
{
    const unsigned char p = ( n == 1 ) ? PD6 : PD5;
    if( st == LED_ON )
    {
	PORTD &= ~_BV(p);
    }
    else
    {
	PORTD |= _BV(p);
    }
}

void LED_toggle(const unsigned char n)
{
    const unsigned char p = ( n == 1 ) ? PD6 : PD5;
    PORTD ^= _BV(p);
}

volatile uint8_t led_cnt = INIT_LED_CNT;

ISR(TIMER0_COMPA_vect)
{
    if( led_cnt )
    {
	--led_cnt;
    }
}
