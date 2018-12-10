/*
 * helper.c
 *
 *  Created on: Apr 25, 2018
 *      Author: User
 */


#include "msp.h"
#include "UART.h"
#include "BM92A_Funcs.h"
#include "BD99954_Funcs.h"
#include "lcd.h"
#include "I2C_Helper.h"
#include "delay.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "GPIO.h"

#include "globals.h"
#define BD99954_ADDRESS 0x09
#define BM92A_ADDRESS 0x18
unsigned char char_Hold;
unsigned short shortData;
unsigned int intData;
unsigned int intArray[];
int i;
int  vccVoltage, acpVoltage, acpCurrent;
char sleepWake = 1;
void sourceNegotiate();

unsigned short two_byteOrg(unsigned char* dataArray)//Rearranges the 4 groups of Bytes so that MSB on the left
{
    i = 0;
    shortData = 0;
    for(i = 0; i < 2; i++)
    {
        char_Hold = *dataArray&0xFF;
        shortData = (char_Hold << (8*i)) | shortData;
        *dataArray++;
    }
    return shortData;
}
unsigned int four_byteOrg(unsigned char* dataArray)//Rearranges the 4 groups of Bytes so that MSB on the left
{
    i = 0;
    intData = 0;
    *dataArray++;
    for(i = 0; i < 4; i++)
    {
        char_Hold = *(dataArray)&0xFF;
        intData = (char_Hold << (8*i)) | intData;
        *dataArray++;

    }
    return intData;
}
void printPDO(unsigned char* dataArray) //Prints out the PDO for UART purposes
{
    terminal_transmitWord("Sink PDOs ");
    i = 0;
    intData = 0;
    unsigned char length = *(dataArray);
    for(intData =0; intData < 1000; intData++);
    terminal_transmitWord("\r0x");
    terminal_transmitByte((length&0xF0)>>4);
    terminal_transmitByte(length & 0x0F);
    *dataArray++;

    for(i = 0; i < length; i++)
    {
        if(i%4 ==0) terminal_transmitWord(", 0x");
        terminal_transmitByte((*(dataArray)&0xF0)>>4);
        terminal_transmitByte(*(dataArray)&0x0F);
        *dataArray++;

    }
    terminal_transmitWord("\r\n");
}
int ngtVoltage(unsigned int currentPDOreg)  //Negotiated Voltage of PDO
{
    return ((currentPDOreg & 0x000FFC00)>>10)*50;   //50mV steps

}
int ngtMaxCurrent(unsigned int currentRDO)  //Negotiated Max Current of PDO
{
    return ((currentRDO & 0x000003FF)>>10)*10; //10 mA steps

}
int ngtOperatingCurrent(unsigned int currentRDO)    //Negotiated Operating Current of PDO
{
    return ((currentRDO & 0x000FFC00)>>10)*10;  //10 mA steps

}
void monitorSnkVoltage(){
    chargeState();  clear_BD_int(); //Check the DIP Switch for CHG Enable and clear BD interrupt
    terminal_transmitWord("Sink Negotiation \n\r");
    LCD_clearLine();  LCD_command(0x01); // clear screen, move cursor home
    LCD_word("Sink from USB-C"); LCD_enter();
    delay_ms(400,CURRENT_FREQ);
    currentPDO(); delay_ms(2000,CURRENT_FREQ);  //View Current PDO and set BD for Sinking
    clear_BD_int(); //Clear BD interrupt
    LCD_clearLine(); LCD_command(0x01);
    LCD_word("Sinking (USB-C)");

    readTwoByte(0x02,BM92A_ADDRESS);
    while(((readTwoByte(0x03,BM92A_ADDRESS)&0x0300)>>8)!=0)
    {
        if(sleepWake == 1)  //any joystick action can wake up from LCD sleep
        {
            LCD_wake();
            sleepWake = 0;
            LCD_word("Sinking (USB-C)");
            LCD_Monitor(acpVoltage,acpCurrent); //Formats the LCD for displaying of voltage and current

        }
        else if(pushFlag == 1)  //Push button has different Flag set
        {
            LCD_toggle();
            pushFlag = 0;
            LCD_word("Sinking (USB-C)");
            LCD_Monitor(acpVoltage,acpCurrent); //Formats the LCD for displaying of voltage and current

        }
        //Monitor the ACP node which is a input current sense resistor
        acpCurrent = readTwoByte(0x59,BD99954_ADDRESS) & 0x7FFF;    //ACP current readback
        acpVoltage = readTwoByte(0x5B,BD99954_ADDRESS) & 0x7FFF;    //ACP Voltage readback

        LCD_Monitor(acpVoltage,acpCurrent); //Formats the LCD for displaying of voltage and current
        chargingStatus();   //Display LEDs to charging state of the battery
        delay_ms(150,CURRENT_FREQ);
    }
    P2 -> OUT &= ~0x0F; //Clear the LEDs of their state
    clear_BD_int(); // clear BD interrupt flags
}
void monitorVCCSnkVoltage(){
    chargeState();
    write_word(0x08,BD99954_ADDRESS,3008);    //ICC_LIM_SET to enable proper VCC sinking
    terminal_transmitWord("VCC Delivery \n\r");
    LCD_clearLine();  LCD_command(0x01); // clear screen, move cursor home
    LCD_word("Sink from VCC");LCD_enter();
    delay_ms(2000,CURRENT_FREQ);
    LCD_clearLine(); LCD_command(0x01);
    LCD_word("Sinking (DC)");
    acpCurrent = 100;   //initial state of the ACP current so that it doesn't exit while loop immediately.
    while((readTwoByte(0x72,BD99954_ADDRESS)&0x0001)!=1 &&
            readTwoByte(0x5F,BD99954_ADDRESS)>1500) //While voltage is greater than 1500 or while
    {                                               // no interrupt is triggered by BD99954
        if(sleepWake == 1)  //any joystick action can wake up from LCD sleep
        {
            LCD_wake();
            sleepWake = 0;
            LCD_word("Sinking (USB-C)");
            LCD_Monitor(acpVoltage,acpCurrent); //Formats the LCD for displaying of voltage and current

        }
        else if(pushFlag == 1)  //Push button has different Flag set
        {
            LCD_toggle();
            pushFlag = 0;
            LCD_word("Sinking (USB-C)");
            LCD_Monitor(acpVoltage,acpCurrent); //Formats the LCD for displaying of voltage and current

        }
        acpCurrent = readTwoByte(0x59,BD99954_ADDRESS) & 0x7FFF;
        acpVoltage = readTwoByte(0x5B,BD99954_ADDRESS) & 0x7FFF;
        LCD_Monitor(acpVoltage,acpCurrent);
        chargingStatus();
        delay_ms(150,CURRENT_FREQ);
    }
    P2 -> OUT &= ~0x0F;
    clear_BD_int();

}
void monitorSrcVoltage()
{
    LCD_clearLine(); LCD_command(0x01);
    LCD_word("Sourcing (USB-C)");
    pushFlag = 0;
    write_word(0x09,BD99954_ADDRESS,3008);    //IOTG_LIM_SET
    write_word(0x07,BD99954_ADDRESS,3008);    //IBUS_LIM_SET

    while(((readTwoByte(0x03,BM92A_ADDRESS)&0x0300)>>8)!=0)
    {
        if(AlertFlag){  //Check for renegotiation. Mostly for the PD load tester as it defaults 5V first
            sourceNegotiate();
            LCD_clearLine(); LCD_command(0x01);
            LCD_word("Sourcing (USB-C)");
            AlertFlag = FALSE;
        }
        if(sleepWake == 1)  //any joystick action can wake up from LCD sleep
        {
            LCD_wake();
            sleepWake = 0;
            LCD_word("Sinking (USB-C)");
            LCD_Monitor(acpVoltage,acpCurrent); //Formats the LCD for displaying of voltage and current

        }
        else if(pushFlag == 1)  //Push button has different Flag set
        {
            LCD_toggle();
            pushFlag = 0;
            LCD_word("Sinking (USB-C)");
            LCD_Monitor(acpVoltage,acpCurrent); //Formats the LCD for displaying of voltage and current

        }
        acpCurrent = readTwoByte(0x59,BD99954_ADDRESS) & 0x7FFF;
        acpVoltage = readTwoByte(0x5B,BD99954_ADDRESS) & 0x7FFF;
        LCD_Monitor(acpVoltage,acpCurrent);
        acpVoltage = readTwoByte(0x5D,BD99954_ADDRESS) & 0x7FFF;

        delay_ms(150,CURRENT_FREQ);
    }
    write_word(0x71,BD99954_ADDRESS,0x000F);

}

