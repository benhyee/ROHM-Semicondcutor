#ifndef DELAY_H
#define DELAY_H

#include "msp.h"

#define FREQ_1_5_MHZ 15
#define FREQ_3_MHZ 30
#define FREQ_6_MHZ 60
#define FREQ_12_MHZ 120
#define FREQ_24_MHZ 240

#define CURRENT_FREQ FREQ_24_MHZ

void delay_ms(int n, int freq);
void delay_us(int n, int freq);
void set_DCO(int freq);

#endif
