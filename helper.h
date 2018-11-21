/*
 * helper.h
 *
 *  Created on: Apr 25, 2018
 *      Author: User
 */

#ifndef HELPER_H_
#define HELPER_H_


unsigned short two_byteOrg(unsigned char* dataArray);
unsigned int four_byteOrg(unsigned char* dataArray);
int transferIntArray(unsigned char* tempArray, unsigned int* PDO);
void blinkLED();
void printPDO(unsigned char* dataArray);
int ngtVoltage(unsigned int currentPDOreg);
int ngtMaxCurrent(unsigned int currentRDO);
int ngtOperatingCurrent(unsigned int currentRDO);
void monitorSrcVoltage();
void sourceNegotiate();
void monitorSnkVoltage();
void monitorVCCSnkVoltage();

#endif /* HELPER_H_ */
