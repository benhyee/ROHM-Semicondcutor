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
#include "delay.h"
#include "globals.h"
#include "BD99954_Funcs.h"
#define BM92A_ADDRESS 0x18
#define BD99954_ADDRESS 0x09

int i;
unsigned short readalertStatus = 0, readstatus1 = 0;
unsigned int currentNgtPDO = 0;


void BM92Asrc_regSet(){
    reverseEnable(1);
    write_word(0x1A,BM92A_ADDRESS,readTwoByte(0x1A,BM92A_ADDRESS)|0x0001);  //Vendor Configuration
    write_word(0x17,BM92A_ADDRESS,0x0080); //controller Config 2
    write_word(0x26,BM92A_ADDRESS,0x9109); //system Config 1
    write_word(0x27,BM92A_ADDRESS,0x0A00); //system Config 2
    write_word(0x2F,BM92A_ADDRESS,0x0001); //system Config 3
}
void BM92Asrc_commandSet(){
    write_word(0x05,BM92A_ADDRESS,0x0909);  // Set Command
    readalertStatus = readTwoByte(0x02,BM92A_ADDRESS); //Read Alert
    readstatus1 = readTwoByte(0x03,BM92A_ADDRESS); //Read Status1
    write_word(0x06,BM92A_ADDRESS,0x0000);  //Controller Config 1
}
void BM92Asnk_regSet(){
    batt_chg = 0;
    write_word(0x1A,BM92A_ADDRESS,readTwoByte(0x1A,BM92A_ADDRESS)&0xFFFE);
    write_word(0x17,BM92A_ADDRESS,0x0000);  //Config 2
    write_word(0x27,BM92A_ADDRESS,0x0046);  // SysConfig2
    write_word(0x2F,BM92A_ADDRESS,0xA400);  // SysConfig3
    write_word(0x06,BM92A_ADDRESS,0xCCE0);  //Controller Config 1
}
void BM92Asnk_commandSet() {
    readTwoByte(0x02,BM92A_ADDRESS);//Read Alert resets the Alert Pin
    write_word(0x05,BM92A_ADDRESS,0x0909);  // Set Command
    readalertStatus = readTwoByte(0x02,BM92A_ADDRESS); //Read Alert
    readstatus1 = readTwoByte(0x03,BM92A_ADDRESS); //Read Status 1
}
void currentPDO() {
    currentNgtPDO = readFourByte(0x28,BM92A_ADDRESS);   //Current PDO
    unsigned short PDOcurrent = currentNgtPDO & 0x000003FF;
    PDOcurrent = PDOcurrent *10;
    if(PDOcurrent == 0)
    {
        write_word(0x07,BD99954_ADDRESS,1500);//Device Capability IBUS_LIM_SET PDO current default
    }
    else{
        write_word(0x07,BD99954_ADDRESS,PDOcurrent);//Device Capability IBUS_LIM_SET PDO current
    }
    unsigned short PDOvoltage = (currentNgtPDO & 0x000FFC00)>>10;
    PDOvoltage = PDOvoltage/20;
    LCD_PDO(PDOvoltage,PDOcurrent);
    terminal_transmitWord("Actual Input Current Limit(0x05): ");
    PDOcurrent = readTwoByte(0x05,BD99954_ADDRESS)&0xCFFF;  //BD99954 Input Current Limit
    terminal_transmitShortByte(PDOcurrent);
    terminal_transmitWord("Selected Input Current Limit(0x06): ");
    PDOcurrent = readTwoByte(0x06,BD99954_ADDRESS)&0xCFFF;  //BD99954 Selected Input Current Limit
    terminal_transmitShortByte(PDOcurrent);


}
void BM92A_source_PDO() {
    unsigned char *PDO = malloc(sizeof(char)*21);
    PDO[0] = 0x00; PDO[1] = 0x00; PDO[2] = 0x00;  PDO[3] = 0x00;    //Remove all the PDOs first
    PDO[4] = 0x00; PDO[5] = 0x00; PDO[6] = 0x00;  PDO[7] = 0x00;
    PDO[8] = 0x00; PDO[9] = 0x00; PDO[10] = 0x00;  PDO[11] = 0x00;
    PDO[12] = 0x00; PDO[13] = 0x00; PDO[14] = 0x00;  PDO[15] = 0x00;
    PDO[16] = 0x00; PDO[17] = 0x00; PDO[18] = 0x00;  PDO[19] = 0x00;
    write_block(0x3C,BM92A_ADDRESS,20,PDO); //PDO Src Prov
    switch(source_set){
        case 1:
            PDO[0] = 0x2C; PDO[1] = 0x91; PDO[2] = 0x01;  PDO[3] = 0x14;    //5V @ 3A
            write_block(0x3C,BM92A_ADDRESS,4,PDO); //PDO Src Prov
            break;
        case 2:
            PDO[0] = 0x2C; PDO[1] = 0x91; PDO[2] = 0x01;  PDO[3] = 0x14;
            PDO[4] = 0x2C; PDO[5] = 0xd1; PDO[6] = 0x02;  PDO[7] = 0x14;    //9V @ 3A
            write_block(0x3C,BM92A_ADDRESS,8,PDO); //PDO Src Prov

            break;
        case 3:
            PDO[0] = 0x2C; PDO[1] = 0x91; PDO[2] = 0x01;  PDO[3] = 0x14;
            PDO[4] = 0x2C; PDO[5] = 0xc1; PDO[6] = 0x03;  PDO[7] = 0x14;  //12V @ 3A
            write_block(0x3C,BM92A_ADDRESS,8,PDO); //PDO Src Prov
            break;
        case 4:
            PDO[0] = 0x2C; PDO[1] = 0x91; PDO[2] = 0x01;  PDO[3] = 0x14;
            PDO[4] = 0x2C; PDO[5] = 0xb1; PDO[6] = 0x04;  PDO[7] = 0x14;  //15V @ 3A
            write_block(0x3C,BM92A_ADDRESS,8,PDO); //PDO Src Prov
            break;
        case 5:
            PDO[0] = 0x2C; PDO[1] = 0x91; PDO[2] = 0x01;  PDO[3] = 0x14;
            PDO[4] = 0x2C; PDO[5] = 0x41; PDO[6] = 0x06;  PDO[7] = 0x14;  //20V @ 3A
            write_block(0x3C,BM92A_ADDRESS,8,PDO); //PDO Src Prov
            break;
        case 6:
            PDO[0] = 0x2C; PDO[1] = 0x91; PDO[2] = 0x01;  PDO[3] = 0x14;    //5V @ 3A
            PDO[4] = 0x2C; PDO[5] = 0xd1; PDO[6] = 0x02;  PDO[7] = 0x14;    //9V @ 3A
            PDO[8] = 0x2C; PDO[9] = 0xc1; PDO[10] = 0x03;  PDO[11] = 0x14;  //12V @ 3A
            PDO[12] = 0x2C; PDO[13] = 0xb1; PDO[14] = 0x04;  PDO[15] = 0x14;//15V @ 3A
            PDO[16] = 0x2C; PDO[17] = 0x41; PDO[18] = 0x06;  PDO[19] = 0x14;//20V @ 3A
            write_block(0x3C,BM92A_ADDRESS,20,PDO); //PDO Src Prov
            break;
        default:
            break;
    }

    free(PDO);
}


