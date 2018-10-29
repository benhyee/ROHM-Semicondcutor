/*
 * interruptPins.c
 *
 *  Created on: May 10, 2018
 *      Author: User
 */
#include "msp.h"
#include "globals.h"
#include "lcd.h"
#include "UART.h"
#include "I2C_Helper.h"
#include "menu.h"

#define TRUE 1
#define FALSE 0
#define JOYCON1 P1
#define JOYCONPB P3
#define JOYCON2 P6

volatile char plugAlertFlag = 0;
char select = 0, rightFlag = 0, leftFlag = 0;
unsigned char readback;
void interruptPinInit()
{
    JOYCON1 -> SEL1 &= ~0x30;
    JOYCON1 -> SEL0 &= ~0x30;
    JOYCON1 -> DIR &= ~0x30;
    JOYCON1 -> OUT |= 0x30;
    JOYCON1 -> REN |= 0x30;
    JOYCON1 -> IES |= 0x30;

    JOYCON2 -> SEL1 &= ~0xC0;
    JOYCON2 -> SEL0 &= ~0xC0;
    JOYCON2 -> DIR &= ~0xC0;
    JOYCON2 -> OUT |= 0xC0;
    JOYCON2 -> REN |= 0xC0;
    JOYCON2 -> IES |= 0xC0;

    JOYCONPB -> SEL1 &= ~0x21;
    JOYCONPB -> SEL0 &= ~0x21;
    JOYCONPB -> DIR &= ~0x21;
    JOYCONPB -> OUT |= 0x21;
    JOYCONPB -> REN |= 0x21;
    JOYCONPB -> IES |= 0x21;


    JOYCON1 -> IFG = 0;
    JOYCON1 -> IE |= 0x30;
    JOYCON2 -> IFG = 0;
    JOYCON2 -> IE |= 0xC0;
    JOYCONPB -> IFG = 0;
    JOYCONPB -> IE |= 0x21;


    NVIC_SetPriority(PORT1_IRQn, 5);
    NVIC_EnableIRQ(PORT1_IRQn);
    NVIC_SetPriority(PORT3_IRQn, 5);
    NVIC_EnableIRQ(PORT3_IRQn);
    NVIC_SetPriority(PORT6_IRQn, 5);
    NVIC_EnableIRQ(PORT6_IRQn);


}
void PORT1_IRQHandler(void){
    if (JOYCON1->IFG & 0x10)    //Up
    {
        terminal_transmitWord("Up");
        settings_menu -= 1;
        if(select == 1)
        {
            settings_menu += 1;
        }
        JOYCON1->IFG &= ~0xFF;

    }
    else if(JOYCON1->IFG & 0x20)//Left
    {
        terminal_transmitWord("Left");
        JOYCON1->IFG &= ~0xFF;
        leftFlag = TRUE;
    }
    cursorFlag = TRUE;


}
void PORT6_IRQHandler(void){
   if (JOYCON2->IFG & 0x80)//Right
   {
       terminal_transmitWord("Right");
       JOYCON2->IFG &= ~0xFF;
       rightFlag = TRUE;

   }
   else if (JOYCON2->IFG & 0x40)//Down
   {
       terminal_transmitWord("Down");
       settings_menu += 1;
       if(select == 1)
       {
           settings_menu -= 1;
       }
       JOYCON2->IFG &= ~0xFF;
   }
   cursorFlag = TRUE;

}

void PORT3_IRQHandler(void){
    if (JOYCONPB->IFG & 0x20)
    {
        terminal_transmitWord("Push\r\n");
        select += 1;
        cursorFlag = 1;


    }
    else if (JOYCONPB->IFG & 0x01)
    {
          terminal_transmitWord("Plug Insert\r\n");
          plugAlertFlag = TRUE;
    }


    JOYCONPB->IFG &= ~0xFF;

}

