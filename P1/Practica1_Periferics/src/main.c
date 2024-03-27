/*
******************************************************************************
File:     main.c
Info:     Generated by Atollic TrueSTUDIO(R) 9.3.0   2024-03-26

The MIT License (MIT)
Copyright (c) 2019 STMicroelectronics

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

******************************************************************************
*/

#include "stm32f4xx.h"
#include "stm32f429i_discovery.h"

int counter_led = 0;
unsigned long num_overflows_right = 0;
unsigned long num_overflows_left = 0;
unsigned int right_speed = 0;
unsigned int left_speed = 0;

/**
**===========================================================================
**
**  Abstract: main program
**
**===========================================================================
*/
int vehicle_speed = 0;
int vehicle_speed_ascendent = 1; //1 = ascendent, 0 = descendent
int i = 0;

int difference_factor_ascendent = 1; //1 = ascendent, 0 = descendent
int num_cycles = 0; //Number of times that we have arrived to/started from the initial speed difference factor (1)
int calculating_right_speed = 0; //1 = calculating right speed, 0 = calculating left speed
float right_wheel_speed = 0;
float left_wheel_speed = 0;
int j = 0;

int tim5_interrupts = 0;

int right_wheel_signal_period_us;
int left_wheel_signal_period_us;

// Executes the TIM5 RSI
void TIM5_IRQHandler(void) {
	if (TIM_GetITStatus(TIM5, TIM_IT_Update) != 0) {

		tim5_interrupts++;
		if (tim5_interrupts == (right_wheel_signal_period_us / 2) || tim5_interrupts == right_wheel_signal_period_us) {
			//TOGGLE PIN PG0
			tim5_interrupts = 0;
		}
		//TIM_SetCounter(TIM5, 0);

		TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
	}
}

// Inits TIM5 for 1us interrupts
void init_TIM5(void){
	TIM_TimeBaseInitTypeDef TIM_TimerConfig;
	NVIC_InitTypeDef NVIC_TimerConfig;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

	TIM_TimerConfig.TIM_Prescaler = 5;
	TIM_TimerConfig.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimerConfig.TIM_Period = 14;
	TIM_TimerConfig.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM5, &TIM_TimerConfig);

    TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);

    NVIC_TimerConfig.NVIC_IRQChannel = TIM5_IRQn;
    NVIC_TimerConfig.NVIC_IRQChannelCmd = ENABLE;
    // TODO: Check priorities
    NVIC_TimerConfig.NVIC_IRQChannelPreemptionPriority = 0x08;
    NVIC_TimerConfig.NVIC_IRQChannelSubPriority = 0x01;
    NVIC_Init(&NVIC_TimerConfig);

    TIM_Cmd(TIM5, ENABLE);
}

void generateRightWheelSignal(int right_wheel_signal_period_us) {


}


