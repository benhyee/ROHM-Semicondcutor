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
unsigned short status,config_1,config_2,Display_alert, vendorConfig;
unsigned short ctrl_config_1, ctrl_config_2;
unsigned short sys_config_1, sys_config_2,sys_config_3 , alertStatus, capability;
unsigned short ALERTEN, SETRDO,FirmType,FirmRev, ManuID,DeviceID,RevID;
//Debugger BM92A Variables
unsigned int maxCurrent, voltage, current ;
unsigned char generalPurpose;
unsigned char SPDSRC, SPDSNK,command ,power_role , plug_orient, data_role;   //status 1 variables
unsigned char recepticle;


void testReadRegistersBM92A()
{
    value=0, RDO = 0, PDO = 0,battery = 0, nonBattery = 0;  //Reinitialize to 0 to ensure fresh write
    status=0,config_1=0,config_2=0,Display_alert = 0, vendorConfig= 0;
    sys_config_1=0, sys_config_2=0, alertStatus=0,capability=0;
    ALERTEN=0, SETRDO=0,FirmType=0,FirmRev=0, ManuID=0,DeviceID=0,RevID=0;

    unsigned char *readBack = malloc(sizeof(char)*30);
    unsigned int *PDO_SNK_Cons = malloc(sizeof(unsigned int)*4);

//    WriteRead(0x08,BM92A_ADDRESS,29,readBack);//PDO Snk register
//    WriteRead(0x33,BM92A_ADDRESS,17,readBack);//PDO Snk register

//    WriteRead(0x33,BM92A_ADDRESS,17,readBack);//PDO SRC register

//    WriteRead(0x08,BM92A_ADDRESS,21,readBack);//PDO SRC register

    WriteRead(0x02,BM92A_ADDRESS,2,readBack); alertStatus = two_byteOrg(readBack);//Alert register
    WriteRead(0x03,BM92A_ADDRESS,2,readBack); status = two_byteOrg(readBack); //status 1 register
    WriteRead(0x04,BM92A_ADDRESS,2,readBack); status = two_byteOrg(readBack);//status 2 register
    WriteRead(0x06,BM92A_ADDRESS,2,readBack); config_1 = two_byteOrg(readBack);//controller config 1
    WriteRead(0x07,BM92A_ADDRESS,2,readBack); capability = two_byteOrg(readBack);//Capability register
    WriteRead(0x17,BM92A_ADDRESS,2,readBack); config_2 = two_byteOrg(readBack);//controller config 2
    WriteRead(0x19,BM92A_ADDRESS,2,readBack); Display_alert = two_byteOrg(readBack);//Display port register
    WriteRead(0x1A,BM92A_ADDRESS,2,readBack); vendorConfig = two_byteOrg(readBack);//Vendor Config register
    WriteRead(0x20,BM92A_ADDRESS,5,readBack); nonBattery = four_byteOrg(readBack);//Autongtsnk Info non-Battery register
    WriteRead(0x23,BM92A_ADDRESS,5,readBack); battery = four_byteOrg(readBack);//Autongtsnk Info Battery register

    WriteRead(0x26,BM92A_ADDRESS,2,readBack); sys_config_1 = two_byteOrg(readBack);//system controller config 1
    WriteRead(0x27,BM92A_ADDRESS,2,readBack); sys_config_2 = two_byteOrg(readBack);//system controller config 2
    WriteRead(0x28,BM92A_ADDRESS,5,readBack); PDO = four_byteOrg(readBack);//PDO register
    WriteRead(0x2B,BM92A_ADDRESS,5,readBack); RDO = four_byteOrg(readBack);//RDO register
    WriteRead(0x2F,BM92A_ADDRESS,2,readBack); sys_config_3 = two_byteOrg(readBack); //system controller config 2


    WriteRead(0x2E,BM92A_ADDRESS,2,readBack); ALERTEN = two_byteOrg(readBack);
    WriteRead(0x30,BM92A_ADDRESS,4,readBack); SETRDO = four_byteOrg(readBack);
    WriteRead(0x4B,BM92A_ADDRESS,2,readBack); FirmType = two_byteOrg(readBack);
    WriteRead(0x4C,BM92A_ADDRESS,2,readBack); FirmRev = two_byteOrg(readBack);
    WriteRead(0x4D,BM92A_ADDRESS,2,readBack); ManuID = two_byteOrg(readBack);
    WriteRead(0x4E,BM92A_ADDRESS,2,readBack); DeviceID = two_byteOrg(readBack);
    WriteRead(0x4F,BM92A_ADDRESS,2,readBack); RevID = two_byteOrg(readBack);

    for(i = 0; i<40; i++);

}


