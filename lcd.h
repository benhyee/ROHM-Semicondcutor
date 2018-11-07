#ifndef LCD_H
#define LCD_H

void LCD_init();
void LCD_nibble_write(unsigned char data, unsigned char control);
void LCD_command(unsigned char command);
void LCD_enter();
void LCD_data(unsigned char data);
void LCD_word(char *word);
void LCD_Voltage(unsigned short voltage);
void LCD_clearLine();
void LCD_PDO(unsigned short voltage, unsigned short current);
#endif
