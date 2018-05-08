/*
 * BM92A.h
 *
 *  Created on: Apr 2, 2018
 *      Author: User
 */

#ifndef BM92AI2C_H_
#define BM92AI2C_H_

#include "delay.h"

void InitBM92A();
void CommandRegister(unsigned short commandCode,unsigned char slaveAddr);
int WriteReadBM92A(unsigned char commandCode,unsigned char slaveAddr, int dataSize, unsigned char* dataArray);
void EUSCIB0_IRQHandler(void);
void testReadRegisters();
void BM92A_Debugger(unsigned int PDO);



#endif /* BM92AI2C_H_ */