void status1_Debug(unsigned short status_1)
{
    terminal_transmitWord("Status 1 States \r\n");
    terminal_transmitWord("------------------------------\r\n");
    SPDSRC = (status_1 & 0x8000)>>15;
    SPDSNK = (status_1 & 0x4000)>>14;

    switch(SPDSRC)
    {
    case 0:
        terminal_transmitWord("SPDSRC1 Off \t");
        break;
    case 1:
        terminal_transmitWord("SPDSRC1 On \t");
        break;
    default:
        break;
    }
    switch(SPDSNK)
    {
    case 0:
        terminal_transmitWord("SPDSNK Off \t");
        break;
    case 1:
        terminal_transmitWord("SPDSNK On \t");
        break;
    default:
        break;
    }
    SPDSRC = (status_1 & 0x8)>>3;
    switch(SPDSRC)
    {
    case 0:
        terminal_transmitWord("SPDSRC2 Off \r\n");
        break;
    case 1:
        terminal_transmitWord("SPDSRC2 On \r\n");
        break;
    default:
        break;
    }
    generalPurpose = (status_1 & 0x800)>>11;
    switch(generalPurpose)
    {
    case 0:
       terminal_transmitWord("CC1 Side Valid \t");
       break;
    case 1:
       terminal_transmitWord("CC2 Side Valid \t");
       break;
    default:
       break;
    }
    generalPurpose = (status_1 &0x2000)>>13;
    switch(generalPurpose)
    {
    case 0:
        terminal_transmitWord("No Command in Progress \r\n");
        break;
    case 1:
        terminal_transmitWord("Command in Progress \r\n");
        break;
    default:
        break;
    }
    generalPurpose = (status_1 & 0x1000)>>12;


    switch(generalPurpose)
    {
    case 0:
       terminal_transmitWord("BM92A is Sink \t");
       break;
    case 1:
       terminal_transmitWord("BM92A is Source \t");
       break;
    default:
       break;
    }

    generalPurpose = (status_1 & 0xC0) >> 8;
    switch(generalPurpose)
    {
    case 1:
        terminal_transmitWord("UFP (Device) \t");
        break;
    case 2:
        terminal_transmitWord("DFP (Host) \t");
        break;
    case 3:
        terminal_transmitWord("In Accessory Mode \t");
        break;
    default:
        terminal_transmitWord("No plug inserted \t");
        break;
    }
    generalPurpose = (status_1 & 0x400) >> 10;
    switch(generalPurpose)
    {
    case 0:
        terminal_transmitWord("Unpowered or vSafe0V\r\n\n");
        break;
    case 1:
        terminal_transmitWord("vSafe5V or negotiated Power Level\r\n");
        break;
    default:
        break;
    }

    terminal_transmitWord("------------------------------\r\n");

}
void status2_Debug(unsigned short status_2)
{
    terminal_transmitWord("Status 2 States \r\n");
    terminal_transmitWord("------------------------------\r\n");
    generalPurpose = (status_2 & 0x1000)>>12;

    switch(generalPurpose)
    {
    case 0:
       terminal_transmitWord("Not Electronically Marked Cable \r\n");
       break;
    case 1:
       terminal_transmitWord("Electronically Marked Cable\r\n");
       break;
    default:
       break;
    }
    generalPurpose = (status_2 & 0xC00)>>10;
    switch(generalPurpose)
    {
    case 1:
        terminal_transmitWord("Audio Accessory \r\n");
        break;
    case 2:
        terminal_transmitWord("Debug Accessory \r\n");
        break;
    case 3:
        terminal_transmitWord("VCONN Powered Accessory \r\n");
        break;
    default:
        terminal_transmitWord("Not in accessory Mode) \r\n");
        break;
    }

    generalPurpose = (status_2 & 0x200) >> 9;
    switch(generalPurpose)
    {
    case 0:
        terminal_transmitWord("VCONN Off\t");
        break;
    case 1:
        terminal_transmitWord("VCONN On\t");
        break;
    default:
        break;
    }
    generalPurpose = (status_2 & 0x8) >> 3;
    switch(generalPurpose)
    {
    case 0:
        terminal_transmitWord("Source PDO or no PDO\r\n\n");
        break;
    case 1:
        terminal_transmitWord("Sink PDO\r\n\n");
        break;
    default:
        break;
    }
    terminal_transmitWord("------------------------------\r\n");

}