void sourceNegotiate(){ //sets the BD99954 to reverse buck boost as fast as possible for timing requirements
    readTwoByte(0x02,BM92A_ADDRESS);
    signed int deltaVolt;
    unsigned char *readBack = malloc(sizeof(char)*21);
    unsigned int PDOvoltage,currentPDOreg;
    currentPDOreg = readFourByte(0x28,BM92A_ADDRESS);   //Negotiated CUrrent PDO BM92A
    PDOvoltage = ngtVoltage(currentPDOreg); //Figures out what the negotiated voltage is
    write_word(0x07,BD99954_ADDRESS,currentPDOreg*10);    //IBUS_LIM_SET

    if(PDOvoltage > 5000) { //if the voltage is greater than 5V
        reverseVoltage(PDOvoltage); //write the voltage from the PDO into the BD99954
        delay_ms(200,CURRENT_FREQ); //Control loop to figure out the delta between actual BD99954 voltage
        deltaVolt = readTwoByte(0x5D,BD99954_ADDRESS)& 0x7FFF;
        deltaVolt = (PDOvoltage - deltaVolt+64)/2;
        reverseVoltage(PDOvoltage+deltaVolt);   //Write new voltage that will take away that offset
    }
    else {
        reverseVoltage(5024);
    }
    terminal_transmitWord("Source Negotiation \n\r");
    WriteRead(0x08,BM92A_ADDRESS,12,readBack);  //Display Sink Capabilites
    printPDO(readBack);
    LCD_clearLine();  LCD_command(0x01); // clear screen, move cursor home
    LCD_word("Source"); LCD_enter();
    currentPDO();
    delay_ms(2000,CURRENT_FREQ);
    readTwoByte(0x02,BM92A_ADDRESS);
    free(readBack);
}
void blinkLED()
{
    P2->OUT |= 1;
    for(i = 0; i< 50000; i++);
    P2->OUT &= ~1;
    for(i = 0; i< 50000; i++);

}
