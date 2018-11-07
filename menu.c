/*
 * menu.c
 *
 *  Created on: Oct 2, 2018
 *      Author: User
 */

#include "interruptPins.h"
#include "LCD.h"
#include "globals.h"
#include "menu.h"
#include "I2C_Helper.h"
#include "debugFunctions.h"
#include "BD99954_Funcs.h"
#include "BM92A_Funcs.h"
#define TRUE 1
#define FALSE 0
#define BD99954_ADDRESS 0x09
#define BM92A_ADDRESS 0x18


unsigned short readTwoByte(unsigned char commandCode,unsigned char slaveAddr);

char cursorFlag = 0;
char settings_menu = 1, standard_menu = 1,advanced_menu =1;
char lock_fast = 1, lock_standard = 0, fast_set = 1;
char sink_set = 0, source_set = 0, sleep_time = 6, high100_mode = 0;
char batt_chg = 0, battery_sel = 0, uart_en = 0;
char lock_sink = 0,lock_source = 0, lock_high = 0, lock_sleep = 0;
unsigned short generalShort;
unsigned int generalInt,temp;
void displayMode()
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
   if(fast_set < 1)fast_set = 1;
   if(fast_set > 2)fast_set = 2;
   LCD_command(0x01); // clear screen, move cursor home
   if(fast_set == 1)
   {
       LCD_word("Ready to Sink");
       LCD_enter();
       LCD_clearLine();
       LCD_word("5V-20V (3A)");
   }
   else if(fast_set == 2){
       LCD_word("Ready to Source");
       LCD_enter();
       LCD_clearLine();
       LCD_word("5V-20V (3A)");
   }


   if(select)
   {
       lock_fast = fast_set;
       if(lock_fast == 1)defaultAllRangeMode();
       select = 0;
   }

}
void standardMenu(){
    if(standard_menu > 4)standard_menu = 4;
    if(standard_menu < 1)standard_menu = 1;
    LCD_clearLine();
    LCD_command(0x01); // clear screen, move cursor home
    LCD_word("Standard Menu");
    LCD_enter();
    switch(standard_menu)
    {
        case 1:
            if(select) sinkPDOMenu();
            LCD_word("       Sink PDO");

            break;
        case 2:
            if(select) sourcePDOMenu();
            LCD_word("     Source PDO");

            break;
        case 3:
            if(select) sleepMenu();
            LCD_word("  Sleep Timeout");
            break;
        case 4:
            if(select) enableMenu(1,high100_mode);
            LCD_word("   100W USB-PD");
            break;
        default:
            break;

    }


}
void sinkPDOMenu(){
    LCD_clearLine();
    LCD_command(0x01); // clear screen, move cursor home
    LCD_word("Sink PDO");
    LCD_enter();
    if(sink_set < 1)sink_set = 1;
    if(sink_set > 6)sink_set = 6;
    switch(sink_set)
    {
        case 1:
            LCD_word("    5V-20V (3A)");
            break;
        case 2:
            LCD_word("    5V-15V (3A)");
            break;
        case 3:
            LCD_word("        5V (3A)");
            break;
        case 4:
            LCD_word("        9V (3A)");
            break;
        case 5:
            LCD_word("        15V (2A)");
            break;
        case 6:
            LCD_word("        20V (3A)");
            break;
        default:
            break;
    }
    if(select == 2){
        BM92A_sink_PDO();
        select = 0;
        displayMode();
    }


}
void sourcePDOMenu(){
    LCD_clearLine();
    LCD_command(0x01); // clear screen, move cursor home
    LCD_word("Source PDO");
    LCD_enter();
    if(source_set < 1)source_set = 1;
    if(source_set > 6)source_set = 6;
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
        select = 0;
        displayMode();
    }


}
void sleepMenu()
{
    LCD_clearLine();
    LCD_command(0x01); // clear screen, move cursor home
    LCD_word("Sleep Timeout");
    LCD_enter();
    if(sleep_time < 1)sleep_time = 1;
    if(sleep_time > 6)sleep_time = 6;
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

        select = 0;
        displayMode();
    }
}
void enableMenu(char mode, char enable){
    LCD_clearLine();
    LCD_command(0x01); // clear screen, move cursor home
    switch(mode){
    case 1:
        LCD_word("100W USB-PD");
        break;
    case 2:
        LCD_word("Battery Charge");
        break;
    case 3:
        LCD_word("UART Debug");
        break;
    default:
        break;
    }
    LCD_enter();

    if(enable == 0)LCD_word("       Disabled");
    else if(enable == 1)LCD_word("        Enabled");
    if(select == 2){
        select = 0;
        if(mode == 2 && enable == 1){
            generalShort = readTwoByte(0x0C,BD99954_ADDRESS);
            chgEnable();
            generalShort = readTwoByte(0x0C,BD99954_ADDRESS);
            BD99954ReadRegister();
        }
        else if(mode == 2 && enable == 0){
            generalShort = readTwoByte(0x0C,BD99954_ADDRESS);
            chgDisable();
            BD99954ReadRegister();
        }
        else if(mode == 1 && enable == 1){
            pdo100WMode();
        }
        else if(mode == 1 && enable == 0){
            defaultAllRangeMode();
        }
        displayMode();

    }

}
void advancedMenu(){
    if(advanced_menu > 3)advanced_menu = 3;
    if(advanced_menu < 1)advanced_menu = 1;
    LCD_clearLine();
    LCD_command(0x01); // clear screen, move cursor home
    LCD_word("Advanced Menu");
    LCD_enter();
    switch(advanced_menu)
    {
        case 1:
            if(select) enableMenu(2,batt_chg);
            LCD_word("  Battery Charge");

            break;
        case 2:
            if(select) batterySelectMenu();
            LCD_word("  Battery Select");
            break;
        case 3:
            if(select) enableMenu(3,uart_en);
            LCD_word("     UART Debug");

            break;
        default:
            break;
    }
}
void batterySelectMenu(){
    if(battery_sel < 1)battery_sel = 1;
    if(battery_sel > 4)battery_sel = 4;
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
void menuScroll(char value){

    if(select == 0)
    {
        switch(settings_menu){
            case 1:
                fast_set += value;
                break;
            case 2:
                standard_menu += value;
                break;
            case 3:
                advanced_menu += value;
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
            switch(advanced_menu){
                case 1:
                    batt_chg ^= 1;
                    break;
                case 2:
                    battery_sel += value;
                    break;
                case 3:
                    uart_en ^= 1;
                    break;
                default:
                    break;
                }

        }
    }
    rightFlag = 0;
    leftFlag = 0;
}


