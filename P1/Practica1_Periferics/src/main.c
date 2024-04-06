#include "stm32f4xx.h"
#include "stm32f429i_discovery.h"

int counter_led = 0;
//unsigned long num_overflows_right = 0;
//unsigned long num_overflows_left = 0;
unsigned int Wd = 0; // Right speed
unsigned int We = 0; // Left speed
uint32_t micros_period_right = 0;
uint32_t micros_period_left = 0;

int vehicle_speed = 35;	//TODO TODO TODO 0
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

// Executes the TIM5 RSI
void TIM5_IRQHandler(void) {
	if (TIM_GetITStatus(TIM5, TIM_IT_Update) != 0) {


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

		//TIM_SetCounter(TIM5, 0);
	}
}

// Function that calculates the Vc signal from We and Wd
void _calculate_final_signal(void){
	unsigned int minW;
	unsigned int maxW;
	unsigned int Vc;

	if (Wd > We) {
		minW = We;
		maxW = Wd;
	} else {
		minW = Wd;
		maxW = We;
	}

	// Middle point of the signal
	if (minW <= 1111/2 || minW * 1.3 >= maxW){
		DAC_SetChannel1Data(DAC_Align_12b_R, 2048);
		return;
	}

	// Right side of the signal
	if ((maxW - minW)/minW >= 1 && maxW == We){
		DAC_SetChannel1Data(DAC_Align_12b_R, 0);
		return;
	}

	// Left side of the signal
	if ((maxW - minW)/minW >= 1 && maxW == Wd){
		DAC_SetChannel1Data(DAC_Align_12b_R, 4095);
		return;
	}

	// Middle - Right side of the signal: Y = 2925X + 1170
	if(maxW == Wd) {
		DAC_SetChannel1Data(DAC_Align_12b_R, 2925 * ((Wd - We)/We) + 1170);
		return;
	}

	// Middle - Left side of the signal: Y = -2925X + 2925
	if(maxW == We) {
		DAC_SetChannel1Data(DAC_Align_12b_R, -2925 * ((Wd - We)/Wd) + 2925);
		return;
	}
}

void init_wheel_signal_ouput(void) {
	GPIO_InitTypeDef GPIO_config;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);

	GPIO_config.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_config.GPIO_OType = GPIO_OType_PP;
	GPIO_config.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_config.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_config.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOG, &GPIO_config);

	/*GPIO_InitTypeDef GPIO_config;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

	GPIO_config.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_config.GPIO_OType = GPIO_OType_PP;
	GPIO_config.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_config.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_3 | GPIO_Pin_4;	//TODO: treure el 4
	GPIO_config.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOE, &GPIO_config);*/
}

// Inits TIM5 for 1us interrupts
void init_TIM5(void){
	//TODOTODOTODO: Find values that make the timer error zero (now is near zero, but not zero)

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
    // TODO: Check priorities
    NVIC_TimerConfig.NVIC_IRQChannelPreemptionPriority = 0x01;
    NVIC_TimerConfig.NVIC_IRQChannelSubPriority = 0x01;
    NVIC_Init(&NVIC_TimerConfig);

    TIM_Cmd(TIM5, ENABLE);
}


//User PB RSI
void EXTI0_IRQHandler(void) {
	//float left;
	//float right;
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

		} else {

			//STM_EVAL_LEDOff(LED4);
			//CASE GPIO AUX C15 HIGH
			if (vehicle_speed != 0) {

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
					//left = left_wheel_speed;
					//right = right_wheel_speed;
					j++;

				} else {

					if (calculating_right_speed) {
						left_wheel_speed = vehicle_speed;
						right_wheel_speed = wheel_speed_difference_factor[j] * left_wheel_speed;
					} else {
						right_wheel_speed = vehicle_speed;
						left_wheel_speed = wheel_speed_difference_factor[j] * right_wheel_speed;
					}

					//left = left_wheel_speed;
					//right = right_wheel_speed;
					j--;

				}

			} else {
				//left = vehicle_speed;
				//right = vehicle_speed;

				right_wheel_speed = vehicle_speed;
				left_wheel_speed = vehicle_speed;
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

		//generateRightWheelSignal(right_wheel_signal_period_us);
		//TODO: generate left wheel signal


		//STM_EVAL_LEDToggle(LED4);
		EXTI_ClearITPendingBit(EXTI_Line0);
	}
}

