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

int counter_led = 0;
unsigned int Wd = 0; // Right speed
unsigned int We = 0; // Left speed
uint32_t micros_period_right = 0;
uint32_t micros_period_left = 0;

int vehicle_speed = 0;//TODO
int vehicle_speed_ascendent = 1; //1 = ascendent, 0 = descendent
int i = 0;

int difference_factor_ascendent = 1; //1 = ascendent, 0 = descendent
int num_cycles = 0; //Number of times that we have arrived to/started from the initial speed difference factor (1)
int calculating_right_speed = 0; //1 = calculating right speed, 0 = calculating left speed
float right_wheel_speed = 0;
float left_wheel_speed = 0;
int j = 0;

int signal1_microseconds = 0;
int signal2_microseconds = 0;


int right_wheel_signal_period_us;
int left_wheel_signal_period_us;

int ms_since_edge_rising = 0;
int bouncing_counter_ms = 0;
int last_bouncing_counter_ms = 0;

#endif /* GLOBALS_H_ */
