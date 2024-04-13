/*
 * interrupts.c
 *
 *  Created on: 13 abr. 2024
 *      Author: angel
 */

#include "interrupts.h"

unsigned int _calculate_rotation_speed(uint32_t us_passed){
	return 100000000/(32 * us_passed);
}

//User PB RSI
void EXTI0_IRQHandler(void) {
	if (bouncing_counter_ms - last_bouncing_counter_ms > MAX_BOUNCES_MS) {
		last_bouncing_counter_ms = bouncing_counter_ms;

		/////////////////////
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

				if (calculating_right_speed) {
					left_wheel_speed = vehicle_speed;
					right_wheel_speed = wheel_speed_difference_factor[j] * left_wheel_speed;
				} else {
					right_wheel_speed = vehicle_speed;
					left_wheel_speed = wheel_speed_difference_factor[j] * right_wheel_speed;
				}

			} else {

					//CASE GPIO AUX C15 HIGH
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
						j++;

					} else {

						if (calculating_right_speed) {
							left_wheel_speed = vehicle_speed;
							right_wheel_speed = wheel_speed_difference_factor[j] * left_wheel_speed;
						} else {
							right_wheel_speed = vehicle_speed;
							left_wheel_speed = wheel_speed_difference_factor[j] * right_wheel_speed;
						}

						j--;

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
		}
	}
	EXTI_ClearITPendingBit(EXTI_Line0);
}

// Speed signals RSI
void EXTI9_5_IRQHandler(void) {
    if (EXTI_GetITStatus(EXTI_Line9) != 0) {
    	//STM_EVAL_LEDOn(LED4);
    	Wd = _calculate_rotation_speed(micros_period_right);
    	micros_period_right = 0;

        EXTI_ClearITPendingBit(EXTI_Line9);
    	//STM_EVAL_LEDOff(LED4);
    }
    if (EXTI_GetITStatus(EXTI_Line8) != 0) {
    	//STM_EVAL_LEDOn(LED4);
    	We = _calculate_rotation_speed(micros_period_left);
    	micros_period_left = 0;
        EXTI_ClearITPendingBit(EXTI_Line8);
        //STM_EVAL_LEDOff(LED4);
    }
}