void init_PB_user(void){
	GPIO_InitTypeDef GPIO_config;
	EXTI_InitTypeDef EXTI_config;
	NVIC_InitTypeDef NVIC_config;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);	//TODO: Check if yes or no

	GPIO_config.GPIO_Mode = GPIO_Mode_IN;
	GPIO_config.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_config.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOA, &GPIO_config);

	//For PC15 AUX INPUT
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	GPIO_config.GPIO_Mode = GPIO_Mode_IN;
	GPIO_config.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_config.GPIO_Pin = GPIO_Pin_15;
	GPIO_Init(GPIOC, &GPIO_config);
	////

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);

	EXTI_config.EXTI_Line = EXTI_Line0;
	EXTI_config.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_config.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_config.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_config);

	NVIC_config.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_config.NVIC_IRQChannelPreemptionPriority = 0x07;
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
	GPIO_config.GPIO_PuPd = GPIO_PuPd_NOPULL;	// TODO: Check if pullups yes or no
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
	NVIC_config.NVIC_IRQChannelPreemptionPriority = 0x02;
	NVIC_config.NVIC_IRQChannelSubPriority = 0x01;
	NVIC_config.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_config);

}

// Inits TIM3 and TIM4 for free-running counters that interrupts every 65535us
void init_TIM3_TIM4(void){
	TIM_TimeBaseInitTypeDef TIM_TimerConfig;
	NVIC_InitTypeDef NVIC_TimerConfig;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	//TIM_TimeBaseStructInit(&TIM_TimerConfig);	//TODO: Check if yes or no
	TIM_TimerConfig.TIM_Prescaler = 94;	// Now we get 1 Tic every 1us
	TIM_TimerConfig.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimerConfig.TIM_Period = 0xFFFFFFFF;
	TIM_TimerConfig.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM3, &TIM_TimerConfig);
    TIM_TimeBaseInit(TIM4, &TIM_TimerConfig);

    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);

    NVIC_TimerConfig.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_TimerConfig.NVIC_IRQChannelCmd = ENABLE;
    // TODO: Check priorities
    NVIC_TimerConfig.NVIC_IRQChannelPreemptionPriority = 0x04;
    NVIC_TimerConfig.NVIC_IRQChannelSubPriority = 0x01;
    NVIC_Init(&NVIC_TimerConfig);
    NVIC_TimerConfig.NVIC_IRQChannel = TIM4_IRQn;
    NVIC_Init(&NVIC_TimerConfig);

    TIM_Cmd(TIM3, ENABLE);
    TIM_Cmd(TIM4, ENABLE);

	/*TIM_TimeBaseInitTypeDef TIM_TimerConfig;
	NVIC_InitTypeDef NVIC_TimerConfig;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	//TIM_TimeBaseStructInit(&TIM_TimerConfig);	//TODO: Check if yes or no
	TIM_TimerConfig.TIM_Prescaler = 94;	// Now we get 1 Tic every 1us
	TIM_TimerConfig.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimerConfig.TIM_Period = 0xFFFFFFFF;
	TIM_TimerConfig.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM9, &TIM_TimerConfig);
    TIM_TimeBaseInit(TIM4, &TIM_TimerConfig);

    TIM_ITConfig(TIM9, TIM_IT_Update, ENABLE);
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);

    NVIC_TimerConfig.NVIC_IRQChannel = TIM1_BRK_TIM9_IRQn;
    NVIC_TimerConfig.NVIC_IRQChannelCmd = ENABLE;
    // TODO: Check priorities
    NVIC_TimerConfig.NVIC_IRQChannelPreemptionPriority = 0x04;
    NVIC_TimerConfig.NVIC_IRQChannelSubPriority = 0x01;
    NVIC_Init(&NVIC_TimerConfig);
    NVIC_TimerConfig.NVIC_IRQChannel = TIM4_IRQn;
    NVIC_Init(&NVIC_TimerConfig);

    TIM_Cmd(TIM9, ENABLE);
    TIM_Cmd(TIM4, ENABLE);*/
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
    // TODO: Check priorities
    NVIC_TimerConfig.NVIC_IRQChannelPreemptionPriority = 0x03;
    NVIC_TimerConfig.NVIC_IRQChannelSubPriority = 0x01;
    NVIC_Init(&NVIC_TimerConfig);

    TIM_Cmd(TIM2, ENABLE);
}

