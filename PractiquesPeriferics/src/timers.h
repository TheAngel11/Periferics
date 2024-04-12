/*
 * timers.h
 *
 *  Created on: 9 abr. 2024
 *      Author: angel
 */

#ifndef TIMERS_H_
#define TIMERS_H_

#include "stm32f4xx.h"
#include "stm32f429i_discovery.h"


void init_TIM2(void); // 1ms
void init_TIM3(void); // 2us
void init_TIM5(void); // 50us

#endif /* TIMERS_H_ */
