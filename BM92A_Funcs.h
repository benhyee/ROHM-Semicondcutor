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
void BM92Asrc_init();
//debugger functions
void status1_Debug(unsigned short status_1);
void status2_Debug(unsigned short status_2);
void control_config1(unsigned short config_1);
void control_config2(unsigned short config_2);
void system_config1(unsigned short sys_config1);
void system_config2(unsigned short sys_config2);
void device_capability(unsigned short cap_config);
#endif /* BM92A_FUNCS_H_ */
