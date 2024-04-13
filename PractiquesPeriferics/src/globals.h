/*
 * globals.h
 *
 *  Created on: 9 abr. 2024
 *      Author: angel
 */

#ifndef GLOBALS_H_
#define GLOBALS_H_

#include "stm32f4xx.h"
#include "stm32f429i_discovery.h"

#define MAX_BOUNCES_MS 300

extern int counter_led;
extern unsigned int Wd;
extern unsigned int We;
extern uint32_t micros_period_right;
extern uint32_t micros_period_left;
extern int vehicle_speed;
extern int vehicle_speed_ascendent;
extern int i;
extern int difference_factor_ascendent;
extern int num_cycles;
extern int calculating_right_speed;
extern float right_wheel_speed;
extern float left_wheel_speed;
extern int j;
extern int signal1_microseconds;
extern int signal2_microseconds;
extern int right_wheel_signal_period_us;
extern int left_wheel_signal_period_us;
extern int ms_since_edge_rising;
extern int bouncing_counter_ms;
extern int last_bouncing_counter_ms;

#endif /* GLOBALS_H_ */
