
#include <BM92AI2C.h>
#include "msp432.h"
#include "delay.h"
#include <time.h>
#include <stdint.h>

#define BM92A_ADDRESS1 0x1A
#define BM92A_ADDRESS2 0x18
#define BD99954_ADDRESS 0x09
#define CURRENT_FREQ FREQ_24_MHZ
int main(void)
{
    uint32_t i;
    uint16_t value;
    unsigned char readBack[30];
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;       // Stop watchdog timer
    InitBM92A();

    __enable_irq();                           // Enable global interrupt
    while(1)
    {
        value = WriteReadBM92A(0x4C,BM92A_ADDRESS2,2,readBack);
        for(i = 0; i < 100; i++);
    }





    __sleep();      // go to lower power mode

}
