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

#define BD99954_ADDRESS 0x09
#define BM92A_ADDRESS 0x18

int main(void)
{
    __disable_irq();


    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;       // Stop watchdog timer
    InitI2C();
    LCD_init();
    LCD_command(0x01); // clear screen, move cursor home
    LCD_word("Select Mode");
    displayMode();

    terminal_init();    //SDA -> P1.6 SCL->P1.7
    unsigned char readBack[30];  //Temp Storage of registers
    interruptPinInit();
    unsigned short alertStatus;
    unsigned int PDO = 0, RDO = 0;
    int i;
//    unsigned char PDORegisters[28];
    __enable_irq();                           // Enable global interrupt
//    write_word(0x2E,BM92A_ADDRESS,0xFFFF);
//    WriteRead(0x2E,BM92A_ADDRESS,2,readBack);  //Alert Enable
//    alertStatus = two_byteOrg(readBack);
//    WriteRead(0x02,BM92A_ADDRESS,2,readBack);  //Alert Enable
//    alertStatus = two_byteOrg(readBack);

    WriteRead(0x02,BM92A_ADDRESS,2,readBack);  //Alert Enable


    while(1)
    {
/*
        if(plugAlertFlag == 1)
        {
//            InitI2C();
            delay_ms(260,CURRENT_FREQ);

            terminal_transmitWord("Start read\r\n");
            PJ -> OUT |= 0x0C;

            WriteRead(0x02,BM92A_ADDRESS,2,readBack);
            PJ -> OUT &= ~0x0C;

            alertStatus = two_byteOrg(readBack);
            for(i=0; i<=16; i++)
            {
                if(alertStatus & 0x1)
                {
                    terminal_transmitChar('1');
                }
                else
                {
                    terminal_transmitChar('0');
                }
                alertStatus = alertStatus >> 1;
            }
            terminal_transmitWord("\r\n");
            terminal_transmitWord("End read \r\n");
            plugAlertFlag = 0;

        }
*/
        if(cursorFlag ==1)
        {
            displayMode();
        }
        if(plugAlertFlag == 1)
        {
            WriteRead(0x02,BM92A_ADDRESS,2,readBack);  //Alert Enable

        }
        __sleep();      // go to lower power mode

    }






}
