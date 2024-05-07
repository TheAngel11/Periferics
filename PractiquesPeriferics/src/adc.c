/*
 * adc.c
 *
 *  Created on: 14 abr. 2024
 *      Author: angel
 */

#include "adc.h"

// Init the ADC for both accelerometers
void init_ADC(void) {
	ADC_InitTypeDef       ADC_init_config;
	ADC_CommonInitTypeDef ADC_common_config;
	GPIO_InitTypeDef      GPIO_config;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);

	GPIO_StructInit(&GPIO_config);
	GPIO_config.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_3;
	GPIO_config.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_config.GPIO_Mode = GPIO_Mode_AN;
	GPIO_Init(GPIOC, &GPIO_config);

    ADC_StructInit(&ADC_init_config);
	ADC_init_config.ADC_Resolution = ADC_Resolution_12b;
	ADC_init_config.ADC_ContinuousConvMode = DISABLE;
	ADC_init_config.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_init_config.ADC_ScanConvMode = ENABLE;
	ADC_init_config.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	ADC_init_config.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_init_config.ADC_NbrOfConversion = 2;
	ADC_Init(ADC3, &ADC_init_config);

    ADC_CommonInit(&ADC_common_config);
	ADC_common_config.ADC_Prescaler = ADC_Prescaler_Div2;
	ADC_common_config.ADC_Mode = ADC_Mode_Independent;
	ADC_common_config.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	ADC_common_config.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInit(&ADC_common_config);

	ADC_RegularChannelConfig(ADC3, ADC_Channel_13, 1, ADC_SampleTime_3Cycles);
	ADC_RegularChannelConfig(ADC3, ADC_Channel_11, 2, ADC_SampleTime_3Cycles);

	ADC_DMARequestAfterLastTransferCmd(ADC3, ENABLE);
	ADC_DMACmd(ADC3, ENABLE);
	ADC_Cmd(ADC3, ENABLE);
}
