/*
 * ILI9341.c
 *
 *  Created on: Dec 3, 2021
 *      Author: micha
 */


#include "main.h"
#include "ILI9341.h"

SPI_HandleTypeDef *Tft_hspi;

static void _ILI9341_Delay(uint32_t ms)
{
	HAL_Delay(ms);
}

static void _ILI9341_SendToTFT(uint8_t* Byte, uint32_t Length)
{
    while (Length > 0U)
    {
      /* Wait until TXE flag is set to send data */
      if (__HAL_SPI_GET_FLAG(Tft_hspi, SPI_FLAG_TXE))
      {
        *((__IO uint8_t *)&Tft_hspi->Instance->DR) = (*Byte);
       Byte ++;
       Length--;
      }
    }

    while(__HAL_SPI_GET_FLAG(Tft_hspi, SPI_FLAG_BSY) != RESET)
    {

    }
}

static void _ILI9341_SendCommand(uint8_t Command)
{
	// CS LOW
	ILI9341_CS_LOW;

	// SET DC Line to Low
	ILI9341_DC_LOW;

	// SPI SEND TO TFT 1 byte
	_ILI9341_SendToTFT(&Command, 1);

	// CS HIGH
	ILI9341_CS_HIGH;
}

static void _ILI9341_SendCommandAndData(uint8_t Command, uint8_t *Data, uint32_t Length)
{
	// CS LOW
	ILI9341_CS_LOW;

	// SET DC Line to Low
	ILI9341_DC_LOW;

	// SPI SEND TO TFT 1 byte
	_ILI9341_SendToTFT(&Command, 1);

	// SET DC Line to High
	ILI9341_DC_HIGH;

	// SPI SEND TO TFT "Length" byte
	_ILI9341_SendToTFT(Data, Length);

	// CS HIGH
	ILI9341_CS_HIGH;

}

static void _ILI9341_SendData16(uint16_t Data)
{
	ILI9341_CS_LOW; // Enable CS

	uint8_t tmp[2];\
	tmp[0] = (Data >> 8);
	tmp[1] = (Data & 0xFF);

	ILI9341_DC_HIGH; // data mode
	_ILI9341_SendToTFT(tmp, 2);
	ILI9341_CS_HIGH; // Disable CS
}

void ILI9341_SetAddrWindow(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h)
{
	// Calculate end ranges
  uint16_t x2 = (x1 + w - 1);
  uint16_t y2 = (y1 + h - 1);

  // Data buffer
  uint8_t DataToTransfer[4];

  //Fullfill X's-Data
  DataToTransfer[0] = (x1 >> 8);
  DataToTransfer[1] = (x1 & 0xFF);
  DataToTransfer[2] = (x2 >> 8);
  DataToTransfer[3] = (x2 & 0xFF);
  //Send X's
  _ILI9341_SendCommandAndData(ILI9341_CASET, DataToTransfer,4);

  //Fullfill Y's- Data
  DataToTransfer[0] = (y1 >> 8);
  DataToTransfer[1] = (y1 & 0xFF);
  DataToTransfer[2] = (y2 >> 8);
  DataToTransfer[3] = (y2 & 0xFF);
  //Send Y's
  _ILI9341_SendCommandAndData(ILI9341_PASET, DataToTransfer, 4);
}

void ILI9341_WritePixel(uint16_t x, uint16_t y, uint16_t color)
{
	// Data buffer
	uint8_t DataToTransfer[2];

	// Check X and Y are into Display
	if(((x >= 0) && (x < ILI9341_TFTWIDTH)) && ((y >= 0) && (y < ILI9341_TFTHEIGHT)))
	{
		DataToTransfer[0] = (color >> 8);
		DataToTransfer[1] = (color & 0xFF);

		ILI9341_SetAddrWindow(x, y, 1, 1);
		// Push color byte to RAM
		_ILI9341_SendCommandAndData(ILI9341_RAMWR, DataToTransfer, 2);
	}
}

