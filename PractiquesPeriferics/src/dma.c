/*
 * dma.c
 *
 *  Created on: 14 abr. 2024
 *      Author: angel
 */

#include "dma.h"

//TODO: DEBUG
float counter_test_signal = -4;

// DMA interrupt handler to calculate acceleration
void DMA2_Stream0_IRQHandler(void) {
	if (DMA_GetITStatus(DMA2_Stream0, DMA_IT_TCIF0) != 0) {
		int average_samples_X = 0;
		int average_samples_Y = 0;
		
		//STM_EVAL_LEDToggle(LED4);

		for(int i = 0; i < 16; i=i+2){
			average_samples_X += DESTINATION_BUFFER[i];
		}

		average_samples_X = average_samples_X / 8;

		for(int i = 1; i < 16; i=i+2){
			average_samples_Y += DESTINATION_BUFFER[i];
		}

		average_samples_Y = average_samples_Y / 8;

		// VALUES OF ACCELERATION!!!
		float acceleration_X = ((float)average_samples_X - 1365.0) / 341.25;
		float acceleration_Y = ((float)average_samples_Y - 1365.0) / 341.25;

		acceleration_X_samples[paint_signal_counter] = acceleration_X;
		acceleration_Y_samples[paint_signal_counter] = acceleration_Y;

		// TODO: Testing
		//acceleration_X_samples[paint_signal_counter] = counter_test_signal;
		//acceleration_Y_samples[paint_signal_counter] = counter_test_signal;

		counter_test_signal += 0.001;
		if(counter_test_signal >= 4.0){
			counter_test_signal = -4.0;
		}

		paint_signal_counter++;

		if(paint_signal_counter >= 10){
			paint_signal_counter = 0;
			flag_paint_signal = 1;
		}

		DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TCIF0);
	}
}

// Init the DMA for the transference ADC -> MEM
void init_ADC_2_mem(void) {
	DMA_InitTypeDef DMA_config;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

	DMA_StructInit(&DMA_config);
	DMA_config.DMA_Channel = DMA_Channel_2;
	DMA_config.DMA_BufferSize = 16; //8 samples for each accelerometer
	DMA_config.DMA_Memory0BaseAddr = (uint32_t) ORIGIN_BUFFER;
	DMA_config.DMA_Priority = DMA_Priority_High;
	DMA_config.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_config.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_config.DMA_PeripheralBaseAddr = (uint32_t) & (ADC3->DR);
	DMA_config.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_config.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_config.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_config.DMA_Mode = DMA_Mode_Circular;
	DMA_config.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_config.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_config.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_config.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_Init(DMA2_Stream1, &DMA_config);
	DMA_Cmd(DMA2_Stream1, ENABLE);
}

// Init the DMA for the MEM -> MEM
void init_mem_2_mem(void) {
	DMA_InitTypeDef DMA_config;
	NVIC_InitTypeDef NVIC_config;

	DMA_StructInit(&DMA_config);
	DMA_config.DMA_Channel = DMA_Channel_1;
	DMA_config.DMA_PeripheralBaseAddr = (uint32_t) ORIGIN_BUFFER;
	DMA_config.DMA_Memory0BaseAddr = (uint32_t) DESTINATION_BUFFER;
	DMA_config.DMA_Priority = DMA_Priority_Low;
	DMA_config.DMA_DIR = DMA_DIR_MemoryToMemory;
	DMA_config.DMA_BufferSize = 16;  //8 samples for each accelerometer
	DMA_config.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
	DMA_config.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_config.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_config.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_config.DMA_Mode = DMA_Mode_Normal;
	DMA_config.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_config.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_config.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_config.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream0, &DMA_config);
	DMA_ITConfig(DMA2_Stream0, DMA_IT_TC, ENABLE);

	NVIC_config.NVIC_IRQChannel = DMA2_Stream0_IRQn;
	NVIC_config.NVIC_IRQChannelPreemptionPriority = 5;
	NVIC_config.NVIC_IRQChannelSubPriority = 1;
	NVIC_config.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_config);
}
