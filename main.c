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

int main(void) {
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

    unsigned short alertRead, BD_rail;


    terminal_transmitWord("Initializing Registers\n\r");

    BD99954_Startup_Routine();
    srcAllPDOMode();
    sinkAllPDOMode();
    if(((readTwoByte(0x03,BM92A_ADDRESS)&0x0300)>>8)!=0){
        monitorSnkVoltage();
        write_word(0x71,BD99954_ADDRESS,0x000F);
    }
    BM92Asnk_regSet();
    BM92Asnk_commandSet();
    terminal_transmitWord("BD9954 Refreshed Registers\n\r");
    cursorFlag = TRUE;
    readTwoByte(0x02,BM92A_ADDRESS);
    terminal_transmitWord("Successful Initialization\n\r");
    clear_BD_int();

    while(1) {

        if(cursorFlag == TRUE) {
            if(rightFlag == TRUE)menuScroll(1);
            if(leftFlag == TRUE) menuScroll(-1);
            displayMode();
            cursorFlag = FALSE;
        }

        if(AlertFlag) {
            alertRead = readTwoByte(0x02,BM92A_ADDRESS);
            BD_rail = readTwoByte(0x70,BD99954_ADDRESS);
            write_word(0x70,BD99954_ADDRESS,0x00FF);
            if(mode_set ==0) {
                if(((readTwoByte(0x03,BM92A_ADDRESS)&0x0300)>>8)!=0){
                    monitorSnkVoltage();
                    write_word(0x71,BD99954_ADDRESS,0x000F);
                }
                if((BD_rail&0x0004)>>2||
                        readTwoByte(0x5F,BD99954_ADDRESS)>1500){
                    monitorVCCSnkVoltage();
                    write_word(0x72,BD99954_ADDRESS,0x000F);
                }
                cursorFlag = FALSE;select = 0;
                displayMode();
                BD99954_INT = FALSE;
                AlertFlag = FALSE;
                readTwoByte(0x02,BM92A_ADDRESS);

            }
            else if(mode_set == 1) {
                if((alertRead &0x2000)>>13) {
                    sourceNegotiate();
                    AlertFlag = FALSE;
                    if(((readTwoByte(0x03,BM92A_ADDRESS)&0x0300)>>8)!=0) monitorSrcVoltage();
                    reverseVoltage(5024);
                    select = 0;
                    displayMode();
                    readTwoByte(0x02,BM92A_ADDRESS);
                }

                else{   //if its just a register update
                   alertRead = readTwoByte(0x02,BM92A_ADDRESS);
               }
            }
            AlertFlag = FALSE;
        }
        __sleep();      // go to lower power mode

    }
}