void BM92A_sink_PDO() {
    unsigned char *readBack = malloc(sizeof(char)*17);
    unsigned char *PDO = malloc(sizeof(char)*6);
    unsigned short controllerConfig = 0;
    controllerConfig = readTwoByte(0x06,BM92A_ADDRESS); //Controller Config 1
    write_word(0x26,BM92A_ADDRESS,0x814A);  //System Configuration
    switch(sink_set){
        case 1:
            controllerConfig &= 0xFFDF;
            write_word(0x06,BM92A_ADDRESS,controllerConfig); //Controller Config 1
            PDO[0] = 0x00; PDO[1] = 0x90;   //batteryNgt 5V max 5V min @ 3A
            PDO[2] = 0x41; PDO[3] = 0x06;
            break;
        case 2:
            controllerConfig &= 0xFFDF;
            write_word(0x06,BM92A_ADDRESS,controllerConfig); //Controller Config 1
            PDO[0] = 0x00; PDO[1] = 0x90;   //batteryNgt 9V max 5V min @ 3A
            PDO[2] = 0x41; PDO[3] = 0x0b;
            break;

        case 3:
            controllerConfig &= 0xFFDF;
            write_word(0x06,BM92A_ADDRESS,controllerConfig); //Controller Config 1
            PDO[0] = 0x00; PDO[1] = 0x90;   //batteryNgt 15V max 5V min @ 3A
            PDO[2] = 0xc1; PDO[3] = 0x12;
            break;
        case 4:
            controllerConfig &= 0xFFDF;
            write_word(0x06,BM92A_ADDRESS,controllerConfig);
            PDO[0] = 0x00; PDO[1] = 0x90;   //batteryNgt 20V max 5V min @ 3A
            PDO[2] = 0x01; PDO[3] = 0x19;
            break;
        default:
            break;
    }
    write_block(0x20,BM92A_ADDRESS,4,PDO);
    free(PDO); free(readBack);
}
void pdo100WMode(){ //For 100 Watt negotiation but not included in the final EVK

    unsigned char *readBack = malloc(sizeof(char)*17);
    unsigned char *PDO = malloc(sizeof(char)*6);
    unsigned short controllerConfig = 0;
    controllerConfig = readTwoByte(0x06,BM92A_ADDRESS);
    write_word(0x26,BM92A_ADDRESS,0x814A);
    controllerConfig |= 0x0020;
    write_word(0x06,BM92A_ADDRESS,controllerConfig);
    PDO[0] = 0xf4; PDO[1] = 0x91; PDO[2] = 0x01; PDO[3] = 0x19;
    write_block(0x20,BM92A_ADDRESS,4,PDO);
    write_word(0x05,BM92A_ADDRESS,0x0909);
    readTwoByte(0x02,BM92A_ADDRESS);
    WriteRead(0x33,BM92A_ADDRESS,13,readBack);
    printPDO(readBack);
    free(readBack);free(PDO);

}
void sinkAllPDOMode() {
    unsigned char *readBack = malloc(sizeof(char)*17);  //For 5-20V sink mode configuration
    unsigned char *PDO = malloc(sizeof(char)*6);
    unsigned short controllerConfig = 0;
    controllerConfig = readTwoByte(0x06,BM92A_ADDRESS);
    write_word(0x26,BM92A_ADDRESS,0x814A);
    controllerConfig |= 0x0020;
    write_word(0x06,BM92A_ADDRESS,controllerConfig);
    delay_ms(1,CURRENT_FREQ);
    PDO[0] = 0x00; PDO[1] = 0x90; PDO[2] = 0x01;PDO[3] = 0x19;
    write_block(0x20,BM92A_ADDRESS,4,PDO);
    delay_ms(1,CURRENT_FREQ);
    printPDO(readBack);
    free(readBack); free(PDO);
}
void srcAllPDOMode() {
    unsigned char *PDO = malloc(sizeof(char)*21);
    PDO[0] = 0x2C; PDO[1] = 0x91; PDO[2] = 0x01;  PDO[3] = 0x14;    //5V @ 3A
                PDO[4] = 0x2C; PDO[5] = 0xd1; PDO[6] = 0x02;  PDO[7] = 0x14;    //9V @ 3A
                PDO[8] = 0x2C; PDO[9] = 0xc1; PDO[10] = 0x03;  PDO[11] = 0x14;  //12V @ 3A
                PDO[12] = 0x2C; PDO[13] = 0xb1; PDO[14] = 0x04;  PDO[15] = 0x14;//15V @ 3A
                PDO[16] = 0x2C; PDO[17] = 0x41; PDO[18] = 0x06;  PDO[19] = 0x14;//20V @ 3A
    write_block(0x3C,BM92A_ADDRESS,20,PDO); //PDO Src Prov
    free(PDO);
}

