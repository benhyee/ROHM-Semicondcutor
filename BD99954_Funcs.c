/*
 * BD99954_Funcs.c
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


#define BD99954_ADDRESS 0x09

int i;
//BD99954 Local Variables
unsigned short IBUS_LIM_SET , ICC_LIM_SET , VRECHG_SET , VBATOVP_SET ;
unsigned short VSYSREG_SET , VPRECHG_TH_SET , VFASTCHG_REG_SET1;
unsigned short ITRICH_SET, IPRECH_SET , ICHG_SET, ITERM_SET, BD99id, BD99rev;
unsigned short VBUS_Average,VSYS_Average, VBAT_Average,otpStatus;

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
