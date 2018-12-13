/*
 * debugFunctions.c
 *
 *  Created on: Oct 23, 2018
 *       Author: Benjamin Yee
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
#include "delay.h"
#include "debugFunctions.h"
#include "helper.h"
int i;

//BD99954 Variables
unsigned short IBUS_LIM_SET , ICC_LIM_SET , VRECHG_SET , VBATOVP_SET ;
unsigned short VSYSREG_SET , VPRECHG_TH_SET , VFASTCHG_REG_SET1;
unsigned short ITRICH_SET, IPRECH_SET , ICHG_SET, ITERM_SET, BD99id, BD99rev;
unsigned short VBUS_Average, VSYS_Average, VBAT_Average, otpStatus,CHGOP_SET2;

unsigned int setValue;

//BM92A Local Variables Storing the Register Reads
unsigned int value, RDO, PDO, battery, nonBattery, PDO_SNK_Cons;
unsigned short status1,status2,config_1,config_2,Display_alert, vendorConfig;
unsigned short ctrl_config_1, ctrl_config_2;
unsigned short sys_config_1, sys_config_2,sys_config_3 , alertStatus, capability;
unsigned short ALERTEN, SETRDO,FirmType,FirmRev, ManuID,DeviceID,RevID;
unsigned short generalShort;
//Debugger BM92A Variables
unsigned int maxCurrent, voltage, current ;
unsigned char generalPurpose;
unsigned char SPDSRC, SPDSNK,command ,power_role , plug_orient, data_role;   //status 1 variables
unsigned char recepticle;

#define BD99954_ADDRESS 0x09
#define BM92A_ADDRESS 0x18

/* BD99954 Debug Functions */

