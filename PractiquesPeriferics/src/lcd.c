/*
 * lcd.c
 *
 *  Created on: 18 abr. 2024
 *      Author: angel
 */

#include "lcd.h"

/* ----- LCD LIBRARY -----*/

// Draw a pixel in col x fila with value ARGB
/*RetSt SetPixel(uint16_t col, uint16_t fila, uint8_t alfa, uint8_t Rval, uint8_t Gval, uint8_t Bval){
	if (col > MAX_COL || fila > MAX_FILA || col < 0 || fila < 0){
		return NO_OK;
	}

    uint32_t color = (alfa << 24) | (Rval << 16) | (Gval << 8) | Bval;
	uint32_t pixel_adr = SDRAM_BANK_ADDR + LAYER_NUM * BUFFER_OFFSET + 4 * ((MAX_FILA + 1) * (MAX_COL + 1 - col) + fila);
    *(uint32_t __IO*) pixel_adr = color;

    return OK;
}*/
#define PIXEL_DECODE(A, R, G, B) ((A << 24) | (R << 16) | (G << 8) | B)

RetSt  __attribute__((optimize("-O2"))) SetPixel (uint16_t col, uint16_t row, uint8_t alpha, uint8_t Rval, uint8_t Gval, uint8_t Bval ) {
	uint32_t color;

	if (col < 0 || col > LCD_PIXEL_HEIGHT || row < 0 || row > LCD_PIXEL_WIDTH) {
		return NO_OK;
	}

	col = 320 - col;

	color = PIXEL_DECODE(alpha, Rval, Gval, Bval);

	if (LAYER_NUM == 0){
		*(__IO uint32_t *) (SDRAM_BANK_ADDR + LAYER_NUM * BUFFER_OFFSET + 4*(row + (LCD_PIXEL_WIDTH*col))) = color;
	} else {
		*(__IO uint32_t *) (SDRAM_BANK_ADDR + BUFFER_OFFSET + 4*(row + (LCD_PIXEL_WIDTH*col))) = color;
	}

	return OK;
}

// Get the value ARGB of the pixel col x line
uint32_t GetPixel(uint16_t col, uint16_t fila){
	if (col > MAX_COL || fila > MAX_FILA || col < 0 || fila < 0){
		return 0;
	}

	uint32_t pixel_adr = SDRAM_BANK_ADDR + LAYER_NUM * BUFFER_OFFSET + 4 * ((MAX_FILA + 1) * fila + col);
	return *(__IO uint32_t *) pixel_adr;
}

// Draws an horizontal line from col_inici to col_fi in the correspoding fila and ARDB values
RetSt DibuixaLiniaHoritzontal(uint16_t col_inici, uint16_t col_fi, uint16_t fila, uint8_t alfa, uint8_t Rval, uint8_t Gval, uint8_t Bval){
	if (col_inici < 0 || col_fi < 0 || fila < 0 || col_inici > MAX_COL || col_fi > MAX_COL || fila > MAX_FILA){
		return NO_OK;
	}

	for (int i = col_inici; i <= col_fi; i++){
		SetPixel(i, fila, alfa, Rval, Gval, Bval);
	}

	return OK;
}

// Draws an vertical line from fila_inici to fila_fi in the correspoding column and ARDB values
RetSt DibuixaLiniaVertical(uint16_t col, uint16_t fila_inici, uint16_t fila_fi, uint8_t alfa, uint8_t Rval, uint8_t Gval, uint8_t Bval){
	if (col < 0 || fila_inici < 0 || fila_fi < 0 || col > MAX_COL || fila_inici > MAX_FILA || fila_fi > MAX_FILA){
		return NO_OK;
	}

	for (int i = fila_inici; i <= fila_fi; i++){
		SetPixel(col, i, alfa, Rval, Gval, Bval);
	}

	return OK;
}

