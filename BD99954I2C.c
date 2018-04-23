#include "msp432.h"

unsigned short BD99954TXFlag;

void InitBD99954(unsigned char DeviceAddress)
{

  P6->SEL0 |= BIT3 | BIT4;                // Set I2C pins of EUSCI_B1

  // Enable EUSCIB1 interrupt in NVIC module
  NVIC->ISER[0] = 1 << ((EUSCIB1_IRQn) & 31);

  // Configure USCI_B0 for I2C mode
  EUSCI_B1->CTLW0 |= EUSCI_A_CTLW0_SWRST;   // Software reset enabled
  EUSCI_B1->CTLW0 = EUSCI_A_CTLW0_SWRST |   // Remain eUSCI in reset mode
          EUSCI_B_CTLW0_MODE_3 |            // I2C mode
          EUSCI_B_CTLW0_MST |               // Master mode
          EUSCI_B_CTLW0_SYNC |              // Sync mode
          EUSCI_B_CTLW0_SSEL__SMCLK;        // SMCLK

  EUSCI_B1->BRW = 30;                       // baudrate = SMCLK / 30 = 100kHz
  EUSCI_B1->I2CSA = DeviceAddress;          // Slave address
  EUSCI_B1->CTLW0 &= ~EUSCI_A_CTLW0_SWRST;  // Release eUSCI from reset

  EUSCI_B1->IE |= EUSCI_A_IE_RXIE |         // Enable receive interrupt
                  EUSCI_A_IE_TXIE;
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
void WriteBD99954(unsigned short MemAddress, unsigned char MemByte)
{
  unsigned char HiAddress;
  unsigned char LoAddress;

  HiAddress = MemAddress >> 8;
  LoAddress = MemAddress & 0xFF;

  EUSCI_B1->CTLW0 |= EUSCI_B_CTLW0_TR;          // Set transmit mode (write)
  EUSCI_B1->CTLW0 |= EUSCI_B_CTLW0_TXSTT;       // I2C start condition

  while (!BD99954TXFlag);                // Wait for EEPROM address to transmit
  BD99954TXFlag = 0;

  EUSCI_B1 -> TXBUF = HiAddress;    // Send the high byte of the memory address

  while (!BD99954TXFlag);            // Wait for the transmit to complete
  BD99954TXFlag = 0;

  EUSCI_B1 -> TXBUF = LoAddress;    // Send the high byte of the memory address

  while (!BD99954TXFlag);            // Wait for the transmit to complete
  BD99954TXFlag = 0;

  EUSCI_B1 -> TXBUF = MemByte;      // Send the byte to store in EEPROM

  while (!BD99954TXFlag);            // Wait for the transmit to complete
  BD99954TXFlag = 0;

  EUSCI_B1 -> CTLW0 |= EUSCI_B_CTLW0_TXSTP;   // I2C stop condition
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
unsigned char ReadBD99954(unsigned short MemAddress)
{
  unsigned char ReceiveByte;
  unsigned char HiAddress;
  unsigned char LoAddress;

  HiAddress = MemAddress >> 8;
  LoAddress = MemAddress & 0xFF;

  EUSCI_B1->CTLW0 |= EUSCI_B_CTLW0_TR;      // Set transmit mode (write)
  EUSCI_B1->CTLW0 |= EUSCI_B_CTLW0_TXSTT;   // I2C start condition

  while (!BD99954TXFlag);                // Wait for EEPROM address to transmit
  BD99954TXFlag = 0;

  EUSCI_B1 -> TXBUF = HiAddress;    // Send the high byte of the memory address

  while (!BD99954TXFlag);            // Wait for the transmit to complete
  BD99954TXFlag = 0;

  EUSCI_B1 -> TXBUF = LoAddress;    // Send the low byte of the memory address

  while (!BD99954TXFlag);            // Wait for the transmit to complete
  BD99954TXFlag = 0;

  EUSCI_B1->CTLW0 &= ~EUSCI_B_CTLW0_TR;   // Set receive mode (read)
  EUSCI_B1->CTLW0 |= EUSCI_B_CTLW0_TXSTT; // I2C start condition (restart)

  // Wait for start to be transmitted
  while ((EUSCI_B1->CTLW0 & EUSCI_B_CTLW0_TXSTT));

  // set stop bit to trigger after first byte
  EUSCI_B1->CTLW0 |= EUSCI_B_CTLW0_TXSTP;

  while (!BD99954TXFlag);            // Wait to receive a byte
  BD99954TXFlag = 0;

  ReceiveByte = EUSCI_B1->RXBUF;    // Read byte from the buffer

  return ReceiveByte;
}

////////////////////////////////////////////////////////////////////////////////
//
// I2C Interrupt Service Routine
//
////////////////////////////////////////////////////////////////////////////////
void EUSCIB1_IRQHandler(void)
{
    if (EUSCI_B1->IFG & EUSCI_B_IFG_TXIFG0)     // Check if transmit complete
    {
        EUSCI_B1->IFG &= ~ EUSCI_B_IFG_TXIFG0;  // Clear interrupt flag
        BD99954TXFlag = 1;                       // Set global flag
    }

    if (EUSCI_B1->IFG & EUSCI_B_IFG_RXIFG0)     // Check if receive complete
    {
        EUSCI_B1->IFG &= ~ EUSCI_B_IFG_RXIFG0;  // Clear interrupt flag
        BD99954TXFlag = 1;                       // Set global flag
    }
}
