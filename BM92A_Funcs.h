/*
 * BM92A_Funcs.h
 *
 *  Created on: Sep 30, 2018
 *      Author: User
 */

#ifndef BM92A_FUNCS_H_
#define BM92A_FUNCS_H_



//BM92A Helper I2C Functions
void testReadRegistersBM92A();
void BM92A_Debugger();
void CommandRegisterBM92A(unsigned short commandCode,unsigned char slaveAddr);
void BM92Asrc_init();

#endif /* BM92A_FUNCS_H_ */