/*
 * menu.c
 *
 *  Created on: Oct 2, 2018
 *      Author: User
 */

#include "interruptPins.h"
#include "LCD.h"
#include "globals.h"
#define TRUE 1
#define FALSE 0
char cursorFlag = 0;
char volt_set_cnt = 1, curr_set_cnt = 1, mode_set = FALSE;
char settings_menu_Count = 0,sysToggle = FALSE, diagnosticToggle = FALSE;
char select = 0, charge_enable = FALSE;

void PDDisplay();
void PowerDisplay();
void onOffDisplay(char toggle);




void displayMode()
{
    LCD_enter();
    LCD_clearLine();
    if(settings_menu_Count < 1)
    {
        settings_menu_Count  = 1;
    }

    else if(settings_menu_Count > 5)

    {
        settings_menu_Count = 5;
    }
    switch(settings_menu_Count)
    {
        case 1:
            LCD_word("PD Mode");
            if(select == 1)
            {
                PDDisplay();
            }
            break;
        case 2:
            LCD_word("Power Config");
            if(select == 1)
            {
                PowerDisplay();
            }
            break;
        case 3:
            LCD_word("Diagnostic");
            if(select == 1)
            {
                onOffDisplay(diagnosticToggle);
            }
            break;
        case 4:
            LCD_word("System toggle");
            if(select == 1)
            {
                onOffDisplay(sysToggle);

            }
            break;
        case 5:
            LCD_word("Charge Enable");
            if(select == 1)
            {
                onOffDisplay(charge_enable);
            }
            break;
        default:
            break;
    }
    cursorFlag = FALSE;
}
void PDDisplay()
{
    LCD_clearLine();
    LCD_word("PD Mode");
    LCD_enter();
    LCD_clearLine();
    switch(mode_set)
    {
        case 0:
            LCD_word("Sink Mode");
            break;
        case 1:
            LCD_word("Source Mode");
        default:
            break;
    }
    if(select == 0)
    {
        displayMode();
    }

}

void PowerDisplay()
{
    LCD_clearLine();
    LCD_word("Power Config");
    LCD_enter();
    LCD_clearLine();

    if(curr_set_cnt < 1)
    {
        curr_set_cnt = 1;
    }
    else if(curr_set_cnt > 3)
    {
        curr_set_cnt = 3;
    }
    if(volt_set_cnt < 1)
    {
        volt_set_cnt = 1;
    }
    else if(volt_set_cnt > 5)
    {
        volt_set_cnt = 5;
    }

    switch(volt_set_cnt)
    {
    case 1:
        LCD_word(" 5V ->");
        break;
    case 2:
        LCD_word("<-9V->");
        break;
    case 3:
        LCD_word("<-12V->");
        break;
    case 4:
        LCD_word("<-15V->");
        break;
    case 5:
        LCD_word("<-20V ");
        break;
    default:
        break;
    }
    LCD_word("at");
    switch(curr_set_cnt)
    {
    case 1:
        LCD_word(" 1.5A->");
        break;
    case 2:
        LCD_word("<-3A->");
        break;
    case 3:
        LCD_word("<-5A ");
        break;
    default:
        break;
    }
    if(select == FALSE)
    {
        displayMode();
    }
}
void onOffDisplay(char toggle)
{
    LCD_enter();
    LCD_clearLine();
    if(toggle == TRUE)
    {
        LCD_word("ON");
    }
    else{
        LCD_word("OFF");
    }
    if(select == FALSE)
    {
        displayMode();
    }

}
