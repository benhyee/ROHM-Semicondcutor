/*
 * BM92A.h
 *
 *  Created on: Apr 2, 2018
 *      Author: User
 */

#ifndef BM92AI2C_H_
#define BM92AI2C_H_

#include "delay.h"

void InitBM92A(unsigned char DeviceAddress);
void WriteBM92A(unsigned char DeviceAddress,unsigned char commandBytes);
unsigned short ReadBM92A(unsigned char SlaveAddress);
void EUSCIB0_IRQHandler(void);
void BM92A_reset();
void BM92init_Read();
void BM92init_Write(unsigned char DeviceAddress);




#endif /* BM92AI2C_H_ */
