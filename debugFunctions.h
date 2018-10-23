/*
 * debugFunctions.h
 *
 *  Created on: Oct 23, 2018
 *      Author: User
 */

#ifndef DEBUGFUNCTIONS_H_
#define DEBUGFUNCTIONS_H_


//BM92A debugger functions
void BM92A_Debugger();
void status1_Debug(unsigned short status_1);
void status2_Debug(unsigned short status_2);
void control_config1(unsigned short config_1);
void control_config2(unsigned short config_2);
void system_config1(unsigned short sys_config1);
void system_config2(unsigned short sys_config2);
void device_capability(unsigned short cap_config);
//BD99954 Debug Functions
void testReadRegistersBM92A();
void BD99954ReadRegister();
void IBUS_LIM_Debug(unsigned short IBUS_LIM);
void ICC_LIM_Debug(unsigned short ICC_LIM);
void VRECHG_Debug(unsigned short VRECHG);
void VBATOVP_Debug(unsigned short VBATOVP);
void VSYSREG_Debug(unsigned short VSYSREG);
void VPRECHG_TH_Debug(unsigned short VPRECHG_TH);
void VFASTCHG_REG_Debug(unsigned short VFASTCHG_REG);
void ITRICH_Debug(unsigned short ITRICH);
void IPRECH_Debug(unsigned short IPRECH);
void ICHG_Debug(unsigned short ICHG);
void ITERM_Debug(unsigned short ITERM);
void BD99ID_Debug(unsigned short CHIP_ID);
void BD99REV_Debug(unsigned short CHIP_REV);
void VBAT_AVE_Debug(unsigned short VBAT_AVE);
void VBUS_AVE_Debug(unsigned short VBUS_AVE);
void VSYS_AVE_Debug(unsigned short VSYS_AVE);
void SYS_STATUS_Debug(unsigned short SYS_STATUS);

#endif /* DEBUGFUNCTIONS_H_ */
