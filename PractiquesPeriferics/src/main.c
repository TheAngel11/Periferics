#include "globals.h"
#include "timers.h"
#include "interrupts.h"
#include "gpios.h"
#include "dac.h"
#include "adc.h"
#include "dma.h"
#include "lcd.h"

int main(void)
{

	init_TIM2(); // 1ms
	init_TIM3(); // 2us
	init_TIM5(); // 50us
	init_wheels_inputs();
	init_wheel_signal_ouput();
	init_PB_user();
	init_DAC();

	init_ADC_2_mem();
	init_mem_2_mem();
	init_ADC();

	init_LCD();
	LAYER_NUM = 0;
	EsborraPantalla(0xFF, 0xFF, 0xFF);
	LAYER_NUM = 1;
	EsborraPantalla(0xFF, 0xFF, 0xFF);

	draw_layer1();
	LAYER_NUM = 0;

	STM_EVAL_LEDInit(LED3);
	STM_EVAL_LEDInit(LED4);

	float avg_acceleration_X = 0;
	float avg_acceleration_Y = 0;
	int col;
	int fila;
	int bitmap_counter = 0;

	/* Infinite loop */
	while (1){
		if(flag_paint_signal == 1){
			flag_paint_signal = 0;
			avg_acceleration_X = 0;
			avg_acceleration_Y = 0;
			bitmap_counter++;

			for(int i = 0; i < 10; i++){
				avg_acceleration_X += acceleration_X_samples[i];
				avg_acceleration_Y += acceleration_Y_samples[i];
			}

			if (avg_acceleration_Y > 40) {
				avg_acceleration_Y = 40;
			}

			avg_acceleration_X = avg_acceleration_X / 10.0;
			avg_acceleration_Y = avg_acceleration_Y / 10.0;




			//SetPixel(pointer_to_paint, pointer_to_paint, 0x0F, 0, 0, 0);
			//SetPixel(pointer_to_paint, pointer_to_paint, 0x0F, 0, 0, 0);

			// Pixen in Acceleration Y will be between column 4 and 86 depending on the value with a step of (8/82) and in row pointer_to_paint + 35
			// Paint it red
			// But first, delete the previous pixel painted deleting the horizontal line in the row pointer_to_paint + 35
			// Paint it white
			//DibuixaLiniaHoritzontal(4, 86, pointer_to_paint + 35, 0x00, 0xFF, 0xFF, 0xFF);

			col = 4 + (int)((avg_acceleration_Y + 4) * 10.25);
			fila = pointer_to_paint + 35;

			if(col > 88) {
				SetPixel(buffer_signal_Y_coords[pointer_to_paint].col, buffer_signal_Y_coords[pointer_to_paint].fila, 0x00, 0xFF, 0xFF, 0xFF);
			}

			// Delete the previous pixel painted
			SetPixel(buffer_signal_Y_coords[pointer_to_paint].col, buffer_signal_Y_coords[pointer_to_paint].fila, 0x00, 0xFF, 0xFF, 0xFF);
			SetPixel(buffer_signal_Y_coords[pointer_to_paint].col, buffer_signal_Y_coords[pointer_to_paint].fila, 0x00, 0xFF, 0xFF, 0xFF);
			SetPixel(buffer_signal_Y_coords[pointer_to_paint].col, buffer_signal_Y_coords[pointer_to_paint].fila, 0x00, 0xFF, 0xFF, 0xFF);
			SetPixel(buffer_signal_Y_coords[pointer_to_paint].col, buffer_signal_Y_coords[pointer_to_paint].fila, 0x00, 0xFF, 0xFF, 0xFF);

			//SetPixel(col, fila, 0x0F, 0xFF, 0, 0);
			SetPixel(col, fila, 0xFF, 0xFF, 0, 0);

			buffer_signal_Y_coords[pointer_to_paint].col = col;
			buffer_signal_Y_coords[pointer_to_paint].fila = fila;
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
			SetPixel(buffer_signal_X_coords[pointer_to_paint].col, buffer_signal_X_coords[pointer_to_paint].fila, 0x00, 0xFF, 0xFF, 0xFF);

			//SetPixel(col, fila, 0x0F, 0, 0, 0xFF);
			SetPixel(col, fila, 0xFF, 0, 0, 0xFF);

			buffer_signal_X_coords[pointer_to_paint].col = col;
			buffer_signal_X_coords[pointer_to_paint].fila = fila;
			//SetPixel(col, fila, 0xFF, 0, 0, 0xFF);

			//SetPixel(col, fila, 0x0F, 0, 0, 0xFF);

			//SetPixel(pointer_to_paint + 101, 236 - (int)((avg_acceleration_X + 4) * 10.25), 0x0F, 0, 0, 0xFF);


			// Create a 3x3 pixel square between the both axis and the four circumferences, in the X axis the square will represent the acceleration in X and in the Y axis the acceleration in Y.
			// The X value is between volumn 140 and 260, being -4 value 140 and 4 value 260 and the Y value is between row 136 and 16 being -4 value 136 and 4 value 16.
			// The square will be painted in green
			// But first, delete the previous square painted
			//DibuixaLiniaHoritzontal(140, 260, 136, 0x00, 0xFF, 0xFF, 0xFF);
			//DibuixaLiniaHoritzontal(140, 260, 16, 0x00, 0xFF, 0xFF, 0xFF);
			//DibuixaLiniaVertical(140, 16, 136, 0x00, 0xFF, 0xFF, 0xFF);
			//DibuixaLiniaVertical(260, 16, 136, 0x00, 0xFF, 0xFF, 0xFF);

			// Delete the previous square painted the center being bitmap_coords[0] and bitmap_coords[1]
			//DibuixaBitmap(bitmap_coords[0], bitmap_coords[1], 0x00, 0xFF, 0xFF, 0xFF);
			//DibuixaBitmap(bitmap_coords[0], bitmap_coords[1], 0x00, 0xFF, 0xFF, 0xFF);
			//DibuixaBitmap(bitmap_coords[0], bitmap_coords[1], 0x00, 0xFF, 0xFF, 0xFF);
			//DibuixaBitmap(bitmap_coords[0], bitmap_coords[1], 0x00, 0xFF, 0xFF, 0xFF);

			// Paint the new square
			if (bitmap_counter >= 10) {
				DibuixaBitmap(bitmap_coords[0], bitmap_coords[1], 0x00, 0xFF, 0xFF, 0xFF);
				DibuixaBitmap(bitmap_coords[0], bitmap_coords[1], 0x00, 0xFF, 0xFF, 0xFF);

				bitmap_coords[0] = 140 + (int)((avg_acceleration_X + 4) * 15);
				bitmap_coords[1] = 136 - (int)((avg_acceleration_Y + 4) * 15);

				DibuixaBitmap(bitmap_coords[0], bitmap_coords[1], 0xFF, 0, 0xFF, 0);
				DibuixaBitmap(bitmap_coords[0], bitmap_coords[1], 0x0F, 0, 0xFF, 0);

				bitmap_counter = 0;
			}

			





			pointer_to_paint++;
			if(pointer_to_paint >= 200){
				pointer_to_paint = 0;
				//EsborraPantalla(0xFF, 0xFF, 0xFF);
				// make a for loop to delete all the pixels painted
				/*for (int i = 0; i < 200; i++){
					SetPixel(buffer_signal_X_coords[i].col, buffer_signal_X_coords[i].fila, 0x00, 0xFF, 0xFF, 0xFF);
					SetPixel(buffer_signal_Y_coords[i].col, buffer_signal_Y_coords[i].fila, 0x00, 0xFF, 0xFF, 0xFF);
				}*/
			}


		} 

	}
}

/*
 * Callback used by stm324xg_eval_i2c_ee.c.
 * Refer to stm324xg_eval_i2c_ee.h for more info.
 */
uint32_t sEE_TIMEOUT_UserCallback(void)
{
  /* TODO, implement your code here */
  while (1){
  }
}
