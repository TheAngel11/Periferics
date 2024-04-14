#include "globals.h"
#include "timers.h"
#include "interrupts.h"
#include "gpios.h"
#include "dac.h"
#include "adc.h"
#include "dma.h"

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

	STM_EVAL_LEDInit(LED3);
	STM_EVAL_LEDInit(LED4);

	/* Infinite loop */
	while (1){

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
