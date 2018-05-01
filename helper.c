/*
 * helper.c
 *
 *  Created on: Apr 25, 2018
 *      Author: User
 */



unsigned char char_Hold;
unsigned short shortData;
unsigned int intData;
int i;

unsigned short two_byteOrg(unsigned char* dataArray)
{
    i = 0;
    shortData = 0;
    for(i = 0; i < 2; i++)
    {
        char_Hold = *dataArray&0xFF;
        shortData = (char_Hold << (8*i)) | shortData;
        *dataArray++;
    }
    return shortData;
}
unsigned int four_byteOrg(unsigned char* dataArray)
{
    i = 0;
    intData = 0;
    for(i = 0; i < 4; i++)
    {
        char_Hold = *dataArray&0xFF;
        intData = (char_Hold << (8*i)) | intData;
        *dataArray++;

    }
    return intData;
}

