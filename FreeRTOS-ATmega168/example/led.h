/*
File: led.h
Author: Adam Rudzinski

A simple example documenting the use of FreeRTOS port for ATmega168.
(led handling - header file)

Use it as you wish, at your own risk.
*/


#ifndef __LED_H
#define __LED_H

typedef enum {
    LED_ON = 0,
    LED_OFF
} LED_st;

void init_LEDs();

void init_tmr0();

void LED(const unsigned char, const LED_st);

void LED_toggle(const unsigned char);

#define INIT_LED_CNT	80
extern volatile uint8_t led_cnt;

#endif
