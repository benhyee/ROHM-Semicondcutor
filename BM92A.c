#include "msp.h"

uint16_t TransmitFlag = 0;

void InitBM92A(uint8_t DeviceAddress)
{
    EUSCI_B0->I2CSA = DeviceAddress;          // Slave address
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
void WriteBM92A(uint16_t MemAddress, uint8_t MemByte)
{
  uint8_t HiAddress;
  uint8_t LoAddress;

  HiAddress = MemAddress >> 8;
  LoAddress = MemAddress & 0xFF;

  EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TR;          // Set transmit mode (write)
  EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT;       // I2C start condition

  while (!TransmitFlag);                // Wait for EEPROM address to transmit
  TransmitFlag = 0;

  EUSCI_B0 -> TXBUF = HiAddress;    // Send the high byte of the memory address

  while (!TransmitFlag);            // Wait for the transmit to complete
  TransmitFlag = 0;

  EUSCI_B0 -> TXBUF = LoAddress;    // Send the high byte of the memory address

  while (!TransmitFlag);            // Wait for the transmit to complete
  TransmitFlag = 0;

  EUSCI_B0 -> TXBUF = MemByte;      // Send the byte to store in EEPROM

  while (!TransmitFlag);            // Wait for the transmit to complete
  TransmitFlag = 0;

  EUSCI_B0 -> CTLW0 |= EUSCI_B_CTLW0_TXSTP;   // I2C stop condition
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
uint8_t ReadBM92A(uint16_t MemAddress)
{
  uint8_t ReceiveByte;
  uint8_t HiAddress;
  uint8_t LoAddress;

  HiAddress = MemAddress >> 8;
  LoAddress = MemAddress & 0xFF;

  EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TR;      // Set transmit mode (write)
  EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT;   // I2C start condition

  while (!TransmitFlag);                // Wait for EEPROM address to transmit
  TransmitFlag = 0;

  EUSCI_B0 -> TXBUF = HiAddress;    // Send the high byte of the memory address

  while (!TransmitFlag);            // Wait for the transmit to complete
  TransmitFlag = 0;

  EUSCI_B0 -> TXBUF = LoAddress;    // Send the low byte of the memory address

  while (!TransmitFlag);            // Wait for the transmit to complete
  TransmitFlag = 0;

  EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_TR;   // Set receive mode (read)
  EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT; // I2C start condition (restart)

  // Wait for start to be transmitted
  while ((EUSCI_B0->CTLW0 & EUSCI_B_CTLW0_TXSTT));

  // set stop bit to trigger after first byte
  EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTP;

  while (!TransmitFlag);            // Wait to receive a byte
  TransmitFlag = 0;

  ReceiveByte = EUSCI_B0->RXBUF;    // Read byte from the buffer

  return ReceiveByte;
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