void IBUS_LIM_Debug(unsigned short IBUS_LIM)
{
    terminal_transmitWord("VBUS input current limit = ");
    setValue = (IBUS_LIM & 0x3FE0) >> 5;
    setValue = setValue * 32;  //32 mA steps
    terminal_transmitInt(setValue);
    terminal_transmitWord("A \r\n");
}
void ICC_LIM_Debug(unsigned short ICC_LIM)
{
    terminal_transmitWord("VCC input current limit = ");
    setValue  = (ICC_LIM & 0x3FE0) >> 5;
    setValue = setValue * 32; //32 mA steps
    terminal_transmitInt(setValue);
    terminal_transmitWord("A \r\n");
}
void VRECHG_Debug(unsigned short VRECHG)
{
    terminal_transmitWord("Re-charge Battery Voltage = ");
    setValue  = (VRECHG & 0x7FF0) >> 4;
    setValue = setValue * 16; //16 mV steps
    terminal_transmitInt(setValue);
    terminal_transmitWord("V \r\n");
}
void VBATOVP_Debug(unsigned short VBATOVP)
{
    terminal_transmitWord("Battery over-voltage detection threshold = ");
    setValue  = (VBATOVP & 0x7FF0) >> 4;
    setValue = setValue * 16; //16 mV steps
    terminal_transmitInt(setValue);
    terminal_transmitWord("V \r\n");
}
void VSYSREG_Debug(unsigned short VSYSREG)
{
    terminal_transmitWord("VSYS regulation voltage : "); //threshold voltage from Pre-charging to Fast-charging.
    setValue  = (VSYSREG & 0x7FC0) >> 6;
    setValue = setValue * 64; //64 mV steps
    terminal_transmitInt(setValue);
    terminal_transmitWord("V \r\n");
}
void VPRECHG_TH_Debug(unsigned short VPRECHG_TH)
{
    terminal_transmitWord("Pre-charge voltage threshold = "); //Threshold voltage from Trickle-charging to Pre-charging
    setValue  = (VPRECHG_TH & 0x7FC0) >> 6;
    setValue = setValue * 64; //64 mV steps
    terminal_transmitInt(setValue);
    terminal_transmitWord("V \r\n");
}
void VFASTCHG_REG_Debug(unsigned short VFASTCHG_REG)
{
    terminal_transmitWord("Fast Charging Regulation = ");
    setValue  = (VFASTCHG_REG & 0x7FF0) >> 4;
    setValue = setValue * 16; //16 mV steps
    terminal_transmitInt(setValue);
    terminal_transmitWord("V \r\n");
}
void ITRICH_Debug(unsigned short ITRICH)
{
    terminal_transmitWord("Trickle-charging current = ");
    setValue  = (ITRICH & 0x7C0) >> 6;
    setValue = setValue * 64; //64 mA steps
    terminal_transmitInt(setValue);
    terminal_transmitWord("A \r\n");
}
void IPRECH_Debug(unsigned short IPRECH)
{
    terminal_transmitWord("Pre-charging current = ");
    setValue  = (IPRECH & 0x7C0) >> 6;
    setValue = setValue * 64; //64 mA steps
    terminal_transmitInt(setValue);
    terminal_transmitWord("A \r\n");
}
void ICHG_Debug(unsigned short ICHG)
{
    terminal_transmitWord("Fast-charging current = ");
    setValue  = (ICHG & 0x3FC0) >> 6;
    setValue = setValue * 64; //64 mA steps
    terminal_transmitInt(setValue);
    terminal_transmitWord("A \r\n");
}
void ITERM_Debug(unsigned short ITERM)
{
    terminal_transmitWord("Charging Termination Current = ");
    setValue  = (ITERM & 0x7C0) >> 6;
    setValue = setValue * 64; //64 mA steps
    terminal_transmitInt(setValue);
    terminal_transmitWord("A \r\n");
}
void BD99ID_Debug(unsigned short CHIP_ID)
{
    terminal_transmitWord("CHIP_ID = 0x");
    generalPurpose = ((CHIP_ID & 0xF000) >> 12);
    terminal_transmitByte(generalPurpose);

    generalPurpose = ((CHIP_ID & 0x0F00) >> 8);
    terminal_transmitByte(generalPurpose);

    generalPurpose = ((CHIP_ID & 0x00F0) >> 4);
    terminal_transmitByte(generalPurpose);

    generalPurpose = (CHIP_ID & 0x000F);
    terminal_transmitByte(generalPurpose);
    terminal_transmitWord("\r\n");
}
void BD99REV_Debug(unsigned short CHIP_REV)
{
    terminal_transmitWord("CHIP_REV = 0x");

    generalPurpose = ((CHIP_REV & 0xF000) >> 12);
    terminal_transmitByte(generalPurpose);

    generalPurpose = ((CHIP_REV & 0x0F00) >> 8);
    terminal_transmitByte(generalPurpose);

    generalPurpose = ((CHIP_REV & 0x00F0) >> 4);
    terminal_transmitByte(generalPurpose);

    generalPurpose = (CHIP_REV & 0x000F);
    terminal_transmitByte(generalPurpose);

    terminal_transmitWord("\r\n");
}
void VBAT_AVE_Debug(unsigned short VBAT_AVE)
{
    terminal_transmitWord("Battery Voltage Average = ");
    setValue  = (VBAT_AVE & 0x7FFF) ;
    terminal_transmitInt(setValue);
    terminal_transmitWord("V \r\n");
}
void VBUS_AVE_Debug(unsigned short VBUS_AVE)
{
    terminal_transmitWord("VBUS Voltage Average = ");
    setValue  = (VBUS_AVE & 0x7FFF) ;
    terminal_transmitInt(setValue);
    terminal_transmitWord("V \r\n");
}
void VSYS_AVE_Debug(unsigned short VSYS_AVE)
{
    terminal_transmitWord("VSYS Voltage Average = ");
    setValue  = (VSYS_AVE & 0x7FFF) ;
    terminal_transmitInt(setValue);
    terminal_transmitWord("V \r\n");
}
void SYS_STATUS_Debug(unsigned short SYS_STATUS)
{
    terminal_transmitWord("\r\n");
    terminal_transmitWord("System Status Registers: ");
    terminal_transmitWord("\r\n");

    generalPurpose = (SYS_STATUS & 0x40) >> 6;
    switch(generalPurpose)
    {
    case 0:
       terminal_transmitWord("MONRST reset released \r\n");
       break;
    case 1:
       terminal_transmitWord("MONRST reset asserted \r\n");
       break;
    default:
       break;
    }

    generalPurpose = (SYS_STATUS & 0x20) >> 5;
    switch(generalPurpose)
    {
    case 0:
       terminal_transmitWord("ALMRST reset released \r\n");
       break;
    case 1:
       terminal_transmitWord("ALMRST reset asserted \r\n");
       break;
    default:
       break;
    }

    generalPurpose = (SYS_STATUS & 0x10) >> 4;
    switch(generalPurpose)
    {
    case 0:
       terminal_transmitWord("CHGRST reset released \r\n");
       break;
    case 1:
       terminal_transmitWord("CHGRST reset asserted \r\n");
       break;
    default:
       break;
    }

    generalPurpose = (SYS_STATUS & 0x02) >> 1;
    switch(generalPurpose)
    {
    case 0:
       terminal_transmitWord("OTPROM loading is not finished \r\n");
       break;
    case 1:
       terminal_transmitWord("OTPROM loading is finished \r\n");
       break;
    default:
       break;
    }

    generalPurpose = (SYS_STATUS & 0x1);
    switch(generalPurpose)
    {
    case 0:
       terminal_transmitWord("ALLRST reset released \r\n");
       break;
    case 1:
       terminal_transmitWord("ALLRST reset asserted \r\n");
       break;
    default:
       break;
    }
}
void BD99954ReadRegister()
{
    IBUS_LIM_SET = 0 , ICC_LIM_SET = 0, VRECHG_SET = 0, VBATOVP_SET = 0;
    VSYSREG_SET = 0, VPRECHG_TH_SET = 0, VFASTCHG_REG_SET1 = 0;
    ITRICH_SET = 0, IPRECH_SET = 0, ICHG_SET = 0, ITERM_SET = 0;
    BD99id = 0, BD99rev = 0, CHGOP_SET2 = 0;
    VBUS_Average = 0,VSYS_Average = 0, VBAT_Average = 0;
    unsigned char *readBack = malloc(sizeof(char)*30);  //Temp Storage of registers

    terminal_transmitWord("\r\n");
    terminal_transmitWord("------------------------------\r\n");
    terminal_transmitWord("BD99954 Debug Registers: \r\n");
    terminal_transmitWord("\r\n");

    WriteRead(0x07,BD99954_ADDRESS,2,readBack);
    IBUS_LIM_SET = two_byteOrg(readBack);
    WriteRead(0x08,BD99954_ADDRESS,2,readBack);
    ICC_LIM_SET = two_byteOrg(readBack);
    WriteRead(0x0A,BD99954_ADDRESS,2,readBack);
    BD99id = two_byteOrg(readBack);
    WriteRead(0x0C,BD99954_ADDRESS,2,readBack);
    CHGOP_SET2 = two_byteOrg(readBack);
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
    WriteRead(0x3C,BD99954_ADDRESS,2,readBack);
    otpStatus = two_byteOrg(readBack);

    IBUS_LIM_Debug(IBUS_LIM_SET);
    ICC_LIM_Debug(ICC_LIM_SET);
    VRECHG_Debug(VRECHG_SET);
    VBATOVP_Debug(VBATOVP_SET);
    VSYSREG_Debug(VSYSREG_SET);
    VPRECHG_TH_Debug(VPRECHG_TH_SET);
    VFASTCHG_REG_Debug(VFASTCHG_REG_SET1);
    ITRICH_Debug(ITRICH_SET);
    IPRECH_Debug(IPRECH_SET);
    ICHG_Debug(ICHG_SET);
    ITERM_Debug(ITERM_SET);
//    BD99ID_Debug(BD99id);
//    BD99REV_Debug(BD99rev);
    VBAT_AVE_Debug(VBAT_Average);
    VBUS_AVE_Debug(VBUS_Average);
    VSYS_AVE_Debug(VSYS_Average);
    SYS_STATUS_Debug(otpStatus);
    LCD_Voltage(VSYS_Average);

    terminal_transmitWord("\r\n");
    terminal_transmitWord("------------------------------\r\n");

    for(i = 0; i < 200; i++);

}


