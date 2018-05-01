
#include <BM92AI2C.h>
#include "msp432.h"
#include "delay.h"
#include <time.h>
#include <stdint.h>
#include "UART.h"
#include "helper.h"

#define BM92A_ADDRESS1 0x1A
#define BM92A_ADDRESS2 0x18
#define BD99954_ADDRESS 0x09
#define CURRENT_FREQ FREQ_24_MHZ

int main(void)
{
    unsigned int i, value=0, RDO = 0, PDO = 0;
    unsigned int battery = 0, nonBattery = 0;
    unsigned char tempHold;
    unsigned short status_1=0,status_2=0,config_1=0,config_2=0;
    unsigned short sys_config_1=0, sys_config_2=0, alertStatus=0,capability=0;
    unsigned short Display_alert = 0;

    unsigned char readBack[30];
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;       // Stop watchdog timer
    InitBM92A();
    terminal_init();

    __enable_irq();                           // Enable global interrupt
    while(1)
    {
        value=0, RDO = 0, PDO = 0;
        battery = 0, nonBattery = 0;
        tempHold;
        status_1=0,status_2=0,config_1=0,config_2=0;
        sys_config_1=0, sys_config_2=0, alertStatus=0,capability=0;
        Display_alert = 0;

//        WriteReadBM92A(0x02,BM92A_ADDRESS2,2,readBack);//capability register
//        for(i = 0; i < 2; i++)
//        {
//            tempHold = (*(readBack+i)&0xFF);
//            alertStatus = (tempHold << (8*i)) | alertStatus;
//        }
//
//        WriteReadBM92A(0x03,BM92A_ADDRESS2,2,readBack); //status 1 register
//        for(i = 0; i < 2; i++)
//        {
//            tempHold = (*(readBack+i)&0xFF);
//            status_1 = (tempHold << (8*i)) | status_1;
//        }
//        WriteReadBM92A(0x04,BM92A_ADDRESS2,2,readBack); //status 1 register
//        for(i = 0; i < 2; i++)
//        {
//            tempHold = (*(readBack+i)&0xFF);
//            status_2 = (tempHold << (8*i)) | status_2;
//        }
//        WriteReadBM92A(0x06,BM92A_ADDRESS2,2,readBack); //controller config 1
//        for(i = 0; i < 2; i++)
//        {
//            tempHold = (*(readBack+i)&0xFF);
//            config_1 = (tempHold << (8*i)) | config_1;
//        }
//        WriteReadBM92A(0x07,BM92A_ADDRESS2,2,readBack);//capability register
//        for(i = 0; i < 2; i++)
//        {
//            tempHold = (*(readBack+i)&0xFF);
//            capability = (tempHold << (8*i)) | capability;
//        }
//        WriteReadBM92A(0x17,BM92A_ADDRESS2,2,readBack); //controller config 1
//        for(i = 0; i < 2; i++)
//        {
//            tempHold = (*(readBack+i)&0xFF);
//            config_2 = (tempHold << (8*i)) | config_2;
//        }
//        WriteReadBM92A(0x19,BM92A_ADDRESS2,2,readBack); //controller config 1
//        for(i = 0; i < 4; i++)
//        {
//            tempHold = (*(readBack+i)&0xFF);
//            Display_alert = (tempHold << (8*i)) | Display_alert;
//        }
//
//        WriteReadBM92A(0x26,BM92A_ADDRESS2,2,readBack); //controller config 1
//        for(i = 0; i < 4; i++)
//        {
//            tempHold = (*(readBack+i)&0xFF);
//            nonBattery = (tempHold << (8*i)) | nonBattery;
//        }
//        WriteReadBM92A(0x27,BM92A_ADDRESS2,2,readBack); //controller config 1
//        for(i = 0; i < 4; i++)
//        {
//            tempHold = (*(readBack+i)&0xFF);
//            battery = (tempHold << (8*i)) | battery;
//        }
//
//
//
//        WriteReadBM92A(0x28,BM92A_ADDRESS2,4,readBack);//PDO register
//        for(i = 0; i < 4; i++)
//        {
//            tempHold = (*(readBack+i)&0xFF);
//            PDO = (tempHold << (8*i)) | PDO;
//        }
//        WriteReadBM92A(0x2B,BM92A_ADDRESS2,4,readBack);//RDO register
//        for(i = 0; i < 4; i++)
//        {
//            tempHold = (*(readBack+i)&0xFF);
//            RDO = (tempHold << (8*i)) | RDO;
//        }

        WriteReadBM92A(0x02,BM92A_ADDRESS2,2,readBack);//Alert register
        alertStatus = two_byteOrg(readBack);

        WriteReadBM92A(0x03,BM92A_ADDRESS2,2,readBack); //status 1 register
        status_1 = two_byteOrg(readBack);

        WriteReadBM92A(0x04,BM92A_ADDRESS2,2,readBack); //status 2 register
        status_2 = two_byteOrg(readBack);

        WriteReadBM92A(0x06,BM92A_ADDRESS2,2,readBack); //controller config 1
        config_1 = two_byteOrg(readBack);

        WriteReadBM92A(0x07,BM92A_ADDRESS2,2,readBack); //Capability
        capability = two_byteOrg(readBack);

        WriteReadBM92A(0x17,BM92A_ADDRESS2,2,readBack); //controller config 1
        config_2 = two_byteOrg(readBack);

        WriteReadBM92A(0x19,BM92A_ADDRESS2,2,readBack); //controller config 1
        Display_alert = two_byteOrg(readBack);

        WriteReadBM92A(0x20,BM92A_ADDRESS2,4,readBack);//Autongtsnk Info non-Battery register
        nonBattery = four_byteOrg(readBack);

        WriteReadBM92A(0x23,BM92A_ADDRESS2,4,readBack);//Autongtsnk Info Battery register
        battery = four_byteOrg(readBack);

        WriteReadBM92A(0x26,BM92A_ADDRESS2,2,readBack); //system controller config 1
        sys_config_1 = two_byteOrg(readBack);

        WriteReadBM92A(0x27,BM92A_ADDRESS2,2,readBack); //system controller config 2
        sys_config_2 = two_byteOrg(readBack);

        WriteReadBM92A(0x28,BM92A_ADDRESS2,4,readBack);//PDO register
        PDO = four_byteOrg(readBack);

        WriteReadBM92A(0x2B,BM92A_ADDRESS2,4,readBack);//RDO register
        RDO = four_byteOrg(readBack);


          for(i = 0; i<40; i++);



    }





    __sleep();      // go to lower power mode

}
