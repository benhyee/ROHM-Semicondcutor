#include "msp.h"
#include <stdint.h>
#include "helper.h"
#include <stdlib.h>
#include "UART.h"
#include "globals.h"


unsigned int value=0, RDO = 0, PDO = 0;
unsigned int battery = 0, nonBattery = 0;
unsigned char tempHold;
unsigned short status_1=0,status_2=0,config_1=0,config_2=0;
unsigned short sys_config_1=0, sys_config_2=0,sys_config_3 = 0, alertStatus=0,capability=0;
unsigned short Display_alert = 0;
unsigned char readBack;
int i, TransmitFlag = 0;
unsigned int PDO_SNK_Cons;
unsigned int maxCurrent = 0, voltage = 0, current = 0;
unsigned char power_role = 0, recepticle = 0, data_role;
#define BM92A_ADDRESS 0x18
#define BD99954_ADDRESS 0x09



void InitI2C()
{

  P1->SEL0 |= BIT6 | BIT7;                // Set I2C pins of eUSCI_B0

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
  EUSCI_B0->BRW =30;                       // baudrate = SMCLK / 30 = 100kHz
  EUSCI_B0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST;  // Release eUSCI from reset

  EUSCI_B0->IE |= EUSCI_A_IE_RXIE |         // Enable receive interrupt
                  EUSCI_A_IE_TXIE;
}


void CommandRegisterBM92A(unsigned short commandCode,unsigned char slaveAddr)
{
    unsigned char highByte;
    unsigned char lowByte;
    highByte = commandCode >> 8;
    lowByte = commandCode & 0xFF;

    EUSCI_B0->I2CSA = slaveAddr;          // Slave address
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

void write_word(unsigned char commandCode,unsigned char slaveAddr, unsigned short content)
{
    unsigned char highByte;
    unsigned char lowByte;
    highByte = content >> 8;
    lowByte = content & 0xFF;
    EUSCI_B0->I2CSA = slaveAddr;          // Slave address
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TR;          // Set transmit mode (write)
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT;       // I2C start condition
    while (!TransmitFlag);            // Wait for the transmit to complete
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
}
void write_block(unsigned char commandCode,unsigned char slaveAddr, int dataSize, unsigned char* dataArray)
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
}