void ILI9341_ClearDisplay(uint16_t color)
{
	uint32_t Length = ILI9341_TFTWIDTH * ILI9341_TFTHEIGHT;

	// Set Window as all Display;
	ILI9341_SetAddrWindow(0, 0, ILI9341_TFTWIDTH, ILI9341_TFTHEIGHT);

	// Push to RAM
	_ILI9341_SendCommand(ILI9341_RAMWR);

	// Enable CS
	ILI9341_CS_LOW;

	// Data mode
	ILI9341_DC_HIGH;

	 while (Length > 0U)
	    {
	      /* Wait until TXE flag is set to send data */
	      if (__HAL_SPI_GET_FLAG(Tft_hspi, SPI_FLAG_TXE))
	      {
	        *((__IO uint8_t *)&Tft_hspi->Instance->DR) = (color >> 8);
	        while(__HAL_SPI_GET_FLAG(Tft_hspi, SPI_FLAG_TXE) != SET)
	        {}
	        *((__IO uint8_t *)&Tft_hspi->Instance->DR) = (color & 0xFF);
	        // Decrease length
	       Length--;
	      }
	    }

	    while(__HAL_SPI_GET_FLAG(Tft_hspi, SPI_FLAG_BSY) != RESET)
	    {

	    }

	 ILI9341_CS_HIGH;
}

static void _ILI9341_SetRotation(uint8_t rotation)
{
	uint8_t data = 0;

	if(rotation <= 3) // can't be higher than 3
	{
		  switch (rotation) {
		  case 0:
			data = (ILI9341_MADCTL_MX | ILI9341_MADCTL_BGR);
		    break;

		  case 1:
			data = (ILI9341_MADCTL_MV | ILI9341_MADCTL_BGR);
		    break;

		  case 2:
			data = (ILI9341_MADCTL_MY | ILI9341_MADCTL_BGR);
		    break;

		  case 3:
			data = (ILI9341_MADCTL_MX | ILI9341_MADCTL_MY | ILI9341_MADCTL_MV | ILI9341_MADCTL_BGR);
		    break;
		  }
	}
  _ILI9341_SendCommandAndData(ILI9341_MADCTL, &data, 1);
}

static const uint8_t initcmd[] = {
  0xEF, 3, 0x03, 0x80, 0x02,
  0xCF, 3, 0x00, 0xC1, 0x30,
  0xED, 4, 0x64, 0x03, 0x12, 0x81,
  0xE8, 3, 0x85, 0x00, 0x78,
  0xCB, 5, 0x39, 0x2C, 0x00, 0x34, 0x02,
  0xF7, 1, 0x20,
  0xEA, 2, 0x00, 0x00,
  ILI9341_PWCTR1  , 1, 0x23,             // Power control VRH[5:0]
  ILI9341_PWCTR2  , 1, 0x10,             // Power control SAP[2:0];BT[3:0]
  ILI9341_VMCTR1  , 2, 0x3e, 0x28,       // VCM control
  ILI9341_VMCTR2  , 1, 0x86,             // VCM control2
  ILI9341_MADCTL  , 1, 0x48,             // Memory Access Control
  ILI9341_VSCRSADD, 1, 0x00,             // Vertical scroll zero
  ILI9341_PIXFMT  , 1, 0x55,
  ILI9341_FRMCTR1 , 2, 0x00, 0x18,
  ILI9341_DFUNCTR , 3, 0x08, 0x82, 0x27, // Display Function Control
  0xF2, 1, 0x00,                         // 3Gamma Function Disable
  ILI9341_GAMMASET , 1, 0x01,             // Gamma curve selected
  ILI9341_GMCTRP1 , 15, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, // Set Gamma
    0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00,
  ILI9341_GMCTRN1 , 15, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, // Set Gamma
    0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F,
  ILI9341_SLPOUT  , 0x80,                // Exit Sleep
  ILI9341_DISPON  , 0x80,                // Display on
  0x00                                   // End of list
};

void ILI9341_Init(SPI_HandleTypeDef *hspi)
{
	Tft_hspi = hspi;
	uint8_t cmd, x, numArgs;
	const uint8_t *addr = initcmd;

    __HAL_SPI_ENABLE(hspi);

#if (ILI9341_USE_HW_RESET == 1)

	ILI9341_RST_LOW;
	_ILI9341_Delay(20);
	ILI9341_RST_HIGH;
	_ILI9341_Delay(20);
#else
	_ILI9341_SendCommand(ILI9341_SWRESET);
	_ILI9341_Delay(150);
#endif

	  while ((cmd = *(addr++)) > 0)
	  {
	    x = *(addr++);
	    numArgs = x & 0x7F;

	    _ILI9341_SendCommandAndData(cmd, (uint8_t*)addr, numArgs);
	    addr += numArgs;

	    if (x & 0x80)
	    {
		    _ILI9341_Delay(150);
	    }
	  }

	  _ILI9341_SetRotation(ILI9341_ROTATION);
}
