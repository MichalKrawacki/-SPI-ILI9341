/*
 * GFX_Font.h
 *
 *  Created on: 14 gru 2021
 *      Author: micha
 */

#include "main.h"


#ifndef INC_GFX_FONT_H_
#define INC_GFX_FONT_H_

typedef struct {
	uint8_t Width;
	uint16_t CharOffset;
}FONT_CHAR_INFO;

typedef struct {
	uint8_t Height;
	uint8_t StartChar;
	uint8_t EndChar;
	uint8_t Gap;
	const FONT_CHAR_INFO *CharDescriptor;
	const uint8_t *CharBitmap;
}FONT_INFO;


void GFX_EF_SetFont(const FONT_INFO *Font);
uint8_t GFX_EF_GetHeight(void);
uint8_t GFX_EF_GetGap(void);
uint8_t GFX_EF_GetStartChar(void);
uint8_t GFX_EF_GetEndChar(void);


#endif /* INC_GFX_FONT_H_ */
