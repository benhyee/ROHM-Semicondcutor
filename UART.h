#ifndef TERMINAL_H
#define TERMINAL_H

void terminal_init();
void terminal_transmitChar(char character);
void terminal_transmitWord(char *word);
void terminal_transmitInt(int input);
void terminal_transmitDouble(double input);
char terminal_receiveChar();
int terminal_receiveInt();

#endif