/*BM92A Debug menu */
void testReadRegistersBM92A()
{
    value=0, RDO = 0, PDO = 0,battery = 0, nonBattery = 0;  //Reinitialize to 0 to ensure fresh write
    status1=0,status2=0,config_1=0,config_2=0,Display_alert = 0, vendorConfig= 0;
    sys_config_1=0, sys_config_2=0, alertStatus=0,capability=0;
    ALERTEN=0, SETRDO=0,FirmType=0,FirmRev=0, ManuID=0,DeviceID=0,RevID=0;

    unsigned char *readBack = malloc(sizeof(char)*30);
    unsigned int *PDO_SNK_Cons = malloc(sizeof(unsigned int)*4);

//    WriteRead(0x08,BM92A_ADDRESS,29,readBack);//PDO Snk register
//    WriteRead(0x33,BM92A_ADDRESS,17,readBack);//PDO Snk register

//    WriteRead(0x33,BM92A_ADDRESS,17,readBack);//PDO SRC register

//    WriteRead(0x08,BM92A_ADDRESS,21,readBack);//PDO SRC register

    WriteRead(0x02,BM92A_ADDRESS,2,readBack); alertStatus = two_byteOrg(readBack);//Alert register
    WriteRead(0x03,BM92A_ADDRESS,2,readBack); status1 = two_byteOrg(readBack); //status 1 register
    WriteRead(0x04,BM92A_ADDRESS,2,readBack); status2 = two_byteOrg(readBack);//status 2 register
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
    free(readBack);
    free(PDO_SNK_Cons);
}

void BM92A_Debugger()
{
    terminal_transmitWord("------------------------------\r\n");
    unsigned char *readBack = malloc(sizeof(char)*30);  //Temp Storage of registers
    WriteRead(0x28,BM92A_ADDRESS,5,readBack);//PDO register
    PDO = four_byteOrg(readBack);
    WriteRead(0x03,BM92A_ADDRESS,2,readBack); //status 1 register
    status1 = two_byteOrg(readBack);
    status1_Debug(status1);

    WriteRead(0x04,BM92A_ADDRESS,2,readBack); //status 2 register
    status2 = two_byteOrg(readBack);
    status2_Debug(status2);

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



