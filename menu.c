/*
 * menu.c
 *
 *  Created on: Oct 2, 2018
 *       Author: Benjamin Yee
 */

#include "interruptPins.h"
#include "LCD.h"
#include "globals.h"
#include "menu.h"
#include "I2C_Helper.h"
#include "debugFunctions.h"
#include "BD99954_Funcs.h"
#include "BM92A_Funcs.h"
#include "msp432.h"
#include "GPIO.h"
#include "UART.h"
#include "delay.h"
#define TRUE 1
#define FALSE 0
#define BD99954_ADDRESS 0x09
#define BM92A_ADDRESS 0x18


unsigned short readTwoByte(unsigned char commandCode,unsigned char slaveAddr);

char cursorFlag = 0;
char settings_menu = 1, standard_menu = 1,advanced_menu =1;
char fast_set = 1;
char sink_set = 4, source_set = 6, sleep_time = 6, high100_mode = 0;
char batt_chg = 0, battery_sel = 0, uart_en = 0;
char mode_set= 0;

unsigned short generalShort;
unsigned int generalInt,temp;
void displayMode()  //The highest level of the menus
{
    if(settings_menu < 1){settings_menu  = 1;}
    else if(settings_menu > 3){settings_menu = 3;}
    switch(settings_menu)
    {
        case 1:
            fastSetMenu();
            break;
        case 2:
            standardMenu();
            break;
        case 3:
            advancedMenu();
            break;
        default:
            break;
    }

    cursorFlag = FALSE;
}
void fastSetMenu()
{
   if(fast_set < 1)fast_set = 2;
   if(fast_set > 2)fast_set = 1;
   LCD_command(0x01); // clear screen, move cursor home
   //Merely being on the fast set display will put the BM92A into either sink or source mode
   if(fast_set == 1)
   {
       LCD_word("Ready to Sink");
       LCD_enter();
       LCD_clearLine();
       LCD_fastSnkSetPDO();
       if(AlertFlag == FALSE)
       {
           mode_set = 0;
           reverseDisable();    //Ensures source mode is not active in sink mode
           clear_BD_int();
           BM92Asnk_regSet();   BM92Asnk_commandSet(); // Register writes the sink mode
           terminal_transmitWord("Sink PDO Set\n\r");

       }
   }
   else if(fast_set == 2){
       LCD_word("Ready to Source");
       LCD_enter();
       LCD_clearLine();
       LCD_fastSrcSetPDO();
       if(AlertFlag == FALSE)
       {
           mode_set = 1;
           clear_BD_int();
           reverseBuckBoost();  //Reverse Buck boost mode setting of registers BD99954
           BM92Asrc_regSet();   BM92Asrc_commandSet(); //Register writes for source mode
           terminal_transmitWord("Source PDO Set\n\r");
       }
   }
   if(pushFlag)
   {
       sleepMode();
   }
}
void standardMenu(){
    if(standard_menu > 2)standard_menu = 1; //Applies wrap around menu
    if(standard_menu < 1)standard_menu = 2;
    LCD_clearLine();
    LCD_command(0x01); // clear screen, move cursor home
    LCD_word("Standard Menu");
    LCD_enter();
    switch(standard_menu)
    {
        case 1:
            if(select) sinkPDOMenu();   //Navigate through the LCD of Sink
            LCD_word("       Sink PDO");
            break;
        case 2:
            if(select) sourcePDOMenu(); //Navigate through the LCD of Source
            LCD_word("     Source PDO");

            break;
        default:
            break;
    }
    pushFlag = 0;

}
void sinkPDOMenu(){
    LCD_clearLine();
    LCD_command(0x01); // clear screen, move cursor home
    LCD_word("Sink PDO");
    LCD_enter();

    if(sink_set < 1)sink_set = 4;
    if(sink_set > 4)sink_set = 1;
    switch(sink_set)
    {
        case 1:
            LCD_word("        5V (3A)");
            break;
        case 2:
            LCD_word("    5V- 9V (3A)");
            break;
        case 3:
            LCD_word("    5V-15V (3A)");
            break;
        case 4:
            LCD_word("    5V-20V (3A)");
            break;
        default:
            break;
    }
    if(select == 2){
        BM92A_sink_PDO();
        if(mode_set==0)
        {
            BM92Asnk_regSet();  //Updates the registers with new PDO
            BM92Asnk_commandSet();  //Does not affect source mode capabilities nor does it change the mode
        }
        select = 0;pushFlag = 0;
        displayMode();
    }


}
void sourcePDOMenu(){
    LCD_clearLine();
    LCD_command(0x01); // clear screen, move cursor home
    LCD_word("Source PDO");
    LCD_enter();

    if(source_set < 1)source_set = 6;
    if(source_set > 6)source_set = 1;
    switch(source_set)
    {
        case 1:
            LCD_word("        5V (3A)");
            break;
        case 2:
            LCD_word("        9V (3A)");
            break;
        case 3:
            LCD_word("        12V (3A)");
            break;
        case 4:
            LCD_word("        15V (3A)");
            break;
        case 5:
            LCD_word("        20V (3A)");
            break;
        case 6:
            LCD_word("    5V-20V (3A)");
            break;
        default:
            break;
    }
    if(select == 2){
        BM92A_source_PDO();
        if(mode_set == 1){
            BM92Asrc_regSet();  //Updates the registers with new SRC mode PDO
            BM92Asrc_commandSet();  //Does not affect Sink mode capabilities or mode change
        }

        select = 0;pushFlag = 0;
        displayMode();
    }


}
void sleepMenu()
{
    LCD_clearLine();
    LCD_command(0x01); // clear screen, move cursor home
    LCD_word("Sleep Timeout");
    LCD_enter();
    if(sleep_time < 1)sleep_time = 6;
    if(sleep_time > 6)sleep_time = 1;
    switch(sleep_time)
    {
        case 1:
            LCD_word("           30s");
            break;
        case 2:
            LCD_word("      1 minute");
            break;
        case 3:
            LCD_word("     2 minutes");
            break;
        case 4:
            LCD_word("     5 minutes");
            break;
        case 5:
            LCD_word("    10 minutes");
            break;
        case 6:
            LCD_word("     Always on");
            break;
        default:
            break;
    }
    if(select == 2){

        select = 0;pushFlag = 0;
        displayMode();
    }
}
void enableMenu(char mode, char enable){
    LCD_clearLine();
    LCD_command(0x01); // clear screen, move cursor home
    /*
    switch(mode){

    case 1:
        LCD_word("100W USB-PD");
        break;
    case 1:
        LCD_word("Battery Charge");
        break;

    case 1:
        LCD_word("UART Debug");
        break;

    default:
        break;
    }
    */
    LCD_word("UART Debug");

    LCD_enter();

    if(enable == 0)LCD_word("       Disabled");
    else if(enable == 1)LCD_word("        Enabled");
    if(select == 2){

//        if(mode == 1 && enable == 0){
//            sinkAllPDOMode();
//        }
//        if(mode == 2 && enable == 1){
//            if(mode_set == 0)chgEnable();
//            else if(mode_set ==1)
//            {
//                LCD_charge_error();
//                delay_ms(2000,CURRENT_FREQ);
//                batt_chg = 0;
//            }
//            delay_ms(100,CURRENT_FREQ);
//            clear_BD_int();
//            BD99954ReadRegister();
//        }
//        else if(mode == 2 && enable == 0){
//            if(mode_set == 0)chgDisable();
//            else if(mode_set ==1)reverseDisable();
//            delay_ms(100,CURRENT_FREQ);
//            clear_BD_int();
//            BD99954ReadRegister();
//
//        }
        if(mode == 3 && enable == 1)
        {
            uart_en = 1;
        }
        else if(mode == 3 && enable == 0)
        {
            uart_en = 0;
        }
        select = 0;
        pushFlag = 0;
        displayMode();

    }

}
void advancedMenu(){
    LCD_clearLine();
    LCD_command(0x01); // clear screen, move cursor home
    LCD_word("Advanced Menu");
    LCD_enter();
    LCD_word("     UART Debug");

    if(select){
        enableMenu(1,uart_en);
        pushFlag = 0;
    }



}
/*
void batterySelectMenu(){
    if(battery_sel < 1)battery_sel = 4;
    if(battery_sel > 4)battery_sel = 1;
    LCD_clearLine();
    LCD_command(0x01); // clear screen, move cursor home
    LCD_word("Battery Select");
    LCD_enter();

    switch(battery_sel)
    {
        case 1:
            LCD_word("      1s config");
            break;
        case 2:
            LCD_word("      2s config");
            break;
        case 3:
            LCD_word("      3s config");
            break;
        case 4:
            LCD_word("      4s config");
            break;
        default:
            break;
    }
   if(select == 2){
       select = 0;
       displayMode();
    }

}
*/
void menuScroll(char temp){
    signed char value;
    if(temp == 2)
    {
        value = -1;
    }
    else{
        value = 1;
    }
    if(select == 0)
    {
        switch(settings_menu){
            case 1:
                fast_set += value;
                break;
            case 2:
                standard_menu += value;
                break;
            default:
                break;
        }
    }
    if(select == 1)
    {
        if(settings_menu == 2)
        {
            switch(standard_menu){
                case 1:
                    sink_set += value;
                    break;
                case 2:
                    source_set += value;
                    break;
                case 3:
                    sleep_time += value;
                    break;
                case 4:
                    high100_mode ^= 1;
                    break;
                default:
                    break;
                }
        }
        else if(settings_menu == 3)
        {
            /*
            switch(advanced_menu){

                case 1:
                    batt_chg ^= 1;
                    break;
                case 2:
                    battery_sel += value;
                    break;

                case 1:
                    uart_en ^= 1;
                    break;
                default:
                    break;
                }
            */
            uart_en ^= 1;
        }
    }
    rightFlag = 0;
    leftFlag = 0;
}


