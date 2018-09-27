#include "msp.h"
#include <stdint.h>
#include "helper.h"
#include <stdlib.h>
#include "UART.h"
#include "globals.h"
#include "lcd.h"
#include <stdio.h>
#define BM92A_ADDRESS 0x18
#define BD99954_ADDRESS 0x09


unsigned char tempHold;
int i, TransmitFlag = 0;

//BM92A Local Variables Storing the Register Reads
unsigned int value, RDO, PDO, battery, nonBattery, PDO_SNK_Cons;
unsigned short status_1,status_2,config_1,config_2,Display_alert, vendorConfig;
unsigned short sys_config_1, sys_config_2,sys_config_3 , alertStatus,capability;
unsigned short ALERTEN, SETRDO,FirmType,FirmRev, ManuID,DeviceID,RevID;
//Debugger BM92A Variables
unsigned int maxCurrent , voltage , current ;
unsigned char power_role , recepticle , data_role;

//BD99954 Local Variables
unsigned short IBUS_LIM_SET , ICC_LIM_SET , VRECHG_SET , VBATOVP_SET ;
unsigned short VSYSREG_SET , VPRECHG_TH_SET , VFASTCHG_REG_SET1;
unsigned short ITRICH_SET, IPRECH_SET , ICHG_SET, ITERM_SET, BD99id, BD99rev;
unsigned short VBUS_Average,VSYS_Average, VBAT_Average,otpStatus;

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