// Draws a circumference in col x fila with radi radious and ARGB values
RetSt DibuixaCircumferencia(uint16_t ccol, uint16_t cfila, uint16_t radi, uint8_t  alfa, uint8_t Rval, uint8_t Gval, uint8_t Bval){
	int x = 0; 
	int y = radi; 
	int d = 3 - (radi << 1); 

	if (radi <= 0 || ccol < 0 || cfila < 0 || ccol > MAX_COL || cfila > MAX_FILA){
		return NO_OK;
	}

	while(x <= y) {
		SetPixel(ccol + x, cfila + y, alfa, Rval, Gval, Bval);
		SetPixel(ccol - x, cfila + y, alfa, Rval, Gval, Bval);
		SetPixel(ccol + x, cfila - y, alfa, Rval, Gval, Bval);
		SetPixel(ccol - x, cfila - y, alfa, Rval, Gval, Bval);
		SetPixel(ccol + y, cfila + x, alfa, Rval, Gval, Bval);
		SetPixel(ccol - y, cfila + x, alfa, Rval, Gval, Bval);
		SetPixel(ccol + y, cfila - x, alfa, Rval, Gval, Bval);
		SetPixel(ccol - y, cfila - x, alfa, Rval, Gval, Bval);
		if(d < 0){
			d += (x << 2) + 6;
		}else{
			d += ((x - y) << 2) + 10;
			y--;
		}
		x++;
	}

	return OK;
}

// Draws a 3x3 bitmap col x fila being the center of the bitmap
RetSt DibuixaBitmap(uint16_t col, uint16_t fila, uint8_t alfa, uint8_t Rval, uint8_t Gval, uint8_t Bval){
	if (col < 0 || fila < 0 || col > MAX_COL || fila > MAX_FILA){
		return NO_OK;
	}

	SetPixel(col - 1, fila - 1, alfa, Rval, Gval, Bval);
	SetPixel(col, fila - 1, alfa, Rval, Gval, Bval);
	SetPixel(col + 1, fila - 1, alfa, Rval, Gval, Bval);
	SetPixel(col - 1, fila, alfa, Rval, Gval, Bval);
	SetPixel(col, fila, alfa, Rval, Gval, Bval);
	SetPixel(col + 1, fila, alfa, Rval, Gval, Bval);
	SetPixel(col - 1, fila + 1, alfa, Rval, Gval, Bval);
	SetPixel(col, fila + 1, alfa, Rval, Gval, Bval);
	SetPixel(col + 1, fila + 1, alfa, Rval, Gval, Bval);

	return OK;
}

// Draws all the layers of the screen with the specified RGB values
RetSt EsborraPantalla(uint8_t Rval, uint8_t Gval, uint8_t Bval){
	for(int i = 0; i <= MAX_COL + 1; i++){
		for (int j = 0; j <= MAX_FILA + 1; j++){
			SetPixel(i, j, 0x00, Rval, Gval, Bval);
		}
	}
	
	return OK;
}


/* ----- END LIBRARY  -----*/

// Inits the LCD
/*void init_LCD(void){
	LCD_Init();
	RCC_PLLSAIConfig(256, 0, 2);

	LCD_LayerInit();
	LTDC_LayerPixelFormat(LTDC_Layer1, LTDC_Pixelformat_ARGB8888);
	LTDC_LayerPixelFormat(LTDC_Layer2, LTDC_Pixelformat_ARGB8888);
	LTDC_ReloadConfig(LTDC_IMReload);

	LTDC_Cmd(ENABLE);
	FMC_SDRAMWriteProtectionConfig(FMC_Bank2_SDRAM, DISABLE);
}*/