//User PB RSI
void EXTI0_IRQHandler(void) {
	//float left;
	//float right;
	int vehicle_speeds[6] = {0,10,35,45,100,270};

	float wheel_speed_difference_factor[5] = {1, 1.25, 1.35, 1.8, 2.2};

	if(EXTI_GetITStatus(EXTI_Line0) != 0) {

		if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_15) == 0) {

			//CASE GPIO AUX C15 LOW
			if (vehicle_speeds[i] == 0) {
				vehicle_speed_ascendent = 1;
			} else if (vehicle_speeds[i] == 270) {
				vehicle_speed_ascendent = 0;
			}

			if (vehicle_speed_ascendent == 1) {
				vehicle_speed = vehicle_speeds[i];
				i++;
			} else {
				vehicle_speed = vehicle_speeds[i];
				i--;
			}
			/////////////////////////

		} else {

			STM_EVAL_LEDOff(LED4);
			//CASE GPIO AUX C15 HIGH
			if (vehicle_speed != 0) {

				if (wheel_speed_difference_factor[j] == 1) {
					difference_factor_ascendent = 1;
					num_cycles++;

					if (num_cycles == 2) {
						num_cycles = 1;
						calculating_right_speed = !calculating_right_speed;
					}
				} else if (wheel_speed_difference_factor[j] == (float) 2.2) {
					difference_factor_ascendent = 0;
				}

				if (difference_factor_ascendent == 1) {

					if (calculating_right_speed) {
						left_wheel_speed = vehicle_speed;
						right_wheel_speed = wheel_speed_difference_factor[j] * left_wheel_speed;
					} else {
						right_wheel_speed = vehicle_speed;
						left_wheel_speed = wheel_speed_difference_factor[j] * right_wheel_speed;
					}
					//left = left_wheel_speed;
					//right = right_wheel_speed;
					j++;

				} else {

					if (calculating_right_speed) {
						left_wheel_speed = vehicle_speed;
						right_wheel_speed = wheel_speed_difference_factor[j] * left_wheel_speed;
					} else {
						right_wheel_speed = vehicle_speed;
						left_wheel_speed = wheel_speed_difference_factor[j] * right_wheel_speed;
					}

					//left = left_wheel_speed;
					//right = right_wheel_speed;
					j--;

				}

			} else {
				//left = vehicle_speed;
				//right = vehicle_speed;

				right_wheel_speed = vehicle_speed;
				left_wheel_speed = vehicle_speed;
			}

			//////////////////
		}

		//We convert km/h to m/s
		float right_wheel_speed_meters_per_second = (right_wheel_speed * 1000) / 3600;
		float left_wheel_speed_meters_per_second = (left_wheel_speed * 1000) / 3600;

		//We convert from m/s to rev/s
		float right_wheel_speed_rev_per_second = right_wheel_speed_meters_per_second / 2;
		float left_wheel_speed_rev_per_second = left_wheel_speed_meters_per_second / 2;

		//We convert rev/s to signal period
		float right_wheel_signal_period_seconds = 1 / (32 * right_wheel_speed_rev_per_second);
		float left_wheel_signal_period_seconds = 1 / (32 * left_wheel_speed_rev_per_second);

		//We convert to signal period from s to us
		right_wheel_signal_period_us = right_wheel_signal_period_seconds * 1000000;
		left_wheel_signal_period_us = left_wheel_signal_period_seconds * 1000000;

		//generateRightWheelSignal(right_wheel_signal_period_us);
		//TODO: generate left wheel signal


		//STM_EVAL_LEDToggle(LED4);
		EXTI_ClearITPendingBit(EXTI_Line0);
	}
}

void init_PB_user(void){
	GPIO_InitTypeDef GPIO_config;
	EXTI_InitTypeDef EXTI_config;
	NVIC_InitTypeDef NVIC_config;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);	//TODO: Check if yes or no

	GPIO_config.GPIO_Mode = GPIO_Mode_IN;
	GPIO_config.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_config.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOA, &GPIO_config);

	//For PC15 AUX INPUT
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	GPIO_config.GPIO_Mode = GPIO_Mode_IN;
	GPIO_config.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_config.GPIO_Pin = GPIO_Pin_15;
	GPIO_Init(GPIOC, &GPIO_config);
	////

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);

	EXTI_config.EXTI_Line = EXTI_Line0;
	EXTI_config.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_config.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_config.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_config);

	NVIC_config.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_config.NVIC_IRQChannelPreemptionPriority = 0x07;
	NVIC_config.NVIC_IRQChannelSubPriority = 0x01;
	NVIC_config.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_config);
}

// Inits the input for each wheel
void init_wheels_inputs(void){
	GPIO_InitTypeDef GPIO_config;
	EXTI_InitTypeDef EXTI_config;
	NVIC_InitTypeDef NVIC_config;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	GPIO_config.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_config.GPIO_PuPd = GPIO_PuPd_UP;	// TODO: Check if pullups yes or no
	GPIO_config.GPIO_Mode = GPIO_Mode_IN;
	//GPIO_config.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_config);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource13);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource14);

	EXTI_config.EXTI_Line = EXTI_Line13;
	EXTI_config.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_config.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_config.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_config);
	EXTI_config.EXTI_Line = EXTI_Line14;
	EXTI_Init(&EXTI_config);

	NVIC_config.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_config.NVIC_IRQChannelPreemptionPriority = 0x02;
	NVIC_config.NVIC_IRQChannelSubPriority = 0x01;
	NVIC_config.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_config);

}

