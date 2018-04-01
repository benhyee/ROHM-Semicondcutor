#include "msp.h"
#include "delay.h"
#include <string.h>
#define RS 1
#define RW 2
#define EN 4

#define LCDPORT P4

void LCD_nibble_write(unsigned char data, unsigned char control)
{
    data &= 0xF0; //clear lower nibble for control
    control &= 0x0F; //clear upper nibble for data
    LCDPORT->OUT = data | control; //RS = 0, RW = 0
    LCDPORT->OUT = data | control | EN; //set EN
    delay_ms(0, CURRENT_FREQ);
    LCDPORT->OUT = data; //clear EN
    LCDPORT->OUT = 0;
}

void LCD_command(unsigned char command)
{
    LCD_nibble_write(command & 0xF0, 0);
    LCD_nibble_write(command << 4, 0);

    if (command < 4)
    {
        delay_ms(4, CURRENT_FREQ);
    }
    else
    {
        delay_ms(1, CURRENT_FREQ);
    }
}

void LCD_data(unsigned char command)
{
    LCD_nibble_write(command & 0xF0, RS);
    LCD_nibble_write(command << 4, RS);

    delay_ms(1, CURRENT_FREQ);
}

void LCD_enter()
{
    LCD_command(0xC0);
}

void LCD_init()
{
    LCDPORT->DIR = 0xFF;
    delay_ms(30, CURRENT_FREQ);
    LCD_nibble_write(0x30, 0);
    delay_ms(10, CURRENT_FREQ);
    LCD_nibble_write(0x30, 0);
    delay_ms(1, CURRENT_FREQ);
    LCD_nibble_write(0x30, 0);
    delay_ms(1, CURRENT_FREQ);
    LCD_nibble_write(0x20, 0);
    delay_ms(1, CURRENT_FREQ);

    LCD_command(0x28); //set 4-bit data, 2-line, 5x7 font
    LCD_command(0x06); //move cursor right after each char
    LCD_command(0x01); // clear screen, move cursor home
    LCD_command(0x0F); // turn on display, cursor blinking
}

void LCD_word(char *word)
{
    int i;
    for (i = 0;  word[i] != 0; i++)
    {
        LCD_data(word[i]);
    }
}
