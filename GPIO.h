/*
 * GPIO.h
 *
 *  Created on: Nov 12, 2018
 *      Author: User
 */

#ifndef GPIO_H_
#define GPIO_H_


void gpio_init();
void sleepMode();
void LCD_toggle();
char readSwitchState();
void chargeState();
void LCD_wake();
void UARTEnable();

#endif /* GPIO_H_ */
