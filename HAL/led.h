/*
 * LED.h
 *
 * Created: 17/04/2026 5:20:32 PM
 *  Author: Alaa
 */ 


#ifndef LED_H_
#define LED_H_

#define LED_PIN	 PB0    

void LED_init(void);
void LED_on(void);
void LED_off(void);
void LED_toggle(void);


#endif /* LED_H_ */