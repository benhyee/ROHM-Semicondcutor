/*
 * I2CFuncs.h
 *
 *  Created on: Mar 31, 2018
 *      Author: dad
 */

#ifndef BD99954I2C_H_
#define BD99954I2C_H_


void InitBD9954(uint8_t DeviceAddress);
void WriteBD99954(uint16_t MemAddress, uint8_t MemByte);
uint8_t ReadBD99954(uint16_t MemAddress);
void EUSCIB0_IRQHandler(void);



#endif /* I2CFUNCS_H_ */
