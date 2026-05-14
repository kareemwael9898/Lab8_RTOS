/*
 * LM35.c
 *
 * Created: 19/04/2026 10:54:17 PM
 *  Author: Alaa
 */ 


#include <avr/io.h>
#include "lm35.h"

#define LM35_ADC_CHANNEL  0    /* PF0 = ADC channel 0 */


void LM35_init(void)
{
    /* AVcc as reference voltage (5V) */
    ADMUX = (1 << REFS0);

    /* Enable ADC, prescaler = 128 (8MHz/128 = 62.5kHz) */
    ADCSRA = (1 << ADEN)  |
             (1 << ADPS2) |
             (1 << ADPS1) |
             (1 << ADPS0);
}

uint8_t LM35_read(void)
{
    /* Select ADC channel 0 (PF0) */
    ADMUX = (ADMUX & 0xE0) | (LM35_ADC_CHANNEL & 0x1F);

    /* Start conversion */
    ADCSRA |= (1 << ADSC);

    /* Wait until conversion is complete */
    while (ADCSRA & (1 << ADSC));

    /* Convert ADC value to Celsius: (ADC * 500) / 1024 */
    return (uint8_t)((ADC * 500UL) / 1024UL);
}