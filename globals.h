/*
 * globals.h
 *
 *  Created on: May 10, 2018
 *      Author: User
 */

#ifndef GLOBALS_H_
#define GLOBALS_H_

#define TRUE 1
#define FALSE 0
extern volatile char AlertFlag;
extern char cursorFlag;
extern char settings_menu;
extern char BD99954_INT;

extern char fast_set;   //For the Fast Set Menu
extern char mode_set;   //Sink or Source mode
extern char standard_menu;  //Which part of the Standard Menu Cursor is on
extern char sink_set;   //PDO Sink menu navigation
extern char source_set; //PDO Source menu navigation
extern char sleep_time; //Sleep mode timer but removed
extern char high100_mode;   //High power mode but removed
extern char advanced_menu;  //Keeps track of which part of Advanced menu
extern char batt_chg;   //Charge enable or disable state
extern char battery_sel;    //Battery configuration but removed
extern char uart_en;    //UART enable flag
extern char rightFlag;  //Triggers on joystick GPIO Interrupt
extern char leftFlag;   //Triggers on joystick GPIO Interrupt
extern char select; //Triggers on joystick GPIO Interrupt
extern char pushFlag; //Triggers on joystick GPIO Interrupt
extern char sleepWake; //Triggers on joystick GPIO Interrupt
#endif /* GLOBALS_H_ */
