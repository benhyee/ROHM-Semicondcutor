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
    gpio_init();    //Enables GPIO Pins such as LEDs
    InitI2C();
    LCD_init();
    LCD_command(0x01); // clear screen, move cursor home
    terminal_init();    //SDA -> P1.6 SCL->P1.7
    interruptPinInit();
    __enable_irq();                           // Enable global interrupt
    unsigned short alertRead, BD_rail;
    terminal_transmitWord("Initializing Registers\n\r");
    chargeState();  //Reads DIP Switch and Charge enable or disable
    srcAllPDOMode();    //Writes all the PDO settings for the BM92A
    sinkAllPDOMode();
    if(((readTwoByte(0x03,BM92A_ADDRESS)&0x0300)>>8)!=0){   //Catch statement for no battery attached
        // Occurs before the snk reg set because 0x0909 sent to 0x05 on the BM92A causes the source to disconnect 5V
        // turning off the BM92A
        BD99954_Startup_Routine();
        readTwoByte(0x02,BM92A_ADDRESS);
        monitorSnkVoltage();
        write_word(0x71,BD99954_ADDRESS,0x000F);
    }
    BM92Asnk_regSet();  //Sets the registers for sink mode initially
    BM92Asnk_commandSet();
    terminal_transmitWord("BD9954 Refreshed Registers\n\r");
    cursorFlag = TRUE;
    terminal_transmitWord("Successful Initialization\n\r");
    BD99954_Startup_Routine();  //Map sets and battery threshold sets
    readTwoByte(0x02,BM92A_ADDRESS);
    clear_BD_int();
    while(1) {

        if(cursorFlag == TRUE) {    //Triggered by joystick action
            if(rightFlag == TRUE)menuScroll(1);
            if(leftFlag == TRUE) menuScroll(-1);
            displayMode();          //refreshes the menu with new joystick presses
            cursorFlag = FALSE;
        }

        if(AlertFlag) {
            alertRead = readTwoByte(0x02,BM92A_ADDRESS);
            BD_rail = readTwoByte(0x70,BD99954_ADDRESS);
            write_word(0x70,BD99954_ADDRESS,0x00FF);
            if(mode_set ==0) {  //Checks to see if in Sink or Source mode
                if(((readTwoByte(0x03,BM92A_ADDRESS)&0x0300)>>8)!=0){
                    monitorSnkVoltage();
                    write_word(0x71,BD99954_ADDRESS,0x000F);    //All this occurs after exiting of the while loop
                    displayMode();
                    readTwoByte(0x02,BM92A_ADDRESS);

                }
                if((BD_rail&0x0004)>>2){
                    monitorVCCSnkVoltage();
                    write_word(0x72,BD99954_ADDRESS,0x000F);//All this occurs after exiting of the while loop
                    displayMode();
                    readTwoByte(0x02,BM92A_ADDRESS);

                }
                cursorFlag = FALSE;select = 0;
                BD99954_INT = FALSE;
                AlertFlag = FALSE;

            }
            else if(mode_set == 1) {
                if((alertRead &0x2000)>>13) {
                    sourceNegotiate();
                    AlertFlag = FALSE;
                    if(((readTwoByte(0x03,BM92A_ADDRESS)&0x0300)>>8)!=0) monitorSrcVoltage();
                    reverseVoltage(5024);//All this occurs after exiting of the while loop
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
