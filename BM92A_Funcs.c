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
#define BM92A_ADDRESS 0x18
int i;
char mode_set= 0;
unsigned short readalertStatus = 0, readstatus1 = 0;
unsigned int currentNgtPDO = 0;

void BM92Asrc_init() {                 //GPIO2 and GPIO3 set the Src Prov Table
                                  //L L -> (60W)  L H -> (45W) H L -> (27W) H H -> (18W)
    mode_set = 1;   //sets the global for Source;
    write_word(0x1A,BM92A_ADDRESS,readTwoByte(0x1A,BM92A_ADDRESS)|0x0001);
    write_word(0x17,BM92A_ADDRESS,0x0080); //controller Config 2
    write_word(0x26,BM92A_ADDRESS,0x9109); //system Config 1
    write_word(0x27,BM92A_ADDRESS,0x0A00); //system Config 2
    write_word(0x2F,BM92A_ADDRESS,0x0001); //system Config 3
    write_word(0x05,BM92A_ADDRESS,0x0909); //Store System Config
    readalertStatus = readTwoByte(0x02,BM92A_ADDRESS); //Read Alert
    readstatus1 = readTwoByte(0x03,BM92A_ADDRESS); //Read Status
    write_word(0x06,BM92A_ADDRESS,0x0000);  //Controller Config 1
}
void BM92Asnk_init() {                //GPIO2 and GPIO3 set the Src Prov Table
    mode_set = 0;    //Sets the global for Sink;
    write_word(0x1A,BM92A_ADDRESS,readTwoByte(0x1A,BM92A_ADDRESS)&0xFFFE);
    write_word(0x17,BM92A_ADDRESS,0x0000);  //config 2
    write_word(0x26,BM92A_ADDRESS,0x8749); //system Config 1
    write_word(0x27,BM92A_ADDRESS,0x0046);  // SysConfig2
    write_word(0x2F,BM92A_ADDRESS,0xA400);  // SysConfig3
    write_word(0x06,BM92A_ADDRESS,0xCCE0);  //Controller Config 1
    readTwoByte(0x02,BM92A_ADDRESS);
    write_word(0x05,BM92A_ADDRESS,0x0909);  // Set Command
    readalertStatus = readTwoByte(0x02,BM92A_ADDRESS); //Read Alert
    readstatus1 = readTwoByte(0x03,BM92A_ADDRESS); //Read Status

}
void currentPDO() {
    currentNgtPDO = readFourByte(0x28,BM92A_ADDRESS);
    unsigned short PDOcurrent = currentNgtPDO & 0x000003FF;
    PDOcurrent = PDOcurrent *10;
    unsigned short PDOvoltage = (currentNgtPDO & 0x000FFC00)>>10;
    PDOvoltage = PDOvoltage/20;
    LCD_PDO(PDOvoltage,PDOcurrent);
}
void BM92A_source_PDO() {
    mode_set = 1;    //Sets the global for source
    unsigned char *readBack = malloc(sizeof(char)*17);
    unsigned char *PDO = malloc(sizeof(char)*21);
    write_word(0x1A,BM92A_ADDRESS,readTwoByte(0x1A,BM92A_ADDRESS)|0x0001);  //Vendor Config
    write_word(0x17,BM92A_ADDRESS,0x0080); //controller Config 2
    write_word(0x26,BM92A_ADDRESS,0x9109); //system Config 1
    write_word(0x27,BM92A_ADDRESS,0x0A00); //system Config 2
    write_word(0x2F,BM92A_ADDRESS,0x0001); //system Config 3
    switch(source_set){
        case 1:
            PDO[0] = 0x32; PDO[1] = 0x90; PDO[2] = 0x01;  PDO[3] = 0x14;
            write_block(0x3C,BM92A_ADDRESS,4,PDO); //PDO Src Prov
            break;
        case 2:
            PDO[0] = 0x32; PDO[1] = 0xd0; PDO[2] = 0x02;  PDO[3] = 0x14;
            write_block(0x3C,BM92A_ADDRESS,4,PDO); //PDO Src Prov

            break;
        case 3:
            PDO[0] = 0x32; PDO[1] = 0xc0; PDO[2] = 0x03;  PDO[3] = 0x14;
            write_block(0x3C,BM92A_ADDRESS,4,PDO); //PDO Src Prov
            break;
        case 4:
            PDO[0] = 0x32; PDO[1] = 0xb0; PDO[2] = 0x04;  PDO[3] = 0x14;
            write_block(0x3C,BM92A_ADDRESS,4,PDO); //PDO Src Prov
            break;
        case 5:
            PDO[0] = 0x32; PDO[1] = 0x40; PDO[2] = 0x06;  PDO[3] = 0x14;
            write_block(0x3C,BM92A_ADDRESS,4,PDO); //PDO Src Prov
            break;
        case 6:
            PDO[0] = 0x32; PDO[1] = 0x90; PDO[2] = 0x01;  PDO[3] = 0x14;
            PDO[4] = 0x32; PDO[5] = 0xd0; PDO[6] = 0x02;  PDO[7] = 0x14;
            PDO[8] = 0x32; PDO[9] = 0xc0; PDO[10] = 0x03;  PDO[11] = 0x14;
            PDO[12] = 0x32; PDO[13] = 0xb0; PDO[14] = 0x04;  PDO[15] = 0x14;
            PDO[16] = 0x32; PDO[17] = 0x40; PDO[18] = 0x06;  PDO[19] = 0x14;
            write_block(0x3C,BM92A_ADDRESS,20,PDO); //PDO Src Prov
            break;
        default:
            break;
    }

    write_word(0x05,BM92A_ADDRESS,0x0909);
    readTwoByte(0x02,BM92A_ADDRESS);    //Read Alert Pin
    write_word(0x06,BM92A_ADDRESS,0x0000);  //Controller Config 1
    free(readBack);
    free(PDO);
}

