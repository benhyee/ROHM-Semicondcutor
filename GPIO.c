/*
 * GPIO.c
 *
 *  Created on: Nov 12, 2018
 *      Author: User
 */
#include "msp432.h"

void gpio_init()
{
    P3 ->SEL1 &= ~0x42;//LTC2953 kill & BM92A15_XRST
    P3 ->SEL0 &= ~0x42;
    P3 ->DIR |= 0x42;
    P3 -> OUT &= ~0x42;
    P1 -> SEL1 &= ~0x02;//BD99954_RESET
    P1 -> SEL0 &= ~0x02;
    P1 -> DIR |= 0x02;

}
void sleepMode(){
    P3 -> OUT |= 0x40;
}