// Inits the LCD
void init_LCD(void){
	// init
	LTDC_InitTypeDef       LTDC_InitStruct;
	GPIO_InitTypeDef       GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	LCD_CtrlLinesConfig();
	LCD_ChipSelect(DISABLE);
	LCD_ChipSelect(ENABLE);
	LCD_SPIConfig();
	LCD_PowerOn();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_LTDC, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2D, ENABLE);

	// PINS
	GPIO_InitTypeDef GPIO_InitStruct;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | \
						 RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD | \
						 RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOG, ENABLE);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_LTDC);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource4, GPIO_AF_LTDC);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_LTDC);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_LTDC);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_LTDC);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_6 | \
							 GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource0, 0x09);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource1, 0x09);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_LTDC);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_LTDC);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_LTDC);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_LTDC);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8 | \
							 GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_LTDC);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_LTDC);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_LTDC);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_10;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource3, GPIO_AF_LTDC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_LTDC);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_6;
	GPIO_Init(GPIOD, &GPIO_InitStruct);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource10, GPIO_AF_LTDC);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOF, &GPIO_InitStruct);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource6, GPIO_AF_LTDC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource7, GPIO_AF_LTDC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource10, 0x09);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource11, GPIO_AF_LTDC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource12, 0x09);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_10 | \
							 GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_Init(GPIOG, &GPIO_InitStruct);
	// FI PINS
	SDRAM_Init();
	LTDC_InitStruct.LTDC_HSPolarity = LTDC_HSPolarity_AL;
	LTDC_InitStruct.LTDC_VSPolarity = LTDC_VSPolarity_AL;
	LTDC_InitStruct.LTDC_DEPolarity = LTDC_DEPolarity_AL;
	LTDC_InitStruct.LTDC_PCPolarity = LTDC_PCPolarity_IPC;
	LTDC_InitStruct.LTDC_BackgroundRedValue = 0;
	LTDC_InitStruct.LTDC_BackgroundGreenValue = 0;
	LTDC_InitStruct.LTDC_BackgroundBlueValue = 0;
	RCC_PLLSAIConfig(256, 0, 2); // MODIFICAT RESPECTE A LA FUNCI� ORIGINAL
	RCC_LTDCCLKDivConfig(RCC_PLLSAIDivR_Div8);
	RCC_PLLSAICmd(ENABLE);
	while(RCC_GetFlagStatus(RCC_FLAG_PLLSAIRDY) == RESET)
	{
	}
	LTDC_InitStruct.LTDC_HorizontalSync = 9;
	LTDC_InitStruct.LTDC_VerticalSync = 1;
	LTDC_InitStruct.LTDC_AccumulatedHBP = 29;
	LTDC_InitStruct.LTDC_AccumulatedVBP = 3;
	LTDC_InitStruct.LTDC_AccumulatedActiveW = 269;
	LTDC_InitStruct.LTDC_AccumulatedActiveH = 323;
	LTDC_InitStruct.LTDC_TotalWidth = 279;
	LTDC_InitStruct.LTDC_TotalHeigh = 327;
	LTDC_Init(&LTDC_InitStruct);

	// LAYER
	LTDC_Layer_InitTypeDef LTDC_Layer_InitStruct;

	LTDC_Layer_InitStruct.LTDC_HorizontalStart = 30;
	LTDC_Layer_InitStruct.LTDC_HorizontalStop = (LCD_PIXEL_WIDTH + 30 - 1);
	LTDC_Layer_InitStruct.LTDC_VerticalStart = 4;
	LTDC_Layer_InitStruct.LTDC_VerticalStop = (LCD_PIXEL_HEIGHT + 4 - 1);
	LTDC_Layer_InitStruct.LTDC_PixelFormat = LTDC_Pixelformat_ARGB8888;
	LTDC_Layer_InitStruct.LTDC_ConstantAlpha = 255;
	LTDC_Layer_InitStruct.LTDC_DefaultColorBlue = 0;
	LTDC_Layer_InitStruct.LTDC_DefaultColorGreen = 0;
	LTDC_Layer_InitStruct.LTDC_DefaultColorRed = 0;
	LTDC_Layer_InitStruct.LTDC_DefaultColorAlpha = 0;
	LTDC_Layer_InitStruct.LTDC_BlendingFactor_1 = LTDC_BlendingFactor1_CA;
	LTDC_Layer_InitStruct.LTDC_BlendingFactor_2 = LTDC_BlendingFactor2_CA;
	LTDC_Layer_InitStruct.LTDC_CFBLineLength = ((LCD_PIXEL_WIDTH * 4) + 3);
	LTDC_Layer_InitStruct.LTDC_CFBPitch = (LCD_PIXEL_WIDTH * 4);
	LTDC_Layer_InitStruct.LTDC_CFBLineNumber = LCD_PIXEL_HEIGHT;
	LTDC_Layer_InitStruct.LTDC_CFBStartAdress = LCD_FRAME_BUFFER;
	LTDC_LayerInit(LTDC_Layer1, &LTDC_Layer_InitStruct);

	LTDC_Layer_InitStruct.LTDC_CFBStartAdress = LCD_FRAME_BUFFER + BUFFER_OFFSET;
	LTDC_Layer_InitStruct.LTDC_BlendingFactor_1 = LTDC_BlendingFactor1_PAxCA;
	LTDC_Layer_InitStruct.LTDC_BlendingFactor_2 = LTDC_BlendingFactor2_PAxCA;
	LTDC_LayerInit(LTDC_Layer2, &LTDC_Layer_InitStruct);

	LTDC_ReloadConfig(LTDC_IMReload);
	LTDC_LayerCmd(LTDC_Layer1, ENABLE);
	LTDC_LayerCmd(LTDC_Layer2, ENABLE);
	LTDC_ReloadConfig(LTDC_IMReload);
	LTDC_DitherCmd(ENABLE);

	LTDC_Cmd(ENABLE);

	// de la 0xD000 0000 fins a la 0xDFFF FFFF
	FMC_SDRAMWriteProtectionConfig(FMC_Bank2_SDRAM, DISABLE);
}

