/*
 * GFX_Font.c
 *
 *  Created on: 14 gru 2021
 *      Author: micha
 */

#include "FontArial/ArialBlack_20pts_Bold.h"
#include "ILI9341.h"
#include "GFX_TFT.h"


const FONT_INFO * CurrentFont;

void GFX_EF_SetFont(const FONT_INFO *Font)
{
	CurrentFont = Font;
}

uint8_t GFX_EF_GetHeight(void)
{
	return CurrentFont ->Height;
}

uint8_t GFX_EF_GetGap(void)
{
	return CurrentFont -> Gap;
}

uint8_t GFX_EF_GetStartChar(void)
{
	return CurrentFont -> StartChar;
}

uint8_t GFX_EF_GetEndChar(void)
{
	return CurrentFont -> EndChar;
}
