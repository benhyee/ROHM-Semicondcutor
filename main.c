
#include <BM92AI2C.h>
#include "msp.h"
#include "delay.h"
#include <time.h>
#include <stdint.h>

#define BM92A_ADDRESS1 0x1A
#define BM92A_ADDRESS2 0x18
#define BD99954_ADDRESS 0x09
#define CURRENT_FREQ FREQ_24_MHZ
int main(void)
{
    unsigned int i;
    InitBM92A();
    __enable_irq();                           // Enable global interrupt
    unsigned short storeVal = 0;
    while(1)
    {
        WriteBM92A(BM92A_ADDRESS2,0x05);

    }
}
