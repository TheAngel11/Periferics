/*
 * lcd.h
 *
 *  Created on: 18 abr. 2024
 *      Author: angel
 */

#ifndef LCD_H_
#define LCD_H_

//#include "globals.h"
//#include "stm32f429i_discovery_sdram.h"
//#include "stm32f429i_discovery_lcd.h"


#include "stm32f4xx.h"
#include "stm32f429i_discovery.h"
#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_sdram.h"
#include "stm32f4xx_ltdc.h"
#include "stm32f4xx_fmc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "globals.h"

#define MAX_COL 319
#define MAX_FILA 239

typedef enum {
 NO_OK = 0 ,
 OK = !NO_OK
} RetSt ;

typedef struct {
	int col;
	int fila;
} BUFFER_SIGNAL_COORDS;

// Init BUFFER_SIGNAL_COORDS as a 200 struct
BUFFER_SIGNAL_COORDS buffer_signal_X_coords[200];
BUFFER_SIGNAL_COORDS buffer_signal_Y_coords[200];
int bitmap_coords[2];


RetSt SetPixel (uint16_t col, uint16_t fila, uint8_t alfa, uint8_t Rval, uint8_t Gval, uint8_t Bval);
uint32_t GetPixel (uint16_t col, uint16_t fila);
RetSt DibuixaLiniaHoritzontal (uint16_t col_inici, uint16_t col_fi, uint16_t fila, uint8_t alfa, uint8_t Rval, uint8_t Gval, uint8_t Bval);
RetSt DibuixaLiniaVertical (uint16_t col, uint16_t fila_inici, uint16_t fila_fi, uint8_t alfa, uint8_t Rval, uint8_t Gval, uint8_t Bval);
RetSt DibuixaCircumferencia (uint16_t ccol, uint16_t cfila, uint16_t radi, uint8_t alfa, uint8_t Rval, uint8_t Gval, uint8_t Bval);
RetSt DibuixaBitmap (uint16_t col, uint16_t fila, uint8_t alfa, uint8_t Rval, uint8_t Gval, uint8_t Bval);
RetSt EsborraPantalla (uint8_t Rval, uint8_t Gval, uint8_t Bval);
void init_LCD(void);
void draw_layer1(void);
void draw_layer2(void);

#endif /* LCD_H_ */
