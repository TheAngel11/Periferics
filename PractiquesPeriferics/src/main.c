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
	LAYER_NUM = 1;

	STM_EVAL_LEDInit(LED3);
	STM_EVAL_LEDInit(LED4);

	/* Infinite loop */
	while (1){
		if(flag_paint_signal == 1){
			draw_layer2();
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