void draw_layer1(void){
	LAYER_NUM = 1;
	DibuixaLiniaVertical(4, 35, 236, 0x0F, 0, 0, 0);
	DibuixaLiniaVertical(86, 35, 236, 0x0F, 0, 0, 0);
	DibuixaLiniaVertical(45, 35, 236, 0x0F, 0, 0, 0);
	DibuixaLiniaHoritzontal(4, 86, 35, 0x0F, 0, 0, 0);
	DibuixaLiniaHoritzontal(4, 86, 236, 0x0F, 0, 0, 0);

	DibuixaLiniaHoritzontal(101, 302, 236, 0x0F, 0, 0, 0);
	DibuixaLiniaHoritzontal(101, 302, 154, 0x0F, 0, 0, 0);
	DibuixaLiniaHoritzontal(101, 302, 195, 0x0F, 0, 0, 0);
	DibuixaLiniaVertical(101, 154, 236, 0x0F, 0, 0, 0);
	DibuixaLiniaVertical(302, 154, 236, 0x0F, 0, 0, 0);

	DibuixaLiniaVertical(200, 8, 144, 0x0F, 0, 0, 0);
	DibuixaLiniaHoritzontal(132, 268, 76, 0x0F, 0, 0, 0);
	DibuixaCircumferencia(200, 76, 15, 0x0F, 0, 0, 0);
	DibuixaCircumferencia(200, 76, 30, 0x0F, 0, 0, 0);
	DibuixaCircumferencia(200, 76, 45, 0x0F, 0, 0, 0);
	DibuixaCircumferencia(200, 76, 60, 0x0F, 0, 0, 0);

}

