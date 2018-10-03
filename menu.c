/*
 * menu.c
 *
 *  Created on: Oct 2, 2018
 *      Author: User
 */

#include "interruptPins.h"
#include "LCD.h"
#define TRUE 1
#define FALSE 0
char upFlag = 0, downFlag = 0, cursorFlag = 0, volt_set_cnt = 0, current_config = 0;
char settings_menu_Count = 0,bat_setting = 1,sysToggle = FALSE;
char select = 0;

void displayMode()
{
    LCD_enter();
    LCD_clearLine();
    switch(settings_menu_Count)
    {
        case 0:
            LCD_word("Voltage Config");
            if(select == 1)
            {
                voltageDisplay();
            }
            break;
        case 1:
            LCD_word("Current Config");
            if(select == 1)
            {
                currentDisplay();
            }
            break;
        case 2:
            LCD_word("Battery Config");
            if(select == 1)
            {
                batteryDisplay();
            }
            break;
        case 3:
            LCD_word("Mode");
            break;

        case 4:
            LCD_word("Diagnostic");
            break;
        case 5:
            LCD_word("System toggle");
            break;


        default:
            break;

    }
    cursorFlag = 0;
}
void voltageDisplay()
{
    LCD_clearLine();
    LCD_word("Voltage Config");
    LCD_enter();
    LCD_clearLine();
    switch(volt_set_cnt)
    {
        case 0:
            LCD_word("   5V->");
            break;
        case 1:
            LCD_word("<- 9V->");
            break;
        case 2:
            LCD_word("<-12V->");
            break;
        case 3:
            LCD_word("<-15V->");
            break;
        case 4:
            LCD_word("<-20V  ");
            break;
        default:
            break;
    }
    if(select == 0)
    {
        displayMode();
    }

}
void batteryDisplay()
{
    LCD_clearLine();
    LCD_word("Battery Config");
    LCD_enter();
    LCD_clearLine();
    switch(volt_set_cnt)
    {
    case 1:
        LCD_word("  1s->");
        break;
    case 2:
        LCD_word("<-2s->");

        break;
    case 3:
        LCD_word("<-3s->");

        break;
    case 4:
        LCD_word("  4s->");
        break;
    default:
        break;
    }
    if(select == 0)
    {
        displayMode();
    }

}
void currentDisplay()
{
    LCD_clearLine();
    LCD_word("Current Config");
    LCD_enter();
    LCD_clearLine();
    switch(volt_set_cnt)
    {
    case 1:
        LCD_word("  3A->");
        break;
    case 2:
        LCD_word("<-5A");
        break;
    default:
        break;
    }
    if(select == 0)
    {
        displayMode();
    }
}