void control_config1(unsigned short config_1)
{
    terminal_transmitWord("Controller Configuration 1 States \r\n");
    terminal_transmitWord("------------------------------\r\n");
    generalPurpose = (config_1 & 0xC000) >> 14;

    switch(generalPurpose)
    {
    case 0:
       terminal_transmitWord("SPDSRC1/SPDSRC2 On/Off \r\n");
       break;
    case 3:
       terminal_transmitWord("SPDSRC1/SPDSRC2 always Off \r\n");
       break;
    default:
       break;
    }

    generalPurpose = (config_1 & 0xC00) >> 10;
    terminal_transmitWord("VIN_EN/VOUT_EN turn-on delay: ");
    switch(generalPurpose)
    {
    case 0:
        terminal_transmitWord("1ms \r\n");
        break;
    case 1:
        terminal_transmitWord("5ms \r\n");
        break;
    case 2:
        terminal_transmitWord("10ms \r\n");
        break;
    case 3:
        terminal_transmitWord("20ms \r\n");
        break;
    default:
        break;
    }

    generalPurpose = (config_1 & 0x200) >> 9;
    terminal_transmitWord("VIN_EN/VOUT_EN turn-off delay: ");
    switch(generalPurpose)
    {
    case 0:
        terminal_transmitWord("<250 us \r\n");
        break;
    case 1:
        terminal_transmitWord("1ms \r\n");
        break;
    default:
        break;
    }

    generalPurpose = (config_1 & 0x100) >> 8;
    terminal_transmitWord("Negotiation using AutoNgtSnk Info Battery: ");
    switch(generalPurpose)
    {
    case 0:
        terminal_transmitWord("Disabled \r\n");
        break;
    case 1:
        terminal_transmitWord("Enabled \r\n");
        break;
    default:
        break;
    }

    generalPurpose = (config_1 & 0x80) >> 7;
    terminal_transmitWord("USB Communication Capable at AutoNgtSnk: ");
    switch(generalPurpose)
    {
    case 0:
        terminal_transmitWord("Not Required \r\n");
        break;
    case 1:
        terminal_transmitWord("Required \r\n");
        break;
    default:
        break;
    }

    generalPurpose = (config_1 & 0x40) >> 6;
    switch(generalPurpose)
    {
    case 0:
        terminal_transmitWord("PD controller does not automatically negotiate as sink \r\n");
        break;
    case 1:
        terminal_transmitWord("PD controller automatically negotiates for a fixed source when operating as sink \r\n");
        break;
    default:
        break;
    }

    generalPurpose = (config_1 & 0x20) >> 5;
    switch(generalPurpose)
    {
    case 0:
        terminal_transmitWord("Negotiates with fixed supply PDO only \r\n");
        break;
    case 1:
        terminal_transmitWord("Negotiates with fixed of variable supply PDO \r\n");
        break;
    default:
        break;
    }

    generalPurpose = (config_1 & 0x10) >> 4;
    switch(generalPurpose)
    {
    case 0:
        terminal_transmitWord("Enter sleep mode request cleared \r\n");
        break;
    case 1:
        terminal_transmitWord("PD controller entering lowest power consumption level \r\n");
        break;
    default:
        break;
    }

    generalPurpose = (config_1 & 0x02) >> 1;
    switch(generalPurpose)
    {
    case 0:
        terminal_transmitWord("Controller does not automatically initiate swap request \r\n\n");
        break;
    case 1:
        terminal_transmitWord("Controller automatically initiates swap request \r\n\n");
        break;
    default:
        break;
    }
    terminal_transmitWord("------------------------------\r\n");
}

