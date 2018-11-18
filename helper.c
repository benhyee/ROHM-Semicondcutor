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
#define BD99954_ADDRESS 0x09
#define BM92A_ADDRESS 0x18
unsigned char char_Hold;
unsigned short shortData;
unsigned int intData;
unsigned int intArray[];
int i;

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
//int transferIntArray( unsigned char* tempArray, unsigned int* PDO_array)
//{
//    unsigned int temp;
//    for(i = 0; i < 3; i++)
//    {
//        temp = four_byteOrg(&tempArray);
//        *tempArray++;
//        *PDO_array = temp;
//        *PDO_array++;
//
//    }
//    return 0;
//}
void monitorVoltage()
{
    LCD_clearLine(); LCD_command(0x01);
    LCD_word("VBUS:");
    LCD_enter();
    LCD_word("VBAT:");
    int busVoltage; int batteryVolt;
    while(((readTwoByte(0x03,BM92A_ADDRESS)&0x0300)>>8)!=0)
    {
        busVoltage = readTwoByte(0x5D,BD99954_ADDRESS)& 0x7FFF;
        batteryVolt = readTwoByte(0x55,BD99954_ADDRESS) & 0x7FFF;
        LCD_Monitor(busVoltage,batteryVolt);
        delay_ms(100,CURRENT_FREQ);
    }
}

void blinkLED()
{

    P2->OUT |= 1;
    for(i = 0; i< 50000; i++);
    P2->OUT &= ~1;
    for(i = 0; i< 50000; i++);

}
