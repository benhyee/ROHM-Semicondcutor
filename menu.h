/*
 * menu.h
 *
 *  Created on: Oct 2, 2018
 *      Author: User
 */

#ifndef MENU_H_
#define MENU_H_


#include "I2C_Helper.h"
#include "BD99954_Funcs.h"
void displayMode();
void fastSetMenu();
void standardMenu();
void advancedMenu();
void sinkPDOMenu();
void sourcePDOMenu();
void sleepMenu();
void enableMenu(char mode, char enable);
void batterySelectMenu();

void sideCheck(int value);
void menuScroll(char value);

#endif /* MENU_H_ */
