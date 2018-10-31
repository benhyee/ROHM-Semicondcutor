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
unsigned short readalertStatus = 0, readstatus1 = 0;

void BM92Asrc_init()                //GPIO2 and GPIO3 set the Src Prov Table
{                                   //L L -> (60W)  L H -> (45W) H L -> (27W) H H -> (18W)


    unsigned char *readBack = malloc(sizeof(char)*30);
    unsigned char *PDO = malloc(sizeof(char)*6);

    PDO[3] = 0x32;
    PDO[2] = 0x90;
    PDO[1] = 0x01;
    PDO[0] = 0x08;
    write_word(0x17,BM92A_ADDRESS,0x0080);  // SysConfig2
    write_word(0x26,BM92A_ADDRESS,0x9109);  // SysConfig2
    write_word(0x27,BM92A_ADDRESS,0x0200);  // SysConfig2
    write_word(0x2F,BM92A_ADDRESS,0x0001);
    write_block(0x27,BM92A_ADDRESS,4,PDO);
    write_word(0x05,BM92A_ADDRESS,0x0909);
    WriteRead(0x02,BM92A_ADDRESS,2,readBack); readalertStatus = two_byteOrg(readBack);//Alert register
    WriteRead(0x03,BM92A_ADDRESS,2,readBack); readstatus1 = two_byteOrg(readBack); //status 1 register
    write_word(0x06,BM92A_ADDRESS,0x0000);  // SysConfig2

    free(readBack);

}
void BM92Asnk_init()                //GPIO2 and GPIO3 set the Src Prov Table
{


    unsigned char *readBack = malloc(sizeof(char)*30);
    unsigned char *PDO = malloc(sizeof(char)*6);
    PDO[0] = 0x00;
    PDO[1] = 0x00;
    PDO[2] = 0x00;
    PDO[3] = 0x00;
    write_word(0x03,BM92A_ADDRESS,0x0000);
    write_word(0x04,BM92A_ADDRESS,0x0000);
    write_word(0x06,BM92A_ADDRESS,0xCCE0);

    write_word(0x17,BM92A_ADDRESS,0x0000);  //config 2
    write_word(0x26,BM92A_ADDRESS,0x8749);  // SysConfig1
    write_word(0x27,BM92A_ADDRESS,0x0046);  // SysConfig2
    write_word(0x2F,BM92A_ADDRESS,0xA400);  // SysConfig3
    write_block(0x3C,BM92A_ADDRESS,4,PDO);
    write_word(0x05,BM92A_ADDRESS,0x0909);  // Set Command
    write_word(0x06,BM92A_ADDRESS,0x0000);  //
    free(readBack);
    free(PDO);

}
void BM92A_sink_PDO(){
    unsigned char *readBack = malloc(sizeof(char)*17);
    unsigned char *PDO = malloc(sizeof(char)*6);

    write_word(0x26,BM92A_ADDRESS,0x8149);
    switch(sink_set){
        case 1:
            PDO[0] = 0x2C;
            PDO[1] = 0x91;
            PDO[2] = 0x41;
            PDO[3] = 0x06;
            write_block(0x20,BM92A_ADDRESS,4,PDO);
            break;
        case 2:
            PDO[0] = 0x2C;
            PDO[1] = 0x91;
            PDO[2] = 0x41;
            PDO[3] = 0x0b;
            write_block(0x20,BM92A_ADDRESS,4,PDO);
            break;
        case 3:
            PDO[0] = 0x2C;
            PDO[1] = 0x91;
            PDO[2] = 0x01;
            PDO[3] = 0x0f;
            write_block(0x20,BM92A_ADDRESS,4,PDO);
            break;
        case 4:
            PDO[0] = 0x2C;
            PDO[1] = 0xa0;
            PDO[2] = 0xC0;
            PDO[3] = 0x12;
            write_block(0x20,BM92A_ADDRESS,4,PDO);
            break;
        case 5:
            PDO[0] = 0x2C;
            PDO[1] = 0x91;
            PDO[2] = 0x01;
            PDO[3] = 0x19;
            write_block(0x20,BM92A_ADDRESS,4,PDO);
            break;
        case 6:
            PDO[0] = 0x2C;
            PDO[1] = 0x91;
            PDO[2] = 0x01;
            PDO[3] = 0x19;
            write_block(0x20,BM92A_ADDRESS,4,PDO);
            break;
        default:
            break;
    }

    write_word(0x05,BM92A_ADDRESS,0x0909);
    readTwoByte(0x02,BM92A_ADDRESS);
    WriteRead(0x33,BM92A_ADDRESS,13,readBack);
    free(readBack);
    free(PDO);
}

