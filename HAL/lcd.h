/*
 * lcd.h
 *
 * LCD driver for HD44780 in 4-bit mode
 */

#ifndef LCD_H_
#define LCD_H_

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

#define LCD_PORT PORTD
#define LCD_DDR  DDRD

#define LCD_RS PD2
#define LCD_EN PD3
#define LCD_D4 PD4
#define LCD_D5 PD5
#define LCD_D6 PD6
#define LCD_D7 PD7

void LCD_init(void);
void LCD_sendCommand(uint8_t cmd);
void LCD_sendChar(char data);
void LCD_sendString(const char *str);
void LCD_printNumber(uint8_t num);
void LCD_setCursor(uint8_t row, uint8_t col);
void LCD_clear(void);

#endif /* LCD_H_ */
