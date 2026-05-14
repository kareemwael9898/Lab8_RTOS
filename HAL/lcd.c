/*
 * lcd.c
 *
 * LCD driver implementation
 */

#include "lcd.h"

// Helper function to send half byte
static void LCD_sendHalfByte(uint8_t data) {
    if(data & 1) LCD_PORT |= (1 << LCD_D4); else LCD_PORT &= ~(1 << LCD_D4);
    if(data & 2) LCD_PORT |= (1 << LCD_D5); else LCD_PORT &= ~(1 << LCD_D5);
    if(data & 4) LCD_PORT |= (1 << LCD_D6); else LCD_PORT &= ~(1 << LCD_D6);
    if(data & 8) LCD_PORT |= (1 << LCD_D7); else LCD_PORT &= ~(1 << LCD_D7);

    // EN pulse
    LCD_PORT |= (1 << LCD_EN);
    _delay_us(1);
    LCD_PORT &= ~(1 << LCD_EN);
    _delay_us(200);
}

void LCD_sendCommand(uint8_t cmd) {
    LCD_PORT &= ~(1 << LCD_RS); // RS = 0 for command
    LCD_sendHalfByte(cmd >> 4);
    LCD_sendHalfByte(cmd);
    if (cmd == 0x01 || cmd == 0x02) {
        _delay_ms(2);
    }
}

void LCD_sendChar(char data) {
    LCD_PORT |= (1 << LCD_RS); // RS = 1 for data
    LCD_sendHalfByte(data >> 4);
    LCD_sendHalfByte(data);
}

void LCD_sendString(const char *str) {
    while(*str) {
        LCD_sendChar(*str++);
    }
}

void LCD_printNumber(uint8_t num) {
    if (num >= 100) LCD_sendChar('0' + (num / 100));
    if (num >= 10)  LCD_sendChar('0' + ((num / 10) % 10));
    LCD_sendChar('0' + (num % 10));
}

void LCD_init(void) {
    // Set pins as output
    LCD_DDR |= (1<<LCD_RS)|(1<<LCD_EN)|(1<<LCD_D4)|(1<<LCD_D5)|(1<<LCD_D6)|(1<<LCD_D7);
    
    _delay_ms(50);
    
    // 4-bit initialization
    LCD_PORT &= ~(1 << LCD_RS); // RS = 0
    LCD_sendHalfByte(0x03);
    _delay_ms(5);
    LCD_sendHalfByte(0x03);
    _delay_us(150);
    LCD_sendHalfByte(0x03);
    LCD_sendHalfByte(0x02); // Set 4-bit mode
    
    LCD_sendCommand(0x28); // 2 lines, 5x8 matrix
    LCD_sendCommand(0x0C); // Display on, cursor off
    LCD_sendCommand(0x06); // Increment cursor
    LCD_clear();
}

void LCD_clear(void) {
    LCD_sendCommand(0x01);
}

void LCD_setCursor(uint8_t row, uint8_t col) {
    uint8_t address = col;
    if (row == 1) {
        address += 0x40;
    }
    LCD_sendCommand(0x80 | address);
}
