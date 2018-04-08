/*
 * I2CFuncs.h
 *
 *  Created on: Mar 31, 2018
 *      Author: dad
 */

#ifndef BD99954I2C_H_
#define BD99954I2C_H_


void InitBD99954(unsigned char DeviceAddress);
void WriteBD99954(unsigned short MemAddress, unsigned char MemByte);
unsigned char ReadBD99954(unsigned short MemAddress);
void EUSCIB1_IRQHandler(void);



#endif /* I2CFUNCS_H_ */
