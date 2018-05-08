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


#define BD99954_ADDRESS 0x09
#define CURRENT_FREQ FREQ_24_MHZ
#define BM92A_ADDRESS1 0x1A
#define BM92A_ADDRESS2 0x18
int main(void)
{

    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;       // Stop watchdog timer
    InitBM92A();
    terminal_init();    //SDA -> P1.6 SCL->P1.7
    unsigned char *readBack = malloc(sizeof(char)*30);  //Temp Storage of registers

    unsigned int value=0, RDO = 0, PDO = 0;
    int i;
    CommandRegister(0x0909,BM92A_ADDRESS2);
    __enable_irq();                           // Enable global interrupt
    while(1)
    {
        WriteReadBM92A(0x28,BM92A_ADDRESS2,5,readBack);//PDO register
        PDO = four_byteOrg(readBack);

        if(PDO !=0)
        {
            BM92A_Debugger(PDO);

        }


        for(i = 0; i<40; i++);



    }





    __sleep();      // go to lower power mode

}
