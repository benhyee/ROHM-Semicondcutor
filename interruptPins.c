/*
 * interruptPins.c
 *
 *  Created on: May 10, 2018
 *      Author: User
 */
#include "msp.h"
#include "globals.h"
#include "lcd.h"
#include "delay.h"
#include "UART.h"
#include "I2C_Helper.h"


#define TRUE 1
#define FALSE 0
#define JOYCON1 P1
#define JOYCONPB P3
#define JOYCON2 P6
#define CURRENT_FREQ FREQ_12_MHZ

volatile char plugAlertFlag = 0;

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
//    P2-> SEL1 &= ~7;
//    P2-> SEL0 &= ~7;
//    P2->DIR |= 7;
//    P2->OUT &= ~7;

//    P3 -> SEL1 &= ~0x60;
//    P3 -> SEL0 &= ~0x60;
//    P3 -> REN |= 0x60;
//    P3 -> DIR &= ~0x60;
//    P3 -> IES = 0x60;

    JOYCON1 -> IFG = 0;
    JOYCON1 -> IE |= 0x30;
    JOYCON2 -> IFG = 0;
    JOYCON2 -> IE |= 0xC0;
    JOYCONPB -> IFG = 0;
    JOYCONPB -> IE |= 0x21;
//    P2 -> IFG = 0;
//    P2 -> IE |= 0x07;
//    P3 -> IFG = 0;
//    P3 -> IE |= 0x60;

    NVIC_SetPriority(PORT1_IRQn, 5);
    NVIC_EnableIRQ(PORT1_IRQn);
    NVIC_SetPriority(PORT3_IRQn, 5);
    NVIC_EnableIRQ(PORT3_IRQn);
    NVIC_SetPriority(PORT6_IRQn, 5);
    NVIC_EnableIRQ(PORT6_IRQn);

//    NVIC_SetPriority(PORT2_IRQn, 6);
//    NVIC_EnableIRQ(PORT2_IRQn);

//    NVIC_SetPriority(PORT3_IRQn, 7);
//    NVIC_EnableIRQ(PORT3_IRQn);

}
void PORT1_IRQHandler(void){
    if (JOYCON1->IFG & 0x10)
    {
        terminal_transmitWord("Up");
        if(settings_menu_Count == 0)
        {
            settings_menu_Count = 1;
        }
        settings_menu_Count -= 1;

        JOYCON1->IFG &= ~0xFF;

    }
    else if(JOYCON1->IFG & 0x20)
    {
        terminal_transmitWord("Left");
        if(volt_set_cnt == 0)
        {
            volt_set_cnt = 1;
        }
        volt_set_cnt -= 1;

        JOYCON1->IFG &= ~0xFF;
    }
    cursorFlag = 1;


}
void PORT6_IRQHandler(void){
   if (JOYCON2->IFG & 0x80)
   {
       terminal_transmitWord("Right");
       if(volt_set_cnt == 4)
       {
           volt_set_cnt = 3;
       }
       volt_set_cnt += 1;

       JOYCON2->IFG &= ~0xFF;

   }
   else if (JOYCON2->IFG & 0x40)
   {
       terminal_transmitWord("Down");
       if(settings_menu_Count == 4)
       {
           settings_menu_Count = 3;
       }
       settings_menu_Count += 1;

       JOYCON2->IFG &= ~0xFF;
   }
    cursorFlag = 1;

}

void PORT3_IRQHandler(void){
    if (JOYCONPB->IFG & 0x20)
    {
        terminal_transmitWord("Push\r\n");
        select ^= 1;
        cursorFlag = 1;

    }
    else if (JOYCONPB->IFG & 0x01)
    {
          terminal_transmitWord("Plug Insert\r\n");
          plugAlertFlag = 1;

    }


    JOYCONPB->IFG &= ~0xFF;

}

