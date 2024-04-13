/*
 * globals.c
 *
 *  Created on: 13 abr. 2024
 *      Author: angel
 */

#include "globals.h"

int counter_led = 0;
unsigned int Wd = 0;
unsigned int We = 0;
uint32_t micros_period_right = 0;
uint32_t micros_period_left = 0;
int vehicle_speed = 0;
int vehicle_speed_ascendent = 1;
int i = 0;
int difference_factor_ascendent = 1;
int num_cycles = 0;
int calculating_right_speed = 0;
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