void draw_layer2(void){
	float avg_acceleration_X = 0;
	float avg_acceleration_Y = 0;
	int col;
	int fila;

	for(int i = 0; i < 10; i++){
		avg_acceleration_X += acceleration_X_samples[i];
		avg_acceleration_Y += acceleration_Y_samples[i];
	}

	avg_acceleration_X = avg_acceleration_X / 10;
	avg_acceleration_Y = avg_acceleration_Y / 10;




	//SetPixel(pointer_to_paint, pointer_to_paint, 0x0F, 0, 0, 0);
	//SetPixel(pointer_to_paint, pointer_to_paint, 0x0F, 0, 0, 0);

	// Pixen in Acceleration Y will be between column 4 and 86 depending on the value with a step of (8/82) and in row pointer_to_paint + 35
	// Paint it red
	// But first, delete the previous pixel painted deleting the horizontal line in the row pointer_to_paint + 35
	// Paint it white
	//DibuixaLiniaHoritzontal(4, 86, pointer_to_paint + 35, 0x00, 0xFF, 0xFF, 0xFF);

	col = 4 + (int)((avg_acceleration_Y + 4) * 10.25);
	fila = pointer_to_paint + 35;
	// Delete the previous pixel painted
	SetPixel(buffer_signal_Y_coords[pointer_to_paint].col, buffer_signal_Y_coords[pointer_to_paint].fila, 0x00, 0xFF, 0xFF, 0xFF);
	SetPixel(buffer_signal_Y_coords[pointer_to_paint].col, buffer_signal_Y_coords[pointer_to_paint].fila, 0x00, 0xFF, 0xFF, 0xFF);
	SetPixel(buffer_signal_Y_coords[pointer_to_paint].col, buffer_signal_Y_coords[pointer_to_paint].fila, 0x00, 0xFF, 0xFF, 0xFF);

	SetPixel(col, fila, 0x0F, 0xFF, 0, 0);
	SetPixel(col, fila, 0xFF, 0xFF, 0, 0);

	buffer_signal_X_coords[pointer_to_paint].col = col;
	buffer_signal_X_coords[pointer_to_paint].fila = fila;
	//SetPixel(col, fila, 0xFF, 0xFF, 0, 0);

	//SetPixel(col, fila, 0x0F, 0xFF, 0, 0);

	//SetPixel(4 + (int)((avg_acceleration_Y + 4) * 10.25), pointer_to_paint + 35, 0x0F, 0xFF, 0, 0);

	// Pixen in Acceleration X will be between row 154 and 236 depending on the value with a step of (8/82) and in column pointer_to_paint + 101
	// Paint it blue
	// But first, delete the previous pixel painted deleting the vertical line in the column pointer_to_paint + 101
	//DibuixaLiniaVertical(pointer_to_paint + 101, 154, 236, 0x00, 0xFF, 0xFF, 0xFF);
	col = pointer_to_paint + 101;
	fila = 236 - (int)((avg_acceleration_X + 4) * 10.25);

	// Delete the previous pixel painted
	SetPixel(buffer_signal_X_coords[pointer_to_paint].col, buffer_signal_X_coords[pointer_to_paint].fila, 0x00, 0xFF, 0xFF, 0xFF);
	SetPixel(buffer_signal_X_coords[pointer_to_paint].col, buffer_signal_X_coords[pointer_to_paint].fila, 0x00, 0xFF, 0xFF, 0xFF);
	SetPixel(buffer_signal_X_coords[pointer_to_paint].col, buffer_signal_X_coords[pointer_to_paint].fila, 0x00, 0xFF, 0xFF, 0xFF);

	SetPixel(col, fila, 0x0F, 0, 0, 0xFF);
	SetPixel(col, fila, 0xFF, 0, 0, 0xFF);

	buffer_signal_Y_coords[pointer_to_paint].col = col;
	buffer_signal_Y_coords[pointer_to_paint].fila = fila;
	//SetPixel(col, fila, 0xFF, 0, 0, 0xFF);

	//SetPixel(col, fila, 0x0F, 0, 0, 0xFF);

	//SetPixel(pointer_to_paint + 101, 236 - (int)((avg_acceleration_X + 4) * 10.25), 0x0F, 0, 0, 0xFF);




	pointer_to_paint++;
	if(pointer_to_paint > 200){
		pointer_to_paint = 0;
		//EsborraPantalla(0xFF, 0xFF, 0xFF);
	}
}
