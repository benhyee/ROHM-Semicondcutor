/*
 * BM92A_Funcs.c
 *
 *  Created on: Sep 30, 2018
 *      Author: User
 */

#include "msp.h"
#include <stdint.h>
#include "helper.h"
#include <stdlib.h>
#include "UART.h"
#include "globals.h"
#include "lcd.h"
#include <stdio.h>
#include "I2C_Helper.h"

#define BM92A_ADDRESS 0x18
int i;
//BM92A Local Variables Storing the Register Reads
unsigned int value, RDO, PDO, battery, nonBattery, PDO_SNK_Cons;
unsigned short status_1,status_2,config_1,config_2,Display_alert, vendorConfig;
unsigned short sys_config_1, sys_config_2,sys_config_3 , alertStatus,capability;
unsigned short ALERTEN, SETRDO,FirmType,FirmRev, ManuID,DeviceID,RevID;
//Debugger BM92A Variables
unsigned int maxCurrent , voltage , current ;
unsigned char power_role , recepticle , data_role;



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