void control_config2(unsigned short config_2)
{
    terminal_transmitWord("Controller Configuration 2 States \r\n");
    terminal_transmitWord("------------------------------\r\n");

    generalPurpose = (config_2 & 0x300) >> 8;
    terminal_transmitWord("USB Type-C Current: ");
    switch(generalPurpose)
    {
    case 0:
       terminal_transmitWord("Default USB \r\n");
       break;
    case 1:
       terminal_transmitWord("1.5A @ 5V \r\n");
       break;
    case 2:
       terminal_transmitWord("3A @ 5V \r\n");
       break;
    default:
       break;
    }

    generalPurpose = (config_2 & 0x80) >> 7;
    switch(generalPurpose)
    {
    case 0:
       terminal_transmitWord("Not Externally Powered \r\n");
       break;
    case 1:
       terminal_transmitWord("Externally powered \r\n");
       break;
    default:
       break;
    }

    generalPurpose = (config_2 & 0x20) >> 5;
    terminal_transmitWord("No USB Suspend flag = ");
    switch(generalPurpose)
    {
    case 0:
       terminal_transmitWord("0b \r\n");
       break;
    case 1:
       terminal_transmitWord("1b \r\n");
       break;
    default:
       break;
    }

    generalPurpose = (config_2 & 0x10) >> 4;
    terminal_transmitWord("VCONN Source Swap request: ");
    switch(generalPurpose)
    {
    case 0:
       terminal_transmitWord("Rejected \r\n");
       break;
    case 1:
       terminal_transmitWord("Accepted \r\n");
       break;
    default:
       break;
    }

    generalPurpose = (config_2 & 0x0C) >> 2;
    terminal_transmitWord("Data Role Swap request: ");
    switch(generalPurpose)
    {
    case 0:
       terminal_transmitWord("Always reject \r\n");
       break;
    case 1:
       terminal_transmitWord("Accept when UFP, reject when DFP \r\n");
       break;
    case 2:
       terminal_transmitWord("Accept when DFP, reject when UFP \r\n");
       break;
    case 3:
       terminal_transmitWord("Always accept \r\n");
       break;
    default:
       break;
    }

    generalPurpose = config_2 & 0x03;
    terminal_transmitWord("Power Role Swap request: ");
    switch(generalPurpose)
    {
    case 0:
       terminal_transmitWord("Always reject \r\n\n");
       break;
    case 1:
       terminal_transmitWord("Accept when power sink, reject when power source \r\n\n");
       break;
    case 2:
       terminal_transmitWord("Accept when power source, reject when power sink \r\n\n");
       break;
    case 3:
       terminal_transmitWord("Always accept \r\n\n");
       break;
    default:
       break;
    }

    terminal_transmitWord("------------------------------\r\n");
}

