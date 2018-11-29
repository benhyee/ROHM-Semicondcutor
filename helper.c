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

void sourceNegotiate();

unsigned short two_byteOrg(unsigned char* dataArray)
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
unsigned int four_byteOrg(unsigned char* dataArray)
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
void printPDO(unsigned char* dataArray)
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
int ngtVoltage(unsigned int currentPDOreg)
{
    return ((currentPDOreg & 0x000FFC00)>>10)*50;

}
int ngtMaxCurrent(unsigned int currentRDO)
{
    return ((currentRDO & 0x000003FF)>>10)*10;

}
int ngtOperatingCurrent(unsigned int currentRDO)
{
    return ((currentRDO & 0x000FFC00)>>10)*10;

}
void monitorSnkVoltage(){
    terminal_transmitWord("Sink Negotiation \n\r");
    LCD_clearLine();  LCD_command(0x01); // clear screen, move cursor home
    LCD_word("Sink"); LCD_enter();
    delay_ms(400,CURRENT_FREQ);
    currentPDO(); delay_ms(2000,CURRENT_FREQ);
    write_word(0x71,BD99954_ADDRESS,0x000F);
    LCD_clearLine(); LCD_command(0x01);
    LCD_word("Sinking (USB-C)");

    readTwoByte(0x02,BM92A_ADDRESS);
    write_word(0x71,BD99954_ADDRESS,0x000F);

    while(((readTwoByte(0x03,BM92A_ADDRESS)&0x0300)>>8)!=0)
    {
        if(pushFlag == 1)
        {
            LCD_toggle();
            pushFlag = 0;
        }
        write_word(0x72,BD99954_ADDRESS,0x000F);
        acpCurrent = readTwoByte(0x59,BD99954_ADDRESS) & 0x7FFF;
        acpVoltage = readTwoByte(0x5B,BD99954_ADDRESS) & 0x7FFF;

        LCD_Monitor(acpVoltage,acpCurrent);
        delay_ms(200,CURRENT_FREQ);
    }
    write_word(0x71,BD99954_ADDRESS,0x000F);
}
void monitorVCCSnkVoltage(){
    LCD_clearLine();  LCD_command(0x01); // clear screen, move cursor home
    write_word(0x72,BD99954_ADDRESS,0x000F);
    LCD_word("Sink from VCC");
    delay_ms(2000,CURRENT_FREQ);
    LCD_clearLine(); LCD_command(0x01);
    LCD_word("Sinking (DC)");
//    LCD_enter();
//    LCD_word("BAT:");
    while((readTwoByte(0x72,BD99954_ADDRESS)&0x0001)!=1)
    {
        if(pushFlag == 1)
        {
            LCD_toggle();
            pushFlag = 0;
        }
        acpCurrent = readTwoByte(0x59,BD99954_ADDRESS) & 0x7FFF;
        acpVoltage = readTwoByte(0x5B,BD99954_ADDRESS) & 0x7FFF;

        LCD_Monitor(acpVoltage,acpCurrent);
        delay_ms(200,CURRENT_FREQ);
    }
    write_word(0x72,BD99954_ADDRESS,0x000F);

}
void monitorSrcVoltage()
{
    LCD_clearLine(); LCD_command(0x01);
    LCD_word("Sourcing (USB-C)");
    while(((readTwoByte(0x03,BM92A_ADDRESS)&0x0300)>>8)!=0)
    {
        if(AlertFlag){
            sourceNegotiate();
            LCD_clearLine(); LCD_command(0x01);
            LCD_word("Sourcing (USB-C)");
            AlertFlag = FALSE;
        }
        if(pushFlag == 1)
        {
            LCD_toggle();
            pushFlag = 0;
        }
        acpCurrent = readTwoByte(0x59,BD99954_ADDRESS) & 0x7FFF;
        acpVoltage = readTwoByte(0x5B,BD99954_ADDRESS) & 0x7FFF;
        LCD_Monitor(acpVoltage,acpCurrent);
        delay_ms(200,CURRENT_FREQ);
    }
    write_word(0x71,BD99954_ADDRESS,0x000F);

}

void sourceNegotiate(){
    readTwoByte(0x02,BM92A_ADDRESS);
    signed int deltaVolt;
    unsigned char *readBack = malloc(sizeof(char)*21);
    unsigned int PDOvoltage,currentPDOreg;
    currentPDOreg = readFourByte(0x28,BM92A_ADDRESS);   //Ngt PDO
    PDOvoltage = ngtVoltage(currentPDOreg);
    if(PDOvoltage > 5000) {
        reverseVoltage(PDOvoltage); //If PDO voltage is not 5V
        delay_ms(200,CURRENT_FREQ);
        deltaVolt = readTwoByte(0x5D,BD99954_ADDRESS)& 0x7FFF;
        deltaVolt = (PDOvoltage - deltaVolt+64)/2;
        reverseVoltage(PDOvoltage+deltaVolt);
    }
    else {
        reverseVoltage(5024);
    }
    terminal_transmitWord("Source Negotiation \n\r");
    WriteRead(0x08,BM92A_ADDRESS,12,readBack);  //Display Sink Cap
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
