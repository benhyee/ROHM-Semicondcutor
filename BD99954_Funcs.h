/*
 * BD99954_Funcs.h
 *
 *  Created on: Sep 30, 2018
 *      Author: User
 */

#ifndef BD99954_FUNCS_H_
#define BD99954_FUNCS_H_


//BD99954 Helper I2C Functions
void BD_INT_INIT();
void clear_BD_int();
void BD99954_Startup_Routine();
void default_BDSettings();
void reverseBuckBoost();
void reverseVoltage(int voltage);
void reverseEnable(char channel);
void reverseDisable();
void chgEnable();
void chgDisable();


#endif /* BD99954_FUNCS_H_ */
