
#include "msp.h"
#include "delay.h"
#include "BD99954I2C.h"
#include <time.h>
#include <stdint.h>

#define BD99954_ADDRESS 0x50

void InitBD99954(uint8_t DeviceAddress);
void WriteBD99954(uint16_t MemAddress, uint8_t MemByte);
uint8_t ReadBD99954(uint16_t MemAddress);


int main(void)
{
    uint32_t i;
    uint8_t value;

    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;       // Stop watchdog timer

    P2->DIR |= BIT2 | BIT1 | BIT0;                    // Configure LED2
    P2->OUT &= ~(BIT2 | BIT1 | BIT0);

    __enable_irq();                           // Enable global interrupt

    InitBD9954(BD99954_ADDRESS);

    WriteBD99954(0x1122, 0x21);

    for (i = 4000; i > 0; i--);    // Delay for EEPROM write cycle (5 ms)

    value = ReadBD99954(0x1122);     // Read value from EEPROM

    P2->OUT |= (value & (BIT2 | BIT1 | BIT0)); // Set LED2 with 3 LSB of value

    __sleep();      // go to lower power mode
}
