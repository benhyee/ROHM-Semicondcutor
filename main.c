/*
 * ROHM SemiConductor USB-C Project
 *  BM92AI2C.h -> All the BM92A code as well as a register read function for testing
 *  BD99954.h -> Not completed
 *  delay.h -> Frequency set and delay functions
 *  helper.h -> Functions that are helpful for organization and exception handling
 *  LCD.h -> Functions for the LCD monitor.
 *  UART.h -> Functions for terminal use on the computer
 *
 *  Pin outs:
 *  P1.6 -> SDA;    P1.7->SCL
 *  P3.6-> GPIO0(#Alert) BM92A
 *  P4.4-4.7 -> D4-D7 LCD
 *  P4.1 -> RS;     P4.2 -> RW;     P4.4->EN (LCD)
 *
 */
#include "I2C_Helper.h"
#include "msp432.h"
#include "delay.h"
#include <time.h>
#include <stdint.h>
#include "UART.h"
#include "helper.h"
#include "interruptPins.h"
#include <stdlib.h>
#include "globals.h"
#include "lcd.h"

#define BD99954_ADDRESS 0x09
#define CURRENT_FREQ FREQ_12_MHZ
#define BM92A_ADDRESS 0x18

int main(void)
{
    __disable_irq();


    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;       // Stop watchdog timer
    set_DCO(CURRENT_FREQ);
    InitI2C();
    LCD_init();
    terminal_init();    //SDA -> P1.6 SCL->P1.7
    unsigned char readBack[30];  //Temp Storage of registers
    interruptPinInit();
    unsigned int PDO = 0, RDO = 0;
    int i;
    unsigned char PDORegisters[28];

    __enable_irq();                           // Enable global interrupt
//    write_word(0x2F,BM92A_ADDRESS,0xA401);
//    testReadRegistersBM92A();
//    BM92A_Debugger();

    BD99954ReadRegister();
    while(1)
    {
        if(plugAlertFlag ==1)
        {
            delay_ms(200,CURRENT_FREQ);
            BM92A_Debugger();
            BM92A_Debugger();

            plugAlertFlag = 0;
        }

        __sleep();      // go to lower power mode

    }






}
