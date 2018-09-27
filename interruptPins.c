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
char upFlag = 0, downFlag = 0, cursorFlag = 0;
char volt_set_cnt = 0;
char settings_menu_Count = 0;
char bat_setting = 1;
char sysToggle = FALSE;
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
void displayMode()
{
    LCD_enter();
    LCD_clearLine();
    switch(settings_menu_Count)
    {
        case 0:
            LCD_word("Snk Mode");
            voltageDisplay();

            break;
        case 1:
            LCD_word("Src Mode");
            voltageDisplay();
            break;
        case 2:
            LCD_word("Bat Config");
            batteryDisplay();
            break;

        case 3:
            if(sysToggle == TRUE)
            {
                LCD_word("Enable");
            }
            if(sysToggle == FALSE)
            {
                LCD_word("Disabled");
            }
            break;

        case 4:
            LCD_word("Diagnostic");
            break;


        default:
            break;

    }
    cursorFlag = 0;
}
void voltageDisplay()
{
    switch(volt_set_cnt)
    {
        case 0:
            LCD_word("   5V->");
            break;
        case 1:
            LCD_word("<- 9V->");
            break;
        case 2:
            LCD_word("<-12V->");
            break;
        case 3:
            LCD_word("<-15V->");
            break;
        case 4:
            LCD_word("<-20V  ");
            break;
        default:
            break;
    }
}
void batteryDisplay()
{
    switch(bat_setting)
    {
    case 1:
        LCD_word("  1s->");
        break;
    case 2:
        LCD_word("<-2s->");

        break;
    case 3:
        LCD_word("<-3s->");

        break;
    case 4:
        LCD_word("  4s->");
        break;
    }
}
void PORT1_IRQHandler(void){
    if (JOYCON1->IFG & 0x10)
    {
        settings_menu_Count += 1;
        terminal_transmitWord("UP");
        if(settings_menu_Count > 4)
        {
            settings_menu_Count = 4;
        }
        JOYCON1->IFG &= ~0xFF;

    }
    else if(JOYCON1->IFG & 0x20)
    {
        settings_menu_Count -= 1;
        terminal_transmitWord("LEFT");

        if(settings_menu_Count <= 0)
        {
            settings_menu_Count = 0;
        }

        JOYCON1->IFG &= ~0xFF;
    }
    cursorFlag = 1;


}
void PORT6_IRQHandler(void){
    if (JOYCON2->IFG & 0x80)
       {
           settings_menu_Count += 1;
           terminal_transmitWord("RIGHT");

           if(settings_menu_Count > 4)
           {
               settings_menu_Count = 4;
           }
           JOYCON2->IFG &= ~0xFF;

       }
    else if (JOYCON2->IFG & 0x40)
       {
           settings_menu_Count -= 1;
           terminal_transmitWord("DOWN");

           if(settings_menu_Count > 4)
           {
               settings_menu_Count = 4;
           }
           JOYCON2->IFG &= ~0xFF;

       }
}

void PORT3_IRQHandler(void){
    if (JOYCONPB->IFG & 0x20)
    {
        terminal_transmitWord("Push\r\n");

    }
    else if (JOYCONPB->IFG & 0x01)
    {
          terminal_transmitWord("Plug Insert\r\n");
    }
    plugAlertFlag = 1;

    JOYCONPB->IFG &= ~0xFF;

}

