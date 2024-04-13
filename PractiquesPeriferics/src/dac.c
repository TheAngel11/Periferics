/*
 * dac.c
 *
 *  Created on: 13 abr. 2024
 *      Author: angel
 */

#ifndef DAC_C_
#define DAC_C_

#include "dac.h"

// Inits the DAC on port PA5
void init_DAC(void){
	GPIO_InitTypeDef GPIO_config;
	DAC_InitTypeDef DAC_config;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

	GPIO_config.GPIO_Pin = GPIO_Pin_5;
	GPIO_config.GPIO_Mode = GPIO_Mode_AN;
	GPIO_config.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_config);

	DAC_config.DAC_Trigger = DAC_Trigger_None;
	DAC_config.DAC_WaveGeneration = DAC_WaveGeneration_None;
	DAC_config.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
	DAC_Init(DAC_Channel_2, &DAC_config);

	DAC_Cmd(DAC_Channel_2, ENABLE);
	DAC_SetChannel2Data(DAC_Align_12b_R, 2048);
}

#endif /* DAC_C_ */