void init_DAC(void){
	DAC_InitTypeDef DAC_config;
	GPIO_InitTypeDef GPIO_config;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
	//RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	GPIO_config.GPIO_Pin = GPIO_Pin_4;
	GPIO_config.GPIO_Mode = GPIO_Mode_AN;
	GPIO_config.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_config);

	//DAC_DeInit();
	DAC_config.DAC_Trigger = DAC_Trigger_None;
	DAC_config.DAC_WaveGeneration = DAC_WaveGeneration_None;
	DAC_config.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
	DAC_Init(DAC_Channel_1, &DAC_config);

	DAC_Cmd(DAC_Channel_1, ENABLE);
	DAC_SetChannel1Data(DAC_Align_12b_R, 0);
}

// Executes the TIM2 RSI
void TIM2_IRQHandler(void) {
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != 0) {

		counter_led++;
		if(counter_led >= 2000){
			counter_led = 0;
			STM_EVAL_LEDToggle(LED3);
		}

		// Update the output signal
		_calculate_final_signal();

		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}

int todoododo = 0;

// Executes the TIM3 RSI
void TIM3_IRQHandler(void) {
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != 0) {
		num_overflows_right++;

		//TIM_SetCounter(TIM3, 0);
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	}
}

// Executes the TIM9 RSI
/*void TIM1_BRK_TIM9_IRQHandler(void) {
	if (TIM_GetITStatus(TIM9, TIM_IT_Update) != 0) {
		num_overflows_right++;

		todoododo++;
		if (todoododo >= 15){
			todoododo = 0;
			STM_EVAL_LEDToggle(LED4);
		}


		//TIM_SetCounter(TIM3, 0);
		TIM_ClearITPendingBit(TIM9, TIM_IT_Update);
	}
}*/

// Executes the TIM4 RSI
void TIM4_IRQHandler(void) {
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != 0) {

		todoododo++;
		if (todoododo >= 15){
			todoododo = 0;
			STM_EVAL_LEDToggle(LED4);
		}

		num_overflows_left++;
		//TIM_SetCounter(TIM4, 0);
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	}
}

unsigned int _calculate_rotation_speed(uint32_t us_passed){
	return 100000000/(32 * us_passed);
}

void EXTI9_5_IRQHandler(void) {
    if (EXTI_GetITStatus(EXTI_Line9) != 0) {

    	Wd = _calculate_rotation_speed(micros_period_right);
    	micros_period_right = 0;

        EXTI_ClearITPendingBit(EXTI_Line9);
    }
    if (EXTI_GetITStatus(EXTI_Line8) != 0) {
    	We = _calculate_rotation_speed(micros_period_right);
    	micros_period_left = 0;

    	//GPIO_ToggleBits(GPIOE, GPIO_Pin_4);
    	//STM_EVAL_LEDToggle(LED4); //TODO: Debug
        EXTI_ClearITPendingBit(EXTI_Line8);
    }
}

int main(void)
{
	init_TIM2();
	init_TIM3_TIM4();
	init_TIM5();
	init_wheels_inputs();
	init_wheel_signal_ouput();
	init_PB_user();
	init_DAC();
	STM_EVAL_LEDInit(LED3);
	STM_EVAL_LEDInit(LED4);


	/* Infinite loop */
	while (1){
	}
}

/*
 * Callback used by stm324xg_eval_i2c_ee.c.
 * Refer to stm324xg_eval_i2c_ee.h for more info.
 */
uint32_t sEE_TIMEOUT_UserCallback(void)
{
  /* TODO, implement your code here */
  while (1){
  }
}
