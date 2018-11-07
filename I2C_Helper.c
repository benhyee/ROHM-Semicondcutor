#include "msp.h"
#include <stdint.h>
#include "helper.h"
#include <stdlib.h>
#include "UART.h"
#include "globals.h"
#include "lcd.h"
#include <stdio.h>


unsigned char tempHold;
int i, TransmitFlag = 0;
#define BM92A_ADDRESS 0x18


void InitI2C()
{

  P1->SEL0 |= BIT6 | BIT7;                // Set I2C pins of eUSCI_B0
  PJ->SEL1 &= ~0x0C;
  PJ->SEL0 &= ~0x0C;
  PJ -> DIR |= 0x0C;

  // Enable eUSCIB0 interrupt in NVIC module
  NVIC->ISER[0] = 1 << ((EUSCIB0_IRQn) & 31);
  NVIC_SetPriority(EUSCIB0_IRQn,3);
  NVIC_EnableIRQ(EUSCIB0_IRQn);
  // Configure USCI_B0 for I2C mode
  EUSCI_B0->CTLW0 |= EUSCI_A_CTLW0_SWRST;   // Software reset enabled
  EUSCI_B0->CTLW0 = EUSCI_A_CTLW0_SWRST |   // Remain eUSCI in reset mode
          EUSCI_B_CTLW0_MODE_3 |            // I2C mode
          EUSCI_B_CTLW0_MST |               // Master mode
          EUSCI_B_CTLW0_SYNC |              // Sync mode
          EUSCI_B_CTLW0_SSEL__SMCLK;        // SMCLK
  EUSCI_B0->BRW =29;                       // baudrate = SMCLK / 26 = 400kHz
  EUSCI_B0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST;  // Release eUSCI from reset

  EUSCI_B0->IE |= EUSCI_A_IE_RXIE |         // Enable receive interrupt
                  EUSCI_A_IE_TXIE;
}

void CommandRegisterBM92A(unsigned short commandCode)
{
    unsigned char highByte;
    unsigned char lowByte;
    highByte = commandCode >> 8;
    lowByte = commandCode & 0xFF;

    EUSCI_B0->I2CSA = BM92A_ADDRESS;          // Slave address
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TR;          // Set transmit mode (write)
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT;       // I2C start condition
    while (!TransmitFlag);            // Wait for the transmit to complete
    TransmitFlag = 0;
    EUSCI_B0 -> TXBUF = 0x05;      // Send the byte to store in BM92A
    while (!TransmitFlag);            // Wait for the transmit to complete
    TransmitFlag = 0;
    EUSCI_B0 -> TXBUF = 0x02;      // Send the byte to store in BM92A

    while (!TransmitFlag);            // Wait for the transmit to complete
    TransmitFlag = 0;
    EUSCI_B0 -> TXBUF = lowByte;      // Send the byte to store in BM92A
    while (!TransmitFlag);            // Wait for the transmit to complete
    TransmitFlag = 0;
    EUSCI_B0 -> TXBUF = highByte;      // Send the byte to store in BM92A
    while (!TransmitFlag);            // Wait for the transmit to complete
    TransmitFlag = 0;
    EUSCI_B0 -> CTLW0 |= EUSCI_B_CTLW0_TXSTP;   // I2C stop condition

}
void clear_register(unsigned char commandCode,unsigned char slaveAddr, int dataSize)
{
   EUSCI_B0->I2CSA = slaveAddr;          // Slave address
   EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TR;          // Set transmit mode (write)
   EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT;       // I2C start condition
   while (!TransmitFlag);            // Wait for the transmit to complete
   TransmitFlag = 0;
   EUSCI_B0 -> TXBUF = commandCode;      // Send the command byte
   while (!TransmitFlag);            // Wait for the transmit to complete
   TransmitFlag = 0;
   EUSCI_B0 -> TXBUF = 0x00;      // Send the dataSize
   do {
           while (!TransmitFlag);            // Wait for the transmit to complete
           TransmitFlag = 0;
           EUSCI_B0 -> TXBUF = 0x00;      //increment through the dataArray
           dataSize--;
       } while(dataSize > 0);

   while (!TransmitFlag);            // Wait for the transmit to complete
   TransmitFlag = 0;
   EUSCI_B0 -> CTLW0 |= EUSCI_B_CTLW0_TXSTP;   // I2C stop condition

}
int write_word(unsigned char commandCode,unsigned char slaveAddr, unsigned short content)
{
    unsigned char highByte;
    unsigned char lowByte;
    highByte = content >> 8;
    lowByte = content & 0x00FF;
    EUSCI_B0->I2CSA = slaveAddr;          // Slave address
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TR;          // Set transmit mode (write)
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT;       // I2C start condition
    while (!TransmitFlag);
    TransmitFlag = 0;
    EUSCI_B0 -> TXBUF = commandCode;      // Send the byte to store in BM9
    while (!TransmitFlag);            // Wait for the transmit to complete
    TransmitFlag = 0;
    EUSCI_B0 -> TXBUF = lowByte;      // Send the byte to store in BM92A
    while (!TransmitFlag);            // Wait for the transmit to complete
    TransmitFlag = 0;
    EUSCI_B0 -> TXBUF = highByte;      // Send the byte to store in BM92A

    while (!TransmitFlag);            // Wait for the transmit to complete
    TransmitFlag = 0;

    EUSCI_B0 -> CTLW0 |= EUSCI_B_CTLW0_TXSTP;   // I2C stop condition
    while(EUSCI_B0->CTLW0 & 4);
    return 0;

}



