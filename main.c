/*
 * ROHM SemiConductor USB-C Project
 *  BM92AI2C.h -> All the BM92A code as well as a register read function for testing
 *  BD99954.h -> Not completed
 *  delay.h -> Frequency set and delay functions
 *  helper.h -> Functions that are helpful for organization and exception handling
 *  LCD.h -> Functions for the LCD monitor.
 *  UART.h -> Functions for terminal use on the computer
 *
 */
#include <BM92AI2C.h>
#include "msp432.h"
#include "delay.h"
#include <time.h>
#include <stdint.h>
#include "UART.h"
#include "helper.h"
#include "interruptPins.h"
#include <stdlib.h>
#include "globals.h"


#define BD99954_ADDRESS 0x09
#define CURRENT_FREQ FREQ_24_MHZ
#define BM92A_ADDRESS 0x18

int main(void)
{
    __disable_irq();


    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;       // Stop watchdog timer
    InitBM92A();
    terminal_init();    //SDA -> P1.6 SCL->P1.7
    unsigned char readBack[30];  //Temp Storage of registers
    interruptPinInit();
    unsigned int PDO = 0, RDO = 0;
    int i;
    unsigned char PDORegisters[28];

//    CommandRegister(0x0909,BM92A_ADDRESS);
    __enable_irq();                           // Enable global interrupt
//    write_word(0x2F,BM92A_ADDRESS,0xA401);
//    write_word(0x2F,BM92A_ADDRESS,0xA400);

    testReadRegisters();

    while(1)
    {
        if(plugAlertFlag ==1)
        {

            BM92A_Debugger();
            for(i = 0; i < 400; i++);
            BM92A_Debugger();
            plugAlertFlag = 0;
        }

        __sleep();      // go to lower power mode

    }






}
