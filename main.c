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
#define BM92A_ADDRESS1 0x1A
#define BM92A_ADDRESS2 0x18

int main(void)
{
    __disable_irq();


    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;       // Stop watchdog timer
    InitBM92A();
    terminal_init();    //SDA -> P1.6 SCL->P1.7
    unsigned char readBack[30];  //Temp Storage of registers
    interruptPinInit();
    unsigned int PDO = 0;
    int i;
//    CommandRegister(0x0909,BM92A_ADDRESS2);
    __enable_irq();                           // Enable global interrupt
    while(1)
    {

        if(plugAlertFlag ==1)
        {
            WriteReadBM92A(0x28,BM92A_ADDRESS2,5,readBack);//PDO register
            PDO = four_byteOrg(readBack);

            BM92A_Debugger(PDO);

        }





    }





    __sleep();      // go to lower power mode

}
