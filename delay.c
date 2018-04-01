#include "msp.h"
#include "delay.h"

void delay_ms(int n, int freq)
{
    int i;
    for (i = 0; i < 7 * n * freq; i++);
}

void delay_us(int n, int freq)
{
    int i;
    int limit = (n * freq / 110) - 4;
    for (i = 0; i < limit; i++)
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
