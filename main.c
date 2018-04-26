
#include <BM92AI2C.h>
#include "msp432.h"
#include "delay.h"
#include <time.h>
#include <stdint.h>

#define BM92A_ADDRESS1 0x1A
#define BM92A_ADDRESS2 0x18
#define BD99954_ADDRESS 0x09
#define CURRENT_FREQ FREQ_24_MHZ

int main(void)
{
    unsigned int i, value, RDO, PDO;
    unsigned int battery, nonBattery;

    unsigned short status_1,status_2,config_1,config_2;

    unsigned char readBack[30];
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;       // Stop watchdog timer
    InitBM92A();
    unsigned char tempHold;

    __enable_irq();                           // Enable global interrupt
    while(1)
    {

        WriteReadBM92A(0x28,BM92A_ADDRESS2,4,readBack);//PDO register
        for(i = 0; i < 4; i++)
        {
            tempHold = (*(readBack+i)&0xFF);
            PDO = (tempHold << (8*i)) | PDO;
        }
        WriteReadBM92A(0x2B,BM92A_ADDRESS2,4,readBack);//RDO register
        for(i = 0; i < 4; i++)
        {
            tempHold = (*(readBack+i)&0xFF);
            RDO = (tempHold << (8*i)) | RDO;
        }
        WriteReadBM92A(0x20,BM92A_ADDRESS2,4,readBack);//Autongtsnk Info non-Battery register
        for(i = 0; i < 4; i++)
        {
            tempHold = (*(readBack+i)&0xFF);
            nonBattery = (tempHold << (8*i)) | nonBattery;
        }
        WriteReadBM92A(0x23,BM92A_ADDRESS2,4,readBack);//Autongtsnk Info Battery register
        for(i = 0; i < 4; i++)
        {
            tempHold = (*(readBack+i)&0xFF);
            battery = (tempHold << (8*i)) | battery;
        }
        WriteReadBM92A(0x03,BM92A_ADDRESS2,2,readBack); //status 1 register
        for(i = 0; i < 2; i++)
        {
            tempHold = (*(readBack+i)&0xFF);
            status_1 = (tempHold << (8*i)) | status_1;
        }
        WriteReadBM92A(0x04,BM92A_ADDRESS2,2,readBack); //status 1 register
        for(i = 0; i < 2; i++)
        {
            tempHold = (*(readBack+i)&0xFF);
            status_2 = (tempHold << (8*i)) | status_2;
        }
        WriteReadBM92A(0x06,BM92A_ADDRESS2,2,readBack); //controller config 1
        for(i = 0; i < 2; i++)
        {
            tempHold = (*(readBack+i)&0xFF);
            config_1 = (tempHold << (8*i)) | config_1;
        }
        WriteReadBM92A(0x17,BM92A_ADDRESS2,2,readBack); //controller config 1
        for(i = 0; i < 2; i++)
        {
            tempHold = (*(readBack+i)&0xFF);
            config_2 = (tempHold << (8*i)) | config_2;
        }
        for(i = 0; i<40; i++);
    }





    __sleep();      // go to lower power mode

}
