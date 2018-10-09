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
        if(select == 0)
        {
            settings_menu_Count -= 1;
        }
        else if(select == 1)
        {
            switch(settings_menu_Count)
            {
            case 1:
                mode_set ^= 1;
               break;
           case 2:
               volt_set_cnt += 1;
               break;
           case 3:
               diagnosticToggle ^= 1;
               break;
           case 4:
               sysToggle ^= 1;
               break;
           default:
               break;
            }
        }
        JOYCON1->IFG &= ~0xFF;

    }
    else if(JOYCON1->IFG & 0x20)//Left
    {
        terminal_transmitWord("Left");
        if(select == 1 && settings_menu_Count == 2)
        {
            curr_set_cnt -= 1;
        }
        JOYCON1->IFG &= ~0xFF;
    }
    cursorFlag = 1;


}
void PORT6_IRQHandler(void){
   if (JOYCON2->IFG & 0x80)//Right
   {
       terminal_transmitWord("Right");
       if(select == 1 && settings_menu_Count == 2)
       {
           curr_set_cnt += 1;
       }
       JOYCON2->IFG &= ~0xFF;

   }
   else if (JOYCON2->IFG & 0x40)//Down
   {
       terminal_transmitWord("Down");
       if(select == 0)  //Main menu
       {

           settings_menu_Count += 1;
       }
       else if(select == 1)
       {
           switch(settings_menu_Count)
           {
           case 1:
               mode_set ^= 1;
              break;
          case 2:
              volt_set_cnt -= 1;
              break;
          case 3:
              diagnosticToggle ^= 1;
              break;
          case 4:
              sysToggle ^= 1;
              break;
          default:
              break;
           }
       }


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
          cursorFlag = 1;

    }


    JOYCONPB->IFG &= ~0xFF;

}

