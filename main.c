
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
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;       // Stop watchdog timer

    unsigned int i;
    set_DCO(CURRENT_FREQ);
    __enable_irq();                           // Enable global interrupt

    InitBM92A(BM92A_ADDRESS2);
    unsigned short storeVal = 0;

    WriteBM92A(BM92A_ADDRESS2,0x4C);
    storeVal = ReadBM92A(BM92A_ADDRESS2);


    __sleep();

}