void system_config1(unsigned short sys_config1)
{
    terminal_transmitWord("System Configuration 1 States \r\n");
    terminal_transmitWord("------------------------------\r\n");

    generalPurpose = (sys_config1 & 0x8000) >> 15;
    switch(generalPurpose)
    {
    case 1:
       terminal_transmitWord("Wake up on plug insert \r\n");
       break;
    default:
       break;
    }

    generalPurpose = (sys_config1 & 0x6000) >> 13;
    terminal_transmitWord("Output of DOUT4 PDO: ");
    switch(generalPurpose)
    {
    case 0:
       terminal_transmitWord("PDO4 \r\n");
       break;
    case 1:
       terminal_transmitWord("PDO5 \r\n");
       break;
    case 2:
       terminal_transmitWord("PDO6 \r\n");
       break;
    case 3:
       terminal_transmitWord("PDO7 \r\n");
       break;
    default:
       break;
    }

    generalPurpose = (sys_config1 & 0x1000) >> 12;
    terminal_transmitWord("PDO Src Prov: ");
    switch(generalPurpose)
    {
    case 0:
       terminal_transmitWord("Not available \r\n");
       break;
    case 1:
       terminal_transmitWord("Available \r\n");
       break;
    default:
       break;
    }

    generalPurpose = (sys_config1 & 0xE00) >> 9;
    terminal_transmitWord("PDO Snk Cons supported: ");
    switch(generalPurpose)
    {
    case 0:
       terminal_transmitWord("None \r\n");
       break;
    case 1:
       terminal_transmitWord("1 \r\n");
       break;
    case 2:
       terminal_transmitWord("2 \r\n");
       break;
    case 3:
       terminal_transmitWord("3 \r\n");
       break;
    case 4:
       terminal_transmitWord("4 \r\n");
       break;
    default:
       break;
    }

    generalPurpose = (sys_config1 & 0x100) >> 8;
    switch(generalPurpose)
    {
    case 1:
       terminal_transmitWord("PDO Src Cons supported \r\n");
       break;
    default:
       break;
    }

    generalPurpose = (sys_config1 & 0x40) >> 6;
    switch(generalPurpose)
    {
    case 0:
       terminal_transmitWord("Never use auto negotiation with no host \r\n");
       break;
    case 1:
       terminal_transmitWord("Use auto negotiation when Host is not present \r\n");
       break;
    default:
       break;
    }

    generalPurpose = (sys_config1 & 0x0F);
    terminal_transmitWord("Receptacle Type: ");
    switch(generalPurpose)
    {
    case 0:
       terminal_transmitWord("Unconfirmed \r\n\n");
       break;
    case 9:
       terminal_transmitWord("Type-C, Receptacle \r\n\n");
       break;
    case 10:
       terminal_transmitWord("Type C, 3A Captive cable \r\n\n");
       break;
    case 11:
       terminal_transmitWord("Type C, 5A Captive cable \r\n\n");
       break;
    default:
       break;
    }

    terminal_transmitWord("------------------------------\r\n");
}


