/*
 * lcd4.h
 *
 *  Created on: Nov 25, 2025
 *      Author: moral
 */
#include "stm32f4xx.h"
#ifndef SRC_LCD4_H_
#define SRC_LCD4_H_

void lcdSetup(GPIO_TypeDef *RSPORT,uint16_t RS,GPIO_TypeDef *EnPORT,uint16_t En,GPIO_TypeDef *D4PORT,uint16_t D4,GPIO_TypeDef *D5PORT,uint16_t D5,GPIO_TypeDef *D6PORT,uint16_t D6,GPIO_TypeDef *D7PORT,uint16_t D7 );
void lcdEnable();
void lcdWrite(uint8_t data);
void lcdWrite4Bit(uint8_t data);
void lcdCommand(uint8_t command);
void lcdInit();
void lcdChar(uint8_t ch);
void lcdString(char * string);
void lcdWriteInt(char * format, uint32_t number );
void lcdWriteFloat(char * format, double number );

void lcdSetCursor(uint8_t row, uint8_t col);

#define lcdClear		0x01
#define lcdCursorOFF	0x0C
#define lcdCursorON		0x0E
#define lcdCursorBlink	0x0F
#define lcdCursorHome	0x02
#define lcdShiftRight	0x1E
#define lcdShiftLeft	0x18
#define lcdOFF			0x08
#define lcdCursorLeft	0x10
#define lcdCursorRight	0x14

#endif /* SRC_LCD4_H_ */