int write_block(unsigned char commandCode,unsigned char slaveAddr, int dataSize, unsigned char* dataArray)
{

    EUSCI_B0->I2CSA = slaveAddr;          // Slave address
       EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TR;          // Set transmit mode (write)
       EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT;       // I2C start condition
       while (!TransmitFlag);            // Wait for the transmit to complete
       TransmitFlag = 0;
       EUSCI_B0 -> TXBUF = commandCode;      // Send the command byte

       while (!TransmitFlag);            // Wait for the transmit to complete
       TransmitFlag = 0;
       EUSCI_B0 -> TXBUF = dataSize;      // Send the dataSize
       do {

           while (!TransmitFlag);            // Wait for the transmit to complete
           TransmitFlag = 0;
           EUSCI_B0 -> TXBUF = *dataArray++;      //increment through the dataArray
           dataSize--;
       } while(dataSize > 0);

       while (!TransmitFlag);            // Wait for the transmit to complete
       TransmitFlag = 0;
       EUSCI_B0 -> CTLW0 |= EUSCI_B_CTLW0_TXSTP;   // I2C stop condition
       return 0;

}
int write_fourByte(unsigned char commandCode,unsigned char slaveAddr, unsigned int content)
{
    unsigned char highByte;
    unsigned char midHigh;
    unsigned char midLow;
    unsigned char lowByte;
    content = content;
    highByte = content >> 24;
    midHigh = content >> 16;
    midHigh = midHigh & 0x000000FF;
    midLow = content >> 8;
    midLow = midLow  & 0x000000FF;
    lowByte = content & 0x000000FF;
    EUSCI_B0->I2CSA = slaveAddr;          // Slave address
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TR;          // Set transmit mode (write)
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT;       // I2C start condition

    while (!TransmitFlag);
    TransmitFlag = 0;
    EUSCI_B0 -> TXBUF = commandCode;      // Send the byte to store in BM9

    while (!TransmitFlag);
    TransmitFlag = 0;
    EUSCI_B0 -> TXBUF = 4;          // Send data size Byte

    while (!TransmitFlag);            // Wait for the transmit to complete
    TransmitFlag = 0;
    EUSCI_B0 -> TXBUF = lowByte;

    while (!TransmitFlag);            // Wait for the transmit to complete
    TransmitFlag = 0;
    EUSCI_B0 -> TXBUF = midLow;

    while (!TransmitFlag);            // Wait for the transmit to complete
    TransmitFlag = 0;
    EUSCI_B0 -> TXBUF = midHigh;

    while (!TransmitFlag);            // Wait for the transmit to complete
    TransmitFlag = 0;
    EUSCI_B0 -> TXBUF = highByte;

    while (!TransmitFlag);            // Wait for the transmit to complete
    TransmitFlag = 0;

    EUSCI_B0 -> CTLW0 |= EUSCI_B_CTLW0_TXSTP;   // I2C stop condition
    while(EUSCI_B0->CTLW0 & 4);
    return 0;

}
int WriteRead(unsigned char commandCode,unsigned char slaveAddr, int dataSize, unsigned char* dataArray)
{
    EUSCI_B0->I2CSA = slaveAddr;          // Slave address

    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TR;          // Set transmit mode (write)
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT;       // I2C start condition
    int timer = 0;
    while (!TransmitFlag)
    {
        timer += 1;
        if(timer > 2147499)
        {
            return 1;
        }
    }
    TransmitFlag = 0;
    EUSCI_B0 -> TXBUF = commandCode;      // Send the byte to store in BM92A
    while (!TransmitFlag);            // Wait for the transmit to complete
    TransmitFlag = 0;

    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_TR;   // Set receive mode (read)
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT; // I2C start condition (restart)

//     Wait for start to be transmitted
    while ((EUSCI_B0->CTLW0 & EUSCI_B_CTLW0_TXSTT));
    do{
        if (dataSize == 1)
        {
            EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTP;    // set stop bit to trigger after byte

        }

        while (!TransmitFlag);            // Wait to receive a byte
        TransmitFlag = 0;
        *dataArray++ = EUSCI_B0->RXBUF;    // Read byte from the buffer
        while(EUSCI_B0->CTLW0 & 5);

        dataSize--;
    }while(dataSize);

    while(EUSCI_B0->CTLW0 & 4);
    return 0;
}
unsigned short readTwoByte(unsigned char commandCode,unsigned char slaveAddr)
{
    unsigned char *readBack = malloc(sizeof(char)*5);
    WriteRead(commandCode,slaveAddr,2,readBack);
    unsigned short temp = two_byteOrg(readBack);
    free(readBack);
    return temp;
}
unsigned int readFourByte(unsigned char commandCode,unsigned char slaveAddr)
{
    unsigned char *readBack = malloc(sizeof(char)*8);
    WriteRead(commandCode,slaveAddr,5,readBack);
    unsigned int temp = four_byteOrg(readBack);
    free(readBack);
    return temp;
}

////////////////////////////////////////////////////////////////////////////////
//
// I2C Interrupt Service Routine
//
////////////////////////////////////////////////////////////////////////////////
void EUSCIB0_IRQHandler(void)
{
    if (EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG0)     // Check if transmit complete
    {
        EUSCI_B0->IFG &= ~ EUSCI_B_IFG_TXIFG0;  // Clear interrupt flag
        TransmitFlag = 1;                       // Set global flag
    }

    if (EUSCI_B0->IFG & EUSCI_B_IFG_RXIFG0)     // Check if receive complete
    {
        EUSCI_B0->IFG &= ~ EUSCI_B_IFG_RXIFG0;  // Clear interrupt flag
        TransmitFlag = 1;                       // Set global flag
    }
}



