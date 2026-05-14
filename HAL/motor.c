/*
 * motor.c
 *
 * Created: 19/04/2026 10:58:59 PM
 *  Author: Alaa
 */ 

#include <avr/io.h>
#include "motor.h"


 void motor_init(void)
{
	DDRB |= (1 << MOTOR_IN1) | (1 << MOTOR_IN2);
}

 void motor_forward(void)
{
	PORTB |=  (1 << MOTOR_IN1);
	PORTB &= ~(1 << MOTOR_IN2);
}

 void motor_stop(void)
{
	PORTB &= ~(1 << MOTOR_IN1);
	PORTB &= ~(1 << MOTOR_IN2);
}