void system_config2(unsigned short sys_config2)
{
    terminal_transmitWord("System Configuration 2 States \r\n");
    terminal_transmitWord("------------------------------\r\n");

    generalPurpose = (sys_config2 & 0xE00) >> 9;
    terminal_transmitWord("PDO Src Prov Count: ");
    switch(generalPurpose)
    {
    case 0:
       terminal_transmitWord("Error \r\n");
       break;
    case 1:
       terminal_transmitWord("1 \r\n");
       break;
    case 2:
       terminal_transmitWord("2 \r\n");
       break;
    case 3:
       terminal_transmitWord("3 \r\n");
       break;
    case 4:
       terminal_transmitWord("4 \r\n");
       break;
    case 5:
       terminal_transmitWord("5 \r\n");
       break;
    case 6:
       terminal_transmitWord("6 \r\n");
       break;
    case 7:
       terminal_transmitWord("7 \r\n");
       break;
    default:
       break;
    }

    generalPurpose = (sys_config2 & 0x100) >> 8;
    terminal_transmitWord("BST_EN: ");
    switch(generalPurpose)
    {
    case 0:
       terminal_transmitWord("Not supported \r\n");
       break;
    case 1:
       terminal_transmitWord("Supported \r\n");
       break;
    default:
       break;
    }

    generalPurpose = (sys_config2 & 0x40) >> 6;
    switch(generalPurpose)
    {
    case 0:
       terminal_transmitWord("SPDSNK is not turned on automatically \r\n");
       break;
    case 1:
       terminal_transmitWord("SPDSNK is turned on automatically without set SYS_RDY. \r\n");
       break;
    default:
       break;
    }

    generalPurpose = (sys_config2 & 0x04) >> 2;
    terminal_transmitWord("Force No USB Comm Capable Dead Battery: ");
    switch(generalPurpose)
    {
    case 0:
       terminal_transmitWord("0b \r\n"); // see datasheet
       break;
    case 1:
       terminal_transmitWord("1b \r\n"); // see datasheet
       break;
    default:
       break;
    }

    generalPurpose = (sys_config2 & 0x02) >> 1;
    terminal_transmitWord("Force No USB Comm Capable DFP: ");
    switch(generalPurpose)
    {
    case 0:
       terminal_transmitWord("0b \r\n"); // see datasheet
       break;
    case 1:
       terminal_transmitWord("1b \r\n"); // see datasheet
       break;
    default:
       break;
    }

    generalPurpose = (sys_config2 & 0x01);
    terminal_transmitWord("Force No USB Comm Capable UFP: ");
    switch(generalPurpose)
    {
    case 0:
       terminal_transmitWord("0b \r\n\n"); // see datasheet
       break;
    case 1:
       terminal_transmitWord("1b \r\n\n"); // see datasheet
       break;
    default:
       break;
    }


    terminal_transmitWord("------------------------------\r\n");
}

