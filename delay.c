#include "msp432.h"
#include "delay.h"

int counti;

void delay_longer(int n){
    for(counti=0;counti<n;counti++){
        delay_ms(300,CURRENT_FREQ);
    }
}
void delay_ms(int n, int freq)
{
    for (counti = 0; counti < 7 * n * freq; counti++);
}

void delay_us(int n, int freq)
{
    int limit = (n * freq / 110) - 4;
    for (counti = 0; counti < limit; counti++)
    {
        //empty loop
    }

    return;
}

void set_DCO(int freq)
{
    CS->KEY = CS_KEY_VAL;
    CS->CTL0 = 0;

    switch (freq)
    {
    case FREQ_1_5_MHZ:
        CS->CTL0 = CS_CTL0_DCORSEL_0;
        break;
    case FREQ_3_MHZ:
        CS->CTL0 = CS_CTL0_DCORSEL_1;
        break;
    case FREQ_6_MHZ:
        CS->CTL0 = CS_CTL0_DCORSEL_2;
        break;
    case FREQ_12_MHZ:
        CS->CTL0 = CS_CTL0_DCORSEL_3;
        break;
    case FREQ_24_MHZ:
        CS->CTL0 = CS_CTL0_DCORSEL_4;
        break;
    default:
        break;
    }

    CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS_3 | CS_CTL1_SELM_3;
    CS->KEY = 0;
}
