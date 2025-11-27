/*
 * lcd4.c
 *
 *  Created on: Nov 25, 2025
 *      Author: moral
 */


/*
 * lcd8.c
 *	HD44780 character LCD library for 8 bit data mode
 *	The  Library Assumes All lines including data and control to be on the same GPIO PORT
 *	The RW Pin is Assumed to be tied to GND
 *  Created on: Jan 20, 2023
 *      Author: Amer Iqbal Qureshi
 *      Email: ameriqbalqureshi@yahoo.com
 */
#include "lcd4.h"
#include <stdio.h>
//Global variables for LCD
GPIO_TypeDef	*lcdRsPort;
uint16_t		lcdRsPin;
GPIO_TypeDef	*lcdEnPort;
uint16_t		lcdEnPin;
GPIO_TypeDef	*lcdDataPort[4];
uint16_t		lcdDataPin[4];

void lcdSetup(GPIO_TypeDef *RSPORT,uint16_t RS,GPIO_TypeDef *EnPORT,uint16_t En,GPIO_TypeDef *D4PORT,uint16_t D4,GPIO_TypeDef *D5PORT,uint16_t D5,GPIO_TypeDef *D6PORT,uint16_t D6,GPIO_TypeDef *D7PORT,uint16_t D7 )
{
// Assign Pin values to global variables
	lcdRsPort=RSPORT;
	lcdRsPin=RS;
	lcdEnPort = EnPORT;
	lcdEnPin=En;
	lcdDataPort[0] = D4PORT;
	lcdDataPin[0]=D4;
	lcdDataPort[1] = D5PORT;
	lcdDataPin[1]=D5;
	lcdDataPort[2] = D6PORT;
	lcdDataPin[2]=D6;
	lcdDataPort[3] = D7PORT;
	lcdDataPin[3]=D7;
}

void lcdEnable()
{
	// Give a small 0 to 1 and 1 to 0 pulse on Enable pin to transfer port data
	HAL_GPIO_WritePin(lcdEnPort, lcdEnPin, GPIO_PIN_SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(lcdEnPort, lcdEnPin, GPIO_PIN_RESET);

}

void lcdWrite(uint8_t data)
//	Writes one byte of data to the LCD Pins
{
	lcdWrite4Bit(data >> 4); 		//send upper nibble
	lcdWrite4Bit(data & 0x0F); 	//send lower nibble
}

void lcdWrite4Bit(uint8_t data)
//	Writes one byte of data to the LCD Pins
{
	for(int i=0;i<4;i++)
	{
		if(data & (1<<i))
			HAL_GPIO_WritePin(lcdDataPort[i], lcdDataPin[i], GPIO_PIN_SET);
		else
			HAL_GPIO_WritePin(lcdDataPort[i], lcdDataPin[i], GPIO_PIN_RESET);
	}
	lcdEnable();
}

void lcdCommand(uint8_t command)
{
//	Sends Command data to LCD
	HAL_GPIO_WritePin(lcdRsPort, lcdRsPin, GPIO_PIN_RESET);	// Set RS to 0 for commands
	lcdWrite(command);
}

void lcdInit()
{
	lcdCommand(0x02); 	//  0 0 1 DL N F — — 0x30= 0011 0000 DL=1 means 8 Bit Mode
	HAL_Delay(10);
	lcdCommand(0x02);
	HAL_Delay(10);
	lcdCommand(0x02);
	HAL_Delay(10);
	lcdCommand(0x28);	// 0 0 0 0 1 DL N F — — 0x28= 0010 1000 N=1 means 2 or more lines
	HAL_Delay(10);
	lcdCommand(0x0E);	// 0 0 0 0 1 D C B		0x0E= 0000 1110 D=1 Display ON, C=1 Cursor ON
	HAL_Delay(10);

}

void lcdChar(uint8_t ch)
{
//	Display One Byte of Data to LCD
	HAL_GPIO_WritePin(lcdRsPort, lcdRsPin, GPIO_PIN_SET);	// Set RS Pin 1 for Data
	lcdWrite(ch);
}

void lcdString(char * string)
{
//	Display a String of characters
	while(*string)
		lcdChar(*string++);
}

void lcdWriteInt(char * format, uint32_t number )
{
//	Display integer numbers format is standard c printf() like %d etc
	char buffer[20];
	sprintf(buffer,format,number);
	lcdString(buffer);
}

void lcdWriteFloat(char * format, double number )
{
//	Display a floating point number
	char buffer[20];
	sprintf(buffer,format,number);
	lcdString(buffer);
}

void lcdSetCursor(uint8_t row, uint8_t col)
{
//	Sets the cursor position
	HAL_GPIO_WritePin(lcdRsPort, lcdRsPin, GPIO_PIN_RESET);	// RS=0;
	switch (row)
	{
	case 0:
		lcdWrite(0x80+col); break;
	case 1:
		lcdWrite(0xC0+col); break;
	case 2:
		lcdWrite(0x80+0x14+col); break;
	case 3:
		lcdWrite(0xC0+0x14+col); break;
	}
}
