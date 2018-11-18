/*
 * GPIO.c
 *
 *  Created on: Nov 12, 2018
 *      Author: User
 */
#include "msp432.h"

void gpio_init()
{

    P7 ->SEL1 &= ~0x08; //5V Enable
    P7 ->SEL0 &= ~0x08;
    P7 ->DIR |= 0x08;
    P7 -> OUT &= ~0x08;
    P8 -> SEL1 &= ~0x03;    //LCD Enable
    P8 -> SEL0 &= ~0x03;
    P8 -> DIR |= 0x03;
    P7->OUT|= 0x08;
    P8->OUT|= 0x03;
}

void sleepMode(){
    P3 ->SEL1 &= ~0x42;//LTC2953 kill & BM92A15_XRST
    P3 ->SEL0 &= ~0x42;
    P3 ->DIR |= 0x42;
    P3 -> OUT &= ~0x42;
    P3 -> OUT |= 0x40;
}

void BD99954_reset()
{

    P1 -> SEL1 &= ~0x02;//BD99954_RESET
    P1 -> SEL0 &= ~0x02;
    P1 -> DIR |= 0x02;

}

