/*
 * BD99954_Funcs.c
 *
 *  Created on: Sep 30, 2018
 *      Author: User
 */
/*
 * BD99954 Functions for register reading and writing
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
#include "GPIO.h"
#include "delay.h"
#include "menu.h"

#define BD99954_ADDRESS 0x09
#define CURRENT_FREQ FREQ_3_MHZ
#define LED P2

//Clears implicitly declared warnings
void default_BDSettings();
void chgDisable();
void reverseVoltage(int voltage);

//BD99954 Local Variables
int i;

void BD_INT_INIT(){ //Initializes Interrupt Flags for the BD99954
    short int_set;
    int_set = readTwoByte(0x68,BD99954_ADDRESS);    //Readback for bit masking
    int_set |= 0x0004;
    write_word(0x68,BD99954_ADDRESS,int_set);

    int_set = readTwoByte(0x6A,BD99954_ADDRESS);    //Readback for bit masking
    int_set |= 0x0003;
    write_word(0x6A,BD99954_ADDRESS,int_set);

    clear_BD_int();
}
void clear_BD_int(){        //Clears the Interrupt flags
    write_word(0x70,BD99954_ADDRESS,0x00FF);    //Int0
    write_word(0x71,BD99954_ADDRESS,0x00FF);    //Int1 VBUS
    write_word(0x72,BD99954_ADDRESS,0x00FF);    //Int2 VCC
}
short VCCInterruptFlag(){   //Returns the contents of Interrupt register VCC
    return readTwoByte(0x72,BD99954_ADDRESS);
}
short VBUSInterruptFlag(){   //Returns the contents of Interrupt register VCC
    return readTwoByte(0x71,BD99954_ADDRESS);
}
void reverseVoltage(int voltage)
{
    voltage = voltage +32 -(voltage % 32);  //Forces integer into multiple of 32
    write_word(0x19,BD99954_ADDRESS,voltage);
}
void chgEnable()    //Battery Charging Function
{
    unsigned short tempCHG = readTwoByte(0x0C, BD99954_ADDRESS);    //Bitmasking
    tempCHG |= 0x0080;
    write_word(0x0C,BD99954_ADDRESS,tempCHG);

}
void chgDisable()   //Turns of Charging of Battery
{
    unsigned short tempCHG = readTwoByte(0x0C, BD99954_ADDRESS);
    tempCHG &= 0xFF7F;
    write_word(0x0C,BD99954_ADDRESS,tempCHG);
}
void reverseBuckBoost() //Initializes reverse Buck Boost
{
    reverseVoltage(5056);
    write_word(0x09,BD99954_ADDRESS,1120);//IOTG_Lim_set
}

void reverseEnable(char channel)    //Enables Reverse Buck Boost and applies voltage
{
    unsigned short tempCHG = readTwoByte(0x0A, BD99954_ADDRESS);
    if(channel == 1){ //Selects VBus
        tempCHG |= 0x5000;
        write_word(0x0A,BD99954_ADDRESS,tempCHG);
    }
    else{  // Selects VCC
        tempCHG |= 0x6000;
        write_word(0x0A,BD99954_ADDRESS,tempCHG);
    }
}
void reverseDisable()
{
    unsigned short tempCHG = readTwoByte(0x0A, BD99954_ADDRESS);
    tempCHG &= 0x8FFF;
    write_word(0x0A,BD99954_ADDRESS,tempCHG);
    reverseVoltage(5024);
}
void BD99954_Startup_Routine()
{

    write_word(0x3E,BD99954_ADDRESS,0x0000);    //Protect set
    write_word(0x3F,BD99954_ADDRESS,0x0001);
    //Map Set (VERY IMPORTANT IF YOU WANT ACCESS TO EXTENDED COMMANDS)

    chargeState();
    BD_INT_INIT();
    for(i=0;i<200;i++);
    default_BDSettings();

    if(readTwoByte(0x5F,BD99954_ADDRESS)>1000)  //When no Battery is attached to the terminal
    {
        clear_BD_int();
        displayMode();
        readTwoByte(0x02,0x18);
        readTwoByte(0x70,BD99954_ADDRESS);
        write_word(0x70,BD99954_ADDRESS,0x00FF);
        monitorVCCSnkVoltage();
        write_word(0x72,BD99954_ADDRESS,0x000F);
        displayMode();
    }

}

void default_BDSettings()
{
    write_word(0x07,BD99954_ADDRESS,1504);    //IBUS_LIM_SET
    write_word(0x08,BD99954_ADDRESS,3008);    //ICC_LIM_SET
    write_word(0x09,BD99954_ADDRESS,3008);    //IOTG_LIM_SET

    write_word(0x1D,BD99954_ADDRESS,7520);    //VRECHG_SET
    write_word(0x1E,BD99954_ADDRESS,9024);    //VBATOVP_SET
    write_word(0x11,BD99954_ADDRESS,6976);    //VSYSREG_SET
    write_word(0x18,BD99954_ADDRESS,6080);    //VPRECHG_TH_SET
    write_word(0x1A,BD99954_ADDRESS,8000);    //VFASTCHG_REG_SET
    write_word(0x14,BD99954_ADDRESS,256);    //ITRICH_SET
    write_word(0x15,BD99954_ADDRESS,512);    //IPRECH_SET
    write_word(0x16,BD99954_ADDRESS,768);    //ICHG_SET
    write_word(0x17,BD99954_ADDRESS,128);    //ITERM_SET
}
void chargingStatus(){
    short chargerState = readTwoByte(0x00,BD99954_ADDRESS) & 0x007F;
    LED -> OUT &= ~0x0F;
    switch(chargerState){
        case 0:
            LED -> OUT &= ~0x0F;//Suspend
            break;
        case 1:
            LED -> OUT |= 0x01;//Trickle-Charging
            break;
        case 2:
            LED -> OUT |= 0x03;//Pre-Charging
            break;
        case 3:
            LED -> OUT |= 0x07;//Fast-Charging
            break;
        case 4:
            LED -> OUT |= 0x0F;//Termination-Charging
            break;
        default:
            break;
    }

}
