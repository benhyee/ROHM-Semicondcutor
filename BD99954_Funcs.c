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

void default_BDSettings();

#define BD99954_ADDRESS 0x09
int i;
//BD99954 Local Variables


// Debug Functions


// Reading Registers (DEBUG)


void BD99954_Startup_Routine()
{
    unsigned char *readBack = malloc(sizeof(char)*30);  //Temp Storage of registers
    write_word(0x3E,BD99954_ADDRESS,0x0000);    //Protect set
    write_word(0x3F,BD99954_ADDRESS,0x0001);    //Map Set (VERY IMPORTANT IF YOU WANT ACCESS TO EXTENDED COMMANDS)
    write_word(0x3D,BD99954_ADDRESS,0x0001);    //All reset
    write_word(0x3D,BD99954_ADDRESS,0x0000);    //Release all reset



    for(i=0;i<200;i++);
    default_BDSettings();



}
void chgEnable()
{
    write_word(0x0C,BD99954_ADDRESS,0x4080);    //ICHG_SET

}
void chgDisable()
{
    write_word(0x0C,BD99954_ADDRESS,0x4000);
}
void reverseBuckBoost()
{
    write_word(0x19,BD99954_ADDRESS,5056);
    write_word(0x09,BD99954_ADDRESS,1472);

}
void reverseEnable(char channel)
{
    if(channel == 1){ //Select VBus
        write_word(0x0A,BD99954_ADDRESS,0x5060);

    }
    else{  // Select VCC
        write_word(0x0A,BD99954_ADDRESS,0x6060);
    }
}
void reverseDisable()
{
    write_word(0x0A,BD99954_ADDRESS,0x0060);
}
void default_BDSettings()
{
    write_word(0x07,BD99954_ADDRESS,1472);    //IBUS_LIM_SET
    write_word(0x08,BD99954_ADDRESS,1472);    //ICC_LIM_SET
    write_word(0x1D,BD99954_ADDRESS,8112);    //VRECHG_SET
    write_word(0x1E,BD99954_ADDRESS,8912);    //VBATOVP_SET
    write_word(0x11,BD99954_ADDRESS,6976);    //VSYSREG_SET
    write_word(0x18,BD99954_ADDRESS,6080);    //VPRECHG_TH_SET
    write_word(0x1A,BD99954_ADDRESS,8000);    //VFASTCHG_REG_SET
    write_word(0x14,BD99954_ADDRESS,256);    //ITRICH_SET
    write_word(0x15,BD99954_ADDRESS,512);    //IPRECH_SET
    write_word(0x16,BD99954_ADDRESS,704);    //ICHG_SET
    write_word(0x17,BD99954_ADDRESS,0);    //ITERM_SET
}
