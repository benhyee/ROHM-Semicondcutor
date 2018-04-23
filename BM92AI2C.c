#include "msp432.h"
#include "delay.h"
#include "BM92AI2C.h"

unsigned short BM92TXFlag;
#define CURRENT_FREQ FREQ_24_MHZ

void InitBM92A(unsigned char DeviceAddress)
{
    __disable_irq();
    P1->SEL0 |= BIT7 | BIT8;                // Set I2C pins of eUSCI_B0


//  P5->SEL1 &= ~0x0F;
//  P5->SEL0 &= ~0x0F;                       // Set Reset PIN for BM92A
//  P5->DIR |= 0x0F;

  // Enable EUSCI_B0 interrupt in NVIC module
  NVIC->ISER[0] = 1 << ((EUSCIB0_IRQn) & 31);

  // Configure USCI_B0 for I2C mode
  EUSCI_B0->CTLW0 |= EUSCI_A_CTLW0_SWRST;   // Software reset enabled
  EUSCI_B0->CTLW0 = EUSCI_A_CTLW0_SWRST |   // Remain eUSCI in reset mode
          EUSCI_B_CTLW0_MODE_3 |            // I2C mode
          EUSCI_B_CTLW0_MST |               // Master mode
          EUSCI_B_CTLW0_SYNC |              // Sync mode
          EUSCI_B_CTLW0_SSEL__SMCLK;        // SMCLK

  EUSCI_B0->BRW = 30;                       // baudrate = SMCLK / 30 = 100kHz
  EUSCI_B0->I2CSA = DeviceAddress;
  EUSCI_B0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST;  // Release eUSCI from reset

  EUSCI_B0->IE |= EUSCI_A_IE_RXIE         // Enable receive interrupt
                 |EUSCI_A_IE_TXIE;

//  P5->OUT |= BIT3;   //Set Slave address for first BM92A
//  P5->OUT &= ~BIT4;  //Set Slave address for second BM92A
//  BM92A_reset();
}
void BM92A_reset()
{
    P5->OUT |= 0x03;    //Reset Pins 5.0 and 5.1
    delay_us(120,CURRENT_FREQ); //Mandatory 100 us wait
    P5->OUT &= ~0x03;           // Reset Pin Low
    delay_ms(250,CURRENT_FREQ); // Delay 200 ms before SMBUS can be used

}
////////////////////////////////////////////////////////////////////////////////
//
//  Function that writes a single byte to the EEPROM.
//
//  MemAddress  - 2 byte address specifies the address in the EEPROM memory
//  MemByte     - 1 byte value that is stored in the EEPROM
//
//  Procedure :
//      start
//      transmit address+W (control+0)     -> ACK (from EEPROM)
//      transmit data      (high address)  -> ACK (from EEPROM)
//      transmit data      (low address)   -> ACK (from EEPROM)
//      transmit data      (data)          -> ACK (from EEPROM)
//      stop
//
////////////////////////////////////////////////////////////////////////////////
void WriteBM92A(unsigned char DeviceAddress,unsigned char commandBytes)
{

    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TR;      // Set transmit mode (write)
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT;   // I2C start condition
    while (!BM92TXFlag);                // Wait for EEPROM address to transmit
    BM92TXFlag = 0;
    EUSCI_B0 -> TXBUF = commandBytes;    // Send the high byte of the memory address
    while (!BM92TXFlag);                // Wait for EEPROM address to transmit
    BM92TXFlag = 0;

    EUSCI_B0 -> CTLW0 |= EUSCI_B_CTLW0_TXSTP;   // I2C stop condition

}
void BM92init_Write(unsigned char DeviceAddress)
{
    EUSCI_B0->I2CSA = DeviceAddress;          // Slave address
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TR;      // Set transmit mode (write)
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT;   // I2C start condition
}

////////////////////////////////////////////////////////////////////////////////
//
//  Function that reads a single byte from the EEPROM.
//
//  MemAddress  - 2 byte address specifies the address in the EEPROM memory
//  ReceiveByte - 1 byte value that is received from the EEPROM
//
//  Procedure :
//      start
//      transmit address+W (control+0)    -> ACK (from EEPROM)
//      transmit data      (high address) -> ACK (from EEPROM)
//      transmit data      (low address)  -> ACK (from EEPROM)
//      start
//      transmit address+R (control+1)    -> ACK (from EEPROM)
//      transmit data      (data)         -> NACK (from MSP432)
//      stop
//
////////////////////////////////////////////////////////////////////////////////
unsigned short ReadBM92A(unsigned char DeviceAddress)
{
  unsigned short ReceiveByte;

  BM92init_Write(DeviceAddress);
  while (!BM92TXFlag);            // Wait for the transmit to complete
  BM92TXFlag = 0;
  BM92init_Read();
  // Wait for start to be transmitted
  while ((EUSCI_B0->CTLW0 & EUSCI_B_CTLW0_TXSTT));

  // set stop bit to trigger after first byte
  EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTP;

  while (!BM92TXFlag);            // Wait to receive a byte
  BM92TXFlag = 0;

  ReceiveByte = EUSCI_B0->RXBUF;    // Read byte from the buffer

  return ReceiveByte;
}
void BM92init_Read()
{
    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_TR;   // Set receive mode (read)
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT; // I2C start condition (restart)
}

////////////////////////////////////////////////////////////////////////////////
//
// I2C Interrupt Service Routine
//
////////////////////////////////////////////////////////////////////////////////
void EUSCI_B0_IRQHandler(void)
{
    if (EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG0)     // Check if transmit complete
    {
        EUSCI_B0->IFG &= ~ EUSCI_B_IFG_TXIFG0;  // Clear interrupt flag
        BM92TXFlag = 1;                       // Set global flag
    }

    if (EUSCI_B0->IFG & EUSCI_B_IFG_RXIFG0)     // Check if receive complete
    {
        EUSCI_B0->IFG &= ~ EUSCI_B_IFG_RXIFG0;  // Clear interrupt flag
        BM92TXFlag = 1;                       // Set global flag
    }
}

