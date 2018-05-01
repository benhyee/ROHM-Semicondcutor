#include "BM92AI2C.h"
#include "msp.h"
#include <stdint.h>
#include "helper.h"
unsigned int TransmitFlag = 0;
#include "helper.h"


#define BM92A_ADDRESS1 0x1A
#define BM92A_ADDRESS2 0x18
void InitBM92A()
{

  P1->SEL0 |= BIT6 | BIT7;                // Set I2C pins of eUSCI_B0

  // Enable eUSCIB0 interrupt in NVIC module
  NVIC->ISER[0] = 1 << ((EUSCIB0_IRQn) & 31);

  // Configure USCI_B0 for I2C mode
  EUSCI_B0->CTLW0 |= EUSCI_A_CTLW0_SWRST;   // Software reset enabled
  EUSCI_B0->CTLW0 = EUSCI_A_CTLW0_SWRST |   // Remain eUSCI in reset mode
          EUSCI_B_CTLW0_MODE_3 |            // I2C mode
          EUSCI_B_CTLW0_MST |               // Master mode
          EUSCI_B_CTLW0_SYNC |              // Sync mode
          EUSCI_B_CTLW0_SSEL__SMCLK;        // SMCLK
  EUSCI_B0->BRW =30;                       // baudrate = SMCLK / 30 = 100kHz
  EUSCI_B0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST;  // Release eUSCI from reset

  EUSCI_B0->IE |= EUSCI_A_IE_RXIE |         // Enable receive interrupt
                  EUSCI_A_IE_TXIE;
}


void WriteBM92A(char commandCode,unsigned char slaveAddr)
{
    EUSCI_B0->I2CSA = slaveAddr;          // Slave address

    int i;

    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TR;          // Set transmit mode (write)
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT;       // I2C start condition
    while (!TransmitFlag);            // Wait for the transmit to complete
    TransmitFlag = 0;
    EUSCI_B0 -> TXBUF = commandCode;      // Send the byte to store in BM92A
    while (!TransmitFlag);            // Wait for the transmit to complete
    TransmitFlag = 0;

//  EUSCI_B0 -> CTLW0 |= EUSCI_B_CTLW0_TXSTP;   // I2C stop condition
}


int WriteReadBM92A(unsigned char commandCode,unsigned char slaveAddr, int dataSize, unsigned char* dataArray)
{
    EUSCI_B0->I2CSA = slaveAddr;          // Slave address

    char highByte;
    char lowByte;

    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TR;          // Set transmit mode (write)
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT;       // I2C start condition
    while (!TransmitFlag);            // Wait for the transmit to complete
    TransmitFlag = 0;
    EUSCI_B0 -> TXBUF = commandCode;      // Send the byte to store in BM92A
    while (!TransmitFlag);            // Wait for the transmit to complete
    TransmitFlag = 0;

    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_TR;   // Set receive mode (read)
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT; // I2C start condition (restart)

    // Wait for start to be transmitted
    while ((EUSCI_B0->CTLW0 & EUSCI_B_CTLW0_TXSTT));
    do{
        if (dataSize == 1)
        {
            EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTP;    // set stop bit to trigger after byte

        }

        while (!TransmitFlag);            // Wait to receive a byte
        TransmitFlag = 0;
        *dataArray++ = EUSCI_B0->RXBUF;    // Read byte from the buffer
        dataSize--;
    }while(dataSize);
    while(EUSCI_B0->CTLW0 & 4);
    return 0;
}
unsigned int value=0, RDO = 0, PDO = 0;
unsigned int battery = 0, nonBattery = 0;
unsigned char tempHold;
unsigned short status_1=0,status_2=0,config_1=0,config_2=0;
unsigned short sys_config_1=0, sys_config_2=0, alertStatus=0,capability=0;
unsigned short Display_alert = 0;
unsigned char readBack[30];

void testReadRegisters()
{
    value=0, RDO = 0, PDO = 0;  //Reinitialize to 0 to ensure fresh write
    battery = 0, nonBattery = 0;
    tempHold;
    status_1=0,status_2=0,config_1=0,config_2=0;
    sys_config_1=0, sys_config_2=0, alertStatus=0,capability=0;
    Display_alert = 0;

    WriteReadBM92A(0x02,BM92A_ADDRESS2,2,readBack);//Alert register
    alertStatus = two_byteOrg(readBack);

    WriteReadBM92A(0x03,BM92A_ADDRESS2,2,readBack); //status 1 register
    status_1 = two_byteOrg(readBack);

    WriteReadBM92A(0x04,BM92A_ADDRESS2,2,readBack); //status 2 register
    status_2 = two_byteOrg(readBack);

    WriteReadBM92A(0x06,BM92A_ADDRESS2,2,readBack); //controller config 1
    config_1 = two_byteOrg(readBack);

    WriteReadBM92A(0x07,BM92A_ADDRESS2,2,readBack); //Capability register
    capability = two_byteOrg(readBack);

    WriteReadBM92A(0x17,BM92A_ADDRESS2,2,readBack); //controller config 2
    config_2 = two_byteOrg(readBack);

    WriteReadBM92A(0x19,BM92A_ADDRESS2,2,readBack); //Display port register
    Display_alert = two_byteOrg(readBack);

    WriteReadBM92A(0x20,BM92A_ADDRESS2,4,readBack);//Autongtsnk Info non-Battery register
    nonBattery = four_byteOrg(readBack);

    WriteReadBM92A(0x23,BM92A_ADDRESS2,4,readBack);//Autongtsnk Info Battery register
    battery = four_byteOrg(readBack);

    WriteReadBM92A(0x26,BM92A_ADDRESS2,2,readBack); //system controller config 1
    sys_config_1 = two_byteOrg(readBack);

    WriteReadBM92A(0x27,BM92A_ADDRESS2,2,readBack); //system controller config 2
    sys_config_2 = two_byteOrg(readBack);

    WriteReadBM92A(0x28,BM92A_ADDRESS2,4,readBack);//PDO register
    PDO = four_byteOrg(readBack);

    WriteReadBM92A(0x2B,BM92A_ADDRESS2,4,readBack);//RDO register
    RDO = four_byteOrg(readBack);
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

