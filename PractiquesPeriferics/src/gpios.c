/*
 * gpios.c
 *
 *  Created on: 13 abr. 2024
 *      Author: angel
 */
#include "gpios.h"

// Inits IRQ inputs wheel signals
void init_wheel_signal_ouput(void) {
	GPIO_InitTypeDef GPIO_config;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);

	GPIO_config.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_config.GPIO_OType = GPIO_OType_PP;
	GPIO_config.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_config.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_config.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOG, &GPIO_config);
}

void init_PB_user(void){
	GPIO_InitTypeDef GPIO_config;
	EXTI_InitTypeDef EXTI_config;
	NVIC_InitTypeDef NVIC_config;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	GPIO_config.GPIO_Mode = GPIO_Mode_IN;
	GPIO_config.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_config.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOA, &GPIO_config);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	GPIO_config.GPIO_Mode = GPIO_Mode_IN;
	GPIO_config.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_config.GPIO_Pin = GPIO_Pin_15;
	GPIO_Init(GPIOC, &GPIO_config);

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);

	EXTI_config.EXTI_Line = EXTI_Line0;
	EXTI_config.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_config.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_config.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_config);

	NVIC_config.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_config.NVIC_IRQChannelPreemptionPriority = 0x04;
	NVIC_config.NVIC_IRQChannelSubPriority = 0x01;
	NVIC_config.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_config);
}

// Inits the input for each wheel
void init_wheels_inputs(void){
	GPIO_InitTypeDef GPIO_config;
	EXTI_InitTypeDef EXTI_config;
	NVIC_InitTypeDef NVIC_config;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	GPIO_config.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_config.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_config.GPIO_OType = GPIO_OType_PP;
	GPIO_config.GPIO_Mode = GPIO_Mode_IN;
	GPIO_config.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOD, &GPIO_config);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource9);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource8);

	EXTI_config.EXTI_Line = EXTI_Line9 ;
	EXTI_config.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_config.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_config.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_config);
	EXTI_config.EXTI_Line = EXTI_Line8;
	EXTI_Init(&EXTI_config);

	NVIC_config.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_config.NVIC_IRQChannelPreemptionPriority = 0x03;
	NVIC_config.NVIC_IRQChannelSubPriority = 0x01;
	NVIC_config.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_config);
}