int WriteRead(unsigned char commandCode,unsigned char slaveAddr, int dataSize, unsigned char* dataArray)
{
    EUSCI_B0->I2CSA = slaveAddr;          // Slave address

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

void testReadRegistersBM92A()
{
    value=0, RDO = 0, PDO = 0;  //Reinitialize to 0 to ensure fresh write
    battery = 0, nonBattery = 0;
    status_1=0,status_2=0,config_1=0,config_2=0;
    sys_config_1=0, sys_config_2=0, alertStatus=0,capability=0;
    Display_alert = 0;
    unsigned char *readBack = malloc(sizeof(char)*30);
    unsigned int *PDO_SNK_Cons = malloc(sizeof(unsigned int)*4);

    WriteRead(0x08,BM92A_ADDRESS,29,readBack);//PDO Snk register
    WriteRead(0x33,BM92A_ADDRESS,17,readBack);//PDO Snk register
    WriteRead(0x3C,BM92A_ADDRESS,29,readBack);//PDO Snk register

    WriteRead(0x02,BM92A_ADDRESS,2,readBack);//Alert register
    alertStatus = two_byteOrg(readBack);

    WriteRead(0x03,BM92A_ADDRESS,2,readBack); //status 1 register
    status_1 = two_byteOrg(readBack);

    WriteRead(0x04,BM92A_ADDRESS,2,readBack); //status 2 register
    status_2 = two_byteOrg(readBack);

    WriteRead(0x06,BM92A_ADDRESS,2,readBack); //controller config 1
    config_1 = two_byteOrg(readBack);

    WriteRead(0x07,BM92A_ADDRESS,2,readBack); //Capability register
    capability = two_byteOrg(readBack);

    WriteRead(0x17,BM92A_ADDRESS,2,readBack); //controller config 2
    config_2 = two_byteOrg(readBack);

    WriteRead(0x19,BM92A_ADDRESS,2,readBack); //Display port register
    Display_alert = two_byteOrg(readBack);

    WriteRead(0x20,BM92A_ADDRESS,5,readBack);//Autongtsnk Info non-Battery register
    nonBattery = four_byteOrg(readBack);

    WriteRead(0x23,BM92A_ADDRESS,5,readBack);//Autongtsnk Info Battery register
    battery = four_byteOrg(readBack);

    WriteRead(0x26,BM92A_ADDRESS,2,readBack); //system controller config 1
    sys_config_1 = two_byteOrg(readBack);

    WriteRead(0x27,BM92A_ADDRESS,2,readBack); //system controller config 2
    sys_config_2 = two_byteOrg(readBack);

    WriteRead(0x28,BM92A_ADDRESS,5,readBack);//PDO register
    PDO = four_byteOrg(readBack);

    WriteRead(0x2B,BM92A_ADDRESS,5,readBack);//RDO register
    RDO = four_byteOrg(readBack);
    WriteRead(0x2F,BM92A_ADDRESS,2,readBack); //system controller config 2
    sys_config_3 = two_byteOrg(readBack);
    for(i = 0; i<40; i++);

}


void BM92A_Debugger()
{
    unsigned char *readBack = malloc(sizeof(char)*30);  //Temp Storage of registers
    WriteRead(0x28,BM92A_ADDRESS,5,readBack);//PDO register
    PDO = four_byteOrg(readBack);
//    testReadRegisters();
    WriteRead(0x03,BM92A_ADDRESS,2,readBack); //status 1 register
    status_1 = two_byteOrg(readBack);
    for(i = 0; i < 100; i++);

    WriteRead(0x26,BM92A_ADDRESS,2,readBack); //system controller config 1
    sys_config_1 = two_byteOrg(readBack);
    for(i = 0; i < 100; i++);

    WriteRead(0x2B,BM92A_ADDRESS,5,readBack);//RDO register
    RDO = four_byteOrg(readBack);
    for(i = 0; i < 100; i++);



    power_role = status_1 & 0x1000;
    power_role = power_role >> 12;
    data_role = status_1 >> 8;
    data_role = data_role & 0x3;
    maxCurrent = PDO & 0x3FF;   // BitMask first 10 Bits
    maxCurrent = maxCurrent * 10;   // Max Current 10 mA
    voltage = PDO >> 10;    // Shift 10 to accomodate Voltage reading
    voltage = (voltage & 0x3FF) * 50;   // 50 mV times 10 bits of Voltage
    RDO = RDO >> 10;
    current = RDO & 0x3FF;          // First 10 register of RDO are operating Current
    current = current * 10;
    recepticle = sys_config_1 & 0xF; // Recepticle Type
    WriteRead(0x20,BM92A_ADDRESS,5,readBack);//Autongtsnk Info non-Battery register
    nonBattery = four_byteOrg(readBack);
    if(power_role == 0)
    {
        terminal_transmitWord("BM92A is Sink \t");
    }
    else
    {
        terminal_transmitWord("BM92A is Source \t");

    }
    if(data_role == 0)
    {
        terminal_transmitWord("No plug inserted \t");
    }
    else if(data_role == 1)
    {
        terminal_transmitWord("UFP (Device) \t");

    }
    else if(data_role == 2)
    {
        terminal_transmitWord("DFP (Host) \t");

    }
    else
    {
        terminal_transmitWord("In Accessory Mode \t");

    }

    switch(recepticle){
    case 0x9:
        terminal_transmitWord("Type-C \n\r");
        break;

    case 0xA:
        terminal_transmitWord("Type-C 3A \n\r");
        break;
    case 0xB:
        terminal_transmitWord("Type=C 5A \n\r");
        break;
    default:
        terminal_transmitWord("Unconfirmed Recepticle Type\n\r");
        break;
    }
    terminal_transmitWord("Max Amps:");
    terminal_transmitInt(maxCurrent);
    terminal_transmitWord("\nOperating Amps:");
    terminal_transmitInt(current);
    terminal_transmitWord("\nVoltage:");
    terminal_transmitInt(voltage);
    terminal_transmitWord("\r\n");
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