void testReadRegistersBM92A()
{
    value=0, RDO = 0, PDO = 0,battery = 0, nonBattery = 0;  //Reinitialize to 0 to ensure fresh write
    status_1=0,status_2=0,config_1=0,config_2=0,Display_alert = 0, vendorConfig= 0;
    sys_config_1=0, sys_config_2=0, alertStatus=0,capability=0;
    ALERTEN=0, SETRDO=0,FirmType=0,FirmRev=0, ManuID=0,DeviceID=0,RevID=0;

    unsigned char *readBack = malloc(sizeof(char)*30);
    unsigned int *PDO_SNK_Cons = malloc(sizeof(unsigned int)*4);

//    WriteRead(0x08,BM92A_ADDRESS,29,readBack);//PDO Snk register
//    WriteRead(0x33,BM92A_ADDRESS,17,readBack);//PDO Snk register
    WriteRead(0x02,BM92A_ADDRESS,2,readBack); alertStatus = two_byteOrg(readBack);//Alert register

//    WriteRead(0x33,BM92A_ADDRESS,17,readBack);//PDO SRC register

//    WriteRead(0x08,BM92A_ADDRESS,21,readBack);//PDO SRC register
//
//    WriteRead(0x02,BM92A_ADDRESS,2,readBack); alertStatus = two_byteOrg(readBack);//Alert register
//    WriteRead(0x03,BM92A_ADDRESS,2,readBack); status_1 = two_byteOrg(readBack); //status 1 register
//    WriteRead(0x04,BM92A_ADDRESS,2,readBack); status_2 = two_byteOrg(readBack);//status 2 register
//    WriteRead(0x06,BM92A_ADDRESS,2,readBack); config_1 = two_byteOrg(readBack);//controller config 1
//    WriteRead(0x07,BM92A_ADDRESS,2,readBack); capability = two_byteOrg(readBack);//Capability register
//    WriteRead(0x17,BM92A_ADDRESS,2,readBack); config_2 = two_byteOrg(readBack);//controller config 2
//    WriteRead(0x19,BM92A_ADDRESS,2,readBack); Display_alert = two_byteOrg(readBack);//Display port register
//    WriteRead(0x1A,BM92A_ADDRESS,2,readBack); vendorConfig = two_byteOrg(readBack);//Vendor Config register
//    WriteRead(0x20,BM92A_ADDRESS,5,readBack); nonBattery = four_byteOrg(readBack);//Autongtsnk Info non-Battery register
//    WriteRead(0x23,BM92A_ADDRESS,5,readBack); battery = four_byteOrg(readBack);//Autongtsnk Info Battery register
//
//    WriteRead(0x26,BM92A_ADDRESS,2,readBack); sys_config_1 = two_byteOrg(readBack);//system controller config 1
//    WriteRead(0x27,BM92A_ADDRESS,2,readBack); sys_config_2 = two_byteOrg(readBack);//system controller config 2
//    WriteRead(0x28,BM92A_ADDRESS,5,readBack); PDO = four_byteOrg(readBack);//PDO register
//    WriteRead(0x2B,BM92A_ADDRESS,5,readBack); RDO = four_byteOrg(readBack);//RDO register
//    WriteRead(0x2F,BM92A_ADDRESS,2,readBack); sys_config_3 = two_byteOrg(readBack); //system controller config 2
//
//    ALERTEN=0, SETRDO=0,FirmType=0,FirmRev=0, ManuID=0,DeviceID=0,RevID=0;
//
//    WriteRead(0x2E,BM92A_ADDRESS,2,readBack); ALERTEN = two_byteOrg(readBack);
//    WriteRead(0x30,BM92A_ADDRESS,4,readBack); SETRDO = four_byteOrg(readBack);
//    WriteRead(0x4B,BM92A_ADDRESS,2,readBack); FirmType = two_byteOrg(readBack);
//    WriteRead(0x4C,BM92A_ADDRESS,2,readBack); FirmRev = two_byteOrg(readBack);
//    WriteRead(0x4D,BM92A_ADDRESS,2,readBack); ManuID = two_byteOrg(readBack);
//    WriteRead(0x4E,BM92A_ADDRESS,2,readBack); DeviceID = two_byteOrg(readBack);
//    WriteRead(0x4F,BM92A_ADDRESS,2,readBack); RevID = two_byteOrg(readBack);
//
//    for(i = 0; i<40; i++);

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

void BM92Asrc_init()                //GPIO2 and GPIO3 set the Src Prov Table
{                                   //L L -> (60W)  L H -> (45W) H L -> (27W) H H -> (18W)




    unsigned char PDO_SRC_Cons[28];
    unsigned char PDO_Snk_Cons[16];

    PDO_SRC_Cons[3] = 0x08;
    PDO_SRC_Cons[2] = 0x01;
    PDO_SRC_Cons[1] = 0x91;
    PDO_SRC_Cons[0] = 0x2C; //PDO1
    PDO_SRC_Cons[7] = 0x00;
    PDO_SRC_Cons[6] = 0x02;
    PDO_SRC_Cons[5] = 0xD1;
    PDO_SRC_Cons[4] = 0x2C; //PDO2
    PDO_SRC_Cons[11] = 0x00;
    PDO_SRC_Cons[10] = 0x02;
    PDO_SRC_Cons[9] = 0xD1;
    PDO_SRC_Cons[8] = 0x2C; // PDO3
    PDO_SRC_Cons[15] = 0x00;
    PDO_SRC_Cons[14] = 0x03;
    PDO_SRC_Cons[13] = 0xC1;
    PDO_SRC_Cons[12] = 0x2C; //PDO4
    PDO_SRC_Cons[19] = 0x00;
    PDO_SRC_Cons[18] = 0x04;
    PDO_SRC_Cons[17] = 0xB1;
    PDO_SRC_Cons[16] = 0x2C; //PDO5
    PDO_SRC_Cons[23] = 0x00;
    PDO_SRC_Cons[22] = 0x00;
    PDO_SRC_Cons[21] = 0x00;
    PDO_SRC_Cons[20] = 0x00;
    PDO_SRC_Cons[27] = 0x00;
    PDO_SRC_Cons[26] = 0x00;
    PDO_SRC_Cons[25] = 0x00;
    PDO_SRC_Cons[24] = 0x00;

    write_block(0x08,BM92A_ADDRESS,28,PDO_SRC_Cons);
    for(i=0;i<80;i++);

    PDO_Snk_Cons[0] = 0x00;
    PDO_Snk_Cons[1] = 0x00;
    PDO_Snk_Cons[2] = 0x00;
    PDO_Snk_Cons[3] = 0x00; //PDO1
    PDO_Snk_Cons[4] = 0x00;
    PDO_Snk_Cons[5] = 0x00;
    PDO_Snk_Cons[6] = 0x00;
    PDO_Snk_Cons[7] = 0x00; //PDO2
    PDO_Snk_Cons[8] = 0x00;
    PDO_Snk_Cons[9] = 0x00;
    PDO_Snk_Cons[10] = 0x00;
    PDO_Snk_Cons[11] = 0x00; // PDO3
    PDO_Snk_Cons[12] = 0x00;
    PDO_Snk_Cons[13] = 0x00;
    PDO_Snk_Cons[14] = 0x00;
    PDO_Snk_Cons[15] = 0x00; //PDO4

    write_block(0x33,BM92A_ADDRESS,16,PDO_Snk_Cons);
    write_word(0x2E,BM92A_ADDRESS,0xFFFF);  //Alert Enable
    write_block(0x20,BM92A_ADDRESS,4,0x00000000);  //NonBattery ngt
    write_block(0x23,BM92A_ADDRESS,4,0x00000000);  //Battery ngt

    for(i=0;i<50;i++);
    write_word(0x06,BM92A_ADDRESS,0x0000);  //Config 1
    write_word(0x17,BM92A_ADDRESS,0x8002);  //config 2
    write_word(0x1A,BM92A_ADDRESS,0x0120);  //Vendor Config
    write_word(0x26,BM92A_ADDRESS,0x0991);  // SysConfig1
    write_word(0x27,BM92A_ADDRESS,0x000A);  // SysConfig2
    write_word(0x2F,BM92A_ADDRESS,0x0100);  // SysConfig3
    write_word(0x05,BM92A_ADDRESS,0x0A0A);  // Set Command
}

void BD99954ReadRegister()
{
    IBUS_LIM_SET = 0 , ICC_LIM_SET = 0, VRECHG_SET = 0, VBATOVP_SET = 0;
    VSYSREG_SET = 0, VPRECHG_TH_SET = 0, VFASTCHG_REG_SET1 = 0;
    ITRICH_SET = 0, IPRECH_SET = 0, ICHG_SET = 0, ITERM_SET = 0;
    BD99id = 0, BD99rev = 0;
    VBUS_Average = 0,VSYS_Average = 0, VBAT_Average = 0;
    unsigned char *readBack = malloc(sizeof(char)*30);  //Temp Storage of registers

    WriteRead(0x07,BD99954_ADDRESS,2,readBack);
    IBUS_LIM_SET = two_byteOrg(readBack);

    WriteRead(0x08,BD99954_ADDRESS,2,readBack);
    ICC_LIM_SET = two_byteOrg(readBack);

    WriteRead(0x1D,BD99954_ADDRESS,2,readBack);
    VRECHG_SET = two_byteOrg(readBack);

    WriteRead(0x1E,BD99954_ADDRESS,2,readBack);
    VBATOVP_SET = two_byteOrg(readBack);

    WriteRead(0x11,BD99954_ADDRESS,2,readBack);
    VSYSREG_SET = two_byteOrg(readBack);

    WriteRead(0x18,BD99954_ADDRESS,2,readBack);
    VPRECHG_TH_SET = two_byteOrg(readBack);

    WriteRead(0x1A,BD99954_ADDRESS,2,readBack);
    VFASTCHG_REG_SET1 = two_byteOrg(readBack);

    WriteRead(0x14,BD99954_ADDRESS,2,readBack);
    ITRICH_SET = two_byteOrg(readBack);

    WriteRead(0x15,BD99954_ADDRESS,2,readBack);
    IPRECH_SET = two_byteOrg(readBack);

    WriteRead(0x16,BD99954_ADDRESS,2,readBack);
    ICHG_SET = two_byteOrg(readBack);

    WriteRead(0x17,BD99954_ADDRESS,2,readBack);
    ITERM_SET = two_byteOrg(readBack);

    WriteRead(0x38,BD99954_ADDRESS,2,readBack);
    BD99id = two_byteOrg(readBack);

    WriteRead(0x39,BD99954_ADDRESS,2,readBack);
    BD99rev = two_byteOrg(readBack);

    WriteRead(0x55,BD99954_ADDRESS,2,readBack);
    VBAT_Average = two_byteOrg(readBack);

    WriteRead(0x5D,BD99954_ADDRESS,2,readBack);
    VBUS_Average = two_byteOrg(readBack);

    WriteRead(0x61,BD99954_ADDRESS,2,readBack);
    VSYS_Average = two_byteOrg(readBack);
    LCD_Voltage(VSYS_Average);
    WriteRead(0x3C,BD99954_ADDRESS,2,readBack);
    otpStatus = two_byteOrg(readBack);
    for(i = 0; i < 200; i++);



}
void BD99954_Startup_Routine()
{
    unsigned char *readBack = malloc(sizeof(char)*30);  //Temp Storage of registers
    write_word(0x3E,BD99954_ADDRESS,0x0000);    //Protect set
    write_word(0x3F,BD99954_ADDRESS,0x0001);    //Map Set (VERY IMPORTANT IF YOU WANT ACCESS TO EXTENDED COMMANDS)
    write_word(0x3D,BD99954_ADDRESS,0x0001);
    write_word(0x3D,BD99954_ADDRESS,0x0000);

    for(i=0;i<200;i++);


    write_word(0x07,BD99954_ADDRESS,1472);    //IBUS_LIM_SET
    write_word(0x08,BD99954_ADDRESS,1472);    //ICC_LIM_SET
    write_word(0x1D,BD99954_ADDRESS,8112);    //VRECHG_SET
    write_word(0x1E,BD99954_ADDRESS,8912);    //VBATOVP_SET
    write_word(0x11,BD99954_ADDRESS,8960);    //VSYSREG_SET
    write_word(0x18,BD99954_ADDRESS,2048);    //VPRECHG_TH_SET
    write_word(0x1A,BD99954_ADDRESS,8400);    //VFASTCHG_REG_SET
    write_word(0x14,BD99954_ADDRESS,256);    //ITRICH_SET
    write_word(0x15,BD99954_ADDRESS,256);    //IPRECH_SET
    write_word(0x16,BD99954_ADDRESS,960);    //ICHG_SET
    write_word(0x17,BD99954_ADDRESS,0);    //ITERM_SET

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



