/*
 * interruptPins.h
 *
 *  Created on: May 10, 2018
 *      Author: User
 */

#ifndef INTERRUPTPINS_H_
#define INTERRUPTPINS_H_



void interruptPinInit();


void PORT1_IRQHandler(void);
void PORT3_IRQHandler(void);
void PORT6_IRQHandler(void);


#endif /* INTERRUPTPINS_H_ */
