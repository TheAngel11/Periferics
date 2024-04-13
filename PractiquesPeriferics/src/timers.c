/*
 * timers.c
 *
 *  Created on: 9 abr. 2024
 *      Author: angel
 */

#include "timers.h"

// Function that calculates the Vc signal from We and Wd
void _calculate_final_signal(void){
	unsigned int minW;
	unsigned int maxW;
	int Vc;

	if (Wd > We) {
		minW = We;
		maxW = Wd;
	} else {
		minW = Wd;
		maxW = We;
	}

	// Middle point of the signal
	if (minW <= 1111/2 || minW * 1.3 >= maxW){
		DAC_SetChannel2Data(DAC_Align_12b_R, 2048);
		return;
	}

	// Right side of the signal
	if ((maxW - minW)/minW >= 1 && maxW == We){
		DAC_SetChannel2Data(DAC_Align_12b_R, 0);
		return;
	}

	// Left side of the signal
	if ((maxW - minW)/minW >= 1 && maxW == Wd){
		DAC_SetChannel2Data(DAC_Align_12b_R, 4095);
		return;
	}

	// Middle - Right side of the signal: Y = 2925X + 1170
	if(maxW == Wd) {
		Vc = (int)(2925 * ((float)(Wd - We) / We)) + 1170;
		DAC_SetChannel2Data(DAC_Align_12b_R, Vc);
		return;
	}

	// Middle - Left side of the signal: Y = -2925X + 2925
	if(maxW == We) {
		Vc = (int)(-2925 * ((float)(We - Wd) / Wd)) + 2925;
		DAC_SetChannel2Data(DAC_Align_12b_R, Vc);
		return;
	}
}

// Executes the TIM2 RSI
void TIM2_IRQHandler(void) {
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != 0) {
		//STM_EVAL_LEDToggle(LED4);
		counter_led++;
		bouncing_counter_ms++;

		if(counter_led >= 200){
			counter_led = 0;
			STM_EVAL_LEDToggle(LED3);
		}

		// Update the output signal
		_calculate_final_signal();

		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		//STM_EVAL_LEDOff(LED4);
	}
}

// Executes the TIM3 RSI
void TIM3_IRQHandler(void) {
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != 0) {
		//STM_EVAL_LEDOn(LED4);
		micros_period_right+=2;
		micros_period_left+=2;
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		//STM_EVAL_LEDOff(LED4);
	}
}

// Executes the TIM5 RSI
void TIM5_IRQHandler(void) {
	if (TIM_GetITStatus(TIM5, TIM_IT_Update) != 0) {
		//STM_EVAL_LEDOn(LED4);
		signal1_microseconds+=50;
		signal2_microseconds+=50;

		TIM_ClearITPendingBit(TIM5, TIM_IT_Update);

		if (signal1_microseconds >= (right_wheel_signal_period_us / 2)) {
			GPIO_ToggleBits(GPIOG, GPIO_Pin_0);
			signal1_microseconds = 0;
		}

		if (signal2_microseconds >= (left_wheel_signal_period_us / 2)) {
			GPIO_ToggleBits(GPIOG, GPIO_Pin_1);
			signal2_microseconds = 0;
		}

		//STM_EVAL_LEDOff(LED4);
	}
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
    NVIC_TimerConfig.NVIC_IRQChannelPreemptionPriority = 0x08;
    NVIC_TimerConfig.NVIC_IRQChannelSubPriority = 0x01;
    NVIC_Init(&NVIC_TimerConfig);

    TIM_Cmd(TIM2, ENABLE);
}

//Interrupt every 1us
void init_TIM3(void){
	TIM_TimeBaseInitTypeDef TIM_TimerConfig;
	NVIC_InitTypeDef NVIC_TimerConfig;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	TIM_TimerConfig.TIM_Prescaler = 1;
	TIM_TimerConfig.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimerConfig.TIM_Period = 90;
	TIM_TimerConfig.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM3, &TIM_TimerConfig);

    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

    NVIC_TimerConfig.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_TimerConfig.NVIC_IRQChannelCmd = ENABLE;
    NVIC_TimerConfig.NVIC_IRQChannelPreemptionPriority = 0x01;
    NVIC_TimerConfig.NVIC_IRQChannelSubPriority = 0x01;
    NVIC_Init(&NVIC_TimerConfig);

    TIM_Cmd(TIM3, ENABLE);
}

// Inits TIM5 for 50us interrupts
void init_TIM5(void){
	TIM_TimeBaseInitTypeDef TIM_TimerConfig;
	NVIC_InitTypeDef NVIC_TimerConfig;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

	TIM_TimerConfig.TIM_Prescaler = 41;
	TIM_TimerConfig.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimerConfig.TIM_Period = 106;
	TIM_TimerConfig.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM5, &TIM_TimerConfig);

    TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);

    NVIC_TimerConfig.NVIC_IRQChannel = TIM5_IRQn;
    NVIC_TimerConfig.NVIC_IRQChannelCmd = ENABLE;
    NVIC_TimerConfig.NVIC_IRQChannelPreemptionPriority = 0x02;
    NVIC_TimerConfig.NVIC_IRQChannelSubPriority = 0x01;
    NVIC_Init(&NVIC_TimerConfig);

    TIM_Cmd(TIM5, ENABLE);
}
