#include <string.h>
#include <UART.h>
#include "msp.h"

#define TRUE 1
#define FALSE 0
//counts the number of bytes read into the buffer
char readCount = 0;

char terminalbufferReady;
int  terminalbuffer;
unsigned int byte;

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
    terminal_transmitChar(((input / 10) % 10) + 48);
    terminal_transmitChar((input % 10) + 48);
    //terminal_transmitChar('\r');
    //terminal_transmitChar('\n');
}

void terminal_transmitByte(unsigned int byte_value)
{

    switch(byte_value)
    {
    case 0:
       terminal_transmitWord("0");
       break;
    case 1:
       terminal_transmitWord("1");
       break;
    case 2:
       terminal_transmitWord("2");
       break;
    case 3:
       terminal_transmitWord("3");
       break;
    case 4:
       terminal_transmitWord("4");
       break;
    case 5:
       terminal_transmitWord("5");
       break;
    case 6:
       terminal_transmitWord("6");
       break;
    case 7:
       terminal_transmitWord("7");
       break;
    case 8:
       terminal_transmitWord("8");
       break;
    case 9:
       terminal_transmitWord("9");
       break;
    case 10:
       terminal_transmitWord("A");
       break;
    case 11:
       terminal_transmitWord("B");
       break;
    case 12:
       terminal_transmitWord("C");
       break;
    case 13:
       terminal_transmitWord("D");
       break;
    case 14:
       terminal_transmitWord("E");
       break;
    case 15:
       terminal_transmitWord("F");
       break;
    default:
       break;
    }
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
