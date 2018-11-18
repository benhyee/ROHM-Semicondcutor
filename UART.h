#ifndef TERMINAL_H
#define TERMINAL_H

void terminal_init();
void terminal_transmitChar(char character);
void terminal_transmitWord(char *word);
void terminal_transmitInt(unsigned int input);
void terminal_transmitByte(unsigned char byte_value);
void terminal_transmitDouble(double input);
char terminal_receiveChar();
int terminal_receiveInt();
void terminal_transmitShortByte(unsigned short byte_short);
void terminal_transmitIntByte(unsigned int byte_int);


#endif
