/*
 * lcd.c
 *
 *  Created on: 18 abr. 2024
 *      Author: angel
 */

#include "lcd.h"

/* ----- LCD LIBRARY -----*/

// Draw a pixel in col x fila with value ARGB
RetSt SetPixel(uint16_t col, uint16_t fila, uint8_t alfa, uint8_t Rval, uint8_t Gval, uint8_t Bval){
	if (col > MAX_COL || fila > MAX_FILA || col < 0 || fila < 0){
		return NO_OK;
	}

    uint32_t color = (alfa << 24) | (Rval << 16) | (Gval << 8) | Bval;
	uint32_t pixel_adr = SDRAM_BANK_ADDR + LAYER_NUM * BUFFER_OFFSET + 4 * ((MAX_FILA + 1) * (MAX_COL + 1 - col) + fila);
    *(uint32_t __IO*) pixel_adr = color;

    return OK;
}

// Get the value ARGB of the pixel col x line
uint32_t GetPixel(uint16_t col, uint16_t fila){
	if (col > MAX_COL || fila > MAX_FILA || col < 0 || fila < 0){
		return NO_OK;
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
void init_LCD(void){
	LCD_Init();
	RCC_PLLSAIConfig(256, 0, 2);

	LCD_LayerInit();
	LTDC_LayerPixelFormat(LTDC_Layer1, LTDC_Pixelformat_ARGB8888);
	LTDC_LayerPixelFormat(LTDC_Layer2, LTDC_Pixelformat_ARGB8888);
	LTDC_ReloadConfig(LTDC_IMReload);

	LTDC_Cmd(ENABLE);
	FMC_SDRAMWriteProtectionConfig(FMC_Bank2_SDRAM, DISABLE);
}

void draw_layer1(void){
	LAYER_NUM = 0;
	DibuixaLiniaVertical(4, 35, 236, 0xFF, 0, 0, 0);
	DibuixaLiniaVertical(86, 35, 236, 0xFF, 0, 0, 0);
	DibuixaLiniaVertical(45, 35, 236, 0xFF, 0, 0, 0);
	DibuixaLiniaHoritzontal(4, 86, 35, 0xFF, 0, 0, 0);
	DibuixaLiniaHoritzontal(4, 86, 236, 0xFF, 0, 0, 0);

	DibuixaLiniaHoritzontal(101, 302, 236, 0xFF, 0, 0, 0);
	DibuixaLiniaHoritzontal(101, 302, 154, 0xFF, 0, 0, 0);
	DibuixaLiniaHoritzontal(101, 302, 195, 0xFF, 0, 0, 0);
	DibuixaLiniaVertical(101, 154, 236, 0xFF, 0, 0, 0);
	DibuixaLiniaVertical(302, 154, 236, 0xFF, 0, 0, 0);

	DibuixaLiniaVertical(200, 8, 144, 0xFF, 0, 0, 0);
	DibuixaLiniaHoritzontal(132, 268, 76, 0xFF, 0, 0, 0);
	DibuixaCircumferencia(200, 76, 15, 0xFF, 0, 0, 0);
	DibuixaCircumferencia(200, 76, 30, 0xFF, 0, 0, 0);
	DibuixaCircumferencia(200, 76, 45, 0xFF, 0, 0, 0);
	DibuixaCircumferencia(200, 76, 60, 0xFF, 0, 0, 0);
}

void draw_layer2(void){
	float avg_acceleration_X = 0;
	float avg_acceleration_Y = 0;
	int col;
	int fila;

	flag_paint_signal = 0;
	bitmap_counter++;

	// Calculate the avg accelerations
	for(int i = 0; i < 10; i++){
		avg_acceleration_X += acceleration_X_samples[i];
		avg_acceleration_Y += acceleration_Y_samples[i];
	}

	if (avg_acceleration_Y > 40) {
		avg_acceleration_Y = 40;
	}

	avg_acceleration_X = avg_acceleration_X / 10.0;
	avg_acceleration_Y = avg_acceleration_Y / 10.0;

	col = 4 + (int)((avg_acceleration_Y + 4) * 10.25);
	fila = pointer_to_paint + 35;

	// Signal Y
	// Delete the previous pixel painted
	SetPixel(buffer_signal_Y_coords[pointer_to_paint].col, buffer_signal_Y_coords[pointer_to_paint].fila, 0x00, 0xFF, 0xFF, 0xFF);
	// Draw new pixel
	SetPixel(col, fila, 0xFF, 0xFF, 0, 0);

	buffer_signal_Y_coords[pointer_to_paint].col = col;
	buffer_signal_Y_coords[pointer_to_paint].fila = fila;

	col = pointer_to_paint + 101;
	fila = 236 - (int)((avg_acceleration_X + 4) * 10.25);

	// Signal X
	// Delete the previous pixel painted
	SetPixel(buffer_signal_X_coords[pointer_to_paint].col, buffer_signal_X_coords[pointer_to_paint].fila, 0x00, 0xFF, 0xFF, 0xFF);
	// Draw new pixel
	SetPixel(col, fila, 0xFF, 0, 0, 0xFF);

	buffer_signal_X_coords[pointer_to_paint].col = col;
	buffer_signal_X_coords[pointer_to_paint].fila = fila;

	// Bitmap
	if (bitmap_counter >= 10) {
		// Delete previous bitmap
		DibuixaBitmap(bitmap_coords[0], bitmap_coords[1], 0x00, 0xFF, 0xFF, 0xFF);

		bitmap_coords[0] = 140 + (int)((avg_acceleration_X + 4) * 15);
		bitmap_coords[1] = 136 - (int)((avg_acceleration_Y + 4) * 15);

		// Draw new bitmap
		DibuixaBitmap(bitmap_coords[0], bitmap_coords[1], 0xFF, 0, 0xFF, 0);

		bitmap_counter = 0;
	}

	pointer_to_paint++;
	if(pointer_to_paint >= 200){
		pointer_to_paint = 0;
	}
}
