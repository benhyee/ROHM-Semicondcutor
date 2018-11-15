#include "msp432.h"
#include "delay.h"
#include <string.h>
#define RS 0x01
#define RW 0x02
#define EN 0x04
#define LCDPORT P5
#define LCDCNTRL P7
//void LCD_nibble_write(unsigned char data, unsigned char control)
//{
//    data &= 0xF0; //clear lower nibble for control
//    control &= 0x0F; //clear upper nibble for data
//    LCDPORT->OUT = data | control; //RS = 0, RW = 0
//    LCDPORT->OUT = data | control | EN; //set EN
//    delay_us(100, CURRENT_FREQ);
//    LCDPORT->OUT = data; //clear EN
//    LCDPORT->OUT = 0;
//}

void LCD_command(unsigned char command)
{
    LCDCNTRL->OUT &= ~(RS|RW);
    LCDPORT->OUT = command;
    LCDCNTRL->OUT |= EN;
    delay_ms(1,CURRENT_FREQ);
    LCDCNTRL->OUT &= ~EN;

    if (command < 4)
    {
        delay_ms(5, CURRENT_FREQ);
    }
    else
    {
        delay_us(50, CURRENT_FREQ);
    }

//    LCD_nibble_write(command & 0xF0, 0);
//    LCD_nibble_write(command << 4, 0);
//
//    if (command < 4)
//    {
//        delay_ms(4, CURRENT_FREQ);
//    }
//    else
//    {
//        delay_us(50, CURRENT_FREQ);
//    }
}

void LCD_data(unsigned char command)
{
//    LCD_nibble_write(command & 0xF0, RS);
//    LCD_nibble_write(command << 4, RS);
    LCDCNTRL->OUT |= RS;
    LCDCNTRL->OUT &= ~RW;
    LCDPORT->OUT = command;
    LCDCNTRL->OUT |= EN;
    delay_ms(0,CURRENT_FREQ);
    LCDCNTRL->OUT &= ~EN;
    delay_ms(1, CURRENT_FREQ);
}

void LCD_enter()
{
    LCD_command(0xC0);
}

void LCD_init()
{
//    LCDPORT->DIR = 0xFF;
//    delay_ms(100, CURRENT_FREQ);
//    LCD_nibble_write(0x30, 0);
//    delay_ms(30, CURRENT_FREQ);
//    LCD_nibble_write(0x30, 0);
//    delay_ms(10, CURRENT_FREQ);
//    LCD_nibble_write(0x30, 0);
//    delay_ms(10, CURRENT_FREQ);
//    LCD_nibble_write(0x20, 0);
//    delay_ms(10, CURRENT_FREQ);
    LCDCNTRL->DIR |= RS| EN | RW;
    LCDPORT->DIR = 0xFF;
    delay_ms(30,CURRENT_FREQ);

//        delay_ms(100, CURRENT_FREQ);
    LCD_command(0x30);
    delay_ms(30, CURRENT_FREQ);
    LCD_command(0x30);
    delay_ms(10, CURRENT_FREQ);
    LCD_command(0x30);
    delay_ms(10, CURRENT_FREQ);
    LCD_command(0x30);
    delay_ms(10, CURRENT_FREQ);
    LCD_command(0x38); //set 8-bit data, 2-line, 5x7 font
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
void LCD_clearLine()
{
    LCD_enter();
    LCD_word("                ");
    LCD_enter();
}

void LCD_PDO(unsigned short voltage, unsigned short current){
    if((voltage /10)%10 != 0) LCD_data(((voltage /10)%10)+48);  //if integer is more that one digits
    LCD_data((voltage%10)+48);

    LCD_word("V  ");
    LCD_data(((current/1000)%10)+48);
    LCD_data(((current/100)%10)+48);
    LCD_data(((current/10)%10)+48);
    LCD_data((current%10)+48);

    LCD_word("mA");


}

void LCD_Voltage(unsigned short voltage)
{
    LCD_command(0x01); // clear screen, move cursor home
    LCD_word("Voltage: ");

    LCD_data(((voltage /1000))+48);
    LCD_data('.');

    LCD_data(((voltage /100)%10)+48);
    LCD_data(((voltage /10)%10)+48);
    LCD_data('V');
}
