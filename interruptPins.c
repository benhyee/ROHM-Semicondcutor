/*
 * interruptPins.c
 *
 *  Created on: May 10, 2018
 *      Author: User
 */
#include "msp.h"
#include "globals.h"

char plugAlertFlag = 0;

void interruptPinInit()
{
    P3 -> SEL1 &= ~0x40;
    P3 -> SEL0 &= ~0x40;
    P3 -> DIR &= ~0x40;
    P3 -> IES = 0x40;

    P3 -> IFG = 0;
    P3 -> IE |= 0x40;
    P2-> SEL1 &= ~7;
    P2-> SEL0 &= ~7;
    P2->DIR |= 7;
    P2->OUT &= ~7;
    NVIC_SetPriority(PORT3_IRQn, 5);
    NVIC_EnableIRQ(PORT3_IRQn);

}

void PORT3_IRQHandler(void){
    plugAlertFlag = 1;
    P2->OUT ^= 0x02;
    P3->IFG &= ~0x40;

}