void device_capability(unsigned short cap_config)
{
    terminal_transmitWord("Device Capability States \r\n");
    terminal_transmitWord("------------------------------\r\n");

    generalPurpose = (cap_config & 0x800) >> 11;
    terminal_transmitWord("VOUT_EN1: ");
    switch(generalPurpose)
    {
    case 0:
        terminal_transmitWord("Not supported \r\n");
        break;
    case 1:
        terminal_transmitWord("Supported \r\n");
        break;
    default:
        break;
    }

    generalPurpose = (cap_config & 0x400) >> 10;
    terminal_transmitWord("DisplayPort Sink: ");
    switch(generalPurpose)
    {
    case 0:
        terminal_transmitWord("Not supported \r\n");
        break;
    case 1:
        terminal_transmitWord("Supported \r\n");
        break;
    default:
        break;
    }

    generalPurpose = (cap_config & 0x200) >> 9;
    terminal_transmitWord("DisplayPort Source: ");
    switch(generalPurpose)
    {
    case 0:
        terminal_transmitWord("Not supported \r\n");
        break;
    case 1:
        terminal_transmitWord("Supported \r\n");
        break;
    default:
        break;
    }

    generalPurpose = (cap_config & 0x100) >> 8;
    terminal_transmitWord("Over Current Protection: ");
    switch(generalPurpose)
    {
    case 0:
        terminal_transmitWord("Not supported \r\n");
        break;
    case 1:
        terminal_transmitWord("Supported \r\n");
        break;
    default:
        break;
    }

    generalPurpose = (cap_config & 0x80) >> 7;
    terminal_transmitWord("SPDSNK: ");
    switch(generalPurpose)
    {
    case 0:
        terminal_transmitWord("Not supported \t");
        break;
    case 1:
        terminal_transmitWord("Supported \t");
        break;
    default:
        break;
    }

    generalPurpose = (cap_config & 0x40) >> 6;
    terminal_transmitWord("SPRL: ");
    switch(generalPurpose)
    {
    case 0:
        terminal_transmitWord("Not supported \r\n");
        break;
    case 1:
        terminal_transmitWord("Supported \r\n");
        break;
    default:
        break;
    }

    generalPurpose = (cap_config & 0x20) >> 5;
    terminal_transmitWord("SPDSRC1: ");
    switch(generalPurpose)
    {
    case 0:
        terminal_transmitWord("Not supported \t");
        break;
    case 1:
        terminal_transmitWord("Supported \t");
        break;
    default:
        break;
    }

    generalPurpose = (cap_config & 0x10) >> 4;
    terminal_transmitWord("SPDSRC2: ");
    switch(generalPurpose)
    {
    case 0:
        terminal_transmitWord("Not supported \r\n");
        break;
    case 1:
        terminal_transmitWord("Supported \r\n");
        break;
    default:
        break;
    }

    generalPurpose = (cap_config & 0x08) >> 3;
    terminal_transmitWord("VOUT_EN: ");
    switch(generalPurpose)
    {
    case 0:
        terminal_transmitWord("Not supported \t");
        break;
    case 1:
        terminal_transmitWord("Supported \t");
        break;
    default:
        break;
    }

    generalPurpose = (cap_config & 0x04) >> 2;
    terminal_transmitWord("VIN_EN: ");
    switch(generalPurpose)
    {
    case 0:
        terminal_transmitWord("Not supported \r\n");
        break;
    case 1:
        terminal_transmitWord("Supported \r\n");
        break;
    default:
        break;
    }

    generalPurpose = (cap_config & 0x02) >> 1;
    terminal_transmitWord("ALERT#: ");
    switch(generalPurpose)
    {
    case 0:
        terminal_transmitWord("Not supported \t");
        break;
    case 1:
        terminal_transmitWord("Supported \t");
        break;
    default:
        break;
    }

    generalPurpose = (cap_config & 0x01);
    terminal_transmitWord("ALERT# Status: ");
    switch(generalPurpose)
    {
    case 0:
        terminal_transmitWord("Not supported \r\n\n");
        break;
    case 1:
        terminal_transmitWord("Supported \r\n\n");
        break;
    default:
        break;
    }

    terminal_transmitWord("------------------------------\r\n");
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

void BM92A_Debugger()
{
    terminal_transmitWord("------------------------------\r\n");
    unsigned char *readBack = malloc(sizeof(char)*30);  //Temp Storage of registers
    WriteRead(0x28,BM92A_ADDRESS,5,readBack);//PDO register
    PDO = four_byteOrg(readBack);
    WriteRead(0x03,BM92A_ADDRESS,2,readBack); //status 1 register
    status = two_byteOrg(readBack);
    status1_Debug(status);

    WriteRead(0x04,BM92A_ADDRESS,2,readBack); //status 2 register
    status = two_byteOrg(readBack);
    status2_Debug(status);

    WriteRead(0x06,BM92A_ADDRESS,2,readBack); //controller config 1
    ctrl_config_1 = two_byteOrg(readBack);
    control_config1(ctrl_config_1);

    WriteRead(0x07,BM92A_ADDRESS,2,readBack); //device capability
    capability = two_byteOrg(readBack);
    device_capability(capability);

    WriteRead(0x17,BM92A_ADDRESS,2,readBack); //controller config 2
    ctrl_config_2 = two_byteOrg(readBack);
    control_config2(ctrl_config_2);

    WriteRead(0x26,BM92A_ADDRESS,2,readBack); //system controller config 1
    sys_config_1 = two_byteOrg(readBack);
    system_config1(sys_config_1);

    WriteRead(0x27,BM92A_ADDRESS,2,readBack); //system controller config 2
    sys_config_2 = two_byteOrg(readBack);
    system_config2(sys_config_2);

    WriteRead(0x2B,BM92A_ADDRESS,5,readBack);//RDO register
    RDO = four_byteOrg(readBack);


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