// Inits TIM3 and TIM4 for 1us free-running counters that interrupts when overflowing
void init_TIM3_TIM4(void){
	TIM_TimeBaseInitTypeDef TIM_TimerConfig;
	NVIC_InitTypeDef NVIC_TimerConfig;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	TIM_TimeBaseStructInit(&TIM_TimerConfig);	//TODO: Check if yes or no
	TIM_TimerConfig.TIM_Prescaler = 94;	// Now we get 1 Tic every 1us
	TIM_TimerConfig.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimerConfig.TIM_Period = 0xFFFFFFFF;
	TIM_TimerConfig.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM3, &TIM_TimerConfig);
    TIM_TimeBaseInit(TIM4, &TIM_TimerConfig);

    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);

    NVIC_TimerConfig.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_TimerConfig.NVIC_IRQChannelCmd = ENABLE;
    // TODO: Check priorities
    NVIC_TimerConfig.NVIC_IRQChannelPreemptionPriority = 0x04;
    NVIC_TimerConfig.NVIC_IRQChannelSubPriority = 0x01;
    NVIC_Init(&NVIC_TimerConfig);
    NVIC_TimerConfig.NVIC_IRQChannel = TIM4_IRQn;
    NVIC_Init(&NVIC_TimerConfig);

    TIM_Cmd(TIM3, ENABLE);
    TIM_Cmd(TIM4, ENABLE);
}

// Inits TIM2 for 1ms interrupts
void init_TIM2(void){
	TIM_TimeBaseInitTypeDef TIM_TimerConfig;
	NVIC_InitTypeDef NVIC_TimerConfig;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	TIM_TimerConfig.TIM_Prescaler = 17;
	TIM_TimerConfig.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimerConfig.TIM_Period = 4999;
	TIM_TimerConfig.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM2, &TIM_TimerConfig);

    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    NVIC_TimerConfig.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_TimerConfig.NVIC_IRQChannelCmd = ENABLE;
    // TODO: Check priorities
    NVIC_TimerConfig.NVIC_IRQChannelPreemptionPriority = 0x03;
    NVIC_TimerConfig.NVIC_IRQChannelSubPriority = 0x01;
    NVIC_Init(&NVIC_TimerConfig);

    TIM_Cmd(TIM2, ENABLE);
}

// Executes the TIM2 RSI
void TIM2_IRQHandler(void) {
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != 0) {

		counter_led++;
		if(counter_led >= 2000){
			counter_led = 0;
			STM_EVAL_LEDToggle(LED3);
		}

		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}

// Executes the TIM3 RSI
void TIM3_IRQHandler(void) {
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != 0) {
		num_overflows_right++;
		TIM_SetCounter(TIM3, 0);
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	}
}

// Executes the TIM4 RSI
void TIM4_IRQHandler(void) {
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != 0) {
		num_overflows_left++;
		TIM_SetCounter(TIM4, 0);
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	}
}

unsigned int _calculate_rotation_speed(unsigned int timer_count, unsigned long overflow_count){
	return 10*10*10*10*10*10*10*10/(32 * (timer_count +  65535 * overflow_count));
}

void EXTI15_10_IRQHandler(void) {
    if (EXTI_GetITStatus(EXTI_Line13) != 0) {

    	right_speed = _calculate_rotation_speed(TIM_GetCounter(TIM3), num_overflows_right);

    	TIM_SetCounter(TIM3, 0);
    	num_overflows_right = 0;

    	STM_EVAL_LEDToggle(LED3); //TODO: Debug
        EXTI_ClearITPendingBit(EXTI_Line13);
    }
    if (EXTI_GetITStatus(EXTI_Line14) != 0) {

    	left_speed = _calculate_rotation_speed(TIM_GetCounter(TIM4), num_overflows_left);

    	TIM_SetCounter(TIM4, 0);
    	num_overflows_left = 0;

    	STM_EVAL_LEDToggle(LED4); //TODO: Debug
        EXTI_ClearITPendingBit(EXTI_Line14);
    }
}

int main(void)
{
	init_TIM2();
	init_TIM3_TIM4();
	init_TIM5();
	init_wheels_inputs();
	init_PB_user();
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
