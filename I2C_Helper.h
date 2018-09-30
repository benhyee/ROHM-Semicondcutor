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







#endif /* BM92AI2C_H_ */
