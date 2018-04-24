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
void WriteBM92A( char MemByte,unsigned char slaveAddr);
int WriteReadBM92A(char commandCode,unsigned char slaveAddr, int dataSize, unsigned char* dataArray);
void EUSCIB0_IRQHandler(void);




#endif /* BM92AI2C_H_ */
