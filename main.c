
#include <BM92AI2C.h>
#include "msp432.h"
#include "delay.h"
#include <time.h>
#include <stdint.h>
#include "UART.h"


#define BD99954_ADDRESS 0x09
#define CURRENT_FREQ FREQ_24_MHZ

int main(void)
{

    unsigned int i = 0;
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;       // Stop watchdog timer
    InitBM92A();
    terminal_init();

    __enable_irq();                           // Enable global interrupt
    while(1)
    {


        testReadRegisters();
        for(i = 0; i<40; i++);



    }





    __sleep();      // go to lower power mode

}
