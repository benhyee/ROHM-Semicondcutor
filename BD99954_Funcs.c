/*
 * BD99954_Funcs.c
 *
 *  Created on: Sep 30, 2018
 *      Author: User
 */
/*
 * BD99954 Functions
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
#include <string.h>


#define BD99954_ADDRESS 0x09

int i;
//BD99954 Local Variables
unsigned short IBUS_LIM_SET , ICC_LIM_SET , VRECHG_SET , VBATOVP_SET ;
unsigned short VSYSREG_SET , VPRECHG_TH_SET , VFASTCHG_REG_SET1;
unsigned short ITRICH_SET, IPRECH_SET , ICHG_SET, ITERM_SET, BD99id, BD99rev;
unsigned short VBUS_Average, VSYS_Average, VBAT_Average, otpStatus;

unsigned int setValue;
unsigned char generalPurpose;


// Debug Functions
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


// Reading Registers (DEBUG)
void BD99954ReadRegister()
{
    IBUS_LIM_SET = 0 , ICC_LIM_SET = 0, VRECHG_SET = 0, VBATOVP_SET = 0;
    VSYSREG_SET = 0, VPRECHG_TH_SET = 0, VFASTCHG_REG_SET1 = 0;
    ITRICH_SET = 0, IPRECH_SET = 0, ICHG_SET = 0, ITERM_SET = 0;
    BD99id = 0, BD99rev = 0;
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
    BD99ID_Debug(BD99id);
    BD99REV_Debug(BD99rev);
    VBAT_AVE_Debug(VBAT_Average);
    VBUS_AVE_Debug(VBUS_Average);
    VSYS_AVE_Debug(VSYS_Average);
    SYS_STATUS_Debug(otpStatus);

  //  LCD_Voltage(VSYS_Average);

    terminal_transmitWord("\r\n");
    terminal_transmitWord("------------------------------\r\n");

    for(i = 0; i < 200; i++);

}

void BD99954_Startup_Routine()
{
    unsigned char *readBack = malloc(sizeof(char)*30);  //Temp Storage of registers
    write_word(0x3E,BD99954_ADDRESS,0x0000);    //Protect set
    write_word(0x3F,BD99954_ADDRESS,0x0001);    //Map Set (VERY IMPORTANT IF YOU WANT ACCESS TO EXTENDED COMMANDS)
    write_word(0x3D,BD99954_ADDRESS,0x0001);    //All reset
    write_word(0x3D,BD99954_ADDRESS,0x0000);    //Release all reset



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
