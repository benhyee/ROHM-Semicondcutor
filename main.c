
#include <BM92AI2C.h>
#include "msp432.h"
#include "delay.h"
#include <time.h>
#include <stdint.h>
#include "UART.h"
#include "helper.h"


#define BD99954_ADDRESS 0x09
#define CURRENT_FREQ FREQ_24_MHZ
#define BM92A_ADDRESS1 0x1A
#define BM92A_ADDRESS2 0x18
int main(void)
{

    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;       // Stop watchdog timer
    InitBM92A();
    terminal_init();
    unsigned char *readBack = malloc(sizeof(char)*30);
    unsigned int i = 0;
    unsigned int maxCurrent = 0, voltage = 0, current = 0;
    unsigned short status_1=0,status_2=0,config_1=0,config_2=0;
    unsigned short sys_config_1=0, sys_config_2=0, alertStatus=0,capability=0;

    unsigned char role = 0, recepticle = 0;
    unsigned int value=0, RDO = 0, PDO = 0;
    __enable_irq();                           // Enable global interrupt
    while(1)
    {
        WriteReadBM92A(0x28,BM92A_ADDRESS2,4,readBack);//PDO register
        PDO = four_byteOrg(readBack);


        if(PDO !=0)
        {
            WriteReadBM92A(0x03,BM92A_ADDRESS2,2,readBack); //status 1 register
            status_1 = two_byteOrg(readBack);
            WriteReadBM92A(0x26,BM92A_ADDRESS2,2,readBack); //system controller config 1
            sys_config_1 = two_byteOrg(readBack);
            WriteReadBM92A(0x2B,BM92A_ADDRESS2,4,readBack);//RDO register
            RDO = four_byteOrg(readBack);

            role = status_1 & 0x1000;
            role = role >> 12;

            maxCurrent = PDO & 0x3FF;   // BitMask first 10 Bits
            maxCurrent = maxCurrent * 10;   // Max Current 10 mA
            voltage = PDO >> 10;    // Shift 10 to accomodate Voltage reading
            voltage = (voltage & 0x3FF) * 50;   // 50 mV times 10 bits of Voltage
            current = RDO & 0x3FF;          // First 10 register of RDO are operating Current
            recepticle = sys_config_1 & 0xF; // Recepticle Type
            if(role == 0)
            {
                terminal_transmitWord("BM92A is Sink\n\r");
            }
            else
            {
                terminal_transmitWord("BM92A is Source\n\r");

            }

            switch(recepticle){
            case 0x9:
                terminal_transmitWord("Type-C \n\r");
                break;

            case 0xA:
                terminal_transmitWord("Type-C 3A \n\r");
                break;
            case 0xB:
                terminal_transmitWord("Type=C 5A \n\r");
                break;
            default:
                terminal_transmitWord("Unconfirmed Recepticle Type\n\r");
                break;
            }
            terminal_transmitWord("Max Amps:");
            terminal_transmitInt(maxCurrent);
            terminal_transmitWord("\nOperating Amps:");
            terminal_transmitInt(current);
            terminal_transmitWord("\nVoltage:");
            terminal_transmitInt(voltage);
            terminal_transmitWord("\r\n");

        }


        for(i = 0; i<40; i++);



    }





    __sleep();      // go to lower power mode

}
