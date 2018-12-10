#include "msp432.h"
#include "delay.h"
#include <string.h>
#include "BM92A_funcs.h"
#include "BD99954_Funcs.h"
#include "globals.h"
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
int k;
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

}

void LCD_data(unsigned char command)
{

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

    LCDCNTRL->DIR |= RS| EN | RW;
    LCDPORT->DIR = 0xFF;
    delay_ms(30,CURRENT_FREQ);
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
    for (k = 0;  word[k] != 0; k++)
    {
        LCD_data(word[k]);
    }
}
void LCD_clearLine()
{
    LCD_enter();
    LCD_word("                ");
    LCD_enter();
}
//Prints out negotiated PDO onto LCD
void LCD_PDO(unsigned short voltage, unsigned short current){
    if(voltage == 0 || current == 0)
    {
        LCD_word("No Negotiation");
    }
    if((voltage /10)%10 != 0) LCD_data(((voltage /10)%10)+48);  //if integer is more that one digits
    LCD_data((voltage%10)+48);
    LCD_word("V  ");
    if(((current/1000)%10) != 0){LCD_data(((current/1000)%10)+48);}
    LCD_data(((current/100)%10)+48);
    LCD_data(((current/10)%10)+48);
    LCD_data((current%10)+48);
    LCD_word("mA");
}
//Monitors the ACP voltage
void LCD_Voltage(unsigned short voltage) {
    LCD_command(0x01); // clear screen, move cursor home
    LCD_word("Voltage: ");

    LCD_data(((voltage /1000))+48);
    LCD_data('.');

    LCD_data(((voltage /100)%10)+48);
    LCD_data(((voltage /10)%10)+48);
    LCD_data('V');
}
void LCD_Monitor(int lineVoltage, int lineCurrent){
    LCD_command(0x02);
    LCD_enter();

    if((lineVoltage /10000)%10 != 0)LCD_data( ((lineVoltage/10000)%10)+48);
    LCD_data( ((lineVoltage/1000)%10)+48);
    LCD_word(".");
    LCD_data( ((lineVoltage/100)%10)+48);
    LCD_data( ((lineVoltage/10)%10)+48);
    LCD_word("V  ");


    if((lineCurrent /1000)%10 != 0){
        LCD_data( ((lineCurrent/1000)%10)+48);
        LCD_data( ((lineCurrent/100)%10)+48);
        LCD_data( ((lineCurrent/10)%10)+48);
    }
    else if((lineCurrent /100)%10 != 0){
        LCD_word(" ");
        LCD_data( ((lineCurrent/100)%10)+48);
        LCD_data( ((lineCurrent/10)%10)+48);
    }
    else if((lineCurrent /10)%10 != 0){
        LCD_word("  ");
        LCD_data( ((lineCurrent/10)%10)+48);
   }
    else{
        LCD_word("   ");
    }
    LCD_data( ((lineCurrent/1)%10)+48);
    LCD_word(" mA");

}
void LCD_charge_error(){
    LCD_command(0x01); // clear screen, move cursor home
    LCD_word("Charging only");
    LCD_enter();
    LCD_word("for Sink Mode");
}
void LCD_fastSnkSetPDO()
{
    switch(sink_set)
    {

        case 1:
            LCD_word("        5V (3A)");
            break;
        case 2:
            LCD_word("    5V- 9V (3A)");
            break;
        case 3:
            LCD_word("    5V-15V (3A)");
            break;
        case 4:
            LCD_word("    5V-20V (3A)");
            break;
        default:
            break;
    }
}
void LCD_fastSrcSetPDO()
{
    switch(source_set)
    {
        case 1:
            LCD_word("        5V (3A)");
            break;
        case 2:
            LCD_word("        9V (3A)");
            break;
        case 3:
            LCD_word("        12V (3A)");
            break;
        case 4:
            LCD_word("        15V (3A)");
            break;
        case 5:
            LCD_word("        20V (3A)");
            break;
        case 6:
            LCD_word("    5V-20V (3A)");
            break;
        default:
            break;
    }
}

