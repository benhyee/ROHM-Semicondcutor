/*
 * GPIO.c
 *
 *  Created on: Nov 12, 2018
 *      Author: User
 */
#include "msp432.h"
#include "BD99954_Funcs.h"
#define LCDPORT P5
#define LCDCNTRL P7
#define CONFIG P4
#define LED P2
#define BD22 P8
void gpio_init()
{

    LCDCNTRL ->SEL1 &= ~0x08; //5V Enable
    LCDCNTRL ->SEL0 &= ~0x08;
    LCDCNTRL ->DIR |= 0x08;
    LCDCNTRL -> OUT &= ~0x08;

    CONFIG ->SEL1 &= ~0xF0;  //DIP Switch readback
    CONFIG ->SEL0 &= ~0xF0;
    CONFIG ->DIR &= ~0xF0;
    CONFIG ->REN &= ~0xF0;

    BD22 -> SEL1 &= ~0x03;    //LCD Enable
    BD22 -> SEL0 &= ~0x03;
    BD22 -> DIR |= 0x03;
    LCDCNTRL->OUT|= 0x08;
    BD22->OUT|= 0x03;

    LED ->SEL1 &= ~0x0F; //LED Enable
    LED ->SEL0 &= ~0x0F;
    LED ->DIR |= 0x0F;
    LED -> OUT &= ~0x0F;

}
void LCD_wake(){
    BD22->OUT |= 0x03;
    LCDCNTRL->DIR |= 0x08;
    LCDPORT->DIR |= 0xFF;
    LCDCNTRL->OUT|= 0x08;
    LCDPORT->OUT |= 0xFF;

}
void LCD_toggle()
{
    BD22->OUT ^= 0x03;
    LCDCNTRL->DIR ^= 0x08;
    LCDPORT->DIR ^= 0xFF;
}
void sleepMode(){
    P3 ->SEL1 &= ~0x42;//LTC2953 kill & BM92A15_XRST
    P3 ->SEL0 &= ~0x42;
    P3 ->DIR |= 0x42;
    P3 -> OUT &= ~0x40;
}

void BD99954_reset()
{

    P1 -> SEL1 &= ~0x02;//BD99954_RESET
    P1 -> SEL0 &= ~0x02;
    P1 -> DIR |= 0x02;

}
char readSwitchState(){
    char sw_state = 0xF0;
    sw_state &= P4->IN;
    sw_state = sw_state >> 4;
    return sw_state;
}
void chargeState(){
    if(readSwitchState()&0x01)
    {
        chgDisable();
        chgEnable();
    }
    else{
        chgDisable();
    }
}

