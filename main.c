/*
 * ROHM SemiConductor USB-C Project
 *  BM92AI2C.h -> All the BM92A code as well as a register read function for testing
 *  BD99954.h -> Not completed
 *  delay.h -> Frequency set and delay functions
 *  helper.h -> Functions that are helpful for organization and exception handling
 *  LCD.h -> Functions for the LCD monitor.
 *  UART.h -> Functions for terminal use on the computer COM4 19200
 *
 *  Pin outs:
 *  P1.6 -> SDA;    P1.7->SCL
 *  P3.6-> GPIO0(#Alert) BM92A
 *  P4.4-4.7 -> D4-D7 LCD
 *  P4.0 -> RS;     P4.1 -> RW;     P4.2->EN (LCD)
 *
 */
#include "I2C_Helper.h"
#include "msp432.h"
#include <time.h>
#include <stdint.h>
#include "UART.h"
#include "helper.h"
#include "interruptPins.h"
#include <stdlib.h>
#include "globals.h"
#include "lcd.h"
#include "BD99954_Funcs.h"
#include "BM92A_Funcs.h"
#include "menu.h"
#include "delay.h"
#include "debugFunctions.h"
#include "GPIO.h"

#define BD99954_ADDRESS 0x09
#define BM92A_ADDRESS 0x18
#define CURRENT_FREQ FREQ_3_MHZ

int main(void)
{
    __disable_irq();

    set_DCO(CURRENT_FREQ);
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;       // Stop watchdog timer
    gpio_init();
    InitI2C();
    LCD_init();
    LCD_command(0x01); // clear screen, move cursor home
    terminal_init();    //SDA -> P1.6 SCL->P1.7
    interruptPinInit();
    __enable_irq();                           // Enable global interrupt

    unsigned short alertRead;
    unsigned char readAlertPin = 0;
    unsigned int RDO;
    unsigned int currentPDOreg;
    unsigned int PDOvoltage;
    unsigned int RDO_OP_Current,RDOMaxCurrent;
    BD99954_Startup_Routine();
    reverseDisable();
    chgDisable();
    cursorFlag = TRUE;
    unsigned char *readBack = malloc(sizeof(char)*21);
    unsigned char *PDO = malloc(sizeof(char)*20);

//    reverseBuckBoost();
//    reverseVoltage(5024);
//    reverseEnable(1);
//    write_word(0x17,BM92A_ADDRESS,0x0080); //controller Config 2
//    write_word(0x26,BM92A_ADDRESS,0x9109); //system Config 1
//    write_word(0x27,BM92A_ADDRESS,0x0A00); //system Config 2
//    write_word(0x2F,BM92A_ADDRESS,0x0001); //system Config 3
//    PDO[0] = 0x32; PDO[1] = 0x90; PDO[2] = 0x01;  PDO[3] = 0x14;
//    PDO[4] = 0x32; PDO[5] = 0xd0; PDO[6] = 0x02;  PDO[7] = 0x14;
//    PDO[8] = 0x32; PDO[9] = 0xc0; PDO[10] = 0x03;  PDO[11] = 0x14;
//    PDO[12] = 0x32; PDO[13] = 0xb0; PDO[14] = 0x04;  PDO[15] = 0x14;
//    PDO[16] = 0x32; PDO[17] = 0x40; PDO[18] = 0x06;  PDO[19] = 0x14;
//    write_block(0x3C,BM92A_ADDRESS,20,PDO); //PDO Src Prov
//    write_word(0x05,BM92A_ADDRESS,0x0909);
//    readTwoByte(0x02,BM92A_ADDRESS);
//    readTwoByte(0x03,BM92A_ADDRESS);
//    write_word(0x06,BM92A_ADDRESS,0x0000);  //Controller Config 1
//    plugAlertFlag = TRUE;
//    WriteRead(0x3C,BM92A_ADDRESS,21,readBack);
//    printPDO(readBack);
//    free(PDO);

    while(1)
    {

        if(cursorFlag == TRUE)
        {
            if(rightFlag == TRUE)menuScroll(1);
            if(leftFlag == TRUE) menuScroll(-1);
            displayMode();
            cursorFlag = FALSE;

        }
        if(plugAlertFlag)
        {
            if(mode_set ==0)
            {
                if((readTwoByte(0x03,BM92A_ADDRESS)&0x0080)>>7) //If plug detected
                {
                    terminal_transmitWord("Ngt\t");
                    LCD_clearLine();
                    LCD_command(0x01); // clear screen, move cursor home
                    if(lock_fast == 1)LCD_word("Sink");
                    else if(lock_fast == 2)LCD_word("Source");
                    LCD_enter();
                    currentPDO();
                    alertRead = readTwoByte(0x02,BM92A_ADDRESS);
                    printPDO(readBack);


    //                delay_longer(5);
                    plugAlertFlag = FALSE;
                }
                else{   //if its just a register update
                    displayMode();
                    alertRead = readTwoByte(0x02,BM92A_ADDRESS);
                    plugAlertFlag = FALSE;
                }

            }
            else if(mode_set == 1)
            {
                alertRead = readTwoByte(0x02,BM92A_ADDRESS);
                if((alertRead &0x2000)>>13){
                    currentPDOreg = readFourByte(0x28,BM92A_ADDRESS);   //Ngt PDO
                    WriteRead(0x08,BM92A_ADDRESS,12,readBack);  //Display Sink Cap
                    printPDO(readBack);
                    RDO = readFourByte(0x2B,BM92A_ADDRESS);
                    PDOvoltage = ngtVoltage(currentPDOreg);
                    if(PDOvoltage > 5000)
                    {
                        reverseVoltage(PDOvoltage); //If PDO voltage is not 5V
                    }
                    else{
                        reverseVoltage(5024);
                    }
                    delay_ms(100,CURRENT_FREQ);
                    alertRead = readTwoByte(0x02,BM92A_ADDRESS);
                    RDO_OP_Current = ngtOperatingCurrent(RDO);
                    RDOMaxCurrent = ngtMaxCurrent(RDO);
                }
                alertRead = readTwoByte(0x02,BM92A_ADDRESS);
            }

        }

        __sleep();      // go to lower power mode

    }


}
