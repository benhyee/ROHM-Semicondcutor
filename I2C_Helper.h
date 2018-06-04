/*
 * BM92A.h
 *
 *  Created on: Apr 2, 2018
 *      Author: User
 */

#ifndef I2C_HELPER_H_
#define I2C_HELPER_H_

#include "delay.h"




void InitI2C();
int WriteRead(unsigned char commandCode,unsigned char slaveAddr, int dataSize, unsigned char* dataArray);
void write_word(unsigned char commandCode,unsigned char slaveAddr, unsigned short content);
void write_block(unsigned char commandCode,unsigned char slaveAddr, int dataSize, unsigned char* dataArray);
int WriteReadBD99954(unsigned char commandCode, unsigned char* dataArray);
void EUSCIB0_IRQHandler(void);
void clear_register(unsigned char commandCode,unsigned char slaveAddr, int dataSize);


//BM92A Helper I2C Functions
void testReadRegistersBM92A();
void BM92A_Debugger();
void CommandRegisterBM92A(unsigned short commandCode,unsigned char slaveAddr);
void BM92Asrc_init();

//BD99954 Helper I2C Functions
void BD99954_Startup_Routine();
void BD99954ReadRegister();



#endif /* BM92AI2C_H_ */
