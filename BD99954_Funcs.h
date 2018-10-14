/*
 * BD99954_Funcs.h
 *
 *  Created on: Sep 30, 2018
 *      Author: User
 */

#ifndef BD99954_FUNCS_H_
#define BD99954_FUNCS_H_


//BD99954 Helper I2C Functions
void BD99954_Startup_Routine();
void BD99954ReadRegister();

//Debug Functions
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




#endif /* BD99954_FUNCS_H_ */
