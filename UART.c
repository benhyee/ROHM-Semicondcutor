#include <string.h>
#include <UART.h>
#include "msp.h"

#define TRUE 1
#define FALSE 0
//counts the number of bytes read into the buffer
char readCount = 0;

char terminalbufferReady;
int  terminalbuffer;

void terminal_init()
{
    EUSCI_A0->CTLW0 |= 1;
    EUSCI_A0->MCTLW = 0;
    EUSCI_A0->CTLW0 = 0x0081;
    EUSCI_A0->BRW = 26;
    P1->SEL0 |= 0x0C;
    P1->SEL1 &= ~0x0C;
    EUSCI_A0->CTLW0 &= ~1;
    EUSCI_A0->IE |= 1;

    NVIC_SetPriority(EUSCIA0_IRQn, 4);
    NVIC_EnableIRQ(EUSCIA0_IRQn);
}

void terminal_transmitChar(char character)
{
    while(!(EUSCI_A0->IFG & 0x02)) { }
    EUSCI_A0->TXBUF = character;
}

void terminal_transmitWord(char *word)
{
    int i;
    for (i = 0; i < strlen(word); i++)
    {
        terminal_transmitChar(word[i]);
    }
}

void terminal_transmitInt(unsigned int input)
{
    if(input / 10000 != 0)
    {
        terminal_transmitChar((input / 10000) + 48);
    }
    terminal_transmitChar(((input / 1000) % 10) + 48);
    terminal_transmitChar('.');

    terminal_transmitChar((input / 100)%10 + 48);


//    terminal_transmitChar(((input / 100) % 10) + 48);
//    terminal_transmitChar(((input / 10) % 10) + 48);
//    terminal_transmitChar((input % 10) + 48);
    terminal_transmitChar('\r');
    //terminal_transmitChar('\n');
}

void terminal_transmitDouble(double input)
{
    if ( input < 0)
    {
        input = 0;
    }
        terminal_transmitChar(((int) (input/10000)%10) + 48);
        terminal_transmitChar(((int) (input/1000)%10) + 48);
        terminal_transmitChar(((int) (input/100)%10) + 48);
        terminal_transmitChar(((int) (input/10)%10) + 48);
        terminal_transmitChar(((int) (input)%10) + 48);

        terminal_transmitChar('V');
        terminal_transmitChar('\r');
        terminal_transmitChar('\n');

}

int terminal_receiveInt()
{
    int result = terminalbuffer;
    terminalbuffer = 0;
    terminalbufferReady = FALSE;
    readCount = 0;
    return result;
}

void EUSCIA0_IRQHandler()
{
    int result = EUSCI_A0->RXBUF;
    terminal_transmitChar((char)result);

    if (result == 13)
    {
        terminalbufferReady = TRUE;
        return;
    }
    if (result < 48 || result > 57)
    {
        return;
    }

    terminalbuffer = (terminalbuffer * 10) + (result - 48);

    readCount++;
}