void BM92A_fastSet() {
    if(lock_fast == 1){
        BM92Asnk_init();
    }
    if(lock_fast == 2){
        BM92Asrc_init();
    }


}
void BM92A_sink_PDO() {
    mode_set = 0;    //Sets the global for Sink;
    unsigned char *readBack = malloc(sizeof(char)*17);
    unsigned char *PDO = malloc(sizeof(char)*6);
    unsigned short controllerConfig = 0;
    BM92Asnk_init();
    controllerConfig = readTwoByte(0x06,BM92A_ADDRESS);
    write_word(0x26,BM92A_ADDRESS,0x8149);
    switch(sink_set){
        case 1:
            controllerConfig |= 0x0020;
            write_word(0x06,BM92A_ADDRESS,controllerConfig);
            PDO[0] = 0x2C; PDO[1] = 0x91;   //batteryNgt 20V max 5V min @ 3A
            PDO[2] = 0x01; PDO[3] = 0x19;
            write_block(0x20,BM92A_ADDRESS,4,PDO);
            break;
        case 2:
            controllerConfig |= 0x0020;
            write_word(0x06,BM92A_ADDRESS,controllerConfig);
            PDO[0] = 0xc8; PDO[1] = 0x90;   //batteryNgt 15V max 5V min @ 3A
            PDO[2] = 0xc1; PDO[3] = 0x12;
            write_block(0x20,BM92A_ADDRESS,4,PDO);
            break;
        case 3:
            controllerConfig &= 0xFFDF;
            write_word(0x06,BM92A_ADDRESS,controllerConfig);
            PDO[0] = 0x2C; PDO[1] = 0x91;   //batteryNgt 5V max 5V min @ 3A
            PDO[2] = 0x41; PDO[3] = 0x06;
            write_block(0x20,BM92A_ADDRESS,4,PDO);
            break;
        case 4:
            controllerConfig &= 0xFFDF;
            write_word(0x06,BM92A_ADDRESS,controllerConfig);
            PDO[0] = 0x2C; PDO[1] = 0x91;   //batteryNgt 9V max 5V min @ 3A
            PDO[2] = 0x41; PDO[3] = 0x0b;
            write_block(0x20,BM92A_ADDRESS,4,PDO);
            break;

        case 5:
            controllerConfig &= 0xFFDF;
            write_word(0x06,BM92A_ADDRESS,controllerConfig);
            PDO[0] = 0xc8; PDO[1] = 0x90;   //batteryNgt 15V max 5V min @ 3A
            PDO[2] = 0xc1; PDO[3] = 0x12;
            write_block(0x20,BM92A_ADDRESS,4,PDO);
            break;
        case 6:
            controllerConfig &= 0xFFDF;
            write_word(0x06,BM92A_ADDRESS,controllerConfig);
            PDO[0] = 0x2C; PDO[1] = 0x91;   //batteryNgt 20V max 5V min @ 3A
            PDO[2] = 0x01; PDO[3] = 0x19;
            write_block(0x20,BM92A_ADDRESS,4,PDO);
            break;
        default:
            break;
    }
    write_word(0x05,BM92A_ADDRESS,0x0909);
    readTwoByte(0x02,BM92A_ADDRESS);
    WriteRead(0x33,BM92A_ADDRESS,13,readBack);
    printPDO(readBack);
    free(readBack);
    free(PDO);
}
void pdo100WMode(){

    unsigned char *readBack = malloc(sizeof(char)*17);
    unsigned char *PDO = malloc(sizeof(char)*6);
    unsigned short controllerConfig = 0;
    controllerConfig = readTwoByte(0x06,BM92A_ADDRESS);
    write_word(0x26,BM92A_ADDRESS,0x8149);

    controllerConfig |= 0x0020;
    write_word(0x06,BM92A_ADDRESS,controllerConfig);
    PDO[0] = 0xf4;
    PDO[1] = 0x91;
    PDO[2] = 0x01;
    PDO[3] = 0x19;
    write_block(0x20,BM92A_ADDRESS,4,PDO);
    write_word(0x05,BM92A_ADDRESS,0x0909);
    readTwoByte(0x02,BM92A_ADDRESS);
    WriteRead(0x33,BM92A_ADDRESS,13,readBack);
    printPDO(readBack);

    free(readBack);
    free(PDO);

}
void sinkAllPDOMode() {
    mode_set = 0;    //Sets the global for Sink;
    unsigned char *readBack = malloc(sizeof(char)*17);
    unsigned char *PDO = malloc(sizeof(char)*6);
    unsigned short controllerConfig = 0;
    write_word(0x1A,BM92A_ADDRESS,readTwoByte(0x1A,BM92A_ADDRESS)&0xFFFE);
    controllerConfig = readTwoByte(0x06,BM92A_ADDRESS);
    write_word(0x26,BM92A_ADDRESS,0x8149);
    controllerConfig |= 0x0020;
    write_word(0x06,BM92A_ADDRESS,controllerConfig);
    PDO[0] = 0x2C; PDO[1] = 0x91; PDO[2] = 0x01;PDO[3] = 0x19;
    write_block(0x20,BM92A_ADDRESS,4,PDO);
    write_word(0x05,BM92A_ADDRESS,0x0909);
    readTwoByte(0x02,BM92A_ADDRESS);
    WriteRead(0x33,BM92A_ADDRESS,13,readBack);
    printPDO(readBack);
    free(readBack);
    free(PDO);
}
void srcAllPDOMode() {
    mode_set = 1;   //sets the global for Source;
    unsigned char *readBack = malloc(sizeof(char)*17);
    unsigned char *PDO = malloc(sizeof(char)*21);
    write_word(0x1A,BM92A_ADDRESS,readTwoByte(0x1A,BM92A_ADDRESS)|0x0001);
    write_word(0x17,BM92A_ADDRESS,0x0080); //controller Config 2
    write_word(0x26,BM92A_ADDRESS,0x9109); //system Config 1
    write_word(0x27,BM92A_ADDRESS,0x0A00); //system Config 2
    write_word(0x2F,BM92A_ADDRESS,0x0001); //system Config 3
    PDO[0] = 0x32; PDO[1] = 0x90; PDO[2] = 0x01;  PDO[3] = 0x14;    //5V @ 0.5A
    PDO[4] = 0x32; PDO[5] = 0xd0; PDO[6] = 0x02;  PDO[7] = 0x14;    //9V @ 0.5A
    PDO[8] = 0x32; PDO[9] = 0xc0; PDO[10] = 0x03;  PDO[11] = 0x14;  //12V @ 0.5A
    PDO[12] = 0x32; PDO[13] = 0xb0; PDO[14] = 0x04;  PDO[15] = 0x14;//15V @ 0.5A
    PDO[16] = 0x32; PDO[17] = 0x40; PDO[18] = 0x06;  PDO[19] = 0x14;//20V @ 0.5A
    write_block(0x3C,BM92A_ADDRESS,20,PDO); //PDO Src Prov
    write_word(0x05,BM92A_ADDRESS,0x0909);
    readTwoByte(0x02,BM92A_ADDRESS);
    write_word(0x06,BM92A_ADDRESS,0x0000);  //Controller Config 1
    free(readBack);
    free(PDO);
}

