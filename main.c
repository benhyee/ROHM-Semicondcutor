/*
 * ROHM SemiConductor USB-C Project
 *  BM92AI2C.h -> All the BM92A code as well as a register read function for testing
 *  BD99954.h -> Not completed
 *  delay.h -> Frequency set and delay functions
 *  helper.h -> Functions that are helpful for organization and exception handling
 *  LCD.h -> Functions for the LCD monitor.
 *  UART.h -> Functions for terminal use on the computer COM4 19200
 *
 *  Pin outs:
 *  P1.6 -> SDA;    P1.7->SCL
 *  P3.6-> GPIO0(#Alert) BM92A
 *  P4.4-4.7 -> D4-D7 LCD
 *  P4.0 -> RS;     P4.1 -> RW;     P4.2->EN (LCD)
 *
 */
#include "I2C_Helper.h"
#include "msp432.h"
#include <time.h>
#include <stdint.h>
#include "UART.h"
#include "helper.h"
#include "interruptPins.h"
#include <stdlib.h>
#include "globals.h"
#include "lcd.h"
#include "BD99954_Funcs.h"
#include "BM92A_Funcs.h"
#include "menu.h"
#include "delay.h"
#include "debugFunctions.h"

#define BD99954_ADDRESS 0x09
#define BM92A_ADDRESS 0x18
#define CURRENT_FREQ FREQ_3_MHZ

int main(void)
{
    __disable_irq();

    set_DCO(CURRENT_FREQ);
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;       // Stop watchdog timer
    InitI2C();
    LCD_init();
    LCD_command(0x01); // clear screen, move cursor home

    displayMode();  //Display Text on screen

    terminal_init();    //SDA -> P1.6 SCL->P1.7

    interruptPinInit();
    __enable_irq();                           // Enable global interrupt
    unsigned char readBack[30];  //Temp Storage of registers
    unsigned short alertRead;
    while(1)
    {
        if(cursorFlag == TRUE)
        {
            if(rightFlag == TRUE)menuScroll(1);
            if(leftFlag == TRUE)menuScroll(-1);
            displayMode();

        }
        if(plugAlertFlag)
        {
            alertRead = readTwoByte(0x02,BM92A_ADDRESS);
            terminal_transmitWord("Ngt");
            plugAlertFlag = FALSE;
        }
        __sleep();      // go to lower power mode

    }


}
